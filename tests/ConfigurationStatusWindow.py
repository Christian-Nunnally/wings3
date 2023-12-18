import tkinter as tk
from tests.SimpleWindow import SimpleWindow

class ConfigurationStatusWindow(SimpleWindow):
    keyToIndexMap = {}

    def __init__(self):
        self.createRoot()
        self.root.geometry("+0+0")

    def sortMetricsList(self):
        newKeyToIndexMap = {}
        metricNamesList = list(self.keyToIndexMap.keys())
        metricNamesList.sort()
        newIndex = 0

        currentValues = {}
        for currentMetric in self.keyToIndexMap:
            currentValues[currentMetric] = self.listbox.get(self.keyToIndexMap[currentMetric])

        for metricName in metricNamesList:
            newKeyToIndexMap[metricName] = newIndex
            self.listbox.delete(newIndex)
            self.listbox.insert(newIndex, currentValues[metricName])
            newIndex += 1
        self.keyToIndexMap = newKeyToIndexMap

    def getOrAssignIndexForKey(self, key):
        if key in self.keyToIndexMap: return self.keyToIndexMap[key]
        self.keyToIndexMap[key] = len(self.keyToIndexMap)
        return self.keyToIndexMap[key]

    def update_single_status(self, key, new_status):
        index = self.getOrAssignIndexForKey(key)
        self.listbox.delete(index)
        self.listbox.insert(index, new_status)

    def runCommand(self, arguments):
        if arguments[0] == "metric":
            self.update_single_status(arguments[1], f"{arguments[1]}: {arguments[2]}")

    def createRoot(self):
        super().createRoot()
        self.root.title("Metric Status Display")
        self.root.configure(bg='black', height=500)  # Set background color to black

        self.custom_font = ('Consolas', 11)
        self.custom_fg = 'white'  # Set text color to white

        self.frame = tk.Frame(self.root, bg='black')  # Set frame background color to black
        self.frame.pack(padx=10, pady=10)

        # Create a Listbox with a custom font and color
        self.listbox = tk.Listbox(self.frame, font=self.custom_font, fg=self.custom_fg, bg='black', width=80, height=60, borderwidth=0, highlightthickness=0)
        self.listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        sortButton = tk.Button(self.root, text='Sort metrics list', background="black", foreground="white", width=100, command=self.sortMetricsList)
        sortButton.pack()

