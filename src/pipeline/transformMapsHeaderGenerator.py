import math
import sys
from src.pipeline.configuration import PhysicalLedIndexMapping, TotalLedCount, PhysicalLedIndexMappingWidth, PhysicalLedIndexMappingHeight

def error(text):
    print(text)
    print()
    print("Example usage: 'python transformMapsHeaderGenerator.py outputFile.h perPixelAngle'")
    exit(1)

def validateArgumentsAndOpenFile():
    if len(sys.argv) < 3:
        error(f"Error: Not enough arguments (expected 3, got {len(sys.argv)}): {sys.argv}")
        error = True
    elif len(sys.argv) > 3:
        error(f"Error: Too many arguments (expected 3, got {len(sys.argv)}): {sys.argv}")
    elif sys.argv[2] != "perPixelAngle" and sys.argv[2] != "perPixelRadius" and sys.argv[2] != "normal":
        error(f"Error: Expected second argument to be perPixelAngle/perPixelRadius/normal (got '{sys.argv[2]}')")
    try:
        return open(sys.argv[1], 'w')
    except:
        error(f"Error: Could not open file at location {sys.argv[1]} for writing.")

def printCommonHeader(name):
    print(f"#ifndef {name}_H", file=outFile)
    print(f"#define {name}_H", file=outFile)
    print(file=outFile)
    print("#include \"../commonHeaders.h\"", file=outFile)
    print("#include \"../settings.h\"", file=outFile)
    print(file=outFile)

def printCommonFooter():
    print(file=outFile)
    print(file=outFile)
    print("#endif", file=outFile)

def mapNumber(value, fromMin, fromMax, toMin, toMax):
    leftSpan = fromMax - fromMin
    rightSpan = toMax - toMin
    valueScaled = float(value - fromMin) / float(leftSpan)
    return toMin + (valueScaled * rightSpan)

def printMap(name, map, outFile, mapNameList):
    mapNameList.append(name)
    print(f"const static uint8_t {name}[{len(map)}] PROGMEM = ", end = "", file=outFile)
    print("{", end = "", file=outFile)
    for i in range(len(map)):
        end = ", " if i != len(map) - 1 else ""
        print(int(map[i]), end = end, file=outFile)
    print("};", file=outFile)

def printMaps(groupName, maps, outFile, mapNameList):
    printMap(f"{groupName}TransformMapX", maps[0], outFile, mapNameList)
    printMap(f"{groupName}TransformMapY", maps[1], outFile, mapNameList)
    printMap(f"{groupName}TopRadiusMap1", maps[2], outFile, mapNameList)
    printMap(f"{groupName}TopAngleMap1", maps[3], outFile, mapNameList)
    printMap(f"{groupName}TopRadiusMap2", maps[4], outFile, mapNameList)
    printMap(f"{groupName}TopAngleMap2", maps[5], outFile, mapNameList)
    printMap(f"{groupName}TopRadiusMap3", maps[6], outFile, mapNameList)
    printMap(f"{groupName}TopAngleMap3", maps[7], outFile, mapNameList)
    printMap(f"{groupName}TopRadiusMap4", maps[8], outFile, mapNameList)
    printMap(f"{groupName}TopAngleMap4", maps[9], outFile, mapNameList)
    printMap(f"{groupName}TopRadiusMap5", maps[10], outFile, mapNameList)
    printMap(f"{groupName}TopAngleMap5", maps[11], outFile, mapNameList)
    printMap(f"{groupName}TopRadiusMap6", maps[12], outFile, mapNameList)
    printMap(f"{groupName}TopAngleMap6", maps[13], outFile, mapNameList)
    print(file=outFile)

