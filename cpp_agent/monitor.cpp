#define _WIN32_WINNT 0x0600
#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

// 1. CPU Function (Unchanged)
double getCPUUsage() {
    static FILETIME prevSysIdle, prevSysKernel, prevSysUser;
    FILETIME sysIdle, sysKernel, sysUser;
    if (GetSystemTimes(&sysIdle, &sysKernel, &sysUser) == 0) return -1.0;
    if (prevSysIdle.dwLowDateTime == 0 && prevSysIdle.dwHighDateTime == 0) {
        prevSysIdle = sysIdle; prevSysKernel = sysKernel; prevSysUser = sysUser;
        return 0.0;
    }
    auto fileTimeToInt64 = [](const FILETIME& ft) { return (((ULONGLONG)ft.dwHighDateTime) << 32) + ft.dwLowDateTime; };
    ULONGLONG sysIdleDiff = fileTimeToInt64(sysIdle) - fileTimeToInt64(prevSysIdle);
    ULONGLONG sysKernelDiff = fileTimeToInt64(sysKernel) - fileTimeToInt64(prevSysKernel);
    ULONGLONG sysUserDiff = fileTimeToInt64(sysUser) - fileTimeToInt64(prevSysUser);
    ULONGLONG sysTotalDiff = sysKernelDiff + sysUserDiff;
    double cpuUsage = 0.0;
    if (sysTotalDiff > 0) cpuUsage = (sysTotalDiff - sysIdleDiff) * 100.0 / sysTotalDiff;
    prevSysIdle = sysIdle; prevSysKernel = sysKernel; prevSysUser = sysUser;
    return cpuUsage;
}

// 2. RAM Function (Unchanged)
double getRAMUsage() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
    DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
    return (physMemUsed * 100.0) / totalPhysMem;
}

int main() {
    std::cout << "Starting C++ System Agent..." << std::endl;

    // --- NEW SOCKET CODE STARTS HERE ---
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization failed!" << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5050);                 // Port 5050 (Matches Python)
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost IP

    std::cout << "Connecting to Python Server on 127.0.0.1:5050..." << std::endl;
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed! Is the Python server running?" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Connected successfully!\n" << std::endl;
    // --- SOCKET CODE ENDS HERE ---

    getCPUUsage(); // Initialize CPU timers
    Sleep(1000); 
    
    while (true) {
        double ram = getRAMUsage();
        double cpu = getCPUUsage();
        
        // Package the data into a string payload
        std::string payload = "CPU:" + std::to_string(cpu) + "%, RAM:" + std::to_string(ram) + "%";
        
        std::cout << "Sending -> " << payload << std::endl;
        
        // Fire it over the network to Python!
        int bytesSent = send(clientSocket, payload.c_str(), payload.length(), 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Failed to send data. Disconnected?" << std::endl;
            break;
        }
        
        Sleep(2000); 
    }
    
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}