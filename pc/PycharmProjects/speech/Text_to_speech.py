import pyttsx3

engine = pyttsx3.init()
voices = engine.getProperty('voices')
for voice in voices:
    print(voice)
    if voice.languages[0] == u'en_US':
        engine.setProperty('voice', voice.id)
        break

engine.say('Hello World')
engine.runAndWait()