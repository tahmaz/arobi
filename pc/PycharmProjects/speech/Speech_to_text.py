import speech_recognition as sr

listener = sr.Recognizer()

while(True):
    try:
        with sr.Microphone(device_index=0) as source:
            listener.adjust_for_ambient_noise(source, duration=0.2)
            voice = listener.listen(source)
            print(f"Listening ...")
            text = listener.recognize_google(voice, language="tr-TR")
            text = text.lower()

            print(f"Recognized: {text}")
    except Exception as e:
        listener = sr.Recognizer()
        print("Exception occurred for value '"+ repr(e))
        continue