effects = '''if (effect == 0)
{
    effect = fastRandomInteger(12);
    if (effect == 0) effect1 = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap1, *effect1TransformMap2, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
    else if (effect == 1) effect1 = [](int ledIndex) { return meteorRain2(frameTimeDelta, effect1Time1, effect1Time2, effect1Time2 >> 3, ledIndex, 30, .12, normalTransformMapX, normalTransformMapY, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
    else if (effect == 2) effect1 = [](int ledIndex) { return meteorRain(frameTimeDelta, effect1Time1,effect1Time2, ledIndex, meteorSize, .12, *effect1TransformMap2, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
    else if (effect == 3) effect1 = [](int ledIndex) { return meteorRain2(frameTimeDelta, effect1Time1, effect1Time2, 0, ledIndex, 50, .12, *effect1TransformMap1, *effect1TransformMap1, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
    else if (effect == 4) effect1 = [](int ledIndex) { return starShimmer(frameTimeDelta, 16, 80, ledIndex, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
    else if (effect == 5) effect1 = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap2, *effect1TransformMap1, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
    else if (effect == 6) effect1 = [](int ledIndex) { return rainShower(frameTimeDelta, effect1Time1, ledIndex, meteorSize, normalTransformMapY, normalTransformMapX, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
    else if (effect == 7) effect1 = [](int ledIndex) { return colorOrb(frameTimeDelta, ledIndex, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
    else if (effect == 8) effect1 = [](int ledIndex) { return lightningBug(frameTimeDelta, ledIndex, *currentPalette1OffsetPointer); };
    else if (fastRandomInteger(5 == 0)) effect1 = [](int ledIndex) { return ledTest(currentTime, ledIndex, effect1Time2, *currentPalette1OffsetPointer, 5, *effect1GlobalBrightnessPointer); };
}'''

paletteToUse = 1
for i in range(1,5):
    modifiedEffects = effects.replace("effect1", "effect" + str(i))
    modifiedEffects = modifiedEffects.replace("effect == 0", "effect == " + str(i))
    modifiedEffects = modifiedEffects.replace("currentPalette1", "currentPalette" + str(paletteToUse))
    print(modifiedEffects)
    paletteToUse = 2 if paletteToUse == 1 else 1