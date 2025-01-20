#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <pthread.h>
#include "server.h"
#include <sys/stat.h>
#include <direct.h>

#pragma comment(lib, "ws2_32.lib") // Linkovanie knižnice Winsock

volatile int running = 1;

void stop_server(int signal) {
    running = 0; // Nastavte na 0 pri signáli (napr. SIGINT)
}

// Inicializácia Winsocku
void initialize_winsock() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code : %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
}

int main() {
    signal(SIGINT, stop_server);

    initialize_winsock();

    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);

    // system("mkdir " FILE_DIR);
    if (mkdir(FILE_DIR) != 0) {
        if (errno == EEXIST) {
            // Directory already exists
            printf("Directory %s already exists.\n", FILE_DIR);
        } else {
            // Error creating directory
            perror("Error creating directory");
        }
    } else {
        printf("Directory %s created successfully.\n", FILE_DIR);
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code : %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code : %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) == SOCKET_ERROR) {
        printf("Listen failed. Error Code : %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Server running on port %d...\n", PORT);

    while (running) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed. Error Code : %d\n", WSAGetLastError());
            continue;
        }

        printf("New client connected.\n");

        pthread_t thread_id;
        SOCKET *client_socket_ptr = malloc(sizeof(SOCKET));
        *client_socket_ptr = client_socket;
        pthread_create(&thread_id, NULL, handle_client, client_socket_ptr);
        pthread_detach(thread_id);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
