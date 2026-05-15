from openai import OpenAI
import os

client = OpenAI(api_key=os.environ.get("OPENAI_API_KEY"))

def generate_ai_response(prompt):
    response = client.chat.completions.create(
        model="gpt-4o-mini",
        messages=[
            {
                "role": "system",
                "content": (
                    "You are replying to an ESP32 with a tiny 128x64 OLED screen. "
                    "Reply in maximum 4 short lines. "
                    "Keep the total answer under 120 characters."
                )
            },
            {
                "role": "user",
                "content": prompt
            }
        ],
        max_tokens=40
    )

    return response.choices[0].message.content.strip()