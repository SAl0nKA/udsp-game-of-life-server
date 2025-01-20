#include <winsock2.h>
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_FILE_TYPE_LEN 16

// Function to send all data
static int send_all(SOCKET socket, const char* buffer, size_t length);

// Function to receive all data
static int recv_all(SOCKET socket, char* buffer, size_t length);

// Function to upload file to the server
void upload(SOCKET client_socket, char filename[BUFFER_SIZE]) {
    char filepath[BUFFER_SIZE];
    snprintf(filepath, BUFFER_SIZE, "%s%s", FILE_DIR, filename);
    FILE *file = fopen(filepath, "wb");
    if (!file) {
        perror("Failed to open file for writing");
        closesocket(client_socket);
        return;
    }

    char buffer[BUFFER_SIZE];
    int bytes_received;
    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
         if (fwrite(buffer, 1, bytes_received, file) != bytes_received)
         {
            perror("Failed to write file data");
            fclose(file);
            closesocket(client_socket);
            return;
        }
    }
    if (bytes_received < 0) {
        perror("Error while receiving file data");
    }
    fclose(file);
     printf("File '%s' uploaded successfully.\n", filename);
}

// Function to download file from the server
void download(SOCKET client_socket, char filename[BUFFER_SIZE]) {
    char filepath[BUFFER_SIZE];
    snprintf(filepath, BUFFER_SIZE, "%s%s", FILE_DIR, filename);

    FILE *file = fopen(filepath, "rb");
    if (!file) {
        perror("Failed to open file for reading");
        closesocket(client_socket);
        return;
    }

     char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (send_all(client_socket, buffer, bytes_read) != 0) {
             perror("Error while sending file data");
             fclose(file);
             closesocket(client_socket);
             return;
        }
    }
    fclose(file);
    printf("File '%s' downloaded successfully.\n", filename);
}

// Function to send the list of files in the server directory
void send_file_list(SOCKET client_socket) {
    DIR *dir;
    struct dirent *entry;
    char buffer[BUFFER_SIZE];
    dir = opendir(FILE_DIR);
    if (dir == NULL) {
        perror("Failed to open directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(buffer, BUFFER_SIZE, "%s", entry->d_name);
            if(send_all(client_socket, buffer, strlen(buffer) + 1) != 0) {
                perror("Failed to send file name");
                closedir(dir);
                return;
            }
        }
    }
    snprintf(buffer, BUFFER_SIZE, "END_OF_LIST");
    if (send_all(client_socket, buffer, strlen(buffer) + 1) != 0) {
        perror("Failed to send end of file list");
    }
    closedir(dir);
}


// Function to handle client requests
void *handle_client(void *arg) {
    SOCKET client_socket = *(SOCKET *)arg;
    free(arg);

    char buffer[BUFFER_SIZE] = {0};
    char command[BUFFER_SIZE], filename[BUFFER_SIZE];

    if (recv(client_socket, buffer, BUFFER_SIZE, 0) <= 0) {
        perror("Error while receiving command");
        closesocket(client_socket);
        return NULL;
    }
    sscanf(buffer, "%s %s", command, filename);
       printf("Received command: %s\n",buffer);

    if (strcmp(command, "UPLOAD") == 0) {
        upload(client_socket, filename);
    } else if (strcmp(command, "DOWNLOAD") == 0) {
        download(client_socket, filename);
    } else if (strcmp(command, "LIST") == 0) {
        send_file_list(client_socket);
    } else {
        printf("Unknown command: %s\n", command);
    }

    closesocket(client_socket);
    return NULL;
}

// Function to send all data
static int send_all(SOCKET socket, const char* buffer, size_t length) {
    size_t sent = 0;
    while (sent < length) {
        int bytes = send(socket, buffer + sent, length - sent, 0);
        if (bytes <= 0) {
             return -1;
        }
        sent += bytes;
    }
    return 0;
}

// Function to receive all data
static int recv_all(SOCKET socket, char* buffer, size_t length) {
      size_t received = 0;
       while(received < length)
      {
          int bytes = recv(socket, buffer + received, length - received, 0);
          if(bytes <= 0) {
             return -1;
           }
            received += bytes;
      }
     return 0;
}