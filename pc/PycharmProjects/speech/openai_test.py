'''
import openai
import os

# API anahtarınızı kaynak kodunda doğrudan kullanmak yerine bir ortam değişkeni olarak depolamanız önerilir
openai.api_key = "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"

def generate_response(prompt, model, engine, temperature):
    """
    OpenAI API'sini kullanarak bir konuşma yanıtı üretin.
    """
    response = openai.Completion.create(
      engine=engine,
      prompt=prompt,
      temperature=temperature,
      max_tokens=1024,
      n=1,
      stop=None,
      model=model
    )
    message = response.choices[0].text.strip()
    return message

prompt = "Merhaba, nasılsın?"
#model = "davinci"  # davinci modeli, en gelişmiş modeldir
#engine = "davinci-codex"  # motor seçeneği, davinci-codex
model = "ada"
engine = "curie"
temperature = 0.7

response = generate_response(prompt, model, engine, temperature)
print(response)
'''
import os
import openai

openai.api_key = "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"

start = "Your are a AI Search Engine, answer the following query with a witty answer and include validated facts only."


def generate(prompt):
    start_sequence = "{}.{}".format(start, prompt)
    completions = openai.Completion.create(
        model="text-davinci-003",
        prompt=start_sequence,
        temperature=0.1,
        max_tokens=256,
        top_p=1,
        frequency_penalty=0.51,
        presence_penalty=0.5,
        # stream = False,
        # echo = True
    )

    message = completions.choices[0].text
    print(message)
    return message

prompt = "Hello"
generate(prompt)