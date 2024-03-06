#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>

#include "proactor.h"

#define BUFFER_SIZE 1024

int *clients = NULL;
int num_clients = 0;
int max_clients = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


// Forward message to all clients but the one who sent it
void forward(int sender_socket, char *message){
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < num_clients; i++)
    {
        int client_socket = clients[i];
        if (client_socket != sender_socket)
        {
            char formatted_message[BUFFER_SIZE];
            snprintf(formatted_message, BUFFER_SIZE, "Client %d: %s", sender_socket, message);
            send(client_socket, formatted_message, strlen(formatted_message), 0);
        }
    }
    pthread_mutex_unlock(&mutex);
}

void *handle_client(void *p_client_socket){
    int client_socket = *(int *)p_client_socket;
    free(p_client_socket);

    pthread_mutex_lock(&mutex);
    clients = realloc(clients, (num_clients + 1) * sizeof(int));
    if (!clients){
        perror("Error reallocating memory");
        exit(1);
    }
    clients[num_clients++] = client_socket;
    pthread_mutex_unlock(&mutex);

    char message[BUFFER_SIZE];

    while (1){
        bzero(message, BUFFER_SIZE);
        ssize_t bytes_received = recv(client_socket, message, BUFFER_SIZE, 0);
        if (bytes_received <= 0)
        {
            printf("Client %d left!\n", client_socket);

            pthread_mutex_lock(&mutex);
            for (int i = 0; i < num_clients; i++)
            {
                if (clients[i] == client_socket)
                {
                    for (int j = i; j < num_clients - 1; j++)
                    {
                        clients[j] = clients[j + 1];
                    }
                    num_clients--;
                    break;
                }
            }

            // Notify all clients that a client has left
            for (int i = 0; i < num_clients; i++){
                char leave_message[BUFFER_SIZE];
                snprintf(leave_message, BUFFER_SIZE, "Client %d has left!", client_socket);
                send(clients[i], leave_message, strlen(leave_message), 0);
            }

            pthread_mutex_unlock(&mutex);

            close(client_socket);
            pthread_exit(NULL);
        }
        message[bytes_received - 1] = '\0';
        printf("Client %d: %s\n", client_socket, message);
        fflush(stdout);
        forward(client_socket, message);
    }
    return NULL;
}


void proactor(int client_socket){
    pthread_t t;
    int *p_client_sock = malloc(sizeof(int));
    *p_client_sock = client_socket;
    pthread_create(&t, NULL, handle_client, p_client_sock);
}