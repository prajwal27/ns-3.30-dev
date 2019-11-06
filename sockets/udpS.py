import socket

IP = '127.0.0.1'
PORT = 1234

UDPServerSocket = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

UDPServerSocket.bind((IP,PORT))

print("UDP server up and listening")

while True:
    message, address = UDPServerSocket.recvfrom(1024)

    print(f"Message from client {address}:{message}")

    UDPServerSocket.sendto("Hi client".encode('utf-8'),address)