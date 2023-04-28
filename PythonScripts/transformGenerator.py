import math

print("#ifndef TRANSFORM_MAPS_H")
print("#define TRANSFORM_MAPS_H")
print()
print("#include <Adafruit_NeoPXL8.h>")
print("#include \"settings.h\"")
print()

globalMapList = []

def mapNumber(value, fromMin, fromMax, toMin, toMax):
    leftSpan = fromMax - fromMin
    rightSpan = toMax - toMin
    valueScaled = float(value - fromMin) / float(leftSpan)
    return toMin + (valueScaled * rightSpan)

def printMap(name, map):
    globalMapList.append(name)
    print(f"const static uint8_t {name}[{len(map)}] PROGMEM = ", end = "")
    print("{", end = "")
    for i in range(len(map)):
        end = ", " if i != len(map) - 1 else ""
        print(int(map[i]), end = end)
    print("};")

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
    print()

originalLayout = [
    [  11, -1, 10, -1,  9, -1,  8, -1,  7, -1,  6, -1,  5, -1,  4, -1,  3, -1,  2, -1,  1, -1,  0, -1, -1, -1, -1, -1,136, -1,137, -1,138, -1,139, -1,140, -1,141, -1,142, -1,143, -1,144, -1,145, -1,146, -1,147],
    [  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1],
    [  12, -1, 13, -1, 64, -1, 63, -1, 62, -1, 61, -1, 60, -1, 59, -1, 58, -1, 57, -1, 56, -1, 55, -1, 54, -1,190, -1,191, -1,192, -1,193, -1,194, -1,195, -1,196, -1,197, -1,198, -1,199, -1,200, -1,149, -1,148],
    [  -1, 14, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,150, -1],
    [  -1, -1, 15, -1, 65, -1, 98, -1, 97, -1, 96, -1, 95, -1, 94, -1, 93, -1, 92, -1, 91, -1, 90, -1, 53, -1,189, -1,226, -1,227, -1,228, -1,229, -1,230, -1,231, -1,232, -1,233, -1,234, -1,201, -1,151, -1, -1],
    [  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1],
    [  -1, -1, 16, -1, 66, -1, 99, -1,100, -1,123, -1,122, -1,121, -1,120, -1,119, -1,118, -1, 89, -1, 52, -1,188, -1,225, -1,254, -1,255, -1,256, -1,257, -1,258, -1,259, -1,236, -1,235, -1,202, -1,152, -1, -1],
    [  -1, -1, -1, -1, -1, 67, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,203, -1, -1, -1, -1, -1],
    [  -1, -1, 17, -1, 18, -1, 68, -1,101, -1,124, -1,125, -1,126, -1,135, -1,134, -1,117, -1, 88, -1, 51, -1,187, -1,224, -1,253, -1,270, -1,271, -1,262, -1,261, -1,260, -1,237, -1,204, -1,154, -1,153, -1, -1],
    [  -1, -1, -1, 19, -1, -1, -1, -1, -1,102, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,238, -1, -1, -1, -1, -1,155, -1, -1, -1],
    [  -1, -1, -1, -1, 20, -1, 69, -1, 70, -1,103, -1,104, -1,127, -1,128, -1,133, -1,116, -1, 87, -1, 50, -1,186, -1,223, -1,252, -1,269, -1,264, -1,263, -1,240, -1,239, -1,206, -1,205, -1,156, -1, -1, -1, -1],
    [  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1],
    [  -1, -1, -1, -1, 21, -1, 22, -1, 71, -1, 72, -1,105, -1,106, -1,129, -1,132, -1,115, -1, 86, -1, 49, -1,185, -1,222, -1,251, -1,268, -1,265, -1,242, -1,241, -1,208, -1,207, -1,158, -1,157, -1, -1, -1, -1],
    [  -1, -1, -1, -1, -1, 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,159, -1, -1, -1, -1, -1],
    [  -1, -1, -1, -1, -1, -1, 24, -1, 25, -1, 73, -1, 74, -1,107, -1,130, -1,131, -1,114, -1, 85, -1, 48, -1,184, -1,221, -1,250, -1,267, -1,266, -1,243, -1,210, -1,209, -1,161, -1,160, -1, -1, -1, -1, -1, -1],
    [  -1, -1, -1, -1, -1, -1, -1, 26, -1, -1, -1, -1, -1, -1, -1,108, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,244, -1, -1, -1, -1, -1, -1, -1,162, -1, -1, -1, -1, -1, -1, -1],
    [  -1, -1, -1, -1, -1, -1, -1, -1, 27, -1, 28, -1, 75, -1, 76, -1,109, -1,110, -1,113, -1, 84, -1, 47, -1,183, -1,220, -1,249, -1,246, -1,245, -1,212, -1,211, -1,164, -1,163, -1, -1, -1, -1, -1, -1, -1, -1],
    [  -1, -1, -1, -1, -1, -1, -1, -1, -1, 29, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,165, -1, -1, -1, -1, -1, -1, -1, -1, -1],
    [  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 30, -1, 31, -1, 77, -1, 78, -1,111, -1,112, -1, 83, -1, 46, -1,182, -1,219, -1,248, -1,247, -1,214, -1,213, -1,167, -1,166, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1],
    [  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 32, -1, -1, -1, -1, -1, 79, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,215, -1, -1, -1, -1, -1,168, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1],
    [  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 33, -1, 34, -1, 35, -1, 80, -1, 81, -1, 82, -1, 45, -1,181, -1,218, -1,217, -1,216, -1,171, -1,170, -1,169, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1],
    [  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 36, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,172, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1],
    [  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 37, -1, 38, -1, 39, -1, 41, -1, 44, -1,180, -1,177, -1,175, -1,174, -1,173, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1],
    [  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 40, -1, -1, -1, -1, -1, -1, -1,176, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1],
    [  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 42, -1, 43, -1,179, -1,178, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1],
]

