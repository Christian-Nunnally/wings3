from subprocess import Popen, PIPE, STDOUT
import tkinter as tk
import psutil
from Configuration.configuration import PhysicalLedIndexMappingWidth, PhysicalLedIndexMapping, PhysicalLedIndexMappingHeight

colors = {}
square_size = 30

def rgbToHex(r,g,b):
    return f'#{r:02x}{g:02x}{b:02x}'

def getColor(index):
    if index in colors: 
        return colors[index]
    return [0,0,0]

def setColor(index, red, green, blue):
    colors[index] = (red, green, blue)

def createSquares(canvas):
    for y in range(PhysicalLedIndexMappingHeight):
        for x in range(PhysicalLedIndexMappingWidth):
            ledIndex = PhysicalLedIndexMapping[y][x]
            if not ledIndex == -1:
                createSquare(x, y, ledIndex, canvas)

def createSquare(x, y, ledIndex, canvas):
    colorRgb = getColor(ledIndex)
    color = rgbToHex(colorRgb[0], colorRgb[1], colorRgb[2])
    x1 = x * square_size
    y1 = y * square_size
    x2 = x1 + square_size
    y2 = y1 + square_size
    canvas.create_rectangle(x1, y1, x2, y2, fill=color, outline="black")

def updateColors(canvas):
    canvas.delete("all")
    createSquares(canvas)

def onClosing():
    parent = psutil.Process(program.pid)
    for child in parent.children(recursive=True):
        child.kill()
        child.wait()
    program.kill()
    program.wait()
    root.destroy()

def runCommand(arguments, canvas):
    if arguments[0] == "setPixel":
        setColor(int(arguments[1]), int(arguments[2]), int(arguments[3]), int(arguments[4]))
    elif arguments[0] == "showFrame":
        updateColors(canvas)
    elif arguments[0] == "print":
        print(arguments[1])

def parseLine(line):
    try:
        preLine = str(line)[2:-5].strip()
        arguments = preLine.split(",")
        return arguments
    except Exception as e:
        print(repr(e))
        return []

def createRoot():
    root = tk.Tk()
    root.title("2D Led Grid Simulator")
    root.protocol("WM_DELETE_WINDOW", onClosing)
    return root

def createCanvas(root):
    canvasWidth = square_size * PhysicalLedIndexMappingWidth
    canvasHeight = square_size * PhysicalLedIndexMappingHeight
    canvas = tk.Canvas(root, width=canvasWidth, height=canvasHeight, bg="black")
    canvas.pack()
    return canvas

root = createRoot()
canvas = createCanvas(root)
program = Popen('bin\\wings.exe', stdout = PIPE, stderr = STDOUT, shell = True)

while True:
    root.update()
    line = program.stdout.readline()
    if not line: break
    arguments = parseLine(line)
    runCommand(arguments, canvas)
