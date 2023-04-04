import colorsys
paletteName = "starterPalette1"
paletteLength = 256
rowLength = 8
print(f"static uint16_t {paletteName}[{paletteLength * 3}] PROGMEM = ")
print("{")

# Generate the RGB values for each color in the rainbow
hue_range = [i/paletteLength for i in range(paletteLength)]
rgb_colors = [colorsys.hsv_to_rgb(hue, 1, 1) for hue in hue_range]

rowCount = 0
# Print the RGB values for each color in the rainbow
for i, color in enumerate(rgb_colors):
    print("    ", end="") if rowCount % rowLength == 0 else print("", end="") 
    lineEnd = "\n" if rowCount % rowLength == rowLength - 1 else ""
    r = f"{int(color[0]*65_535)},"
    r = f"{r:{' '}{'<'}{6}}"
    g = f"{int(color[1]*65_535)},"
    g = f"{g:{' '}{'<'}{6}}"
    b = f"{int(color[2]*65_535)},"
    b = f"{b:{' '}{'<'}{6}}"
    print(f"{r} {g} {b} ", end=lineEnd)
    rowCount += 1
print("};")
print()