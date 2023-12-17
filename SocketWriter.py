import socket
import time
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = '127.0.0.1'
port = 3232
server_socket.bind((host, port))
server_socket.listen(5)
print("Waiting for incoming connection...")
client_socket, addr = server_socket.accept()
print("Connection established with:", addr)
message = "Hello World!\n"
i = 0
while(True):
    client_socket.send((message + str(i)).encode('utf-8'))
    if input() == "":
        break
    print("sending..." + str(i))
    i += 1

client_socket.close()