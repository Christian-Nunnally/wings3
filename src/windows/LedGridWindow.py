import tkinter as tk
from src.pipeline.configuration import PhysicalLedIndexMappingWidth, PhysicalLedIndexMapping, PhysicalLedIndexMappingHeight
from src.windows.SimpleWindow import SimpleWindow

colors = {}
square_size = 52

#STANDARD_OUTPUT_OPERATION_CODE_SET_METRIC = 0
STANDARD_OUTPUT_OPERATION_CODE_SET_LED = 1
STANDARD_OUTPUT_OPERATION_CODE_SHOW_LEDS = 2
STANDARD_OUTPUT_OPERATION_CODE_CLEAR_LEDS = 3


class LedGridWindow(SimpleWindow):
    canvas = None

    def __init__(self) -> None:
        self.createRoot()
        self.createGridSimulatorCanvas()
        self.center()
        self.squares = {}

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
                    self.updateOrCreateSquare(x, y, ledIndex)

    def updateOrCreateSquare(self, x, y, ledIndex):
        colorRgb = self.getColor(ledIndex)
        color = self.rgbToHex(colorRgb[0], colorRgb[1], colorRgb[2])
        if ledIndex in self.squares:
            self.canvas.itemconfig(self.squares[ledIndex], fill=color)
        else:
            x1 = x * square_size
            y1 = y * square_size
            x2 = x1 + square_size
            y2 = y1 + square_size
            self.squares[ledIndex] = self.canvas.create_rectangle(x1, y1, x2, y2, fill=color, outline="black")

    def updateColors(self):
        self.createSquares()

    def runCommand(self, arguments):
        try:
            commandOperationCode = int(arguments[0])
            if commandOperationCode == STANDARD_OUTPUT_OPERATION_CODE_SET_LED:
                if len(arguments) == 3:
                    color = int(arguments[2], 16)
                    self.setColor(int(arguments[1], 16), (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF)
            elif commandOperationCode == STANDARD_OUTPUT_OPERATION_CODE_SHOW_LEDS:
                self.updateColors()
            elif commandOperationCode == STANDARD_OUTPUT_OPERATION_CODE_CLEAR_LEDS:
                colors.clear()
                self.updateColors()
        except Exception as e:
            print(e)
            print("ERROR Invalid arguments: ")
            print(arguments)

    def createRoot(self):
        super().createRoot()
        self.root.title("2D Led Grid Simulator")

    def createGridSimulatorCanvas(self):
        canvasWidth = square_size * PhysicalLedIndexMappingWidth
        canvasHeight = square_size * PhysicalLedIndexMappingHeight
        self.canvas = tk.Canvas(self.root, width=canvasWidth, height=canvasHeight, bg="black", borderwidth=0, highlightthickness=0)
        self.canvas.pack()