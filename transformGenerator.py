originalLayout = [
    [11,10,9,8,7,6,5,4,3,2,1,0,-1,-1,136,137,138,139,140,141,142,143,144,145,146,147],
    [12,13,64,63,62,61,60,59,58,57,56,55,54,190,191,192,193,194,195,196,197,198,199,200,149,148],
    [14,15,65,98,97,96,95,94,93,92,91,90,53,189,226,227,228,229,230,231,232,233,234,201,151,150],
    [-1,16,66,99,100,123,122,121,120,119,118,89,52,188,225,254,255,256,257,258,259,236,235,202,152,-1],
    [-1,17,18,68,101,124,125,126,135,134,117,88,51,187,224,253,270,271,262,261,260,237,204,154,153,-1],
    [67,19,20,69,70,103,104,127,128,133,116,87,50,186,223,252,269,264,263,240,239,206,205,156,155,203],
    [-1,-1,21,22,71,72,105,106,129,132,115,86,49,185,222,251,268,265,242,241,208,207,158,157,-1,-1],
    [-1,-1,23,24,25,73,74,107,130,131,114,85,48,184,221,250,267,266,243,210,209,161,160,159,-1,-1],
    [-1,-1,-1,26,27,28,75,76,109,110,113,84,47,183,220,249,246,245,212,211,164,163,162,-1,-1,-1],
    [102,-1,-1,-1,29,30,32,77,78,111,112,83,46,182,219,248,247,214,213,168,166,165,-1,-1,-1,238],
    [-1,-1,-1,-1,-1,31,33,34,36,80,81,82,45,181,218,217,216,172,170,169,167,-1,-1,-1,-1,-1],
    [-1,-1,-1,-1,-1,-1,-1,35,37,38,39,41,44,180,177,175,174,173,171,-1,-1,-1,-1,-1,-1,-1],
    [-1,-1,-1,108,-1,-1,-1,79,-1,-1,40,42,43,179,178,176,-1,-1,215,-1,-1,-1,244,-1,-1,-1],
]

totalHeight = 127.0
totalWidth = 255.0
ledsWide = 26.0
ledsTall = 13.0

totalLeds = 0
for y in range(len(originalLayout)):
    print (y)
    for x in range(len(originalLayout[0])):
        if (originalLayout[y][x] != -1):
            totalLeds += 1
print("Total leds" + str(totalLeds))

transformMapX = []
transformMapY = []

for i in range(totalLeds):
    for y in range(len(originalLayout)):
        for x in range(len(originalLayout[0])):
            if (originalLayout[y][x] == i):
                transformMapX.append((totalWidth / (ledsWide - 1)) * x)
                transformMapY.append((totalHeight / (ledsTall - 1)) * y)

for i in range(len(transformMapX)):
    print(int(transformMapX[i]), end = ", ")