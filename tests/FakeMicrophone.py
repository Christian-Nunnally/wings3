import pyaudio
import numpy as np
import sys
import struct

# Function to calculate root mean square (RMS)
def calculate_rms_2(audio_data):
    return np.sqrt(np.mean(np.square(audio_data)))

def np_audioop_rms(data, width):
    """audioop.rms() using numpy; avoids another dependency for app"""
    #_checkParameters(data, width)
    if len(data) == 0: return None
    fromType = (np.int8, np.int16, np.int32)[width//2]
    d = np.frombuffer(data, fromType).astype(float)
    rms = np.sqrt( np.mean(d**2) )
    return int( rms )

# Constants for audio parameters
FORMAT = pyaudio.paInt16  # Audio format (16-bit PCM)
CHANNELS = 1  # Number of audio channels (1 for mono, 2 for stereo)
RATE = 22100  # Sample rate (samples per second)
CHUNK = 512  # Number of frames per buffer

# Initialize PyAudio
audio = pyaudio.PyAudio()

# Get available audio input devices
num_devices = audio.get_device_count()
print("Available audio input devices:")
for i in range(num_devices):
    device_info = audio.get_device_info_by_index(i)
    print(f"{i}: {device_info['name']}")

# Open a stream to capture audio from the system's default audio input
stream = audio.open(format=FORMAT, channels=CHANNELS, rate=RATE, input=True, frames_per_buffer=CHUNK)

print("Listening to audio...")

try:
    while True:
        # Read audio data from the stream
        audio_data = np.frombuffer(stream.read(CHUNK), dtype=np.int16).astype(float)

        # Calculate RMS value of the audio data
        # rms = calculate_rms_2(audio_data)
        rms = np_audioop_rms(audio_data, 2)
        # Print the RMS value (you can perform any further operations here)
        if np.isnan(rms):
            print(audio_data)
        print(f"{rms}")
        sys.stdout.flush()

except KeyboardInterrupt:
    pass

finally:
    # Close the audio stream and terminate PyAudio
    stream.stop_stream()
    stream.close()
    audio.terminate()
    print("Audio stream closed.")