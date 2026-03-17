from flask import Flask, render_template, request, jsonify
import json
from datetime import datetime
from pathlib import Path

app = Flask(__name__)
NOTES_FILE = "notes.json"

def load_notes():
    if Path(NOTES_FILE).exists():
        with open(NOTES_FILE, "r") as f:
            return json.load(f)
    return []

def save_notes(notes):
    with open(NOTES_FILE, "w") as f:
        json.dump(notes, f)

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/api/notes", methods=["GET"])
def get_notes():
    return jsonify(load_notes())

@app.route("/api/notes", methods=["POST"])
def create_note():
    data = request.json
    notes = load_notes()
    note = {
        "id": len(notes) + 1,
        "title": data.get("title", "Sin título"),
        "content": data.get("content", ""),
        "date": datetime.now().isoformat()
    }
    notes.append(note)
    save_notes(notes)
    return jsonify(note), 201

@app.route("/api/notes/<int:note_id>", methods=["PUT"])
def update_note(note_id):
    data = request.json
    notes = load_notes()
    for note in notes:
        if note["id"] == note_id:
            note.update(data)
            save_notes(notes)
            return jsonify(note)
    return jsonify({"error": "Nota no encontrada"}), 404

@app.route("/api/notes/<int:note_id>", methods=["DELETE"])
def delete_note(note_id):
    notes = load_notes()
    notes = [n for n in notes if n["id"] != note_id]
    save_notes(notes)
    return jsonify({"success": True})

if __name__ == "__main__":
    app.run(debug=True)