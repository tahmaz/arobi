#import playsound
#import playsoundsimple as playsound
import  speech_recognition as sr
from gtts import gTTS
import random

import time
import playsoundsimple as pss
#s = pss.Sound("main.wav")
#s.play(1)
#while s.planing:
#    time.sleep(0.1)



def speak(yazı):
    tts = gTTS(text = yazı, lang= "tr")
    dosya_ismi = "ses"+ str(random.randint(0,1000000000000000000000)) + ".mp3"
    tts.save(dosya_ismi)
    #playsound.playsound(dosya_ismi)
    s = pss.Sound(dosya_ismi)
    s.play(1)

def sesi_kaydet():
    print("kayit")
    r = sr.Recognizer()
    try:
        with sr.Microphone(device_index=0) as source:
            ses = r.listen(source)

            centences = ""

            try:
                print("kaydediliyor...")
                centences = r.recognize_google(ses, language="Tr-tr")
                print(centences)

            except Exception:
                speak("söylediğin cümleyi anlayamadım")
    except Exception as e:
        listener = sr.Recognizer()
        print("Exception occurred for value '"+ repr(e))

    return centences

speak("nasılsın")
while True:
    yazı = sesi_kaydet()
    print(yazı)
    if "nasılsın" in yazı:
        speak("iyiyim sen nasılsın")