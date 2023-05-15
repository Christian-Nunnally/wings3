import math

# og map
# squares = []
# for i in range(8192 * 2):
#     b = i << 18
#     print(b)
#     print(int(math.sqrt(b)))
#     squares.append(int(math.sqrt(b)))
#     print(b >> 18)

# print(squares)
# print(len(squares))
# print()
# print()


squares = []
for i in range(2**14):
    b = i << 2
    print(b)
    print(int(math.sqrt(b)))
    squares.append(int(math.sqrt(b)))

print(squares)
print(len(squares))

