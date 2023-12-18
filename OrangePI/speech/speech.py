import speech_recognition as sr
import pyttsx3

listener = sr.Recognizer()
engine = pyttsx3.init()
engine.say('Hello, I am Tom')
engine.runAndWait()
while(True):
    try:
        with sr.Microphone(device_index=0) as source:
            listener.adjust_for_ambient_noise(source, duration=0.2)
            voice = listener.listen(source)
            print(f"Listening ...")
            #text = listener.recognize_google(voice, language="tr-TR")
            text = listener.recognize_google(voice)
            text = text.lower()
            #if 'tom' in text:
            engine.say(text)
            engine.runAndWait()

            print(f"Recognized: {text}")
    except Exception as e:
        listener = sr.Recognizer()
        print("Exception occurred for value '"+ repr(e))
        continue