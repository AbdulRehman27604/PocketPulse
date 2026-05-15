from flask import Flask, request, jsonify
from ai import generate_ai_response

app = Flask(__name__)

@app.route("/")
def home():
    return "ESP32 AI Flask backend is running!"

@app.route("/ask", methods=["POST"])
def ask():
    data = request.get_json()

    if not data:
        return jsonify({"error": "No JSON received"}), 400

    message = data.get("message", "")

    if message.strip() == "":
        return jsonify({"error": "Message is empty"}), 400

    try:
        reply = generate_ai_response(message)

        return jsonify({
            "reply": reply
        })

    except Exception as e:
        return jsonify({
            "error": str(e)
        }), 500

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5050)