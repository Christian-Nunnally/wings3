import math

sqrts = []
for i in range(8192 * 2):
    b = i << 18
    print(b)
    print(int(math.sqrt(b)))
    sqrts.append(int(math.sqrt(b)))
    print(b >> 18)

print(sqrts)
print(len(sqrts))
