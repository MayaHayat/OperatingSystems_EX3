#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h> 

#define BUFFER_SIZE 1024

void* handle_keyboard(void *p_client_socket)
{
    int client_socket = *((int*)p_client_socket);
    char buffer[BUFFER_SIZE];

    while(1) {
        bzero(buffer, BUFFER_SIZE);
        printf(">>");
        printf("\n");
        fflush(stdout); // shouldn't get to this, just in case!
        fgets(buffer, BUFFER_SIZE, stdin);


        if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
            perror("Error sending to server");
            exit(1);
        }
    }

    return NULL;
}

int main()
{
    const char *server_address = "127.0.0.1";
    const char *server_port = "8080";

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket");
        exit(1);
    }

    struct addrinfo hints, *server;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int result = getaddrinfo(server_address, server_port, &hints, &server);
    if (result != 0) {
        fprintf(stderr, "Error: %s\n", gai_strerror(result));
        close(client_socket);
        exit(1);
    }

    struct sockaddr_in *server_addr = (struct sockaddr_in *)server->ai_addr;

    if (connect(client_socket, (struct sockaddr *)server_addr, sizeof(*server_addr)) == -1) {
        perror("Connection error");
        close(client_socket);
        freeaddrinfo(server);
        exit(1);
    }

    freeaddrinfo(server);

    
    pthread_t t;
    // Thread for keyboard listening
    pthread_create(&t, NULL, handle_keyboard, (void*)&client_socket);

    
    char server_response[BUFFER_SIZE];
    while(1) {
        bzero(server_response, BUFFER_SIZE);
        ssize_t len = recv(client_socket, server_response, BUFFER_SIZE, 0);
        server_response[len] ='\0';
        if (len == -1) {
            perror("Didn't get server's response!");

            exit(1);
        } else if (len == 0) {
            printf("Lost connection, sorry\n");
            break;
        }
        printf("%s\n", server_response);
        fflush(stdout);
        printf(">>");
        printf("\n");
    }

    // Cleanup
    close(client_socket);
    pthread_cancel(t); 
    pthread_join(t, NULL); 

    return 0;
}
