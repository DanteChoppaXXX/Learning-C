#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6302
#define MAX_SIZE 20

char path[255];

// Struct to store client handler arguments.
typedef struct
{
    int client_socket;
    char filename[100];
    long fileSize;

} ClientArgs;

// Struct to store user credentials for authentication.
typedef struct
{
    char username[MAX_SIZE];
    char password[MAX_SIZE];
} User;

// Send File Function.
void *sendFile(const int client_socket)
{

    struct stat file_stat;

    printf("Enter the file path of the file you want to send:\n");
    printf("=================================================\n");
    scanf("%s", path);

    if (stat(path, &file_stat) < 0)
    {
        perror("Failed to get file description!\n");
        exit(1);
    }

    // Get the file size.
    long fileSize;
    fileSize = file_stat.st_size;

    // Get the filename.
    char *extractFilename = strrchr(path, '/');
    char *filename = extractFilename + 1;

    printf("%s\n", filename);

    // Allocate memory for the file details struct.
    ClientArgs *client_args = malloc(sizeof(ClientArgs));
    if (client_args == NULL)
    {
        perror("Memory allocation failed");
    }

    client_args->fileSize = fileSize;
    strncpy(client_args->filename, filename, sizeof(client_args->filename));

    // serialize the file details struct before sending.
    char buffer[sizeof(ClientArgs)]; // Buffer to store struct

    memcpy(buffer, client_args, sizeof(ClientArgs)); // Copy the struct into the buffer

    if (send(client_socket, buffer, sizeof(ClientArgs), 0) < 0)
    {
        perror("Sending Failed!");
        exit(1);
    }

    printf("Sending file of %ld bytes...\n", fileSize);

    // Read file in chunks: In a loop, read fixed-size chunks of data from the file.
    FILE *file = fopen(path, "rb");
    if (file == NULL)
    {
        perror("Failed to open file!\n");
        exit(1);
    }
    char chunk[1024];
    size_t bytes_read;
    while ((bytes_read = fread(chunk, 1, sizeof(chunk), file)) > 0)
    {
        if (send(client_socket, chunk, bytes_read, 0) < 0)
        {
            perror("Sending Failed!\n");
            break;
        }
        printf("Sent %ld bytes\n", bytes_read);
    }
    printf("File sent successfully!\n");
    printf("=======================\n");
    fclose(file);
}

// Receive File Function.
void *receiveFile(const int client_socket)
{

    // Receive list of files from the server.
    char fileList[1024];

    printf("Received list of files!\n");
    printf("========================\n");

    ssize_t listBytes; // Define listBytes here for clarity
    listBytes = recv(client_socket, fileList, sizeof(fileList) - 1, 0);

    fileList[listBytes] = '\0'; // Null-terminate the received data

    printf("%s\n", fileList); // Print the received data

    // After exiting the loop, check if there was an error or the connection was closed
    if (listBytes == 0)
    {
        printf("Connection closed by peer.\n");
    }
    else if (listBytes < 0)
    {
        perror("Receive Failed!");
    }

    // Ask the user to choose file to download from the received list.
    char filename[100];
    printf("Enter the name of the file you want to download:\n");
    printf("==================================================\n");
    scanf("%s", filename);
    printf("Sending filename to server...\n");

    if (send(client_socket, filename, strlen(filename), 0) < 0)
    {
        perror("Sending Failed!");
        exit(1);
    }
    printf("Filename sent successfully!\n");

    ClientArgs *client_args = malloc(sizeof(ClientArgs));
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
}

int main()
{

    int client_socket;
    struct sockaddr_in server_addr;

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

    // Prompt user for username and password.
    char username[MAX_SIZE];
    char password[MAX_SIZE];

    printf("Enter username: ");
    scanf("%s", &username);
    printf("\nEnter password: ");
    scanf("%s", &password);

    // Create struct instance.
    User *user = malloc(sizeof(User));
    if (user == NULL)
        {
            perror("Memory allocation failed");
        }

    // Store username and password in the struct
    strncpy(user->username, username, sizeof(user->username)); 
    strncpy(user->password, password, sizeof(user->password));

    // Send the user credential struct to the server.
    char buffer[sizeof(User)];  // Buffer to store struct
    
    memcpy(buffer, user, sizeof(User)); // Copy the struct into the buffer

    if (send(client_socket, buffer, sizeof(User), 0) < 0)
    {
        perror("Sending Failed!");
        exit(1);
    }

    // Receive response from server.
    char server_res[1024];
    int bytes_received = recv(client_socket, server_res, sizeof(server_res), 0);
    if (bytes_received < 0)
    {
        perror("Receiving Failed!");
        exit(1);
    }
    else{
        server_res[bytes_received] = '\0';
        printf("%s\n", server_res);
        if (strcmp(server_res, "Authentication Failed!\n") == 0)
        {
            printf("Disconnected!\n");
            close(client_socket);
            exit(1);
        }
        
    }
    
    

    // Ask the user if they want to send or receive a file.
    int choice;
    printf("Enter 1 to send a file, 2 to receive a file: ");
    scanf("%d", &choice);

    // Send choice to server.
    if (send(client_socket, &choice, sizeof(choice), 0) < 0)
    {
        perror("Sending Failed!\n");
        return -1;
    }

    if (choice == 1)
    {
        sendFile(client_socket);
    }
    else if (choice == 2)
    {
        receiveFile(client_socket);
    }
    else
    {
        printf("Invalid choice!\n");
        close(client_socket);
        return 0;
    }

    printf("Closing Program...\n");

    close(client_socket);

    return 0;
}
