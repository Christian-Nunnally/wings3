from PIL import Image

paletteWidth = 256
paletteHeights = []
numbersPerRow = 66
paletteStarts = []

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
        for i in range(paletteHeight):
            for j in range(paletteWidth):
                index = (i * paletteWidth) + j
                currentIndex += 1
                colors.append(pixels[index][0])
                colors.append(pixels[index][1])
                colors.append(pixels[index][2])
    return colors

def printPaletteCode(colors):
    print(f"const static uint8_t palette[{len(colors)}] PROGMEM = ")
    print("{")
    print("    ", end="")
    for i in range(len(colors)):

        print(f"{colors[i]:{' '}{'>'}{3}}", end = "")
        if (not i == len(colors) - 1):
            print(", ", end="")
        if (i % numbersPerRow == numbersPerRow - 1):
            print()
            print("    ", end="")

    print("};")
    print()
    print(f"const static int paletteStarts[{len(paletteStarts)}] = ")
    print("{")
    print("    ", end="")
    print(*paletteStarts, sep = ",") 
    print("};")
    print()
    print(f"const static int paletteHeights[{len(paletteHeights)}] = ")
    print("{")
    print("    ", end="")
    print(*paletteHeights, sep = ",") 
    print("};")

colors = createPaletteFromImages(['palette.bmp', 'palette2.bmp'])
print("#ifndef PALETTES_H")
print("#define PALETTES_H")
print()
print("#include <pgmspace.h>")
print()
print(f"#define TOTAL_NUMBER_OF_PALETTES {len(paletteHeights)}")
print(f"#define PALETTE_LENGTH {paletteWidth}")
print()
printPaletteCode(colors)
print()
print("#endif")