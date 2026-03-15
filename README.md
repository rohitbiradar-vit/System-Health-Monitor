# 🚀 AI-Powered Remote System Health Monitor

A real-time, cross-language system monitoring tool that captures hardware telemetry (CPU & RAM) via a low-level C++ agent and visualizes it on a modern web dashboard with AI-driven health diagnostics.

---

## 🛠️ The Tech Stack

| Layer | Technology |
| :--- | :--- |
| **System Agent** | C++ (Win32 API) |
| **Networking** | TCP Sockets (Winsock / Python Sockets) |
| **Backend** | Python (Flask, Threading) |
| **Intelligence** | Google Gemini LLM (AI Diagnostics) |
| **Frontend** | HTML5, CSS3 (Glassmorphism), JavaScript (Vanilla), Chart.js |

---

## ✨ Key Features

* **Low-Level Telemetry:** Custom C++ agent queries the Windows Kernel directly for hardware states.
* **Socket Bridge:** Real-time data transmission from C++ to Python using TCP sockets.
* **AI Diagnostics:** Integrates Gemini LLM to analyze hardware usage and provide human-readable system advice.
* **Live Visualizations:** Smooth, real-time line charts built with Chart.js.
* **Glassmorphic UI:** Modern, responsive dashboard with neon accents and a live-status indicator.

---

## 📂 Folder Structure

```text
System-Health-Monitor/
├── cpp_agent/
│   └── monitor.cpp         # C++ Data Collector & Socket Client
├── python_backend/
│   ├── app.py              # Flask Server & AI Logic
│   ├── templates/
│   │   └── index.html      # Glassmorphic Web Dashboard
│   └── .env                # API Keys (Excluded from Git)
├── .gitignore
└── README.md
