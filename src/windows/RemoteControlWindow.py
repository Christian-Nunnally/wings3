import tkinter as tk
from src.windows.SimpleWindow import SimpleWindow
import random
import socket
import time
import threading
import queue

BRIGHTNESS_CHANGE_INCREMENT = 10
SPEED_CHANGE_INCREMENT = .1

REMOTE_OPERATION_CODE_NO_OP = 0
REMOTE_OPERATION_CODE_CONTROL_LEDS = 1
REMOTE_OPERATION_CODE_CONTROL_MICROPHONE = 2
REMOTE_OPERATION_CODE_CONTROL_SPEED = 3
REMOTE_OPERATION_CODE_CONTROL_BRIGHTNESS = 4
REMOTE_OPERATION_CODE_CONTROL_MOVEMENT_DETECTION = 5
REMOTE_OPERATION_CODE_CONTROL_STEP_DETECTION = 6
REMOTE_OPERATION_CODE_CONTROL_TEMPORAL_FADE = 7
REMOTE_OPERATION_CODE_CONTROL_SECONDARY_EFFECT = 8
REMOTE_OPERATION_CODE_CONTROL_BACKGROUND_EFFECT = 9
REMOTE_OPERATION_CODE_CONTROL_RANDOM_EFFECT_CHANGE = 10
REMOTE_OPERATION_CODE_CONTROL_PRESET = 11
REMOTE_OPERATION_CODE_EFFECT_TRIGGER = 12

REMOTE_OPERATION_TYPE_NOT_APPLICABLE = 0
REMOTE_OPERATION_TYPE_GET = 1
REMOTE_OPERATION_TYPE_SET = 2
REMOTE_OPERATION_TYPE_INCREMENT = 3
REMOTE_OPERATION_TYPE_DECREMENT = 4
REMOTE_OPERATION_TYPE_ENABLE = 5
REMOTE_OPERATION_TYPE_DISABLE = 6


class RemoteControlWindow(SimpleWindow):
    def __init__(self):
        self.buttonCount = 0
        self.createRoot()
        self.root.geometry("-0+0")
        self.keepServerAlive = True
        self.messageQueue = queue.Queue()
        self.serverThread = threading.Thread(target=self.runServerSocket)
        self.serverThread.daemon = True
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

    def runCommand(self, operationCode, operationType = 0, operationValue = 0):
        commandCode = (operationCode << 24) | (operationType << 16) | (operationValue)
        self.messageQueue.put(f"{commandCode}")

    def onClosing(self):
        self.client_socket.close()
        self.keepServerAlive = False
        self.serverThread.join()
        super().onClosing()

    def createRoot(self):
        super().createRoot()
        self.root.title("Simulated Remote Control")
        self.create_grid(self.root)

    def makeButton(self, text, command):
        button = tk.Button(self.root, width=30, height=2, background="black", foreground="white", text=text, command=command)
        button.grid(row=self.buttonCount, column=0)
        self.buttonCount += 1

    def makeSlider(self, label, minimumValue, maximumValue, increment, updateCommand):
        label = tk.Label(
            self.root, 
            text=label, 
            background="black", 
            foreground="white")
        label.grid(row=self.buttonCount, column=0)
        self.buttonCount += 1

        slider = tk.Scale(
            self.root, 
            from_=minimumValue, 
            to=maximumValue, 
            resolution=increment,
            orient=tk.HORIZONTAL, 
            command=updateCommand, 
            background="black", 
            foreground="white", 
            length=200)
        slider.grid(row=self.buttonCount, column=0)
        self.buttonCount += 1

    def create_grid(self, root):
        self.makeButton("Enable leds", lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_LEDS, REMOTE_OPERATION_TYPE_ENABLE))
        self.makeButton("Disable leds", lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_LEDS, REMOTE_OPERATION_TYPE_DISABLE))
        self.makeButton("Enable music detection", lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_MICROPHONE, REMOTE_OPERATION_TYPE_ENABLE))
        self.makeButton("Disable music detection", lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_MICROPHONE, REMOTE_OPERATION_TYPE_DISABLE))
        self.makeButton("Enable movement detection", lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_MOVEMENT_DETECTION, REMOTE_OPERATION_TYPE_ENABLE))
        self.makeButton("Disable movement detection", lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_MOVEMENT_DETECTION, REMOTE_OPERATION_TYPE_DISABLE))
        self.makeButton("Enable step detection", lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_STEP_DETECTION, REMOTE_OPERATION_TYPE_ENABLE))
        self.makeButton("Disable step detection", lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_STEP_DETECTION, REMOTE_OPERATION_TYPE_DISABLE))
        self.makeButton("Enable random effect change", lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_RANDOM_EFFECT_CHANGE, REMOTE_OPERATION_TYPE_ENABLE))
        self.makeButton("Disable random effect change", lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_RANDOM_EFFECT_CHANGE, REMOTE_OPERATION_TYPE_DISABLE))
        self.makeButton("Increase brightness", lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_BRIGHTNESS, REMOTE_OPERATION_TYPE_INCREMENT))
        self.makeButton("Decrease brightness", lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_BRIGHTNESS, REMOTE_OPERATION_TYPE_DECREMENT))
        self.makeButton("Increase speed", lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_BRIGHTNESS, REMOTE_OPERATION_TYPE_INCREMENT, BRIGHTNESS_CHANGE_INCREMENT))
        self.makeButton("Decrease speed", lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_BRIGHTNESS, REMOTE_OPERATION_TYPE_DECREMENT, BRIGHTNESS_CHANGE_INCREMENT))
        self.makeButton("Save to preset #1",  lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_PRESET, REMOTE_OPERATION_TYPE_SET, 1))
        self.makeButton("Save to preset #2",  lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_PRESET, REMOTE_OPERATION_TYPE_SET, 2))
        self.makeButton("Save to preset #3",  lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_PRESET, REMOTE_OPERATION_TYPE_SET, 3))
        self.makeButton("Load from preset #1",  lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_PRESET, REMOTE_OPERATION_TYPE_GET, 1))
        self.makeButton("Load from preset #2",  lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_PRESET, REMOTE_OPERATION_TYPE_GET, 2))
        self.makeButton("Load from preset #3",  lambda : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_PRESET, REMOTE_OPERATION_TYPE_GET, 3))
        self.makeButton("Trigger effect",  lambda : self.runCommand(REMOTE_OPERATION_CODE_EFFECT_TRIGGER, REMOTE_OPERATION_TYPE_INCREMENT, 0))
        self.makeButton("Trigger effect 2",  lambda : self.runCommand(REMOTE_OPERATION_CODE_EFFECT_TRIGGER, REMOTE_OPERATION_TYPE_INCREMENT, 1))
        self.makeSlider("Set fade", 0, 255, 1,  lambda v : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_TEMPORAL_FADE, REMOTE_OPERATION_TYPE_SET, v))
        self.makeSlider("Set brightness", 0, 255, 1,  lambda v : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_BRIGHTNESS, REMOTE_OPERATION_TYPE_SET, v))
        self.makeSlider("Set speed", .1, 2, SPEED_CHANGE_INCREMENT,  lambda v : self.runCommand(REMOTE_OPERATION_CODE_CONTROL_SPEED, REMOTE_OPERATION_TYPE_SET, v))

if __name__ == "__main__":
    RemoteControlWindow()