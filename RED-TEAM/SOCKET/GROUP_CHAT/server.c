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
#define USERNAME_SIZE 256
#define BUFFER_SIZE 1024

int client_sockets[MAX_CLIENTS];    // Array to store client sockets.
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex for thread safety.

// Function to broadcast messages to all connected clients.
void broadcastMessage(char *message, int sender_socket){
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_sockets[i] != 0 && client_sockets[i] != sender_socket)
        {
            send(client_sockets[i], message, strlen(message), 0);
        }
        
    }
    pthread_mutex_unlock(&clients_mutex);
    
}

typedef struct
{
    int client_socket;
    char *username;
} ClientArgs;

void *clientHandler(void *args)
{
    ClientArgs *client_args = (ClientArgs *)args;
    int client_socket = client_args->client_socket;
    char *username = client_args->username;
    char buffer[BUFFER_SIZE], *welcomeMessage;

    welcomeMessage = "WELCOME TO THE CHAT ROOM\n==========================\n";
    // Display a welcome message and prompt for a username.
    send(client_socket, welcomeMessage, strlen(welcomeMessage), 0);

    // Handle client communication.
    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);

        // Receive a message from the client.
        int bytes_recieved = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_recieved <= 0)
        {
            close(client_socket);
            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_sockets[i] == client_socket)
                {
                    client_sockets[i] = 0;
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);
            break;
        }

        buffer[bytes_recieved] = '\0';

        // Send the message to the clients.
        broadcastMessage(buffer, client_socket);

    }
    // Close client socket and free memory
    close(client_socket);
    free(client_args);
    pthread_exit(NULL);
}

int main()
{

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    // char *message;
    char *username = malloc(USERNAME_SIZE);
    if (username == NULL)
    {
        perror("Memory allocation failed");
        exit(1);
    }

    // Create server socket.
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

    // Bind the socket to the server IP and port.
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_socket);
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
        pthread_mutex_lock(&clients_mutex);     // Lock the mutex to prevent race conditions.

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
        // Receive data into 'username' buffer.
        int bytes_received = recv(client_socket, username, USERNAME_SIZE - 1, 0);
        if (bytes_received < 0)
        {
            perror("Received Failed!\n");
        }
        else
        {
            // Null-terminate the received string.
            username[bytes_received] = '\0';
            printf("Received username: %s\n", username);
            printf("====================================\n");
        }

        // Free allocated memory when done.

        // Allocate and initialize arguments struct.
        ClientArgs *client_args = malloc(sizeof(ClientArgs));
        if (client_args == NULL)
        {
            perror("Memory allocation failed");
            continue;
        }

        client_args->client_socket = client_socket;
        client_args->username = username;

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
    }
    free(username);
    if (client_socket < 0)
    {
        perror("Failed to accept client connection!\n");
        return -1;
    }

    // Close the socket after handling the client
    close(server_socket);

    return 0;
}
