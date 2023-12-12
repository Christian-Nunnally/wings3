CXX := g++
CXXFLAGS := 
OBJ_DIR := objects
SRCS := \
	tests/TestRunner.cpp \
	tests/testLeds.cpp \
	src/IO/leds.cpp \
	src/Peripherals/microphone.cpp \
	src/Peripherals/movementDetection.cpp \
	src/Graphics/effectController.cpp \
	src/Graphics/colorMixing.cpp \
	src/Graphics/palettes.cpp \
	src/Graphics/effect.cpp \
	src/Graphics/effects.cpp \
	src/Graphics/moods.cpp \
	src/Graphics/timeModes.cpp \
	src/Graphics/mixingModes.cpp \
	src/Graphics/brightnessControlModes.cpp \
	src/IO/analogInput.cpp \
	src/Observers/stepDectectedObserver.cpp \
	src/Observers/movementDetectedObserver.cpp \
	src/Utility/time.cpp \
	src/Utility/fastRandom.cpp \
	src/Utility/fastMath.cpp \
	src/Graphics/Effects/solidColorFillEffect.cpp \
	src/Graphics/Effects/lightChaseEffect.cpp \
	src/Graphics/Effects/fireworksEffect.cpp \
	src/Graphics/Effects/fireEffect.cpp \
	src/Graphics/Effects/expandingColorOrbEffect.cpp \
	src/Graphics/Effects/lightningBugEffect.cpp \
	src/Graphics/Effects/rainShowerEffect.cpp \
	src/Graphics/Effects/meteorRainEffect.cpp \
	src/Graphics/Effects/gradientWaveEffect.cpp \
	src/Graphics/Effects/starFieldEffect.cpp \

OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRCS))
# Target executable name
TARGET := a
comma = ,
# Compile and link
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile source files into object files
$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# $(OBJ_DIR):
# 	mkdir $(addsuffix $(comma),$(dir $(OBJS))) $(OBJ_DIR)

# Clean
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Phony targets
.PHONY: clean
