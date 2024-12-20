#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

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

    // Close the socket.
    close(server_socket);

    return 0;
}