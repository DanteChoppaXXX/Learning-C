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

typedef struct
{
    int client_socket;
    char *username;
} ClientArgs;

void *send_messages(void *args)
{
    ClientArgs *client_args = (ClientArgs *)args;
    int client_socket = client_args->client_socket;
    char *username = client_args->username;
    char client_message[2000], server_message[2000];

    while (1)
    {

        // Get a message to send to the server.
        printf("%s #=> ", username);
        fgets(client_message, sizeof(client_message), stdin);

        char buffer[2000];
        snprintf(buffer, sizeof(buffer), "%s #=> %s\n", username, client_message);

        // Send the message to the server.
        if (send(client_socket, buffer, strlen(buffer), 0) < 0)
        {
            perror("Failed to send message to the server!");
            break;
        }

        // memset(server_message, 0, sizeof(server_message));

        // if (recv(client_socket, server_message, sizeof(server_message), 0) < 0)
        // {
        //     perror("Couldn't receive message from server!\n");
        //     break;
        // }

        // printf("%s\n", server_message);
    }

    pthread_exit(NULL);
}

void *receive_messages(void *args)
{
    ClientArgs *client_args = (ClientArgs *)args;
    int client_socket = client_args->client_socket;
    char *username = client_args->username;
    char buffer[1024];
    int bytes_received;

    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
    }

    if (bytes_received == 0)
    {
        printf("Server Disconnected!\n");
    }
    else if (bytes_received < 0)
    {
        perror("Error receiving data\n");
    }

    pthread_exit(NULL);
}

int main()
{

    char username[100];
    int client_socket;
    struct sockaddr_in server_addr;
    char server_message[2000];
    pthread_t receive_thread, send_thread;

    printf("Choose a username to chat with: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    if (strlen(username) < 3)
    {
        printf("You must enter a username to process!\n");
        return 1;
    }

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
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection Failed!\n");
        printf("==================\n");
        return -1;
    }
    printf("Connection Successful!\n");
    printf("======================\n");
    // Send username to server.
    send(client_socket, username, strlen(username), 0);

    // char *message;
    // message = "HELLO MATRIX!\n";
    // Communicate with the server.
    // Display welcome message from server.
    recv(client_socket, server_message, sizeof(server_message), 0);
    printf("%s\n", server_message);

    // Allocate and initialize arguments struct.
    ClientArgs *client_args = malloc(sizeof(ClientArgs));
    if (client_args == NULL)
    {
        perror("Memory allocation failed");
        return 1;
    }

    client_args->client_socket = client_socket;
    client_args->username = username;

    // Create a thread for sending messages.
    if (pthread_create(&send_thread, NULL, send_messages, (void *)client_args) != 0)
    {
        perror("Failed to create thread!\n");
        free(client_args);
        return 1;
    }

    // Create a thread for receiving messages.
    if (pthread_create(&receive_thread, NULL, receive_messages, (void *)client_args) != 0)
    {
        perror("Failed to create thread!\n");
        free(client_args);
        return 1;
    }

    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    // Clean Up.
    free(client_args);
    close(client_socket);

    return 0;
}