outFile = validateArgumentsAndOpenFile()

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
        for i in range(TotalLedCount // 2):
            mirroredTransformMap.append(transformMapToMirror[i])
    return mirroredTransformMap

def computeMaps(layout, totalWidth, totalHeight, centerX, centerY):
    ledsWide = PhysicalLedIndexMappingWidth
    ledsTall = PhysicalLedIndexMappingHeight
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

def computeCenterOfAllPixels():
    centerPositions = []
    for i in range(TotalLedCount):
        for y in range(PhysicalLedIndexMappingHeight):
            for x in range(PhysicalLedIndexMappingWidth):
                if (PhysicalLedIndexMapping[y][x] == i):
                    if PhysicalLedIndexMapping[y][x] != -1:
                        xPos = x / (len(PhysicalLedIndexMapping[0]) - 1)
                        yPos = y / (len(PhysicalLedIndexMapping) - 1)
                        centerPositions.append([xPos, yPos])
    return centerPositions

centerPositions = [
    [0.0, 0.0],
    [0.5, 0.0],
    [1.0, 0.0],
    [0.5, 0.5],
    [0.33, 0.5],
    [0.5, 1],
]

if (sys.argv[2] == "normal"):
    printCommonHeader("NORMAL_TRANSFORM_MAPS")

    globalMapList = []
    maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, 255.0, 127.0, centerPositions)
    printMaps("normal", maps, outFile, globalMapList)

    maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, 8.0, 4.0, centerPositions)
    printMaps("normalLowResolution8x4", maps, outFile, globalMapList)

    maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, 16.0, 8.0, centerPositions)
    printMaps("normalLowResolution16x8", maps, outFile, globalMapList)

    maps = computeMapsAcrossAllCenters(flippedXLayout, 255.0, 127.0, centerPositions)
    printMaps("swappedX", maps, outFile, globalMapList)

    maps = computeMapsAcrossAllCenters(flippedYLayout, 255.0, 127.0, centerPositions)
    printMaps("swappedY", maps, outFile, globalMapList)

    maps = computeMapsAcrossAllCenters(flippedXYLayout, 255.0, 127.0, centerPositions)
    printMaps("swappedXY", maps, outFile, globalMapList)

    print(f"const int transformMapsCount = {len(globalMapList)};", file=outFile)
    print("const static uint8_t* transformMaps[transformMapsCount]", end=" PROGMEM = {", file=outFile)
    print(*globalMapList, sep=", ", end=" };", file=outFile)
    print(file=outFile)
    print(file=outFile)
    print(file=outFile)

    globalMapList=[]
    maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, 255.0, 127.0, centerPositions, onlyPolar=False, shouldBeMirrored=True)
    printMaps("normalMirrored", maps, outFile, globalMapList)

    maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, 8.0, 4.0, centerPositions, onlyPolar=False, shouldBeMirrored=True)
    printMaps("normalLowResolution8x4Mirrored", maps, outFile, globalMapList)

    maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, 16.0, 8.0, centerPositions, onlyPolar=False, shouldBeMirrored=True)
    printMaps("normalLowResolution16x8Mirrored", maps, outFile, globalMapList)

    maps = computeMapsAcrossAllCenters(flippedXLayout, 255.0, 127.0, centerPositions, onlyPolar=False, shouldBeMirrored=True)
    printMaps("swappedXMirrored", maps, outFile, globalMapList)

    maps = computeMapsAcrossAllCenters(flippedYLayout, 255.0, 127.0, centerPositions, onlyPolar=False, shouldBeMirrored=True)
    printMaps("swappedYMirrored", maps, outFile, globalMapList)

    maps = computeMapsAcrossAllCenters(flippedXYLayout, 255.0, 127.0, centerPositions, onlyPolar=False, shouldBeMirrored=True)
    printMaps("swappedXYMirrored", maps, outFile, globalMapList)

    print(f"const int mirroredTransformMapsCount = {len(globalMapList)};", file=outFile)
    print("const static uint8_t* mirroredTransformMaps[mirroredTransformMapsCount]", end=" PROGMEM = {", file=outFile)
    print(*globalMapList, sep=", ", end=" };", file=outFile)
    print(file=outFile)

elif sys.argv[2] == "perPixelRadius":
    printCommonHeader("PER_PIXEL_RADIUS_TRANSFORM_MAPS")
    centerPositions = computeCenterOfAllPixels()
    maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, 255.0, 127.0, centerPositions, True)
    print(file=outFile)
    globalPixelRadiusMapList = []
    mapIndex = 0
    for mapIndex in range(int(len(maps) / 2)):
        printMap(f"radiusTransformMapForPixel{mapIndex}", maps[mapIndex * 2], outFile, [])
        globalPixelRadiusMapList.append(f"radiusTransformMapForPixel{mapIndex}")
    print(file=outFile)
    print(f"const static int pixelRadiusTransformMapsCount = {len(globalPixelRadiusMapList)};", file=outFile)
    print("const static uint8_t* pixelRadiusTransformMaps[pixelRadiusTransformMapsCount]", end=" = {", file=outFile)
    print(*globalPixelRadiusMapList, sep=", ", end=" };", file=outFile)

elif sys.argv[2] == "perPixelAngle":
    printCommonHeader("PER_PIXEL_ANGLE_TRANSFORM_MAPS")
    centerPositions = computeCenterOfAllPixels()
    maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, 255.0, 127.0, centerPositions, True)
    print(file=outFile)
    globalPixelAngleMapList = []
    mapIndex = 0
    for mapIndex in range(int(len(maps) / 2)):
        printMap(f"angleTransformMapForPixel{mapIndex}", maps[(mapIndex * 2) + 1], outFile, [])
        globalPixelAngleMapList.append(f"angleTransformMapForPixel{mapIndex}")
    print(file=outFile)
    print(f"const static int pixelAngleTransformMapsCount = {len(globalPixelAngleMapList)};", file=outFile)
    print("const static uint8_t* pixelAngleTransformMaps[pixelAngleTransformMapsCount]", end=" = {", file=outFile)
    print(*globalPixelAngleMapList, sep=", ", end=" };", file=outFile)

printCommonFooter()