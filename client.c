#define  _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h> // 用于 inet_pton

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = { 0 };

    // 初始化 Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // 创建 socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 使用 inet_pton 替代 inet_addr（避免过时 API 警告）
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // 连接到服务器
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection failed. Error Code: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Connected to server\n");

    while (1) {
        // 发送消息
        printf("Client: ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }
        buffer[strcspn(buffer, "\n")] = '\0'; // 移除换行符

        if (send(sock, buffer, (int)strlen(buffer), 0) == SOCKET_ERROR) {
            printf("Send failed. Error Code: %d\n", WSAGetLastError());
            break;
        }

        // 接收回复（防止缓冲区溢出）
        int valread = recv(sock, buffer, BUFFER_SIZE - 1, 0); // 预留 1 字节给 '\0'
        if (valread <= 0) {
            printf("Server disconnected or error occurred\n");
            break;
        }

        buffer[valread] = '\0'; // 确保字符串以 '\0' 结尾
        printf("Server: %s\n", buffer);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}