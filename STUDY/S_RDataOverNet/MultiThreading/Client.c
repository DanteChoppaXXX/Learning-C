#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1" // Localhost
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main()
{

    // Create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("Socket Creation Failed!");
        exit(1);
    }

    // Define server address
    struct sockaddr_in server_address;
    socklen_t server_addr_len = sizeof(server_address);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(SERVER_PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, server_addr_len) < 0)
    {
        perror("Connection Failed!");
        close(client_socket);
        exit(1);
    }

    printf("Connection Successful!\n");

    // Send data to the server
    char *message = "Hello Matrix!\n";
    ssize_t bytes_sent = send(client_socket, message, strlen(message), 0);
    if (bytes_sent == -1)
    {
        perror("Sending Failed!");
        close(client_socket);
        exit(1);
    }

    printf("Message successfully sent to the server!\n");

    // Receive response from the server
    char buffer[BUFFER_SIZE];

    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received > 0)
    {
        buffer[bytes_received] = '\0'; // NULL-terminate the data
        printf("Server: %s\n", buffer);
    }
    else
    {
        perror("Receive Failed!");
        close(client_socket);
        exit(1);
    }

    // Close the socket
    close(client_socket);
    printf("Disconnected from server.\n");

    return 0;
}