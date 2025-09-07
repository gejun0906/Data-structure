#define _CRT_SECURE_NO_WARNINGS 1
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <math.h>
#include <process.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

// 线程参数结构体
typedef struct {
    SOCKET client_socket;
    struct sockaddr_in client_addr;
} client_thread_params_t;

// 互斥锁
HANDLE client_count_mutex;
int active_clients = 0;

// 函数声明
int initialize_server();
void run_server();
void shutdown_server();
unsigned __stdcall client_handler(void* params);

int init_winsock();
SOCKET create_socket();
int setup_server_address(struct sockaddr_in* address, int port);
int bind_socket(SOCKET sock, struct sockaddr_in* address);
int start_listening(SOCKET sock, int backlog);
SOCKET accept_client_connection(SOCKET server_sock, struct sockaddr_in* addr, int* addrlen);
int receive_from_client(SOCKET client_sock, char* buffer, int buffer_size);
int send_to_client(SOCKET client_sock, const char* message);
double calculate_math_expression(const char* expression);
void handle_math_communication(SOCKET client_sock);
void print_client_info(struct sockaddr_in* client_addr);
void cleanup_socket(SOCKET sock);
void cleanup_winsock();
void increment_client_count();
void decrement_client_count();
int get_client_count();

// 全局变量
WSADATA wsa_data;
SOCKET server_socket = INVALID_SOCKET;

int main() {
    printf("=== Multi-threaded Math Server Starting ===\n");

    // 初始化互斥锁
    client_count_mutex = CreateMutex(NULL, FALSE, NULL);
    if (client_count_mutex == NULL) {
        printf("Failed to create mutex. Error: %d\n", GetLastError());
        return 1;
    }

    if (initialize_server() != 0) {
        CloseHandle(client_count_mutex);
        return 1;
    }

    run_server();
    shutdown_server();

    CloseHandle(client_count_mutex);
    printf("=== Server Shutdown Complete ===\n");
    return 0;
}

// 初始化服务器
int initialize_server() {
    printf("Initializing math server...\n");

    if (init_winsock() != 0) {
        return 1;
    }

    server_socket = create_socket();
    if (server_socket == INVALID_SOCKET) {
        cleanup_winsock();
        return 1;
    }

    // 设置SO_REUSEADDR选项
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) {
        printf("Setsockopt failed. Error Code: %d\n", WSAGetLastError());
        cleanup_socket(server_socket);
        cleanup_winsock();
        return 1;
    }

    struct sockaddr_in server_address;
    if (setup_server_address(&server_address, PORT) != 0) {
        cleanup_socket(server_socket);
        cleanup_winsock();
        return 1;
    }

    if (bind_socket(server_socket, &server_address) != 0) {
        cleanup_socket(server_socket);
        cleanup_winsock();
        return 1;
    }

    if (start_listening(server_socket, MAX_CLIENTS) != 0) {
        cleanup_socket(server_socket);
        cleanup_winsock();
        return 1;
    }

    printf("Math server initialized successfully on port %d\n", PORT);
    return 0;
}

// 运行服务器主循环
void run_server() {
    struct sockaddr_in client_address;
    int addrlen = sizeof(struct sockaddr_in);

    printf("Server is running and ready for client connections...\n");
    printf("Maximum concurrent clients: %d\n", MAX_CLIENTS);

    while (1) {
        SOCKET client_socket = accept_client_connection(server_socket, &client_address, &addrlen);
        if (client_socket == INVALID_SOCKET) {
            continue;
        }

        // 检查客户端数量
        if (get_client_count() >= MAX_CLIENTS) {
            printf("Server busy. Maximum clients reached. Rejecting connection.\n");
            send_to_client(client_socket, "Server busy. Please try again later.");
            cleanup_socket(client_socket);
            continue;
        }

        // 创建线程参数
        client_thread_params_t* params = (client_thread_params_t*)malloc(sizeof(client_thread_params_t));
        if (params == NULL) {
            printf("Memory allocation failed for thread parameters.\n");
            cleanup_socket(client_socket);
            continue;
        }

        params->client_socket = client_socket;
        params->client_addr = client_address;

        // 创建客户端处理线程
        uintptr_t thread_handle = _beginthreadex(NULL, 0, client_handler, params, 0, NULL);
        if (thread_handle == 0) {
            printf("Failed to create client handler thread.\n");
            free(params);
            cleanup_socket(client_socket);
            continue;
        }

        CloseHandle((HANDLE)thread_handle);
        increment_client_count();

        printf("Active clients: %d\n", get_client_count());
    }
}

// 客户端处理线程函数
unsigned __stdcall client_handler(void* params) {
    client_thread_params_t* thread_params = (client_thread_params_t*)params;
    SOCKET client_socket = thread_params->client_socket;
    struct sockaddr_in client_addr = thread_params->client_addr;

    print_client_info(&client_addr);
    handle_math_communication(client_socket);

    // 清理资源
    cleanup_socket(client_socket);
    free(params);
    decrement_client_count();

    printf("Client disconnected. Active clients: %d\n", get_client_count());
    return 0;
}

