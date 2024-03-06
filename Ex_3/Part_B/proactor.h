
#ifndef PROACTOR_H
#define PROACTOR_H

#include <pthread.h>

void proactor(int client_socket);
void *handle_client(void *p_client_socket);
void forward(int sender_socket, char *message);

#endif /* PROACTOR_H */


