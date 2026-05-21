from openai import OpenAI
import os

client = OpenAI(
    api_key=os.environ.get("OPENAI_API_KEY")
)

def generate_ai_response(prompt):
    response = client.chat.completions.create(
        model="gpt-4o-mini",
        messages=[
            {
                "role": "system",
                "content": (
                    "Reply for a tiny 128x64 OLED screen. "
                    "Use maximum 4 short lines. "
                    "Keep the answer under 80 characters."
                )
            },
            {
                "role": "user",
                "content": prompt
            }
        ],
        max_tokens=30
    )

    return response.choices[0].message.content.strip()


def transcribe_audio_file(file_path):
    with open(file_path, "rb") as audio_file:
        transcript = client.audio.transcriptions.create(
            model="gpt-4o-mini-transcribe",
            file=audio_file
        )

    return transcript.text.strip()