// 关闭服务器
void shutdown_server() {
    printf("Shutting down server...\n");
    cleanup_socket(server_socket);
    cleanup_winsock();
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
int setup_server_address(struct sockaddr_in* address, int port) {
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(port);
    printf("Server address configured: 0.0.0.0:%d\n", port);
    return 0;
}

// 绑定socket到端口
int bind_socket(SOCKET sock, struct sockaddr_in* address) {
    if (bind(sock, (struct sockaddr*)address, sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Socket bound to port successfully.\n");
    return 0;
}

// 开始监听连接
int start_listening(SOCKET sock, int backlog) {
    if (listen(sock, backlog) == SOCKET_ERROR) {
        printf("Listen failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Socket listening with backlog %d.\n", backlog);
    return 0;
}

// 接受客户端连接
SOCKET accept_client_connection(SOCKET server_sock, struct sockaddr_in* addr, int* addrlen) {
    printf("Waiting for client connection...\n");
    SOCKET client_sock = accept(server_sock, (struct sockaddr*)addr, addrlen);
    if (client_sock == INVALID_SOCKET) {
        printf("Accept failed. Error Code: %d\n", WSAGetLastError());
    }
    return client_sock;
}

// 接收客户端消息
int receive_from_client(SOCKET client_sock, char* buffer, int buffer_size) {
    int bytes_received = recv(client_sock, buffer, buffer_size - 1, 0);
    if (bytes_received <= 0) {
        if (bytes_received == 0) {
            printf("Client disconnected.\n");
        }
        else {
            printf("Receive error. Error Code: %d\n", WSAGetLastError());
        }
        return -1;
    }

    buffer[bytes_received] = '\0';
    return bytes_received;
}

// 发送消息到客户端
int send_to_client(SOCKET client_sock, const char* message) {
    int bytes_sent = send(client_sock, message, (int)strlen(message), 0);
    if (bytes_sent == SOCKET_ERROR) {
        printf("Send failed. Error Code: %d\n", WSAGetLastError());
        return -1;
    }
    return bytes_sent;
}

// 计算数学表达式
double calculate_math_expression(const char* expression) {
    double num1, num2, result = 0.0;
    char op;
    int parsed = sscanf(expression, "%lf %c %lf", &num1, &op, &num2);

    if (parsed != 3) {
        return -999999;
    }

    switch (op) {
    case '+': result = num1 + num2; break;
    case '-': result = num1 - num2; break;
    case '*': result = num1 * num2; break;
    case '/':
        if (num2 == 0) return -999998;
        result = num1 / num2;
        break;
    case '%': result = (int)num1 % (int)num2; break;
    case '^': result = pow(num1, num2); break;
    default: return -999997;
    }

    return result;
}

// 打印客户端信息
void print_client_info(struct sockaddr_in* client_addr) {
    printf("Math client connected from: %s:%d\n",
        inet_ntoa(client_addr->sin_addr),
        ntohs(client_addr->sin_port));
}

// 处理数学计算通信
void handle_math_communication(SOCKET client_sock) {
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    printf("=== Math Communication Started ===\n");
    send_to_client(client_sock, "Welcome to Math Server! Send math problems (e.g., 5 + 3)");

    while (1) {
        printf("\nWaiting for math problem...\n");
        int receive_result = receive_from_client(client_sock, buffer, BUFFER_SIZE);
        if (receive_result <= 0) break;

        printf("Client math problem: %s\n", buffer);

        if (strcmp(buffer, "quit") == 0 || strcmp(buffer, "exit") == 0) {
            send_to_client(client_sock, "Goodbye! Thanks for using Math Server.");
            break;
        }

        double result = calculate_math_expression(buffer);

        if (result == -999999) {
            snprintf(response, BUFFER_SIZE, "Error: Invalid format. Use: number operator number");
        }
        else if (result == -999998) {
            snprintf(response, BUFFER_SIZE, "Error: Division by zero is not allowed");
        }
        else if (result == -999997) {
            snprintf(response, BUFFER_SIZE, "Error: Unsupported operator. Use: +, -, *, /, %%, ^");
        }
        else {
            snprintf(response, BUFFER_SIZE, "Result: %.6f", result);
        }

        printf("Server response: %s\n", response);

        if (send_to_client(client_sock, response) < 0) break;
    }

    printf("=== Math Communication Ended ===\n");
}

// 增加客户端计数（线程安全）
void increment_client_count() {
    WaitForSingleObject(client_count_mutex, INFINITE);
    active_clients++;
    ReleaseMutex(client_count_mutex);
}

// 减少客户端计数（线程安全）
void decrement_client_count() {
    WaitForSingleObject(client_count_mutex, INFINITE);
    active_clients--;
    ReleaseMutex(client_count_mutex);
}

// 获取客户端计数（线程安全）
int get_client_count() {
    WaitForSingleObject(client_count_mutex, INFINITE);
    int count = active_clients;
    ReleaseMutex(client_count_mutex);
    return count;
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