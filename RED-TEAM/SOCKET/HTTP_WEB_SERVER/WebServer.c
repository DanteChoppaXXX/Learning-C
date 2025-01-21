#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/stat.h>

#define SERVER_PORT 8080
#define DOCUMENT_ROOT "./static/"
#define HTTP_BUFFER_SIZE 1024
#define THREAD_POOL_SIZE 10
#define MAX_CLIENTS 10

typedef struct{
    int client_socket;
    char client_request;
} ClientArgs;

void *client_handler(void *args){
    ClientArgs *client_args = (ClientArgs *) args;
    int client_socket = client_args->client_socket;
    char buffer[HTTP_BUFFER_SIZE];

    ssize_t bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytesReceived < 0)
    {
        perror("Failed To Receive Request!");
        close(client_socket);
    }
    else{
        buffer[bytesReceived] = '\0';
        printf("%s\n", buffer);
    }
    
    close(client_socket);
}

int main(){

    pthread_t clientThread;

    // Create a socket.
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Socket Creation Failed!");
        exit(1);
    }
    
    // Define the address structure.
    struct sockaddr_in address;
    socklen_t address_len = sizeof(address);

    address.sin_family = AF_INET;
    address.sin_port = htons(SERVER_PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket.
    if (bind(server_socket, (struct sockaddr *)&address, address_len) < 0)
    {
        perror("Failed To Bind Socket!");
        close(server_socket);
        exit(1);
    }
    printf("Socket Bound To Port %d\n", SERVER_PORT);

    // Listen for incoming connections.
    if (listen(server_socket, MAX_CLIENTS) < 0)
    {
        perror("Failed To Listen!");
        close(server_socket);
        exit(1);
    }
    printf("Server Listening On Port %d\n", SERVER_PORT);
    
    // Accept incoming client connections.
    int client_socket;
    while (1)
    {
        client_socket = accept(server_socket, (struct sockaddr *)&address, &address_len);
        if (client_socket < 0)
        {
            perror("Failed To Accept Client!");
            close(client_socket);
            continue;
        }
        printf("Connection Successful! _FD: %d\n", client_socket);

        // Initialize thread Arguments.
        ClientArgs *client_args = malloc(sizeof(ClientArgs));
        if (client_args == NULL)
        {
            perror("Memory Allocation Failed!");
            close(client_socket);
            continue;
        }
        client_args->client_socket = client_socket;

        //Create new thread for the client.
        if(pthread_create(&clientThread, NULL, client_handler, client_args) < 0)
        {
            perror("Failed To Create Thread!");
            free(client_args);
            exit(1);
        }

        pthread_detach(clientThread);    
    }
    
    // Close the socket after handling the client
    close(server_socket);
    return 0;
}