from PIL import Image
import os
import sys

paletteWidth = 256
paletteHeights = []
numbersPerRow = 66
paletteStarts = []
paletteNames = []

def createPaletteFromImages(paletteFileNames):
    colors = []
    currentIndex = 0
    for paletteFileName in paletteFileNames:
        image = Image.open(paletteFileName, 'r')
        pixels = list(image.getdata())
        pixelCount = len(pixels)
        paletteHeight = int(pixelCount / paletteWidth)
        paletteStarts.append(currentIndex)
        paletteHeights.append(paletteHeight)
        paletteNames.append(paletteFileName)
        for i in range(paletteHeight):
            for j in range(paletteWidth):
                index = (i * paletteWidth) + j
                currentIndex += 1
                colors.append(pixels[index][0])
                colors.append(pixels[index][1])
                colors.append(pixels[index][2])
    return colors

def printPaletteCode(colors):
    print(f"const static uint8_t palette[{len(colors)}] PROGMEM = ", file=outFile)
    print("{", file=outFile)
    print("    ", end="", file=outFile)
    for i in range(len(colors)):

        print(f"{colors[i]}", end = "", file=outFile)
        if (not i == len(colors) - 1):
            print(", ", end="", file=outFile)
        if (i % numbersPerRow == numbersPerRow - 1):
            print(file=outFile)
            print("    ", end="", file=outFile)

    print("};", file=outFile)
    print(file=outFile)
    print(f"const static int paletteStarts[{len(paletteStarts)}] = ", file=outFile)
    print("{", file=outFile)
    print("    ", end="", file=outFile)
    print(*paletteStarts, sep = ",", file=outFile)
    print("};", file=outFile)
    print(file=outFile)
    print(f"const static int paletteHeights[{len(paletteHeights)}] = ", file=outFile)
    print("{", file=outFile)
    print("    ", end="", file=outFile)
    print(*paletteHeights, sep = ",", file=outFile)
    print("};", file=outFile)

if not len(sys.argv) == 3:
    print("Error: Must provide a folder name with bitmaps inside and the path the the output file.")
bitmapFolder = sys.argv[1]
bitmapFiles = []
for filename in os.listdir(bitmapFolder):
    if filename.endswith(".bmp"):
        bitmapFiles.append(bitmapFolder + filename)

outFile = open(sys.argv[2], 'w')
colors = createPaletteFromImages(bitmapFiles)

print("#ifndef PALETTES_H", file=outFile)
print("#define PALETTES_H", file=outFile)
print(file=outFile)
print("#include \"../commonHeaders.h\"", file=outFile)
print("#include \"../Graphics/color.h\"", file=outFile)
print(file=outFile)
print(f"#define TOTAL_NUMBER_OF_PALETTES {len(paletteHeights)}", file=outFile)
print(f"#define PALETTE_LENGTH {paletteWidth}", file=outFile)
print(file=outFile)
print("Color colorFromPalette(int index, uint8_t brightness);", file=outFile)
print(file=outFile)
printPaletteCode(colors)
print(file=outFile)
print("#endif", file=outFile)