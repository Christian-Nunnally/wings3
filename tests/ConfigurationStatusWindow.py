import tkinter as tk
from tests.SimpleWindow import SimpleWindow

class ConfigurationStatusWindow(SimpleWindow):
    keyToIndexMap = {}

    def __init__(self):
        self.createRoot()
        self.update_status()

    def getOrAssignIndexForKey(self, key):
        if key in self.keyToIndexMap: return self.keyToIndexMap[key]
        self.keyToIndexMap[key] = len(self.keyToIndexMap)
        return self.keyToIndexMap[key]

    def update_single_status(self, key, new_status):
        index = self.getOrAssignIndexForKey(key)
        self.listbox.delete(index)
        self.listbox.insert(index, new_status)

    def update_status(self):
        # Replace this with your function to fetch updated status strings
        # For demonstration purposes, I'm using a static list of strings
        status_list = [
            "Status 1: Updated",
            "Status 2: In Progress",
            "Status 3: Pending",
            "Status 4: Completed",
            "Status 5: Error"
        ]

        # Update the first status line with a new status
        #new_status = "Status 1: Newly Updated"
        #self.update_single_status(0, new_status)

    def runCommand(self, arguments):
        if arguments[0] == "metric":
            self.update_single_status(arguments[1], f"{arguments[1]}: {arguments[2]}")

    def createRoot(self):
        super().createRoot()
        self.root.title("Metric Status Display")
        self.root.configure(bg='black')  # Set background color to black

        # Use a custom font and color for the listbox
        self.custom_font = ('Arial', 12)
        self.custom_fg = 'white'  # Set text color to white

        self.frame = tk.Frame(self.root, bg='black')  # Set frame background color to black
        self.frame.pack(padx=10, pady=10)

        # Create a Listbox with a custom font and color
        self.listbox = tk.Listbox(self.frame, font=self.custom_font, fg=self.custom_fg, bg='black', width=40, height=10)
        self.listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)