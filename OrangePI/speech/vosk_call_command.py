from vosk import Model, KaldiRecognizer
import pyaudio
import subprocess,os

model = Model("vosk-model-small-en-us-0.15")
#model = Model("vosk-model-small-tr-0.3")
recognizer = KaldiRecognizer(model, 16000)

CHUNK = 8192
FORMAT = pyaudio.paInt16
CHANNELS = 1
DEVICE_INDEX = 1
RATE = 16000
RECORD_SECONDS = 5

mic = pyaudio.PyAudio()

stream = mic.open(format=FORMAT,
                channels=CHANNELS,
                input_device_index=DEVICE_INDEX,				  
                rate=RATE,
                input=True,
                frames_per_buffer=CHUNK)

#stream = mic.open(format=pyaudio.paInt16, input_device_index=2, channels=1, rate=16000, input=True, frames_per_buffer=8192)
stream.start_stream()

while True:
    try:
        data = stream.read(4096, exception_on_overflow = False)

        if recognizer.AcceptWaveform(data):
            text = recognizer.Result()
            textp = text[14:-3]
            print(textp)
            if(textp == "okay"):
                os.system(f"/root/seon-robot/commands/track_ball_stop.sh")
            elif(textp == "up"):
                subprocess.run(['/root/seon-robot/commands/track_ball_start.sh'],shell=True)

    except Exception as e:
        recognizer = KaldiRecognizer(model, 16000)
        print("Exception occurred for value '"+ repr(e))
        continue