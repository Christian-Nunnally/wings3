import pyaudio
import numpy as np

# Function to calculate root mean square (RMS)
def calculate_rms(audio_data):
    return np.sqrt(np.mean(np.square(audio_data)))

# Constants for audio parameters
FORMAT = pyaudio.paInt16  # Audio format (16-bit PCM)
CHANNELS = 1  # Number of audio channels (1 for mono, 2 for stereo)
RATE = 44100  # Sample rate (samples per second)
CHUNK = 1024  # Number of frames per buffer

# Initialize PyAudio
audio = pyaudio.PyAudio()

# Open a stream to capture audio from the system's default audio input
stream = audio.open(format=FORMAT, channels=CHANNELS, rate=RATE, input=True, frames_per_buffer=CHUNK)

print("Listening to audio...")

try:
    while True:
        # Read audio data from the stream
        audio_data = np.frombuffer(stream.read(CHUNK), dtype=np.int16)

        # Calculate RMS value of the audio data
        rms = calculate_rms(audio_data)
        
        # Print the RMS value (you can perform any further operations here)
        print(f"Root Mean Square (RMS): {rms}")

except KeyboardInterrupt:
    pass

finally:
    # Close the audio stream and terminate PyAudio
    stream.stop_stream()
    stream.close()
    audio.terminate()
    print("Audio stream closed.")