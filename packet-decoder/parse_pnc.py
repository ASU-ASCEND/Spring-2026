import struct 

num = 60
packet_fmt = "<IIBBfIfIfIfffIB?BB8ib"
packet_struct = struct.Struct(packet_fmt)
total_packets = []

while True:
    try:
        filename = f"data{num}.bin"
        with open(filename, "rb") as file:
            packet = file.read(packet_struct.size)
            while len(packet) == packet_struct.size:
                data = packet_struct.unpack(packet)
                print(",".join([str(d) for d in data]))
                total_packets.append(data)
                packet = file.read(packet_struct.size)
    except FileNotFoundError:
        print("error")
        break 
    num+=1