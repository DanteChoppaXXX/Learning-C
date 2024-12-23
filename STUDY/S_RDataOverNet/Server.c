#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080 // Port number for the server

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
    /* code */

    
    // Close the socket.
    close(server_socket);

    return 0;
}