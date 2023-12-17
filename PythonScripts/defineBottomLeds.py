
# Number of total leds
ledCount = 272

# List out the index's of leds considered to be on the 'bottom' of the scene:
bottomLeds = [
    12, 14, 17, 19, 21, 23, 24, 26, 27, 29, 30, 32, 33,
    34, 36, 37, 38, 39, 40, 42, 43, 179, 178, 176,
    175, 174, 173, 172, 170, 169, 168, 166, 165,
    163, 162, 160, 159, 157, 155, 153, 150, 148
    ]

def generateIsBottomLedMap():
    isBottomLed = []
    for i in range(ledCount):
        if i in bottomLeds:
            if bottomLeds.index(i) < len(bottomLeds) / 2:
                isBottomLed.append(bottomLeds[bottomLeds.index(i) + 1])
            else:
                isBottomLed.append(bottomLeds[bottomLeds.index(i) - 1])
        else:
            isBottomLed.append(-1)
    return isBottomLed

print(generateIsBottomLedMap())