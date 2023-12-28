import math
import sys
from src.build.configuration import PhysicalLedIndexMapping, TotalLedCount, PhysicalLedIndexMappingWidth, PhysicalLedIndexMappingHeight

outFile = open(sys.argv[1], 'w')

print("#ifndef TRANSFORM_MAPS_H", file=outFile)
print("#define TRANSFORM_MAPS_H", file=outFile)
print(file=outFile)
print("#include \"../commonHeaders.h\"", file=outFile)
print("#include \"../settings.h\"", file=outFile)
print(file=outFile)

globalMapList = []

def mapNumber(value, fromMin, fromMax, toMin, toMax):
    leftSpan = fromMax - fromMin
    rightSpan = toMax - toMin
    valueScaled = float(value - fromMin) / float(leftSpan)
    return toMin + (valueScaled * rightSpan)

def printMap(name, map):
    globalMapList.append(name)
    print(f"const static uint8_t {name}[{len(map)}] PROGMEM = ", end = "", file=outFile)
    print("{", end = "", file=outFile)
    for i in range(len(map)):
        end = ", " if i != len(map) - 1 else ""
        print(int(map[i]), end = end, file=outFile)
    print("};", file=outFile)

def printMaps(groupName, maps):
    printMap(f"{groupName}TransformMapX", maps[0])
    printMap(f"{groupName}TransformMapY", maps[1])
    printMap(f"{groupName}TopRadiusMap1", maps[2])
    printMap(f"{groupName}TopAngleMap1", maps[3])
    printMap(f"{groupName}TopRadiusMap2", maps[4])
    printMap(f"{groupName}TopAngleMap2", maps[5])
    printMap(f"{groupName}TopRadiusMap3", maps[6])
    printMap(f"{groupName}TopAngleMap3", maps[7])
    printMap(f"{groupName}TopRadiusMap4", maps[8])
    printMap(f"{groupName}TopAngleMap4", maps[9])
    printMap(f"{groupName}TopRadiusMap5", maps[10])
    printMap(f"{groupName}TopAngleMap5", maps[11])
    printMap(f"{groupName}TopRadiusMap6", maps[12])
    printMap(f"{groupName}TopAngleMap6", maps[13])
    print(file=outFile)

flippedXLayout = []
for y in range(PhysicalLedIndexMappingHeight):
    newRow = []
    for x in range(PhysicalLedIndexMappingWidth):
        if (x == 25 or ((x < 25 and (x % 4 == 0 or x % 4 == 1)) or (x > 25 and (x % 4 == 1 or x % 4 == 2)))):
            newRow.append(PhysicalLedIndexMapping[y][x])
        else:
            newRow.append(PhysicalLedIndexMapping[y][len(PhysicalLedIndexMapping[0]) - x - 1])
    flippedXLayout.append(newRow)

