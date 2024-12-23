#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080 // Port number for the server
#define BACKLOG 5 // Maximum number of queued connections
#define BUFFER_SIZE 1024

int main()
{

    // Create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Check if socket was created successfully.
    if (server_socket < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Socket created successfully! Socket descriptor: %d\n", server_socket);

    // Define server address.
    struct sockaddr_in address;
    address.sin_family = AF_INET;         // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Listen on any available interface
    address.sin_port = htons(PORT);       // Convert PORT to network byte order

    // Bind the socket.
    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind Failed!\n");
        close(server_socket); // Close the socket if binding fails
        exit(EXIT_FAILURE);
    }

    printf("Socket successfully bound to port %d!\n", PORT);

    // Start listening
    if (listen(server_socket, BACKLOG) > 0)
    {
        perror("Listening Failed!\n");
        close(server_socket); // Close the socket if listening fails
        exit(EXIT_FAILURE);
    }

    printf("Server is Listening on port %d...\n", PORT);

    // Accept incoming connections
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);

    int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);

    if (client_socket < 0)
    {
        perror("Accept failed\n");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Client connected successfully!\n");

    // Buffer for data transfer
    char buffer[BUFFER_SIZE];

    // Receive data from the client
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0)
    {
        buffer[bytes_received] = '\0'; // Null-terminate the received data
        printf("Client: %s\n", buffer);

        // Send a response.
        char *message = "Welcome To The Matrix!";
        ssize_t bytes_sent = send(client_socket, message, strlen(message), 0);
        if (bytes_sent == -1)
        {
            perror("Send Failed");
            close(client_socket);
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        printf("Sent %zd bytes to client.\n", bytes_sent);
    }
    else if (bytes_received == 0)
    {
        printf("Client disconnected!\n");
    }
    else
    {
        perror("Receive failed!");
    }

    // Close the sockets.
    close(client_socket);
    close(server_socket);

    return 0;
}