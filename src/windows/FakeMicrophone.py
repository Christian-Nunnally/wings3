import pyaudio
import numpy as np
import sys
from OptionDialog import showOptionsDialog

AUDIO_FORMAT = pyaudio.paInt16
CHANNELS = 1
SAMPLES_PER_SECOND = 8000
FRAMES_PER_BUFFER = 256

useDefaultInputDevice = True

def calculateRms(data):
    if len(data) == 0: 
        return None
    dataArray = np.frombuffer(data, np.int16).astype(float)
    rms = np.sqrt(np.mean(dataArray**2))
    return int(rms)

audio = pyaudio.PyAudio()
deviceCount = audio.get_device_count()
print("Available audio input devices:")
devicesNames = []
for i in range(deviceCount):
    device_info = audio.get_device_info_by_index(i)
    devicesNames.append(f"{i}: {device_info['name']}")
    print(f"{i}: {device_info['name']}")

if not useDefaultInputDevice:
    option = showOptionsDialog(devicesNames)
    deviceIndex = int(option[0])
    stream = audio.open(format=AUDIO_FORMAT, channels=CHANNELS, rate=SAMPLES_PER_SECOND, input=True, frames_per_buffer=FRAMES_PER_BUFFER, input_device_index=deviceIndex)
else:
    stream = audio.open(format=AUDIO_FORMAT, channels=CHANNELS, rate=SAMPLES_PER_SECOND, input=True, frames_per_buffer=FRAMES_PER_BUFFER)
print("Listening to audio...")

try:
    while True:
        audio_data = np.frombuffer(stream.read(FRAMES_PER_BUFFER)).astype(float)
        rms = calculateRms(audio_data)
        print(f"{rms}")
        sys.stdout.flush()
finally:
    stream.stop_stream()
    stream.close()
    audio.terminate()
    print("Audio stream closed.")