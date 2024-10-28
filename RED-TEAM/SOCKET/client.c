#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/time.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6302

int main(){

    int client_socket;
    struct sockaddr_in server_addr;
    char server_message[2000];

    // Create the client socket.
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("Failed to create socket!\n");
        return -1;
    }
    
    // Setup the server address structure.
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    // Connect to the server.
    if(connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("Connection Failed!\n");
        return -1;
    }
    printf("Connection Successful!\n");

    // Communicate with the server.

    // char *message;
    // message = "HELLO MATRIX!\n";
    while (1)
    {
        if(recv(client_socket, server_message, strlen(server_message), 0) < 0){
            perror("Couldn't receive message from server!\n");
        }

        printf("Server Message: %s\n", server_message);
    }
    
    

    //close(client_socket);
    

    //printf("%s\n", *message);
    return 0;
}