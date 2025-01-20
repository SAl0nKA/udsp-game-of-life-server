//
// Created by Salonka on 19. 1. 2025.
//

#ifndef SERVER_H
#define SERVER_H



#define PORT 8080
#define BUFFER_SIZE 1024
#define FILE_DIR "./server_files/"


void *handle_client(void *arg);
void upload(SOCKET client_socket, char filename[BUFFER_SIZE]);
void download(SOCKET client_socket, char filename[BUFFER_SIZE]);
void send_file_list(SOCKET client_socket);

#endif //SERVER_H
