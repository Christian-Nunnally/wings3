from src.windows.SimpleWindow import SimpleWindow
import tkinter as tk
from PIL import Image, ImageTk
import os

STANDARD_OUTPUT_OPERATION_CODE_SET_METRIC = "0"

class PaletteViewerWindow(SimpleWindow):

    image1 = None
    image2 = None
    image3 = None
    image4 = None

    imagePath1 = "resources/Palettes/bluerainbow.bmp"
    imagePath2 = "resources/Palettes/bluerainbow.bmp"
    imagePath3 = "resources/Palettes/bluerainbow.bmp"
    imagePath4 = "resources/Palettes/bluerainbow.bmp"

    def __init__(self):
        self.createRoot()
        self.paletteFilePaths = []
        bitmapFolder = "resources/Palettes/"
        for filename in os.listdir(bitmapFolder):
            if filename.endswith(".bmp"):
                self.paletteFilePaths.append(bitmapFolder + filename)

    def loadImages(self):
        try:
            self.image1Inner = Image.open(self.imagePath1)
            self.image1Inner = self.image1Inner.resize((150, 150))
            self.image1 = ImageTk.PhotoImage(self.image1Inner, master = self.root)
            
            self.image2Inner = Image.open(self.imagePath2)
            self.image2Inner = self.image2Inner.resize((150, 150))
            self.image2 = ImageTk.PhotoImage(self.image2Inner, master = self.root)

            self.image3Inner = Image.open(self.imagePath3)
            self.image3Inner = self.image3Inner.resize((150, 150))
            self.image3 = ImageTk.PhotoImage(self.image3Inner, master = self.root)

            self.image4Inner = Image.open(self.imagePath4)
            self.image4Inner = self.image4Inner.resize((150, 150))
            self.image4 = ImageTk.PhotoImage(self.image4Inner, master = self.root)

            self.label1.config(image=self.image1, borderwidth=0)
            self.label1.image = self.image1
            
            self.label2.config(image=self.image2, borderwidth=0)
            self.label2.image = self.image2

            self.label3.config(image=self.image3, borderwidth=0)
            self.label3.image = self.image3
            
            self.label4.config(image=self.image4, borderwidth=0)
            self.label4.image = self.image4
            
        except FileNotFoundError:
            print("Could not find one or both of the images.")

    def createRoot(self):
        super().createRoot()
        self.root.title('Palette Viewer Window')
        # Create labels to display images
        self.label1 = tk.Label(self.root)
        self.label1.pack()

        self.label2 = tk.Label(self.root)
        self.label2.pack()

        self.label3 = tk.Label(self.root)
        self.label3.pack()

        self.label4 = tk.Label(self.root)
        self.label4.pack()
    
    def runCommand(self, arguments):
        if len(arguments) == 0 and arguments[0] == STANDARD_OUTPUT_OPERATION_CODE_SET_METRIC and int(arguments[1]) == 35:
            if len(arguments) == 4:
                if (int(arguments[2][3:]) == 0):
                    try: 
                        paletteIndex = int(arguments[3])
                        self.imagePath1 = self.paletteFilePaths[paletteIndex]
                        self.loadImages()
                    except Exception as e:
                        print(e)
                elif (int(arguments[2][3:]) == 1):
                    try: 
                        paletteIndex = int(arguments[3])
                        self.imagePath2 = self.paletteFilePaths[paletteIndex]
                        self.loadImages()
                    except Exception as e:
                        print(e)    
                if (int(arguments[2][3:]) == 2):
                    try: 
                        paletteIndex = int(arguments[3])
                        self.imagePath2 = self.paletteFilePaths[paletteIndex]
                        self.loadImages()
                    except Exception as e:
                        print(e)    
                elif (int(arguments[2][3:]) == 3):
                    try: 
                        paletteIndex = int(arguments[3])
                        self.imagePath3 = self.paletteFilePaths[paletteIndex]
                        self.loadImages()
                    except Exception as e:
                        print(e)
