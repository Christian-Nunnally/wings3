import random
shimmerList = []
bits = 8
for i in range (2**bits):
    shimmerList.append(1 if random.random() * (2**bits) < i else 0)
print(shimmerList)