from PIL import Image

image = Image.open('palette2.bmp', 'r')

pix_val = list(image.getdata())
final_list = []
print(f"static uint8_t palette[196608] PROGMEM = ")
print("{", end="")
for i in range(256):
    print("    ", end="")
    for j in range(256):
        final_list.append(pix_val[((i*256)+j)][0])
        final_list.append(pix_val[((i*256)+j)][1])
        final_list.append(pix_val[((i*256)+j)][2])
        print(pix_val[((i*256)+j)][0], end="")
        print(", ", end="")
        print(pix_val[((i*256)+j)][1], end="")
        print(", ", end="")
        print(pix_val[((i*256)+j)][2], end="")
        if (i == 255 and j == 255):
            continue
        print(", ", end="")
    print()
print("};", end="")
print(len(final_list))