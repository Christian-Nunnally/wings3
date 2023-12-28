# Make file for building an execuable that will run on windows.

compiler := g++
compilerFlags := -std=c++11 -Wall
ompilerFlagsThatGoAfterObjectFiles := -lws2_32
objectsDirectory := obj
binariesDirectory := bin
executableName := wings
sourceFilesList := \
	src\windows\TestRunner.cpp \
	src\windows\testLeds.cpp \
	src\windows\testMicrophone.cpp \
	src\windows\socketRemoteControl.cpp \
	src\Control\remoteCommandInterpreter.cpp \
	src\Control\initializeAndRun.cpp \
	src\IO\leds.cpp \
	src\Peripherals\microphone.cpp \
	src\Peripherals\movementDetection.cpp \
	src\Graphics\effectController.cpp \
	src\Graphics\colorMixing.cpp \
	src\Graphics\palettes.cpp \
	src\Graphics\effect.cpp \
	src\Graphics\effects.cpp \
	src\Graphics\moods.cpp \
	src\Graphics\timeModes.cpp \
	src\Graphics\mixingModes.cpp \
	src\Graphics\brightnessControlModes.cpp \
	src\IO\analogInput.cpp \
	src\IO\tracing.cpp \
	src\Observers\stepDectectedObserver.cpp \
	src\Observers\movementDetectedObserver.cpp \
	src\Utility\time.cpp \
	src\Utility\fastRandom.cpp \
	src\Utility\fastMath.cpp \
	src\Graphics\Effects\solidColorFillEffect.cpp \
	src\Graphics\Effects\lightChaseEffect.cpp \
	src\Graphics\Effects\fireworksEffect.cpp \
	src\Graphics\Effects\fireEffect.cpp \
	src\Graphics\Effects\expandingColorOrbEffect.cpp \
	src\Graphics\Effects\lightningBugEffect.cpp \
	src\Graphics\Effects\rainShowerEffect.cpp \
	src\Graphics\Effects\meteorRainEffect.cpp \
	src\Graphics\Effects\gradientWaveEffect.cpp \
	src\Graphics\Effects\starFieldEffect.cpp \
	src\PeakDetection\peakDetection.cpp \

objectsFilesList := $(patsubst %.cpp,$(objectsDirectory)/%.o,$(sourceFilesList))
executablePath := $(binariesDirectory)\$(executableName).exe

all: $(executablePath)

$(executablePath): $(objectsFilesList)
	@if not exist $(binariesDirectory) mkdir $(binariesDirectory)
	$(compiler) $(compilerFlags) $^ -o $@ $(ompilerFlagsThatGoAfterObjectFiles)

$(objectsDirectory)/%.o: %.cpp src/Graphics/palettes.h src/Graphics/screenMaps.h src/Graphics/transformMaps.h src/Graphics/directionMaps.h
	@if not exist $(objectsDirectory)\$(dir $<) mkdir $(objectsDirectory)\$(dir $<)
	$(compiler) $(compilerFlags) -c $< -o $@ $(ompilerFlagsThatGoAfterObjectFiles)

paletteImageFiles = $(wildcard Palettes/*.bmp)
src/Graphics/palettes.h: Palettes $(paletteImageFiles) src\build\imagePaletteExtractor.py
	python .\src\build\imagePaletteExtractor.py .\Palettes\ src\Graphics\palettes.h

screenMapFiles = $(wildcard ScreenMaps/*.bmp)
src/Graphics/screenMaps.h: ScreenMaps $(screenMapFiles) src\build\screenMapGenerator.py src\build\configuration.py
	python .\src\build\screenMapGenerator.py .\ScreenMaps\ src\Graphics\screenMaps.h

src/Graphics/transformMaps.h: src\build\transformMapsHeaderGenerator.py src\build\configuration.py
	python .\src\build\transformMapsHeaderGenerator.py src\Graphics\transformMaps.h

src/Graphics/directionMaps.h: src\build\directionMapGenerator.py src\build\configuration.py
	python .\src\build\directionMapGenerator.py src\Graphics\directionMaps.h

diagram.svg: diagram.dot
	dot .\diagram.dot -Tsvg -o diagram.svg

clean:
	rm -rf $(objectsDirectory) $(binariesDirectory)

.PHONY: all clean