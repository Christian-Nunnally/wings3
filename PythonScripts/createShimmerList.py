import random

# The length of the generated list (higher numbers result in more random shimmer)
bits = 8

shimmerList = []
for i in range (2**bits):
    shimmerList.append(1 if random.random() * (2**bits) < i else 0)

print(shimmerList)