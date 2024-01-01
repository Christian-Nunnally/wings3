import math
import sys
from src.pipeline.configuration import PhysicalLedIndexMapping, TotalLedCount, PhysicalLedIndexMappingWidth, PhysicalLedIndexMappingHeight, VirtualCanvasWidth, VirtualCanvasHeight 

options = ["normal", "mirrored", "perPixelAngle", "perPixelRadius"]

def error(text):
    print(text)
    print()
    print("Example usage: 'python transformMapsHeaderGenerator.py outputFile.h perPixelAngle'")
    exit(1)

def validateArgumentsAndOpenFile():
    if len(sys.argv) < 3:
        error(f"Error: Not enough arguments (expected 3, got {len(sys.argv)}): {sys.argv}")
    elif len(sys.argv) > 3:
        error(f"Error: Too many arguments (expected 3, got {len(sys.argv)}): {sys.argv}")
    elif sys.argv[2] not in options:
        error(f"Error: Expected second argument to be one of {options} (got '{sys.argv[2]}')")
    try:
        return open(sys.argv[1], 'w')
    except Exception as e:
        print(e)
        error(f"Error: Could not open file at location {sys.argv[1]} for writing.")

def printCommonHeader(name):
    print(f"#ifndef {name.upper()}_TRANSFORM_MAPS_H", file=outFile)
    print(f"#define {name.upper()}_TRANSFORM_MAPS_H", file=outFile)
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


def flipAndInterleaveAlongXAxis(arr):
    arrayFlippedAlongXAxis = arr[::-1]
    interleaved = []
    for originalRow, flippedRow in zip(arr, arrayFlippedAlongXAxis):
        interleaved.append(originalRow)
        interleaved.append(flippedRow)
    half_length = len(interleaved) // 2
    return interleaved[:half_length]

def flipAndInterleaveAlongYAxis(original2dArray):
    arrayFlippedAlongYAxis = [row[::-1] for row in original2dArray]
    transposedFlippedArray = [list(row) for row in zip(*arrayFlippedAlongYAxis)]
    interleaved = []
    for originalColumn, flippedColumn in zip(zip(*original2dArray), transposedFlippedArray):
        interleaved.extend([list(originalColumn), flippedColumn])
    interleavedAlongYAxis = list(map(list, zip(*interleaved)))
    interleavedAlongYAxisHalf = []
    for line in interleavedAlongYAxis:
        currentLine = []
        for i in range(len(line) // 2):
            currentLine.append(line[i])
        interleavedAlongYAxisHalf.append(currentLine)
    return interleavedAlongYAxisHalf

outFile = validateArgumentsAndOpenFile()
flippedXLayout = flipAndInterleaveAlongXAxis(PhysicalLedIndexMapping)
flippedYLayout = flipAndInterleaveAlongYAxis(PhysicalLedIndexMapping)

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
        foundLed = False
        for y in range(len(layout)):
            for x in range(len(layout[0])):
                if (layout[y][x] == i):
                    xMap.append((totalWidth / (ledsWide - 1)) * x)
                    yMap.append((totalHeight / (ledsTall - 1)) * y)
                    mappedX = mapNumber(int(xMap[i]), 0, totalWidth, 0, mapRange)
                    mappedY = mapNumber(int(yMap[i]), 0, totalHeight, 0, mapRange)
                    transformMapX.append(mappedX)
                    transformMapY.append(mappedY)
                    foundLed = True
                if foundLed: 
                    break
            if foundLed: 
                break

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

printCommonHeader(sys.argv[2])

if (sys.argv[2] == "normal"):
    globalMapList = []
    maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, VirtualCanvasWidth, VirtualCanvasHeight, centerPositions)
    printMaps("normal", maps, outFile, globalMapList)

    lowResolutionGridAmount = 32
    if (VirtualCanvasWidth // lowResolutionGridAmount > 2 and VirtualCanvasHeight // lowResolutionGridAmount > 2):
        maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, VirtualCanvasWidth // lowResolutionGridAmount, VirtualCanvasHeight // lowResolutionGridAmount, centerPositions)
        printMaps("normalLowResolution8x4", maps, outFile, globalMapList)
    
    lowResolutionGridAmount = 16
    if (VirtualCanvasWidth // lowResolutionGridAmount > 2 and VirtualCanvasHeight // lowResolutionGridAmount > 2):
        maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, VirtualCanvasWidth // lowResolutionGridAmount, VirtualCanvasHeight // lowResolutionGridAmount, centerPositions)
        printMaps("normalLowResolution16x8", maps, outFile, globalMapList)

    maps = computeMapsAcrossAllCenters(flippedXLayout, VirtualCanvasWidth, VirtualCanvasHeight, centerPositions)
    printMaps("swappedX", maps, outFile, globalMapList)

    maps = computeMapsAcrossAllCenters(flippedYLayout, VirtualCanvasWidth, VirtualCanvasHeight, centerPositions)
    printMaps("swappedY", maps, outFile, globalMapList)

    maps = computeMapsAcrossAllCenters(flippedXYLayout, VirtualCanvasWidth, VirtualCanvasHeight, centerPositions)
    printMaps("swappedXY", maps, outFile, globalMapList)

    print(f"const int transformMapsCount = {len(globalMapList)};", file=outFile)
    print("const static uint8_t* transformMaps[transformMapsCount]", end=" PROGMEM = {", file=outFile)
    print(*globalMapList, sep=", ", end=" };", file=outFile)

elif (sys.argv[2] == "mirrored"):
    globalMapList=[]
    maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, 255.0, 127.0, centerPositions, onlyPolar=False, shouldBeMirrored=True)
    printMaps("normalMirrored", maps, outFile, globalMapList)

    lowResolutionGridAmount = 32
    if (VirtualCanvasWidth // lowResolutionGridAmount > 2 and VirtualCanvasHeight // lowResolutionGridAmount > 2):
        maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, VirtualCanvasWidth // lowResolutionGridAmount, VirtualCanvasHeight // lowResolutionGridAmount, centerPositions, onlyPolar=False, shouldBeMirrored=True)
        printMaps("normalLowResolution8x4Mirrored", maps, outFile, globalMapList)
    
    lowResolutionGridAmount = 16
    if (VirtualCanvasWidth // lowResolutionGridAmount > 2 and VirtualCanvasHeight // lowResolutionGridAmount > 2):
        maps = computeMapsAcrossAllCenters(PhysicalLedIndexMapping, VirtualCanvasWidth // lowResolutionGridAmount, VirtualCanvasHeight // lowResolutionGridAmount, centerPositions, onlyPolar=False, shouldBeMirrored=True)
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

elif sys.argv[2] == "perPixelRadius":
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