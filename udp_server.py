import socket
import struct

UDP_IP = "192.168.1.101"
UDP_PORT = 6000
packets_received = 0

RESPONSE_MESSAGE = "Messaggio ricevuto"
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"Server in ascolto su {UDP_IP}:{UDP_PORT}")

while True:
    data, addr = sock.recvfrom(2048)  # max buffer size is 2048 bytes
    packets_received += 1
    
    # Retrieve sequence number from packet
    packet_count, = struct.unpack('!I', data[:4])
    message = data[4:].decode()
    
    print(f"Pacchetto {packet_count} ricevuto: {message} da {addr}")

    # Send response to port 6001 after receiving 35 packets
    if packets_received == 35:
        response_addr = (addr[0], 6001)
        sock.sendto(RESPONSE_MESSAGE.encode(), response_addr)
        print(f"Numero di pacchetti ricevuti: {packets_received}")
        print(f"Risposta inviata a {addr}")
        packets_received = 0  # reset the counter after sending the response to port 6001
