import os
from subprocess import Popen, PIPE, STDOUT
import tkinter as tk
import random
import time

num_cols = 27
num_rows = 13
colors = {}
square_size = 20

def rgbToHex(r,g,b):
    return f'#{r:02x}{g:02x}{b:02x}'

def getColor(index):
    if index in colors: 
        return colors[index]
    return [0,0,0]

def setColor(index, red, green, blue):
    colors[index] = (red, green, blue)

def create_squares():
    index = 0
    for i in range(num_rows):
        for j in range(num_cols):
            colorRgb = getColor(index)
            color = rgbToHex(colorRgb[0], colorRgb[1], colorRgb[2])
            x1 = j * square_size
            y1 = i * square_size
            x2 = x1 + square_size
            y2 = y1 + square_size
            canvas.create_rectangle(x1, y1, x2, y2, fill=color, outline="black")
            index += 1

def update_colors():
    canvas.delete("all")
    create_squares()

root = tk.Tk()
root.title("Dynamic Colored Squares")

canvas = tk.Canvas(root, width=600, height=300, bg="white")
canvas.pack()

os.system("make")
p = Popen('a.exe', stdout = PIPE, 
        stderr = PIPE, shell = True)

while True:
    root.update()
    line = p.stdout.readline()
    if not line: break
    #print(line)
    try:
        preLine = str(line)[2:-5].strip()
        splitLine = preLine.split(",")
        if splitLine[0] == "setPixel":
            setColor(int(splitLine[1]), int(splitLine[2]), int(splitLine[3]), int(splitLine[4]))
            #print("Setting pixel")
        elif splitLine[0] == "showFrame":
            try:
                update_colors()
            except:
                break
        elif splitLine[0] == "print":
            print(splitLine[1])
    except:
        pass

p.terminate()