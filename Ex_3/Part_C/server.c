#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>

#include "../Part_B/proactor.h"

#define BUFFER_SIZE 1024



int main(){

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1){
        perror("Socket");
        exit(1);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1){
        perror("Binding");
        close(server_socket);
        exit(1);
    }

    if (listen(server_socket, 5) == -1){
        perror("Listening (error)");
        close(server_socket);
        exit(1);
    }
    printf("Server is listening on 8080!\n");
    while (1)
    {
        int client_socket = accept(server_socket, NULL, NULL);

        printf("Client %d joined the groupchat!\n", client_socket);
        fflush(stdout);

        proactor(client_socket);
        
    }
}