flippedYLayout = []
toggle = True
toggleToggle = False
index = 0
reversedPhysicalLedIndexMapping = list(PhysicalLedIndexMapping)
reversedPhysicalLedIndexMapping.reverse()
for y in range(PhysicalLedIndexMappingHeight // 2):
    if toggle:
        flippedYLayout.append(PhysicalLedIndexMapping[index])
    else:
        flippedYLayout.append(reversedPhysicalLedIndexMapping[index])
    if toggleToggle:
        toggle = not toggle
    toggleToggle = not toggleToggle
    index += 1

if PhysicalLedIndexMappingHeight % 2 == 1:
    flippedYLayout.append(PhysicalLedIndexMapping[PhysicalLedIndexMappingHeight // 2])
    index += 1

for y in range(PhysicalLedIndexMappingHeight // 2):
    if toggle:
        flippedYLayout.append(reversedPhysicalLedIndexMapping[index])
    else:
        flippedYLayout.append(PhysicalLedIndexMapping[index])
    if toggleToggle:
        toggle = not toggle
    toggleToggle = not toggleToggle
    index += 1

flippedXYLayout = []
toggle = True
toggleToggle = False
index = 0
reversedFlippedXLayout = list(flippedXLayout)
reversedFlippedXLayout.reverse()
for y in range(PhysicalLedIndexMappingHeight // 2):
    if toggle:
        flippedXYLayout.append(flippedXLayout[index])
    else:
        flippedXYLayout.append(reversedFlippedXLayout[index])
    if toggleToggle:
        toggle = not toggle
    toggleToggle = not toggleToggle
    index += 1

if PhysicalLedIndexMappingHeight % 2 == 1:
    flippedXYLayout.append(flippedXLayout[PhysicalLedIndexMappingHeight // 2])
    index += 1

for y in range(PhysicalLedIndexMappingHeight // 2):
    if toggle:
        flippedXYLayout.append(reversedFlippedXLayout[index])
    else:
        flippedXYLayout.append(flippedXLayout[index])
    if toggleToggle:
        toggle = not toggle
    toggleToggle = not toggleToggle
    index += 1

def computeMapsAcrossAllCenters(layout, totalWidth, totalHeight, centers, onlyPolar = False, shouldBeMirrored = False):
    maps = []
    printedXYMaps = False
    for center in centers:
        transformMapX, transformMapY, radiusMap, angleMap = computeMaps(layout, totalWidth, totalHeight, center[0], center[1])
        if (not printedXYMaps):
            printedXYMaps = True
            if not onlyPolar:
                if not shouldBeMirrored:
                    maps.append(transformMapX)
                    maps.append(transformMapY)
                else:
                    maps.append(mirrorTransformMap(transformMapX))
                    maps.append(mirrorTransformMap(transformMapY))
        if not shouldBeMirrored:
            maps.append(radiusMap)
            maps.append(angleMap)
        else:
            maps.append(mirrorTransformMap(radiusMap))
            maps.append(mirrorTransformMap(angleMap))
    return maps

def mirrorTransformMap(transformMapToMirror):
    mirroredTransformMap = []
    for _ in range(2):
        for i in range(128):
            mirroredTransformMap.append(transformMapToMirror[i])
    return mirroredTransformMap

def computeMaps(layout, totalWidth, totalHeight, centerX, centerY):
    ledsWide = 26.0 * 2 - 1
    ledsTall = 13.0 * 2 - 1
    mapRange = 255.0

    totalLeds = 0
    maxRadius = 0
    for y in range(len(layout)):
        totalLeds += len(layout[y]) - layout[y].count(-1)

    xMap = []
    yMap = []
    transformMapX = []
    transformMapY = []
    radiusMap = []
    angleMap = []

    for i in range(totalLeds):
        for y in range(len(layout)):
            for x in range(len(layout[0])):
                if (layout[y][x] == i):
                    xMap.append((totalWidth / (ledsWide - 1)) * x)
                    yMap.append((totalHeight / (ledsTall - 1)) * y)
                    mappedX = mapNumber(int(xMap[i]), 0, totalWidth, 0, mapRange)
                    mappedY = mapNumber(int(yMap[i]), 0, totalHeight, 0, mapRange)
                    transformMapX.append(mappedX)
                    transformMapY.append(mappedY)

    for i in range(totalLeds):
        for y in range(len(layout)):
            for x in range(len(layout[0])):
                if (layout[y][x] == i):
                    xPosition = (totalWidth * centerX) - xMap[i]
                    yPosition = (totalHeight * centerY) - yMap[i]
                    radius = int(math.sqrt(xPosition * xPosition + yPosition * yPosition))
                    maxRadius = radius if radius > maxRadius else maxRadius

    for i in range(totalLeds):
        for y in range(len(layout)):
            for x in range(len(layout[0])):
                if (layout[y][x] == i):
                    xPosition = int((totalWidth * centerX) - xMap[i])
                    yPosition = int((totalHeight * centerY) - yMap[i])
                    radius = math.sqrt(xPosition * xPosition + yPosition * yPosition)
                    maxRadius = radius if radius > maxRadius else maxRadius
                    angle = math.degrees(math.atan2(yPosition, xPosition)) + 180 if xPosition != 0 else 0
                    while (angle < 0):
                        angle += 360
                    while (angle > 360):
                        angle -= 360
                    mappedRadius = mapNumber(radius, 0.0, maxRadius, 0.0, mapRange)
                    mappedAngle = mapNumber(angle, 0.0, 360.0, 0.0, mapRange)
                    radiusMap.append(mappedRadius)
                    angleMap.append(mappedAngle)
    return transformMapX, transformMapY, radiusMap, angleMap

centerPositions = [
    [0.0, 0.0],
    [0.5, 0.0],
    [1.0, 0.0],
    [0.5, 0.5],
    [0.33, 0.5],
    [0.5, 1],
]

maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, 255.0, 127.0, centerPositions)
printMaps("normal", maps)

maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, 8.0, 4.0, centerPositions)
printMaps("normalLowResolution8x4", maps)

maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, 16.0, 8.0, centerPositions)
printMaps("normalLowResolution16x8", maps)

maps = computeMapsAcrossAllCenters(flippedXLayout, 255.0, 127.0, centerPositions)
printMaps("swappedX", maps)

maps = computeMapsAcrossAllCenters(flippedYLayout, 255.0, 127.0, centerPositions)
printMaps("swappedY", maps)

maps = computeMapsAcrossAllCenters(flippedXYLayout, 255.0, 127.0, centerPositions)
printMaps("swappedXY", maps)

print(f"const int transformMapsCount = {len(globalMapList)};", file=outFile)
print("const static uint8_t* transformMaps[transformMapsCount]", end=" PROGMEM = {", file=outFile)
print(*globalMapList, sep=", ", end=" };", file=outFile)
print(file=outFile)


maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, 255.0, 127.0, centerPositions, onlyPolar=False, shouldBeMirrored=True)
printMaps("normalMirrored", maps)

maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, 8.0, 4.0, centerPositions, onlyPolar=False, shouldBeMirrored=True)
printMaps("normalLowResolution8x4Mirrored", maps)

maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, 16.0, 8.0, centerPositions, onlyPolar=False, shouldBeMirrored=True)
printMaps("normalLowResolution16x8Mirrored", maps)

maps = computeMapsAcrossAllCenters(flippedXLayout, 255.0, 127.0, centerPositions, onlyPolar=False, shouldBeMirrored=True)
printMaps("swappedXMirrored", maps)

maps = computeMapsAcrossAllCenters(flippedYLayout, 255.0, 127.0, centerPositions, onlyPolar=False, shouldBeMirrored=True)
printMaps("swappedYMirrored", maps)

maps = computeMapsAcrossAllCenters(flippedXYLayout, 255.0, 127.0, centerPositions, onlyPolar=False, shouldBeMirrored=True)
printMaps("swappedXYMirrored", maps)

print(f"const int mirroredTransformMapsCount = {len(globalMapList)};", file=outFile)
print("const static uint8_t* mirroredTransformMaps[mirroredTransformMapsCount]", end=" PROGMEM = {", file=outFile)
print(*globalMapList, sep=", ", end=" };", file=outFile)
print(file=outFile)


centerPositions = []
for i in range(TotalLedCount):
    for y in range(PhysicalLedIndexMappingHeight):
        for x in range(PhysicalLedIndexMappingWidth):
            if (PhysicalLedIndexMapping[y][x] == i):
                if PhysicalLedIndexMapping[y][x] != -1:
                    xPos = x / (len(PhysicalLedIndexMapping[0]) - 1)
                    yPos = y / (len(PhysicalLedIndexMapping) - 1)
                    centerPositions.append([xPos, yPos])
maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, 255.0, 127.0, centerPositions, True)

print(file=outFile)
globalPixelRadiusMapList = []
globalPixelPolarMapList = []
mapIndex = 0
for mapIndex in range(int(len(maps) / 2)):
    printMap(f"radiusTransformMapForPixel{mapIndex}", maps[mapIndex * 2])
    printMap(f"angleTransformMapForPixel{mapIndex}", maps[(mapIndex * 2) + 1])
    globalPixelRadiusMapList.append(f"radiusTransformMapForPixel{mapIndex}")
    globalPixelPolarMapList.append(f"angleTransformMapForPixel{mapIndex}")

print(file=outFile)
print(f"const static int pixelRadiusTransformMapsCount = {len(globalPixelRadiusMapList)};", file=outFile)
print("const static uint8_t* pixelRadiusTransformMaps[pixelRadiusTransformMapsCount]", end=" = {", file=outFile)
print(*globalPixelRadiusMapList, sep=", ", end=" };", file=outFile)
print(file=outFile)
print(f"const static int pixelAngleTransformMapsCount = {len(globalPixelPolarMapList)};", file=outFile)
print("const static uint8_t* pixelAngleTransformMaps[pixelAngleTransformMapsCount]", end=" = {", file=outFile)
print(*globalPixelPolarMapList, sep=", ", end=" };", file=outFile)
print(file=outFile)
print(file=outFile)
print("#endif", file=outFile)