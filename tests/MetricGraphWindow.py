from tests.SimpleWindow import SimpleWindow
import tkinter as tk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import threading
import queue

class MetricGraphWindow(SimpleWindow):

    def __init__(self):
        self.yMin = 0
        self.yMax = 1
        self.dataQueue = queue.Queue()
        self.thread = threading.Thread(target=self.runMetricGraphWindowAsItsOwnThread)
        self.thread.daemon = True
        self.thread.start()
        self.keepRunning = True
        self.shouldResetMinMax = False

    def runMetricGraphWindowAsItsOwnThread(self):
        self.yLength = 30
        self.x_data = []
        self.y_data = []
        for i in range(self.yLength):
            self.x_data.append(i)
            self.y_data.append(i)
        self.createRoot()
        self.root.geometry("+0-0")
        self.root.mainloop()

    def createRoot(self):
        super().createRoot()
        self.root.title('Metric Graph Window')
        self.root.configure(bg='black')
        plt.style.use('dark_background')
        self.fig, self.ax = plt.subplots()
        self.root.plot = FigureCanvasTkAgg(self.fig, master=self.root)
        self.root.plot.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)
        self.ax.set_xticks([])
        self.ax.set_yticks([])
        self.root.plot.draw()
        self.line, = self.ax.plot(self.y_data, self.x_data, marker='o', linestyle='-')
        self.ax.set_xlabel('X-axis')
        self.ax.set_ylabel('Y-axis')
        self.ax.set_title('Console self.root Output Graph')
        self.root.after(100, self.updateGraph)

    def updateGraph(self):
        while not self.dataQueue.empty():
            try:
                data = self.dataQueue.get(block=False)
                del self.x_data[0]
                self.x_data.append(data)
            except queue.Empty:
                pass
        self.ax.clear()
        self.line, = self.ax.plot(self.y_data, self.x_data, marker='o', linestyle='-', color="cyan")
        self.ax.set_facecolor("black")
        self.ax.set_ylim(self.yMin, self.yMax)
        self.root.plot.draw()
        if self.keepRunning:
            self.root.after(33, self.updateGraph)
        else:
            self.root.destroy()

    def resetMinMax(self):
        self.shouldResetMinMax = True

    def insertData(self, value):
        self.dataQueue.put(value)
        if (value > self.yMax): self.yMax = value
        if (value < self.yMin): self.yMin = value
        if self.shouldResetMinMax:
            self.yMax = value
            self.yMin = value
            self.shouldResetMinMax = False