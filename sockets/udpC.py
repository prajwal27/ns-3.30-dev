import socket

IP = '127.0.0.1'
PORT = 1234

UDPClientSocket = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

UDPClientSocket.sendto("HI server".encode('utf-8'),(IP,PORT))

msgFromSErver = UDPClientSocket.recvfrom(1024)

print(msgFromSErver)