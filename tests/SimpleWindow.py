import tkinter as tk

class SimpleWindow:
    root = None
    isClosed = False

    def __init__(self):
        self.createRoot()

    def createRoot(self):
        self.root = tk.Tk()
        self.root.protocol("WM_DELETE_WINDOW", self.onClosing)

    def onClosing(self):
        self.root.destroy()
        self.isClosed = True

    def update(self):
        if self.root is not None: self.root.update()