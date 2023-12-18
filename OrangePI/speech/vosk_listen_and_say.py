from vosk import Model, KaldiRecognizer
import pyaudio
import pyttsx3

model = Model("vosk-model-small-en-us-0.15")
#model = Model("vosk-model-small-tr-0.3")
recognizer = KaldiRecognizer(model, 16000)

mic = pyaudio.PyAudio()

stream = mic.open(format=pyaudio.paInt16, input_device_index=2, channels=1, rate=16000, input=True, frames_per_buffer=8192)
stream.start_stream()

engine = pyttsx3.init()
voices = engine.getProperty('voices')
for voice in voices:
    #print(voice)
    if voice.languages[0] == u'en_US':
        engine.setProperty('voice', voice.id)
        break

while True:
    data = stream.read(4096, exception_on_overflow = False)

    if recognizer.AcceptWaveform(data):
        text = recognizer.Result()
        #print(text)
        textp = text[14:-3]
        print(textp)
        #engine.say(textp)
        #engine.runAndWait()