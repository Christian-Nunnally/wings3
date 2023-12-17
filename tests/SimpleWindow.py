import tkinter as tk
import ctypes

class SimpleWindow:
    root = None
    isClosed = False

    def __init__(self):
        self.createRoot()

    def createRoot(self):
        self.root = tk.Tk()
        self.dark_title_bar(self.root)
        self.root.configure(background='black')
        self.root.protocol("WM_DELETE_WINDOW", self.onClosing)

    def onClosing(self):
        self.root.destroy()
        self.isClosed = True

    def update(self):
        if self.root is not None: self.root.update()

    def center(self):
        self.root.update_idletasks()
        # PyQt way to find the screen resolution
        user32 = ctypes.windll.user32
        screen_width = user32.GetSystemMetrics(0)
        screen_height = user32.GetSystemMetrics(1)

        size = tuple(int(_) for _ in self.root.geometry().split('+')[0].split('x'))
        x = screen_width/2 - size[0]/2
        y = screen_height/2 - size[1]/2
        self.root.geometry("+%d+%d" % (x, y))

    def dark_title_bar(self, window):
        """
        MORE INFO:
        https://learn.microsoft.com/en-us/windows/win32/api/dwmapi/ne-dwmapi-dwmwindowattribute
        """
        window.update()
        set_window_attribute = ctypes.windll.dwmapi.DwmSetWindowAttribute
        get_parent = ctypes.windll.user32.GetParent
        hwnd = get_parent(window.winfo_id())
        value = 2
        value = ctypes.c_int(value)
        set_window_attribute(hwnd, 20, ctypes.byref(value), 4)
