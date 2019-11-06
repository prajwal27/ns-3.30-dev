import socket
import time
import pickle

HEADERSIZE = 10

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.bind((socket.gethostname(),1236))

s.listen(5)

while True:
    clientSocket, address = s.accept()
    print(f"Connction from {address} accepted")

    # msg = "welcome to the server!"
    # msg = f"{len(msg):<{HEADERSIZE}}" + msg

    d = {1:'hey',2:'there'}
    msg = pickle.dumps(d)

    msg = bytes(f'{len(msg):<{HEADERSIZE}}',"utf-8") + msg


    clientSocket.send(msg)
    # clientSocket.close()

