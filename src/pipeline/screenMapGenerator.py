from PIL import Image
import os
import sys
from configuration import PhysicalLedIndexMapping, TotalLedCount, PhysicalLedIndexMappingWidth, PhysicalLedIndexMappingHeight

outFile = open(sys.argv[2], 'w')
print(file=outFile)
print("#ifndef SCREEN_MAPS_H",file=outFile)
print("#define SCREEN_MAPS_H",file=outFile)
print(file=outFile)

bitmapFolder = sys.argv[1]
bitmapFiles = []
bitmapFileNames = []
for filename in os.listdir(bitmapFolder):
    if filename.endswith(".bmp"):
        bitmapFiles.append(bitmapFolder + filename)
        bitmapFileNames.append(filename.split(".")[0])
counter = 0

screenMapNames = []
print("#include \"../commonHeaders.h\"", file=outFile)
print("#include \"../settings.h\"", file=outFile)
print(file=outFile)
for fileName in bitmapFiles:
    image = Image.open(fileName, 'r')
    pixels = list(image.getdata())
    if image.palette is None:
        image = image.convert('RGB')
    
    screenMap = []
    for i in range(TotalLedCount):
        for x in range(PhysicalLedIndexMappingHeight):
            for y in range(PhysicalLedIndexMappingWidth):
                if (PhysicalLedIndexMapping[x][y] == i):
                    RGB = image.getpixel((y,x))
                    R = 0
                    G = 0
                    B = 0
                    if image.palette is None:
                        R = RGB[0]
                        G = RGB[1]
                        B = RGB[2]
                        pass
                    else:
                        R = image.palette.tobytes()[RGB * 3 + 0]
                        G = image.palette.tobytes()[RGB * 3 + 1]
                        B = image.palette.tobytes()[RGB * 3 + 2]
                    screenValue = 0 if R > 128 and G > 128 and B > 128 else 0
                    screenValue = 1 if R < 128 and G > 128 and B < 128 else screenValue
                    screenValue = 2 if R < 128 and G < 128 and B > 128 else screenValue
                    screenValue = 3 if R > 128 and G < 128 and B < 128 else screenValue
                    screenValue = 4 if R < 128 and G < 128 and B < 128 else screenValue
                    screenMap.append(screenValue)
    print("uint8_t ", end="",file=outFile)
    screenMapName = bitmapFileNames[counter] + "ScreenMap"
    screenMapNames.append(screenMapName)
    print(screenMapName, end="",file=outFile)
    print("[TOTAL_LEDS]", end=" = {",file=outFile)
    print(*screenMap, sep = ",", end="",file=outFile)
    print("};",file=outFile)
    counter += 1
print(file=outFile)
print(f"const int screenMapsCount = {len(screenMapNames)};",file=outFile)
print("uint8_t *screenMaps[screenMapsCount]", end=" = {",file=outFile)
print(*screenMapNames, sep=", ", end=" };",file=outFile)

print(file=outFile)
print(file=outFile)
print("#endif",file=outFile)
