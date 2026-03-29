from construct import (
    Checksum, ConstError, ChecksumError, ConstructError,
    Const, Array, Struct,
    Int32ul, Int16ul, Int8sl,
    Byte, Bytes, this, Pointer
)
from os import path, mkdir
import re
from time import sleep 
from datetime import datetime 
from tkinter import filedialog as fd 
import sys
from ConfigLoader import load_config

# Define the path to the configuration file
FILE_PATH = "./config.csv"

# Load configuration file
bitmask_to_struct, bitmask_to_name, num_sensors = load_config(FILE_PATH)

# set up header 
header_arr = ["Millis"]
header_key = {
  "Millis": 1, 
}
sensor_arr = []
sensor_reading_order_key = {}
with open(FILE_PATH) as f:
  for line in f: 
    fields = [str(i).strip() for i in line.split(",")]
    if fields[0] == "BitIndex": continue 

    # add to header_key 
    header_key[fields[1]] = len(fields) // 2 - 1 
    sensor_arr.append(fields[1])
    sensor_reading_order_key[fields[1]] = [] 

    # populate header array 
    for i in range(2, len(fields), 2): 
      header_arr.append(fields[1] + " " + fields[i]) # spaces are ok (only for display)
      sensor_reading_order_key[fields[1]].append("_".join(fields[i].split())) # no spaces in actual keys only underscores 
header_info = (header_key, header_arr, sensor_arr, sensor_reading_order_key)

header_size = 4 + 4 + 2 + 4
checksum_size = 1

packet_struct = Struct(
        "sync"        / Const(b"ASU!"), # Sync byte: b"\x41\x53\x55\x21"
        "bitmask"     / Int32ul,
        "length"      / Int16ul,
        "timestamp"   / Int32ul,
        "sensor_data" / Array(this.length - header_size - checksum_size, Byte),
        "checksum"    / Int8sl #Checksum(Byte, self.validate, this)
    )

# Validate checksum
def validate(packet: bytearray) -> bool:
    total = 0
    for i in range(len(packet)-1):
      total += int.from_bytes(bytes(packet[i]), byteorder='little', signed=False)

    checksum = int.from_bytes(bytes(packet[-1]), byteorder='little', signed=True)
    return total + checksum == 0

def convert_bin(filename: str) -> None:
  global header_size, checksum_size, bitmask_to_struct, bitmask_to_name, num_sensors, header_info, packet_struct
  print("Converting " + filename)
  with open(filename, "rb") as f, open(filename[:-4] + ".csv", "w") as fout: 

    # add header 
    fout.write(",".join(header_info[1]) + "\n")

    buffer = bytearray()
    while(byte := f.read(1)):
      buffer.append(byte[0])

      if buffer.find(b"ASU!", 10) > 0: # not if it's the first one 
        # print("Attempting conversion")
        # ignore mismatches 
        if buffer[0:len(b"ASU!")] != b"ASU!": 
          buffer = buffer[buffer.find(b"ASU!"):]
        packet_bytes = buffer[0:buffer.find(b"ASU!", len(b"ASU!")+1)]
        buffer = buffer[buffer.find(b"ASU!", len(b"ASU")+1):]
        # Parse packet bytes & catch possible errors
        try:
          if validate(packet_bytes) == False: raise ChecksumError("Checksum Error")
          parsed_packet = packet_struct.parse(packet_bytes)
        except ConstError as e: # Catch sync byte mistmatch
          print(f"[ERROR] Sync byte mismatch: {e}")
          continue
        except ChecksumError as e: # Catch checksum validation errors
          print(f"[ERROR] Checksum validation failed: {e}")
          continue
        except ConstructError as e: # Catch-all for other parse errors
          print(f"[ERROR] Packet parsing failed: {e}")
          continue

        # Extract sensor ID & sensor data
        bitmask = parsed_packet.bitmask
        sensor_data = bytes(parsed_packet.sensor_data)
        timestamp = parsed_packet.timestamp

        # print("Mask: ", bin(bitmask))

        # Parse each sensor data field
        offset = 0
        parsed = {}
        parse_error = False

        for bitmask_index in reversed(range(num_sensors)): # Iterate through each sensor (0 -> num_sensors)
          if bitmask & (1 << bitmask_index):
            if bitmask_index not in bitmask_to_struct: # Check if sensor exists
              print(f"[ERROR] No sensor found for bitmask index: {bitmask_index}")
              continue

            # Extract sensor data fields & sensor name
            sensor_fields = bitmask_to_struct[num_sensors - bitmask_index - 1]
            sensor_name = bitmask_to_name[num_sensors - bitmask_index - 1]

            try: # Parse sensor data fields
              temp_parsed = sensor_fields.parse(sensor_data[offset:])
              # print("Temp Parsed:", temp_parsed)
            except ConstructError as e: # Catch errors in parsing sensor data
              print(f"[ERROR] Parsing sensor {sensor_name} (bitmask {bitmask_index}) failed: {e}")
              parse_error = True 
              break

            # Store parsed sensor data
            parsed[sensor_name] = temp_parsed
            offset += sensor_fields.sizeof()

        if parse_error == False:
          # print("\tSuccess")
          packet = {
              "timestamp": timestamp,
              "sensor_data": parsed
          }

          row = [] 

          row.append(str(packet["timestamp"]))
          for sensor in header_info[0].keys():
            if sensor == "Millis": continue 
            if sensor in packet["sensor_data"]:
              for i in list(packet["sensor_data"][sensor].keys())[1:]:
                row.append(str(packet["sensor_data"][sensor][i]))
            else:
              for i in range(header_info[0][sensor]):
                row.append("")
          
          fout.write(",".join(row) + "\n")
                

        else: 
          # print("\tFailure")
          pass
  print("Done")

def main():
  global header_size, checksum_size, bitmask_to_struct, bitmask_to_name, num_sensors, header_info, packet_struct

  print("Loaded config")
  print("{:<20}{}".format("header_size:", header_size))
  print("{:<20}{}".format("checksum_size:", checksum_size))
  print("{:<20}{}".format("num_sensors:", num_sensors))
  print("Using CSV Header:")
  print(", ".join(header_info[1]))

  if len(sys.argv) > 1:
    for filepath in sys.argv[1:]:
      convert_bin(filepath)
  else: 
    filepath = fd.askopenfilename()

    convert_bin(filepath)


if __name__ == '__main__':
  main()