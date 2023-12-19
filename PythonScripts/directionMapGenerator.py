import sys
from configuration import PhysicalLedIndexMapping, TotalLedCount, PhysicalLedIndexMappingWidth, PhysicalLedIndexMappingHeight

outFile = open(sys.argv[1], 'w')

print("#ifndef DIRECTION_MAPS_H", file=outFile)
print("#define DIRECTION_MAPS_H", file=outFile)
print(file=outFile)
print("#include \"..\commonHeaders.h\"", file=outFile)
print("#include \"..\settings.h\"", file=outFile)
print(file=outFile)


# List out the index's of leds considered to be on the 'bottom' of the scene:
bottomLeds = [
    12, 14, 17, 19, 21, 23, 24, 26, 27, 29, 30, 32, 33,
    34, 36, 37, 38, 39, 40, 42, 43, 179, 178, 176,
    175, 174, 173, 172, 170, 169, 168, 166, 165,
    163, 162, 160, 159, 157, 155, 153, 150, 148
    ]

northernLedMap = []
for i in range(TotalLedCount):
    for y in range(PhysicalLedIndexMappingHeight):
        for x in range(PhysicalLedIndexMappingWidth):
            if PhysicalLedIndexMapping[y][x] == i:
                if y == 0:
                    northernLedMap.append(TotalLedCount)
                else:
                    for y2 in reversed(range(y)):
                        if not PhysicalLedIndexMapping[y2][x] == -1:
                            northernLedMap.append(PhysicalLedIndexMapping[y2][x])
                            break
                        elif y2 == 0:
                            northernLedMap.append(TotalLedCount)

print("const static uint16_t northernLedMap[TOTAL_LEDS]", end=" = {", file=outFile)
print(*northernLedMap, sep=", ", end=" };", file=outFile)
print(file=outFile)

southernLedMap = []
for i in range(TotalLedCount):
    for y in range(PhysicalLedIndexMappingHeight):
        for x in range(PhysicalLedIndexMappingWidth):
            if PhysicalLedIndexMapping[y][x] == i:
                if y == PhysicalLedIndexMappingHeight - 1:
                    southernLedMap.append(TotalLedCount)
                else:
                    for y2 in range(y + 1, PhysicalLedIndexMappingHeight):
                        if not PhysicalLedIndexMapping[y2][x] == -1:
                            southernLedMap.append(PhysicalLedIndexMapping[y2][x])
                            break
                        elif y2 == PhysicalLedIndexMappingHeight - 1:
                            southernLedMap.append(TotalLedCount)

print("const static uint16_t southernLedMap[TOTAL_LEDS]", end=" = {", file=outFile)
print(*southernLedMap, sep=", ", end=" };", file=outFile)
print(file=outFile)

def generateIsBottomLedMap():
    isBottomLed = []
    for i in range(TotalLedCount):
        if i in bottomLeds:
            if bottomLeds.index(i) < len(bottomLeds) / 2:
                isBottomLed.append(bottomLeds[bottomLeds.index(i) + 1])
            else:
                isBottomLed.append(bottomLeds[bottomLeds.index(i) - 1])
        else:
            isBottomLed.append(-1)
    return isBottomLed

print("const static int isBottomLed[TOTAL_LEDS]", end=" = {", file=outFile)
print(*generateIsBottomLedMap(), sep=", ", end=" };", file=outFile)
print(file=outFile)
print(file=outFile)
print("#endif", file=outFile)