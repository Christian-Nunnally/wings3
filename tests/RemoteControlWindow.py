import tkinter as tk
from tests.SimpleWindow import SimpleWindow
import random
import socket
import time
import threading
import queue


class RemoteControlWindow(SimpleWindow):
    def __init__(self):
        self.buttonCount = 0
        self.createRoot()
        self.root.geometry("-0+0")
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
        self.create_grid(self.root)

    def makeButton(self, root, text, command):
        button = tk.Button(root, width=30, height=2, background="black", foreground="white", text=text, command=command)
        button.grid(row=self.buttonCount, column=0)
        self.buttonCount += 1

    def create_grid(self, root):
        self.makeButton(root, "turnoff", self.turnOffWings)
        self.makeButton(root, "randomize", self.randomizeEffect)
        self.makeButton(root, "simulate beat", self.randomizeEffect)
        self.makeButton(root, "simulate step", self.randomizeEffect)
        self.makeButton(root, "enable music detection", self.randomizeEffect)
        self.makeButton(root, "disable music detection", self.randomizeEffect)
        self.makeButton(root, "enable movement detection", self.randomizeEffect)
        self.makeButton(root, "disable movement detection", self.randomizeEffect)
        self.makeButton(root, "set effect preset #1", self.randomizeEffect)
        self.makeButton(root, "set effect preset #2", self.randomizeEffect)
        self.makeButton(root, "set effect preset #3", self.randomizeEffect)

    def turnOffWings(self):
        self.runCommand(0,0,0)

    def randomizeEffect(self):
        self.runCommand(0,0,0)

if __name__ == "__main__":
    RemoteControlWindow()