import tkinter as tk
from tests.SimpleWindow import SimpleWindow
import random
import socket
import time
import threading
import queue


class RemoteControlWindow(SimpleWindow):
    def __init__(self):
        self.createRoot()
        self.keepServerAlive = True
        self.messageQueue = queue.Queue()
        self.serverThread = threading.Thread(target=self.runServerSocket)
        self.serverThread.start()

    def runServerSocket(self):
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        host = '127.0.0.1'
        port = 3232
        server_socket.bind((host, port))
        server_socket.listen(5)
        print("Waiting for incoming connection...")
        self.client_socket, self.addr = server_socket.accept()
        print("Connected")
        while self.keepServerAlive:
            if not self.messageQueue.empty():
                self.client_socket.send(f"{self.messageQueue.get()}\n".encode('utf-8'))
            else:
                time.sleep(.2)

    def change_color(self, button):
        colors = ['red', 'blue', 'green', 'yellow', 'purple', 'orange']
        new_color = random.choice(colors)
        self.sendCommand()
        button.config(bg=new_color)

    def sendCommand(self):
        self.messageQueue.put("new_color")

    def create_grid(self, root, rows, columns):
        buttonCount = 0
        button = tk.Button(root, width=10, height=2, text="turnoff", command=self.turnOffWings)
        button.grid(row=buttonCount, column=0)
        buttonCount += 1

    def turnOffWings(self):
        self.runCommand(0,0,0)

    def runCommand(self, operationCode, operationValue, operationFlags):
        self.messageQueue.put(f"{operationCode},{operationValue},{operationFlags}")

    def onClosing(self):
        self.client_socket.close()
        self.keepServerAlive = False
        self.serverThread.join()
        super().onClosing()

    def createRoot(self):
        super().createRoot()
        self.root.title("Simulated Remote Control")

        rows = 5
        columns = 5

        self.create_grid(self.root, rows, columns)

if __name__ == "__main__":
    RemoteControlWindow()