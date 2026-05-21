from flask import Flask, request, jsonify
from ai import generate_ai_response, transcribe_audio_file
import tempfile
import os

app = Flask(__name__)

@app.route("/")
def home():
    return "ESP32 AI Flask backend is running!"


@app.route("/ask", methods=["POST"])
def ask():
    data = request.get_json(silent=True)

    if not data:
        return jsonify({"error": "No JSON received"}), 400

    message = data.get("message", "")

    if message.strip() == "":
        return jsonify({"error": "Message is empty"}), 400

    try:
        reply = generate_ai_response(message)
        return jsonify({"reply": reply})

    except Exception as e:
        return jsonify({"error": str(e)}), 500


@app.route("/voice-ask", methods=["POST"])
def voice_ask():
    raw_audio = request.data

    if not raw_audio:
        return jsonify({"error": "No audio received"}), 400

    temp_path = None

    try:
        with tempfile.NamedTemporaryFile(delete=False, suffix=".wav") as temp_file:
            temp_path = temp_file.name
            temp_file.write(raw_audio)

        question_text = transcribe_audio_file(temp_path)

        if question_text.strip() == "":
            return jsonify({"error": "Could not understand audio"}), 400

        reply = generate_ai_response(question_text)

        return jsonify({
            "question": question_text,
            "reply": reply
        })

    except Exception as e:
        return jsonify({"error": str(e)}), 500

    finally:
        if temp_path and os.path.exists(temp_path):
            os.remove(temp_path)


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)