import socket
import threading
from flask import Flask, jsonify, render_template

app = Flask(__name__)

# A global dictionary to store the latest data as it arrives
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
                
                # Split the string "CPU:2.3%, RAM:55.0%" to extract the numbers
                parts = decoded_data.split(',')
                if len(parts) == 2:
                    cpu_val = parts[0].split(':')[1].replace('%', '')
                    ram_val = parts[1].split(':')[1].replace('%', '')
                    latest_data["cpu"] = cpu_val
                    latest_data["ram"] = ram_val
        except Exception as e:
            pass
        finally:
            client_socket.close()

# Route 1: Serves the HTML webpage
@app.route('/')
def index():
    return render_template('index.html')

# Route 2: An API endpoint that sends the latest data as JSON
@app.route('/api/data')
def get_data():
    return jsonify(latest_data)

if __name__ == '__main__':
    # Start the socket listener in the background
    threading.Thread(target=socket_listener, daemon=True).start()
    
    # Start the web server on port 3000
    print("[*] Starting Web Server at http://127.0.0.1:3000")
    app.run(port=3000, debug=True, use_reloader=False)