flippedXLayout = []
for y in range(len(originalLayout)):
    newRow = []
    for x in range(len(originalLayout[0])):
        if (x == 25 or ((x < 25 and (x % 4 == 0 or x % 4 == 1)) or (x > 25 and (x % 4 == 1 or x % 4 == 2)))):
            newRow.append(originalLayout[y][x])
        else:
            newRow.append(originalLayout[y][len(originalLayout[0]) - x - 1])
    flippedXLayout.append(newRow)

flippedYLayout = [
    originalLayout[0], originalLayout[1], 
    originalLayout[22], originalLayout[21], 
    originalLayout[4], originalLayout[5], 
    originalLayout[18], originalLayout[17], 
    originalLayout[8], originalLayout[9], 
    originalLayout[14], originalLayout[13],
    originalLayout[12],
    originalLayout[11], originalLayout[10], 
    originalLayout[15], originalLayout[16], 
    originalLayout[7], originalLayout[6], 
    originalLayout[19], originalLayout[20], 
    originalLayout[3], originalLayout[2], 
    originalLayout[23], originalLayout[24],
    ]

flippedXYLayout = [
    flippedXLayout[0], flippedXLayout[1], 
    flippedXLayout[22], flippedXLayout[21], 
    flippedXLayout[4], flippedXLayout[5], 
    flippedXLayout[18], flippedXLayout[17], 
    flippedXLayout[8], flippedXLayout[9], 
    flippedXLayout[14], flippedXLayout[13],
    flippedXLayout[12],
    flippedXLayout[11], flippedXLayout[10], 
    flippedXLayout[15], flippedXLayout[16], 
    flippedXLayout[7], flippedXLayout[6], 
    flippedXLayout[19], flippedXLayout[20], 
    flippedXLayout[3], flippedXLayout[2], 
    flippedXLayout[23], flippedXLayout[24],
    ]

def computeMapsAcrossAllCenters(layout, totalWidth, totalHeight, centers, onlyPolar = False):
    maps = []
    printedXYMaps = False
    for center in centers:
        transformMapX, transformMapY, radiusMap, angleMap = computeMaps(layout, totalWidth, totalHeight, center[0], center[1])
        if (not printedXYMaps):
            printedXYMaps = True
            if not onlyPolar:
                maps.append(transformMapX)
                maps.append(transformMapY)
        maps.append(radiusMap)
        maps.append(angleMap)
    return maps


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

