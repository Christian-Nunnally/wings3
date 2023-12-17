import tkinter as tk
from Configuration.configuration import PhysicalLedIndexMappingWidth, PhysicalLedIndexMapping, PhysicalLedIndexMappingHeight
from tests.SimpleWindow import SimpleWindow

colors = {}
square_size = 30

class LedGridWindow(SimpleWindow):
    canvas = None

    def __init__(self) -> None:
        self.createRoot()
        self.createGridSimulatorCanvas()

    def rgbToHex(self, red, green, blue):
        return f'#{red:02x}{green:02x}{blue:02x}'

    def getColor(self, index):
        if index in colors: 
            return colors[index]
        return [0,0,0]

    def setColor(self, index, red, green, blue):
        colors[index] = (red, green, blue)

    def createSquares(self):
        for y in range(PhysicalLedIndexMappingHeight):
            for x in range(PhysicalLedIndexMappingWidth):
                ledIndex = PhysicalLedIndexMapping[y][x]
                if not ledIndex == -1:
                    self.createSquare(x, y, ledIndex)

    def createSquare(self, x, y, ledIndex):
        colorRgb = self.getColor(ledIndex)
        color = self.rgbToHex(colorRgb[0], colorRgb[1], colorRgb[2])
        x1 = x * square_size
        y1 = y * square_size
        x2 = x1 + square_size
        y2 = y1 + square_size
        self.canvas.create_rectangle(x1, y1, x2, y2, fill=color, outline="black")

    def updateColors(self):
        self.canvas.delete("all")
        self.createSquares()

    def runCommand(self, arguments):
        if arguments[0] == "setPixel":
            if (len(arguments) == 5):
                self.setColor(int(arguments[1]), int(arguments[2]), int(arguments[3]), int(arguments[4]))
        elif arguments[0] == "showFrame":
            self.updateColors()

    def createRoot(self):
        super().createRoot()
        self.root.title("2D Led Grid Simulator")

    def createGridSimulatorCanvas(self):
        canvasWidth = square_size * PhysicalLedIndexMappingWidth
        canvasHeight = square_size * PhysicalLedIndexMappingHeight
        self.canvas = tk.Canvas(self.root, width=canvasWidth, height=canvasHeight, bg="black")
        self.canvas.pack()