
class Color:
    red = 0
    green = 0
    blue = 0

    def __init__(self, red, green, blue):
        self.red = red
        self.green = green
        self.blue = blue

def blendColorsUsingMultiply(color1, color2, blendFactor):
    color1Red = color1.red
    color2Red = color2.red
    if (blendFactor > 65535 / 2): color1Red = ((65535 - ((blendFactor - 32768) * 2)) * color1.red) >> 16;
    else: color2Red = (blendFactor * 2 * color2.red) >> 16;

    color1Green = color1.green
    color2Green = color2.green
    if (blendFactor > 65535 / 2): color1Green = ((65535 - ((blendFactor - 32768) * 2)) * color1.green) >> 16;
    else: color2Green = (blendFactor * 2 * color2.green) >> 16;

    color1Blue = color1.blue
    color2Blue = color2.blue
    if (blendFactor > 65535 / 2): color1Blue = ((65535 - ((blendFactor - 32768) * 2)) * color1.blue) >> 16;
    else: color2Blue = (blendFactor * 2 * color2.blue) >> 16;

    r = (color1Red * color2Red) >> 16
    g =(color1Green * color2Green) >> 16
    b = (color1Blue * color2Blue) >> 16
    print(f"{r}, {g}, {b}")

for i in range (10):
    blendColorsUsingMultiply(Color(65535, 50, 50), Color(50, 65535, 0), int(65535 * ((i + 1) / 10.0)))