maps = computeMapsAcrossAllCenters(originalLayout, 255.0, 127.0, centerPositions)
printMaps("normal", maps)

maps = computeMapsAcrossAllCenters(originalLayout, 8.0, 4.0, centerPositions)
printMaps("normalLowResolution8x4", maps)

maps = computeMapsAcrossAllCenters(originalLayout, 16.0, 8.0, centerPositions)
printMaps("normalLowResolution16x8", maps)

maps = computeMapsAcrossAllCenters(flippedXLayout, 255.0, 127.0, centerPositions)
printMaps("swappedX", maps)

# maps = computeMapsAcrossAllCenters(flippedXLayout, 8.0, 4.0, centerPositions)
# printMaps("swappedXLowResolution8x4", maps)

# maps = computeMapsAcrossAllCenters(flippedXLayout, 16.0, 8.0, centerPositions)
# printMaps("swappedXLowResolution16x8", maps)

maps = computeMapsAcrossAllCenters(flippedYLayout, 255.0, 127.0, centerPositions)
printMaps("swappedY", maps)

# maps = computeMapsAcrossAllCenters(flippedXLayout, 8.0, 4.0, centerPositions)
# printMaps("swappedYLowResolution8x4", maps)

# maps = computeMapsAcrossAllCenters(flippedXLayout, 16.0, 8.0, centerPositions)
# printMaps("swappedYLowResolution16x8", maps)

maps = computeMapsAcrossAllCenters(flippedXYLayout, 255.0, 127.0, centerPositions)
printMaps("swappedXY", maps)

# maps = computeMapsAcrossAllCenters(flippedXLayout, 8.0, 4.0, centerPositions)
# printMaps("swappedXYLowResolution8x4", maps)

# maps = computeMapsAcrossAllCenters(flippedXLayout, 16.0, 8.0, centerPositions)
# printMaps("swappedXYLowResolution16x8", maps)

print(f"const int transformMapsCount = {len(globalMapList)};")
print("const static uint8_t* transformMaps[transformMapsCount]", end=" PROGMEM = {")
print(*globalMapList, sep=", ", end=" };")
print()

centerPositions = []
for i in range(272):
    for y in range(len(originalLayout)):
        for x in range(len(originalLayout[0])):
            if (originalLayout[y][x] == i):
                if originalLayout[y][x] != -1:
                    xPos = x / len(originalLayout[0])
                    yPos = y / len(originalLayout)
                    centerPositions.append([xPos, yPos])
maps = computeMapsAcrossAllCenters(originalLayout, 255.0, 127.0, centerPositions, True)

print()
globalPixelRadiusMapList = []
globalPixelPolarMapList = []
mapIndex = 0
for mapIndex in range(int(len(maps) / 2)):
    printMap(f"radiusTransformMapForPixel{mapIndex}", maps[mapIndex * 2])
    printMap(f"angleTransformMapForPixel{mapIndex}", maps[(mapIndex * 2) + 1])
    globalPixelRadiusMapList.append(f"radiusTransformMapForPixel{mapIndex}")
    globalPixelPolarMapList.append(f"angleTransformMapForPixel{mapIndex}")

print()
print(f"const static int pixelRadiusTransformMapsCount = {len(globalPixelRadiusMapList)};")
print("const static uint8_t* pixelRadiusTransformMaps[pixelRadiusTransformMapsCount]", end=" = {")
print(*globalPixelRadiusMapList, sep=", ", end=" };")
print()
print(f"const static int pixelAngleTransformMapsCount = {len(globalPixelPolarMapList)};")
print("const static uint8_t* pixelAngleTransformMaps[pixelAngleTransformMapsCount]", end=" = {")
print(*globalPixelPolarMapList, sep=", ", end=" };")
print()
print()
print("#endif")

# run with: python .\PythonScripts\transformGenerator.py | out-file -encoding ASCII transformMaps.h