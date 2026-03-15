import socket
import threading
from flask import Flask, jsonify, render_template
import google.generativeai as genai

# --- SET UP THE AI ---
# We will get your free API key in the next step!
genai.configure(api_key="Enter your Api Key",transport='rest')
model = genai.GenerativeModel('models/gemini-1.5-flash-latest')

app = Flask(__name__)

# Global dictionary for live data
latest_data = {"cpu": "0.00", "ram": "0.00"}

def socket_listener():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('127.0.0.1', 5050))
    server_socket.listen(1)
    print("[*] Socket listener running on port 5050...")
    
    while True:
        client_socket, _ = server_socket.accept()
        try:
            while True:
                data = client_socket.recv(1024)
                if not data:
                    break
                decoded_data = data.decode('utf-8')
                
                parts = decoded_data.split(',')
                if len(parts) == 2:
                    latest_data["cpu"] = parts[0].split(':')[1].replace('%', '')
                    latest_data["ram"] = parts[1].split(':')[1].replace('%', '')
        except Exception as e:
            pass
        finally:
            client_socket.close()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/api/data')
def get_data():
    return jsonify(latest_data)

# --- NEW ROUTE: AI DIAGNOSTICS ---
@app.route('/api/diagnose')
def diagnose():
    try:
        # Create a dynamic prompt using the live hardware data
        prompt = f"""
        You are an expert Windows System Administrator. 
        My computer's current CPU usage is {latest_data['cpu']}% and RAM usage is {latest_data['ram']}%.
        Provide a strict, maximum 2-sentence technical diagnosis and a quick recommendation. 
        Keep it professional and concise.
        """
        response = model.generate_content(prompt)
        return jsonify({"diagnosis": response.text})
    except Exception as e:
        print(f"!!! AI ERROR DETAILS: {e} !!!") # <--- This will show us the exact problem
        return jsonify({"diagnosis": "AI Systems Offline. Please check your API key and connection."})
if __name__ == '__main__':
    threading.Thread(target=socket_listener, daemon=True).start()
    print("[*] Starting Web Server at http://127.0.0.1:3000")
    app.run(port=3000, debug=True, use_reloader=False)