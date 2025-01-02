#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6302
#define MAX_CLIENTS 5

int client_sockets[MAX_CLIENTS];                           // Array to store client sockets.
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for thread safety.

// Struct to store client handler arguments.
typedef struct
{
    int client_socket;
    char filename[100];
    long fileSize;

} ClientArgs;

void *clientHandler(void *args)
{
    ClientArgs *client_args = (ClientArgs *)args;
    int client_socket = client_args->client_socket;

    client_args = malloc(sizeof(ClientArgs));
    if (client_args == NULL)
    {
        perror("Memory allocation failed");
        close(client_socket);
        return NULL;
    }

    char buffer[sizeof(ClientArgs)];

    ssize_t bytesReceived = recv(client_socket, buffer, sizeof(ClientArgs), 0);
    if (bytesReceived <= 0) // Handle errors or closed connections
    {
        if (bytesReceived == 0)
            printf("Connection closed by peer.\n");
        else
            perror("Receive Failed!");

        free(client_args);
        close(client_socket);
        return NULL;
    }

    printf("Received File Details!\n");
    printf("======================\n");

    memcpy(client_args, buffer, sizeof(ClientArgs)); // Copy the buffer into the struct

    // Ensure filename is null-terminated to prevent undefined behavior
    client_args->filename[sizeof(client_args->filename) - 1] = '\0';

    // printf("Filename: %s\nFileSize: %ld bytes\n", client_args->filename, client_args->fileSize);

    char filename[100];
    strncpy(filename, client_args->filename, sizeof(filename));
    long fileSize = client_args->fileSize;

    printf("Filename: %s\nFileSize: %ld bytes\n", filename, fileSize);
    FILE *file;

    file = fopen(filename, "wb");
    if (file == NULL)
    {
        perror("Failed to open file!");
        close(client_socket);
        exit(1);
    }

    printf("File opened successfully!\n");

    // Receive data in chunks: In a loop, receive data from the client using recv() with a fixed buffer size.
    char chunk[1024];
    ssize_t bytes_written;
    ssize_t total_bytes_received = 0;
    while (total_bytes_received < fileSize)
    {
        ssize_t bytes_received = recv(client_socket, chunk, sizeof(chunk), 0);
        if (bytes_received <= 0)
        {
            if (bytes_received == 0)
                printf("Connection closed by peer.\n");
            else
                perror("Receive Failed!");

            fclose(file);
            close(client_socket);
            return NULL;
        }

        bytes_written = fwrite(chunk, 1, bytes_received, file);
        if (bytes_written != bytes_received)
        {
            perror("Failed to write to file!");
            fclose(file);
            close(client_socket);
            return NULL;
        }

        total_bytes_received += bytes_received;
    }
    printf("File received successfully!\n");

    fclose(file);
    free(client_args);
    close(client_socket);
}

int main()
{

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    char filename[100];
    long fileSize;

    // Create the server socket.
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Failed to create socket!\n");
        return -1;
    }

    // Setup the server address structure.
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // Bind the socket to the server IP and Port.
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind Failed!\n");
        return 1;
    }
    printf("Server is ready and bound to port %d\n", SERVER_PORT);
    printf("=======================================\n");

    // Listen for incoming connections.
    listen(server_socket, 3);

    printf("Waiting for incoming connections...\n");
    printf("====================================\n");

    // Accept incoming client connections.
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size)))
    {
        printf("Connection Accepted!\n");
        printf("=====================\n");

        // Add the client socket to the client_sockets array.
        pthread_mutex_lock(&clients_mutex); // Lock the mutex to prevent race conditions.

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (client_sockets[i] == 0)
            {
                client_sockets[i] = client_socket;
                printf("Added new client socket %d at index %d\n", client_socket, i);
                printf("=========================================\n");
                break;
            }
        }

        pthread_mutex_unlock(&clients_mutex);

        // Allocate and initialize arguments struct.
        ClientArgs *client_args = malloc(sizeof(ClientArgs));
        if (client_args == NULL)
        {
            perror("Memory allocation failed");
            continue;
        }

        client_args->client_socket = client_socket;
        strncpy(client_args->filename, filename, sizeof(client_args->filename));
        client_args->fileSize = fileSize;

        // printf("%s\n", username);

        // Create new thread for the client.
        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, clientHandler, (void *)client_args) < 0)
        {
            perror("Failed to create thread!\n");
            free(client_args);
            return 1;
        }

        pthread_detach(client_thread);
        /*Code*/
    }
    if (client_socket < 0)
    {
        perror("Failed to accept client connection!\n");
        return -1;
    }

    // Close the socket after handling the client
    close(server_socket);

    return 0;
}
