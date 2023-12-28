import tkinter as tk

class OptionDialog:
    def __init__(self, parent, options):
        self.parent = parent
        self.options = options
        self.selected_option = None
        self.dialog = tk.Toplevel(parent)
        self.dialog.title("Option Selection")
        
        self.create_buttons()

    def create_buttons(self):
        for option in self.options:
            button = tk.Button(self.dialog, text=option, command=lambda opt=option: self.on_button_click(opt))
            button.pack(padx=5, pady=5)

    def on_button_click(self, option):
        self.selected_option = option
        self.dialog.destroy()

    def get_selected_option(self):
        self.dialog.wait_window()
        return self.selected_option

def showOptionsDialog(options):
    root = tk.Tk()
    root.withdraw()  # Hide the root window
    dialog = OptionDialog(root, options)
    selected_option = dialog.get_selected_option()
    root.deiconify()  # Show the root window after the dialog is closed
    root.mainloop()
    return selected_option

# # Example usage:
# options_list = ["Option 1", "Option 2", "Option 3", "Option 4"]
# selected = open_option_dialog(options_list)
# print("Selected Option:", selected)