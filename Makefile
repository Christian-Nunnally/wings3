# Make file for building an execuable that will run on windows.

compiler := g++
compilerFlags := -std=c++11 -Wall
ompilerFlagsThatGoAfterObjectFiles := -lws2_32
objectsDirectory := obj
binariesDirectory := bin
executableName := wings
sourceFilesList := \
	src/windows/TestRunner.cpp \
	src/windows/testLeds.cpp \
	src/windows/testMicrophone.cpp \
	src/windows/socketRemoteControl.cpp \
	src/common/Control/remoteCommandInterpreter.cpp \
	src/common/Control/initializeAndRun.cpp \
	src/common/IO/leds.cpp \
	src/common/Peripherals/microphone.cpp \
	src/common/Peripherals/movementDetection.cpp \
	src/common/Graphics/effectController.cpp \
	src/common/Graphics/colorMixing.cpp \
	src/common/Graphics/palettes.cpp \
	src/common/Graphics/effect.cpp \
	src/common/Graphics/effects.cpp \
	src/common/Graphics/moods.cpp \
	src/common/Graphics/timeModes.cpp \
	src/common/Graphics/mixingModes.cpp \
	src/common/Graphics/brightnessControlModes.cpp \
	src/common/IO/analogInput.cpp \
	src/common/IO/tracing.cpp \
	src/common/Observers/stepDectectedObserver.cpp \
	src/common/Observers/movementDetectedObserver.cpp \
	src/common/Utility/time.cpp \
	src/common/Utility/fastRandom.cpp \
	src/common/Utility/fastMath.cpp \
	src/common/Graphics/Effects/solidColorFillEffect.cpp \
	src/common/Graphics/Effects/lightChaseEffect.cpp \
	src/common/Graphics/Effects/fireworksEffect.cpp \
	src/common/Graphics/Effects/fireEffect.cpp \
	src/common/Graphics/Effects/expandingColorOrbEffect.cpp \
	src/common/Graphics/Effects/lightningBugEffect.cpp \
	src/common/Graphics/Effects/rainShowerEffect.cpp \
	src/common/Graphics/Effects/meteorRainEffect.cpp \
	src/common/Graphics/Effects/gradientWaveEffect.cpp \
	src/common/Graphics/Effects/starFieldEffect.cpp \
	src/common/PeakDetection/peakDetection.cpp \

objectsFilesList := $(patsubst %.cpp,$(objectsDirectory)/%.o,$(sourceFilesList))
executablePath := $(binariesDirectory)/$(executableName).exe

all: $(executablePath)

$(executablePath): $(objectsFilesList)
	@if not exist $(binariesDirectory) mkdir $(binariesDirectory)
	$(compiler) $(compilerFlags) $^ -o $@ $(ompilerFlagsThatGoAfterObjectFiles)

$(objectsDirectory)/%.o: %.cpp src/common/Graphics/palettes.h src/common/Graphics/screenMaps.h src/common/Graphics/transformMaps.h src/common/Graphics/directionMaps.h
	@if not exist $(objectsDirectory)/$(dir $<) mkdir $(objectsDirectory)/$(dir $<)
	$(compiler) $(compilerFlags) -c $< -o $@ $(ompilerFlagsThatGoAfterObjectFiles)

paletteImageFiles = $(wildcard resources/Palettes/*.bmp)
src/common/Graphics/palettes.h: resources/Palettes $(paletteImageFiles) src/pipeline/imagePaletteExtractor.py
	python ./src/pipeline/imagePaletteExtractor.py ./resources/Palettes/ src/common/Graphics/palettes.h

screenMapFiles = $(wildcard resources/ScreenMaps/*.bmp)
src/common/Graphics/screenMaps.h: resources/ScreenMaps $(screenMapFiles) src/pipeline/screenMapGenerator.py src/pipeline/configuration.py
	python ./src/pipeline/screenMapGenerator.py ./resources/ScreenMaps/ src/common/Graphics/screenMaps.h

src/common/Graphics/transformMaps.h: src/pipeline/transformMapsHeaderGenerator.py src/pipeline/configuration.py
	python ./src/pipeline/transformMapsHeaderGenerator.py src/common/Graphics/transformMaps.h

src/common/Graphics/directionMaps.h: src/pipeline/directionMapGenerator.py src/pipeline/configuration.py
	python ./src/pipeline/directionMapGenerator.py src/common/Graphics/directionMaps.h

diagram.svg: diagram.dot
	dot ./diagram.dot -Tsvg -o diagram.svg

clean:
	rm -rf $(objectsDirectory) $(binariesDirectory)

.PHONY: all clean