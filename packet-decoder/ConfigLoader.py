import csv
import struct
from dataclasses import dataclass


TYPE_KEY = {
    "uint8_t": "B",
    "uint16_t": "H",
    "uint32_t": "I",
    "int8_t": "b",
    "int16_t": "h",
    "int32_t": "i",
    "float": "f",
    "double": "d",
}


@dataclass(frozen=True)
class SensorField:
    label: str
    type_name: str
    format_char: str


@dataclass(frozen=True)
class SensorDefinition:
    bit_index: int
    name: str
    fields: tuple[SensorField, ...]

    @property
    def payload_format(self) -> str:
        return "<" + "".join(field.format_char for field in self.fields)

    @property
    def payload_size(self) -> int:
        return struct.calcsize(self.payload_format)


def load_config(filepath: str) -> list[SensorDefinition]:
    sensors: list[SensorDefinition] = []

    with open(filepath, "r", newline="") as config_file:
        reader = csv.reader(config_file)
        header = next(reader)
        header = [column.strip() for column in header]

        bit_index_column = header.index("BitIndex")
        sensor_name_column = header.index("SensorName")

        for row in reader:
            if not row:
                continue

            bit_index = int(row[bit_index_column])
            name = row[sensor_name_column].strip()
            field_tokens = [token.strip() for token in row[sensor_name_column + 1 :] if token.strip()]

            fields: list[SensorField] = []
            for label, type_name in zip(field_tokens[0::2], field_tokens[1::2]):
                fields.append(
                    SensorField(
                        label=label,
                        type_name=type_name,
                        format_char=TYPE_KEY[type_name],
                    )
                )

            sensors.append(
                SensorDefinition(
                    bit_index=bit_index,
                    name=name,
                    fields=tuple(fields),
                )
            )

    sensors.sort(key=lambda sensor: sensor.bit_index)
    return sensors
