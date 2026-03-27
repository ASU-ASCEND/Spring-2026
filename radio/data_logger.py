import argparse
import pathlib
import os
import serial
import serial.tools.list_ports as sp
from datetime import datetime

# get args
parser = argparse.ArgumentParser(description="ASCEND Radio Receiver Data Logger")
# output name
parser.add_argument(
    "-o",
    "--output",
    type=str,
    help="Unique identifier for output",
    required=True,
)
parser.add_argument(
    "-f",
    "--folder",
    type=pathlib.Path,
    required=False,
    default="data",
    help="Path to folder to put output file into (default ./data)",
)

args = parser.parse_args()

OUTPUT_FILE = os.path.join(
    args.folder, f"{args.output}_{datetime.now().strftime('%Y_%m_%d_%H_%M_%S.txt')}.txt"
)

# open serial
ports = list(sp.comports())
for i in ports:
    print(i)
print("Enter the Port name")
comport = str(input())
print()

# Open Serial Port,
ser = serial.Serial(
    port=comport,
    baudrate=112500,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=None,
)

# create output folder if it doesn't exist
os.makedirs(args.folder, exist_ok=True)

print(f"Creating {OUTPUT_FILE}")

while 1:
    with open(OUTPUT_FILE, "a", newline="\n") as f:
        line = ser.readline().decode("utf-8", errors="replace")

        timestamp = datetime.now().strftime("%H:%M:%S")

        print(f"{timestamp} -> {line}", end="")
        f.write(f"{timestamp} -> {line}")
