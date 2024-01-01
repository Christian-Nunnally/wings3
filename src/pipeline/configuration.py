
# 2D Array describing which pixel of a virtual screen would correspond 
# to which led is physically wired into the microcontroller.
# If PhysicalLedIndexMapping[x][y] == -1, that means there are no leds for that 'pixel'
# If PhysicalLedIndexMapping[x][y] == 0..n, that means that the led at index n represents that pixel.
PhysicalLedIndexMapping = [
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

# Defines the aspect ratio of the virtual canvas.
VirtualCanvasWidth = 256.0
VirtualCanvasHeight = 128.0

# PhysicalLedIndexMapping = []
# width = 40
# height = 20
# for i in range(height):
#     PhysicalLedIndexMapping.append([])
#     for j in range(width):
#         PhysicalLedIndexMapping[i].append(i * width + j)

# print(PhysicalLedIndexMapping)

TotalLedCount = 0
for y in range(len(PhysicalLedIndexMapping)):
    TotalLedCount += len(PhysicalLedIndexMapping[y]) - PhysicalLedIndexMapping[y].count(-1)

PhysicalLedIndexMappingWidth = len(PhysicalLedIndexMapping[0])
PhysicalLedIndexMappingHeight = len(PhysicalLedIndexMapping)
