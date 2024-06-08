import socket
import struct

UDP_IP = "192.168.1.101"
UDP_PORT = 6000

RESPONSE_MESSAGE = "Messaggio ricevuto"
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"Server in ascolto su {UDP_IP}:{UDP_PORT}")

while True:
    data, addr = sock.recvfrom(2048)  # max buffer size is 2048 bytes
    
    # retrive sequence number from packet
    packet_count = struct.unpack('!I', data[:4])[0] 
    print(f"Pacchetto {packet_count} ricevuto: {data.decode()} da {addr}")

    sock.sendto(RESPONSE_MESSAGE.encode(), addr)
    print("Risposta inviata a {}".format(addr))
