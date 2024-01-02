import tkinter as tk
from src.windows.SimpleWindow import SimpleWindow
import random
import socket
import time
import threading
import queue

REMOTE_OPERATION_CODE_ENABLE_LEDS = 0
REMOTE_OPERATION_CODE_DISABLE_LEDS = 1
REMOTE_OPERATION_CODE_ENABLE_MUSIC_DETECTION = 2
REMOTE_OPERATION_CODE_DISABLE_MUSIC_DETECTION = 3
REMOTE_OPERATION_CODE_ENABLE_MOVEMENT_DETECTION = 4
REMOTE_OPERATION_CODE_DISABLE_MOVEMENT_DETECTION = 5
REMOTE_OPERATION_CODE_ENABLE_STEP_DETECTION = 6
REMOTE_OPERATION_CODE_DISABLE_STEP_DETECTION = 7
REMOTE_OPERATION_CODE_ENABLE_RANDOM_EFFECT_CHANGE = 8
REMOTE_OPERATION_CODE_DISABLE_RANDOM_EFFECT_CHANGE = 9
REMOTE_OPERATION_CODE_INCREASE_BRIGHTNESS = 10
REMOTE_OPERATION_CODE_DECREASE_BRIGHTNESS = 11
REMOTE_OPERATION_CODE_INCREASE_SPEED = 12
REMOTE_OPERATION_CODE_DECREASE_SPEED = 13
REMOTE_OPERATION_CODE_SELECT_PRESET = 14
REMOTE_OPERATION_CODE_SET_PRESET = 15
REMOTE_OPERATION_CODE_RANDOMIZE_EFFECTS = 16
REMOTE_OPERATION_CODE_EFFECT_TRIGGER = 17
REMOTE_OPERATION_CODE_ENTER_SETTINGS = 18
REMOTE_OPERATION_CODE_NAVIGATE_UP = 19
REMOTE_OPERATION_CODE_NAVIGATE_DOWN = 20
REMOTE_OPERATION_CODE_NAVIGATE_LEFT = 21
REMOTE_OPERATION_CODE_NAVIGATE_RIGHT = 22
REMOTE_OPERATION_CODE_NAVIGATE_ENTER = 23
REMOTE_OPERATION_CODE_NAVIGATE_BACK = 24
REMOTE_OPERATION_CODE_INCREASE_FADE = 25
REMOTE_OPERATION_CODE_DECREASE_FADE = 26
REMOTE_OPERATION_CODE_SET_FADE = 27
REMOTE_OPERATION_CODE_ENABLE_SECONDARY_EFFECT = 28
REMOTE_OPERATION_CODE_DISABLE_SECONDARY_EFFECT = 29
REMOTE_OPERATION_CODE_ENABLE_BACKGROUND_EFFECT = 30
REMOTE_OPERATION_CODE_DISABLE_BACKGROUND_EFFECT = 31


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

    def change_color(self, button):
        colors = ['red', 'blue', 'green', 'yellow', 'purple', 'orange']
        new_color = random.choice(colors)
        self.sendCommand()
        button.config(bg=new_color)

    def sendCommand(self):
        self.messageQueue.put("new_color")

    def runCommand(self, operationCode, operationValue = 0, operationFlags = 0):
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

    def makeButton(self, text, command):
        button = tk.Button(self.root, width=30, height=2, background="black", foreground="white", text=text, command=command)
        button.grid(row=self.buttonCount, column=0)
        self.buttonCount += 1

    def makeSlider(self, label, updateCommand):
        label = tk.Label(self.root, text=label, background="black", foreground="white")
        label.grid(row=self.buttonCount, column=0)
        self.buttonCount += 1

        slider = tk.Scale(self.root, from_=0, to=255, orient=tk.HORIZONTAL, command=updateCommand, background="black", foreground="white", length=200)
        slider.grid(row=self.buttonCount, column=0)
        self.buttonCount += 1

    def create_grid(self, root):
        self.makeButton("Enable leds", lambda : self.runCommand(REMOTE_OPERATION_CODE_ENABLE_LEDS))
        self.makeButton("Disable leds", lambda : self.runCommand(REMOTE_OPERATION_CODE_DISABLE_LEDS))
        self.makeButton("Enable music detection", lambda : self.runCommand(REMOTE_OPERATION_CODE_ENABLE_MUSIC_DETECTION))
        self.makeButton("Disable music detection", lambda : self.runCommand(REMOTE_OPERATION_CODE_DISABLE_MUSIC_DETECTION))
        self.makeButton("Enable movement detection", lambda : self.runCommand(REMOTE_OPERATION_CODE_ENABLE_MOVEMENT_DETECTION))
        self.makeButton("Disable movement detection", lambda : self.runCommand(REMOTE_OPERATION_CODE_DISABLE_MOVEMENT_DETECTION))
        self.makeButton("Enable step detection", lambda : self.runCommand(REMOTE_OPERATION_CODE_ENABLE_STEP_DETECTION))
        self.makeButton("Disable step detection", lambda : self.runCommand(REMOTE_OPERATION_CODE_DISABLE_STEP_DETECTION))
        self.makeButton("Enable random effect change", lambda : self.runCommand(REMOTE_OPERATION_CODE_ENABLE_RANDOM_EFFECT_CHANGE))
        self.makeButton("Disable random effect change", lambda : self.runCommand(REMOTE_OPERATION_CODE_DISABLE_RANDOM_EFFECT_CHANGE))
        self.makeButton("Increase brightness", lambda : self.runCommand(REMOTE_OPERATION_CODE_INCREASE_BRIGHTNESS))
        self.makeButton("Decrease brightness", lambda : self.runCommand(REMOTE_OPERATION_CODE_DECREASE_BRIGHTNESS))
        self.makeButton("Increase speed", lambda : self.runCommand(REMOTE_OPERATION_CODE_INCREASE_SPEED))
        self.makeButton("Decrease speed", lambda : self.runCommand(REMOTE_OPERATION_CODE_DECREASE_SPEED))
        self.makeButton("Randomize effect randomly", lambda : self.runCommand(REMOTE_OPERATION_CODE_RANDOMIZE_EFFECTS))
        self.makeButton("Save to preset #1",  lambda : self.runCommand(REMOTE_OPERATION_CODE_SET_PRESET, 1))
        self.makeButton("Save to preset #2",  lambda : self.runCommand(REMOTE_OPERATION_CODE_SET_PRESET, 2))
        self.makeButton("Save to preset #3",  lambda : self.runCommand(REMOTE_OPERATION_CODE_SET_PRESET, 3))
        self.makeButton("Load from preset #1",  lambda : self.runCommand(REMOTE_OPERATION_CODE_SELECT_PRESET, 1))
        self.makeButton("Load from preset #2",  lambda : self.runCommand(REMOTE_OPERATION_CODE_SELECT_PRESET, 2))
        self.makeButton("Load from preset #3",  lambda : self.runCommand(REMOTE_OPERATION_CODE_SELECT_PRESET, 3))
        self.makeButton("Trigger effect",  lambda : self.runCommand(REMOTE_OPERATION_CODE_EFFECT_TRIGGER, 0))
        self.makeButton("Trigger effect 2",  lambda : self.runCommand(REMOTE_OPERATION_CODE_EFFECT_TRIGGER, 1))
        # self.makeButton("Settings",  lambda : self.runCommand(REMOTE_OPERATION_CODE_ENTER_SETTINGS))
        # self.makeButton("Up",  lambda : self.runCommand(REMOTE_OPERATION_CODE_NAVIGATE_UP))
        # self.makeButton("Right",  lambda : self.runCommand(REMOTE_OPERATION_CODE_NAVIGATE_RIGHT))
        # self.makeButton("Down",  lambda : self.runCommand(REMOTE_OPERATION_CODE_NAVIGATE_DOWN))
        # self.makeButton("Left",  lambda : self.runCommand(REMOTE_OPERATION_CODE_NAVIGATE_LEFT))
        # self.makeButton("Enter",  lambda : self.runCommand(REMOTE_OPERATION_CODE_NAVIGATE_ENTER))
        # self.makeButton("Back",  lambda : self.runCommand(REMOTE_OPERATION_CODE_NAVIGATE_BACK))
        self.makeSlider("Set fade",  lambda v : self.runCommand(REMOTE_OPERATION_CODE_SET_FADE, v))

if __name__ == "__main__":
    RemoteControlWindow()