import struct
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable

from ConfigLoader import SensorDefinition, SensorField, load_config

try:
    from tkinter import filedialog as fd
except Exception:
    fd = None


FILE_PATH = Path(__file__).with_name("config.csv")
SYNC_BYTES = b"ASU!"
HEADER_FORMAT = struct.Struct("<4sIHI")
HEADER_SIZE = HEADER_FORMAT.size
CHECKSUM_SIZE = 1
MIN_PACKET_SIZE = HEADER_SIZE + CHECKSUM_SIZE


@dataclass(frozen=True)
class Packet:
    offset: int
    bitmask: int
    length: int
    timestamp: int
    payload: bytes


def build_csv_header(sensors: list[SensorDefinition]) -> list[str]:
    header = ["Millis"]
    for sensor in sensors:
        for field in sensor.fields:
            header.append(f"{sensor.name} {field.label}")
    return header


def build_legacy_schema(current_sensors: list[SensorDefinition]) -> list[SensorDefinition]:
    legacy_analog_temp = SensorDefinition(
        bit_index=len(current_sensors),
        name="AnalogTemp",
        fields=(
            SensorField(
                label="ADC_Read",
                type_name="int32_t",
                format_char="i",
            ),
        ),
    )
    return [*current_sensors, legacy_analog_temp]


def checksum_valid(packet_bytes: bytes) -> bool:
    return sum(packet_bytes) & 0xFF == 0


def resolve_sensor_presence(bitmask: int, sensors: list[SensorDefinition], payload_size: int) -> list[bool]:
    if bitmask == 0:
        raise ValueError("bitmask is zero")

    sensor_count = len(sensors)
    candidates: list[tuple[int, int, list[bool]]] = []
    highest_bit = bitmask.bit_length() - 1

    for millis_bit in {highest_bit, sensor_count, sensor_count + 1}:
        if millis_bit <= 0 or not (bitmask & (1 << millis_bit)):
            continue

        presence: list[bool] = []
        expected_payload_size = 0
        for sensor_index, sensor in enumerate(sensors):
            bit_position = millis_bit - sensor_index - 1
            is_present = bit_position >= 0 and bool(bitmask & (1 << bit_position))
            presence.append(is_present)
            if is_present:
                expected_payload_size += sensor.payload_size

        if expected_payload_size != payload_size:
            continue

        ignored_mask = bitmask & ~(((1 << (millis_bit + 1)) - 1) if millis_bit < 31 else 0xFFFFFFFF)
        ignored_set_bits = ignored_mask.bit_count()
        candidates.append((ignored_set_bits, abs(millis_bit - highest_bit), presence))

    if not candidates:
        raise ValueError(
            f"bitmask 0x{bitmask:08x} does not match payload size {payload_size}"
        )

    candidates.sort(key=lambda candidate: (candidate[0], candidate[1]))
    return candidates[0][2]


def iter_packets(data: bytes) -> Iterable[Packet]:
    offset = 0
    while offset < len(data):
        sync_offset = data.find(SYNC_BYTES, offset)
        if sync_offset < 0:
            break

        if sync_offset + MIN_PACKET_SIZE > len(data):
            break

        sync, bitmask, packet_length, timestamp = HEADER_FORMAT.unpack_from(data, sync_offset)
        if sync != SYNC_BYTES or packet_length < MIN_PACKET_SIZE:
            offset = sync_offset + 1
            continue

        packet_end = sync_offset + packet_length
        if packet_end > len(data):
            offset = sync_offset + 1
            continue

        packet_bytes = data[sync_offset:packet_end]
        if not checksum_valid(packet_bytes):
            offset = sync_offset + 1
            continue

        payload = packet_bytes[HEADER_SIZE:-CHECKSUM_SIZE]
        yield Packet(
            offset=sync_offset,
            bitmask=bitmask,
            length=packet_length,
            timestamp=timestamp,
            payload=payload,
        )
        offset = packet_end


def decode_with_schema(
    payload: bytes,
    bitmask: int,
    schema: list[SensorDefinition],
) -> tuple[list[SensorDefinition], list[str]]:
    presence = resolve_sensor_presence(bitmask, schema, len(payload))

    values: list[str] = []
    payload_offset = 0

    for sensor, is_present in zip(schema, presence):
        if not is_present:
            values.extend("" for _ in sensor.fields)
            continue

        sensor_payload = payload[payload_offset : payload_offset + sensor.payload_size]
        if len(sensor_payload) != sensor.payload_size:
            raise ValueError(f"truncated payload for {sensor.name}")

        decoded_values = struct.unpack(sensor.payload_format, sensor_payload)
        values.extend(str(value) for value in decoded_values)
        payload_offset += sensor.payload_size

    if payload_offset != len(payload):
        raise ValueError(
            f"payload decode consumed {payload_offset} bytes, expected {len(payload)}"
        )

    return schema, values


def decode_payload(
    payload: bytes,
    bitmask: int,
    schema_candidates: list[list[SensorDefinition]],
    output_sensors: list[SensorDefinition],
) -> list[str]:
    last_error: ValueError | None = None

    for schema in schema_candidates:
        try:
            matched_schema, values = decode_with_schema(payload, bitmask, schema)
        except ValueError as error:
            last_error = error
            continue

        value_by_sensor_name: dict[str, list[str]] = {}
        field_offset = 0
        for sensor in matched_schema:
            field_count = len(sensor.fields)
            value_by_sensor_name[sensor.name] = values[field_offset : field_offset + field_count]
            field_offset += field_count

        ordered_values: list[str] = []
        for sensor in output_sensors:
            ordered_values.extend(value_by_sensor_name.get(sensor.name, [""] * len(sensor.fields)))

        return ordered_values

    if last_error is None:
        raise ValueError("no schema candidates were provided")
    raise last_error


def convert_bin(filename: str) -> None:
    current_sensors = load_config(str(FILE_PATH))
    legacy_sensors = build_legacy_schema(current_sensors)
    output_sensors = legacy_sensors
    schema_candidates = [current_sensors, legacy_sensors]
    input_path = Path(filename)
    output_path = input_path.with_suffix(".csv")

    packet_count = 0
    skipped_packets = 0

    data = input_path.read_bytes()
    header = build_csv_header(output_sensors)

    with output_path.open("w", newline="") as csv_file:
        csv_file.write(",".join(header) + "\n")

        for packet in iter_packets(data):
            try:
                row = [
                    str(packet.timestamp),
                    *decode_payload(
                        packet.payload,
                        packet.bitmask,
                        schema_candidates=schema_candidates,
                        output_sensors=output_sensors,
                    ),
                ]
            except ValueError as error:
                skipped_packets += 1
                print(f"[WARN] Skipping packet at offset {packet.offset}: {error}")
                continue

            csv_file.write(",".join(row) + "\n")
            packet_count += 1

    print(f"Converted {input_path} -> {output_path}")
    print(f"Decoded packets: {packet_count}")
    if skipped_packets:
        print(f"Skipped packets: {skipped_packets}")


def main() -> None:
    if len(sys.argv) > 1:
        for filepath in sys.argv[1:]:
            convert_bin(filepath)
        return

    if fd is None:
        raise SystemExit("No input file provided and tkinter is unavailable.")

    filepath = fd.askopenfilename()
    if filepath:
        convert_bin(filepath)


if __name__ == "__main__":
    main()
