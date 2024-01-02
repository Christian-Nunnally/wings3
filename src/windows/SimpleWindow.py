import tkinter as tk
import ctypes

class SimpleWindow:
    root = None
    isClosed = False

    def __init__(self):
        self.createRoot()

    def createRoot(self):
        self.root = tk.Tk()
        self.darkenTitleBar(self.root)
        self.root.configure(background='black')
        self.root.protocol("WM_DELETE_WINDOW", self.onClosing)

    def onClosing(self):
        self.root.destroy()
        self.isClosed = True

    def update(self):
        if self.root is not None: 
            self.root.update()

    def center(self):
        self.root.update_idletasks()
        user32 = ctypes.windll.user32
        screenWidth = user32.GetSystemMetrics(0)
        screenHeight = user32.GetSystemMetrics(1)

        size = tuple(int(_) for _ in self.root.geometry().split('+')[0].split('x'))
        x = screenWidth/2 - size[0]/2
        y = screenHeight/2 - size[1]/2
        self.root.geometry("+%d+%d" % (x, y))

    def darkenTitleBar(self, window):
        window.update()
        setWindowAttribute = ctypes.windll.dwmapi.DwmSetWindowAttribute
        getParent = ctypes.windll.user32.GetParent
        hwnd = getParent(window.winfo_id())
        value = 2
        value = ctypes.c_int(value)
        setWindowAttribute(hwnd, 20, ctypes.byref(value), 4)
