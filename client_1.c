#define _CRT_SECURE_NO_WARNINGS 1
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

// 线程参数结构体
typedef struct {
    SOCKET server_socket;
    volatile int* running;
} client_thread_params_t;

// 函数声明
int initialize_client();
void run_client_session();
void shutdown_client();
unsigned __stdcall receive_handler(void* params);

int init_winsock();
SOCKET create_socket();
int setup_server_address(struct sockaddr_in* address, const char* ip, int port);
int connect_to_server(SOCKET sock, struct sockaddr_in* address);
int send_to_server(SOCKET sock, const char* message);
int receive_from_server(SOCKET sock, char* buffer, int buffer_size);
void handle_user_input(SOCKET server_sock, volatile int* running);
void display_welcome_message();
void cleanup_socket(SOCKET sock);
void cleanup_winsock();

// 全局变量
WSADATA wsa_data;
SOCKET client_socket = INVALID_SOCKET;
volatile int client_running = 1;

int main() {
    printf("=== Multi-threaded Math Client Starting ===\n");

    if (initialize_client() != 0) {
        return 1;
    }

    run_client_session();
    shutdown_client();

    printf("=== Client Shutdown Complete ===\n");
    return 0;
}

// 初始化客户端
int initialize_client() {
    printf("Initializing math client...\n");

    if (init_winsock() != 0) {
        return 1;
    }

    client_socket = create_socket();
    if (client_socket == INVALID_SOCKET) {
        cleanup_winsock();
        return 1;
    }

    struct sockaddr_in server_address;
    if (setup_server_address(&server_address, SERVER_IP, SERVER_PORT) != 0) {
        cleanup_socket(client_socket);
        cleanup_winsock();
        return 1;
    }

    if (connect_to_server(client_socket, &server_address) != 0) {
        cleanup_socket(client_socket);
        cleanup_winsock();
        return 1;
    }

    printf("Client initialized successfully\n");
    return 0;
}

// 运行客户端会话
void run_client_session() {
    printf("Starting math session with server...\n");
    display_welcome_message();

    // 创建接收线程参数
    client_thread_params_t params;
    params.server_socket = client_socket;
    params.running = &client_running;

    // 创建接收线程
    uintptr_t receive_thread = _beginthreadex(NULL, 0, receive_handler, &params, 0, NULL);
    if (receive_thread == 0) {
        printf("Failed to create receive thread.\n");
        return;
    }

    // 在主线程中处理用户输入
    handle_user_input(client_socket, &client_running);

    // 等待接收线程结束
    WaitForSingleObject((HANDLE)receive_thread, INFINITE);
    CloseHandle((HANDLE)receive_thread);
}

// 关闭客户端
void shutdown_client() {
    printf("Shutting down client...\n");
    cleanup_socket(client_socket);
    cleanup_winsock();
}

// 接收消息线程函数
unsigned __stdcall receive_handler(void* params) {
    client_thread_params_t* thread_params = (client_thread_params_t*)params;
    SOCKET server_sock = thread_params->server_socket;
    volatile int* running = thread_params->running;

    char buffer[BUFFER_SIZE];

    while (*running) {
        int receive_result = receive_from_server(server_sock, buffer, BUFFER_SIZE);
        if (receive_result <= 0) {
            printf("Connection lost or server disconnected.\n");
            *running = 0;
            break;
        }

        printf("Server: %s\n", buffer);

        // 检查是否是退出消息
        if (strstr(buffer, "Goodbye") != NULL) {
            *running = 0;
            break;
        }
    }

    return 0;
}

// 初始化Winsock
int init_winsock() {
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Winsock initialized successfully.\n");
    return 0;
}

// 创建socket
SOCKET create_socket() {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Could not create socket. Error Code: %d\n", WSAGetLastError());
    }
    else {
        printf("Socket created successfully.\n");
    }
    return sock;
}

// 设置服务器地址
int setup_server_address(struct sockaddr_in* address, const char* ip, int port) {
    address->sin_family = AF_INET;
    address->sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &address->sin_addr) <= 0) {
        printf("Invalid address/ Address not supported: %s\n", ip);
        return 1;
    }

    printf("Server address configured: %s:%d\n", ip, port);
    return 0;
}

// 连接到服务器
int connect_to_server(SOCKET sock, struct sockaddr_in* address) {
    printf("Connecting to math server...\n");
    if (connect(sock, (struct sockaddr*)address, sizeof(struct sockaddr_in)) < 0) {
        printf("Connection failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Connected to math server successfully\n");
    return 0;
}

// 发送消息到服务器
int send_to_server(SOCKET sock, const char* message) {
    int bytes_sent = send(sock, message, (int)strlen(message), 0);
    if (bytes_sent == SOCKET_ERROR) {
        printf("Send failed. Error Code: %d\n", WSAGetLastError());
        return -1;
    }
    return bytes_sent;
}

// 接收服务器消息
int receive_from_server(SOCKET sock, char* buffer, int buffer_size) {
    int bytes_received = recv(sock, buffer, buffer_size - 1, 0);
    if (bytes_received <= 0) {
        if (bytes_received == 0) {
            printf("Server disconnected.\n");
        }
        else {
            printf("Receive error. Error Code: %d\n", WSAGetLastError());
        }
        return -1;
    }

    buffer[bytes_received] = '\0';
    return bytes_received;
}

// 显示欢迎消息
void display_welcome_message() {
    char buffer[BUFFER_SIZE];
    int welcome_result = receive_from_server(client_socket, buffer, BUFFER_SIZE);
    if (welcome_result > 0) {
        printf("Server: %s\n", buffer);
    }

    printf("\n=== Multi-threaded Math Calculator Client ===\n");
    printf("Supported operations: +, -, *, /, %%, ^\n");
    printf("Format: number operator number (e.g., 5 + 3)\n");
    printf("Type 'quit' to exit.\n");
    printf("You can send requests while receiving responses.\n");
    printf("===============================================\n");
}

// 处理用户输入
void handle_user_input(SOCKET server_sock, volatile int* running) {
    char buffer[BUFFER_SIZE];

    while (*running) {
        printf("\nEnter math problem (or 'quit' to exit): ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            printf("Input error.\n");
            break;
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, "quit") == 0 || strcmp(buffer, "exit") == 0) {
            if (send_to_server(server_sock, buffer) >= 0) {
                printf("Disconnecting from server...\n");
            }
            *running = 0;
            break;
        }

        if (send_to_server(server_sock, buffer) < 0) {
            *running = 0;
            break;
        }

        printf("Math problem sent. You can continue sending requests.\n");
    }
}

// 清理socket
void cleanup_socket(SOCKET sock) {
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
        printf("Socket closed.\n");
    }
}

// 清理Winsock
void cleanup_winsock() {
    WSACleanup();
    printf("Winsock cleanup completed.\n");
}