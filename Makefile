compiler := g++
compilerFlags := -std=c++11 -Wall
ompilerFlagsThatGoAfterObjectFiles := -lws2_32
objectsDirectory := obj
binariesDirectory := bin
executableName := wings
sourceFilesList := \
	tests\TestRunner.cpp \
	tests\testLeds.cpp \
	tests\SocketReader.cpp \
	src\Control\remoteCommandInterpreter.cpp \
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

objectsFilesList := $(patsubst %.cpp,$(objectsDirectory)/%.o,$(sourceFilesList))
executablePath := $(binariesDirectory)\$(executableName).exe

all: $(executablePath)

$(executablePath): $(objectsFilesList)
	@if not exist $(binariesDirectory) mkdir $(binariesDirectory)
	$(compiler) $(compilerFlags) $^ -o $@ $(ompilerFlagsThatGoAfterObjectFiles)

$(objectsDirectory)/%.o: %.cpp src/Graphics/palettes.h
	@if not exist $(objectsDirectory)\$(dir $<) mkdir $(objectsDirectory)\$(dir $<)
	$(compiler) $(compilerFlags) -c $< -o $@ $(ompilerFlagsThatGoAfterObjectFiles)

paletteImageFiles = $(wildcard Palettes/*)
src/Graphics/palettes.h: Palettes $(paletteImageFiles)
	python .\PythonScripts\imagePaletteExtractor.py .\Palettes\ src\Graphics\palettes.h

clean:
	rm -rf $(objectsDirectory) $(binariesDirectory)

.PHONY: all clean