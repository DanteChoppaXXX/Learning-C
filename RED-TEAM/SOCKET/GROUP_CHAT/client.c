// INCOMPLETE PROGRAM DUE TO TERMINAL-BASED APPLICATION LIMITATIONS.

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

volatile int is_connected = 1;                    // Shared flag for connection status
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // Mutex for thread safety

// Struct to hold client arguments
typedef struct
{
    int client_socket;
    char *username;
} ClientArgs;

void print_prompt(const char* username) {
    printf("%s #=> ", username);  // Print the label "User1: "
    fflush(stdout);  // Make sure the prompt is displayed before waiting for input
}

// Function for sending messages
void *send_messages(void *args)
{
    ClientArgs *client_args = (ClientArgs *)args;
    int client_socket = client_args->client_socket;
    char *username = client_args->username;
    char client_message[2000];

    while (is_connected)
    {
        print_prompt(username);
        fgets(client_message, sizeof(client_message) - 1, stdin);
        client_message[strcspn(client_message, "\n")] = '\0'; // Remove newline

        char buffer[2000];
        snprintf(buffer, sizeof(buffer), "%s #=> %s\n", username, client_message);

        pthread_mutex_lock(&lock);
        if (send(client_socket, buffer, strlen(buffer), 0) < 0)
        {
            perror("Failed to send message to the server!");
            is_connected = 0;
            pthread_mutex_unlock(&lock);
            break;
        }
        pthread_mutex_unlock(&lock);
    }

    pthread_exit(NULL);
}

// Function for receiving messages
void *receive_messages(void *args)
{
    ClientArgs *client_args = (ClientArgs *)args;
    int client_socket = client_args->client_socket;
    char buffer[1024];
    int bytes_received;

    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[bytes_received] = '\0';
        // printf("\033[1K");  // Clear the entire current line
        // printf("\033[1A");  // Move the cursor up by one line (clear prompt line)
        printf("\r%s", buffer);
        fflush(stdout);
        print_prompt(client_args->username);
    }

    if (bytes_received == 0)
    {
        printf("Server Disconnected!\n");
    }
    else if (bytes_received < 0)
    {
        perror("Error receiving data");
    }

    is_connected = 0;
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
        printf("You must enter a username to proceed!\n");
        return 1;
    }

    // Create the client socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("Failed to create socket");
        return -1;
    }

    // Setup the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection Failed");
        close(client_socket);
        return -1;
    }

    printf("Connection Successful!\n");

    // Send username to server
    send(client_socket, username, strlen(username), 0);

    // Display welcome message from server
    recv(client_socket, server_message, sizeof(server_message), 0);
    printf("%s\n", server_message);

    // Allocate and initialize arguments struct
    ClientArgs *client_args = malloc(sizeof(ClientArgs));
    if (client_args == NULL)
    {
        perror("Memory allocation failed");
        close(client_socket);
        return 1;
    }

    client_args->client_socket = client_socket;
    client_args->username = strdup(username);

    // Create a thread for sending messages
    if (pthread_create(&send_thread, NULL, send_messages, (void *)client_args) != 0)
    {
        perror("Failed to create send thread");
        free(client_args->username);
        free(client_args);
        close(client_socket);
        return 1;
    }

    // Create a thread for receiving messages
    if (pthread_create(&receive_thread, NULL, receive_messages, (void *)client_args) != 0)
    {
        perror("Failed to create receive thread");
        pthread_cancel(send_thread);
        free(client_args->username);
        free(client_args);
        close(client_socket);
        return 1;
    }

    // Wait for threads to finish
    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    // Cleanup
    free(client_args->username);
    free(client_args);
    close(client_socket);

    return 0;
}
