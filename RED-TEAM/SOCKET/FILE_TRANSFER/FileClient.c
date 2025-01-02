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

char path[255];

// Struct to store client handler arguments.
typedef struct
{
    int client_socket;
    char filename[100];
    long fileSize;

} ClientArgs;

int main()
{

    int client_socket;
    struct sockaddr_in server_addr;
    struct stat file_stat;

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

    printf("Enter the file path of the file you want to send:\n");
    printf("=================================================\n");
    scanf("%s", path);

    if (stat(path, &file_stat) < 0)
    {
        perror("Failed to get file description!\n");
        return 1;
    }

    // Get the file size.
    long fileSize;
    fileSize = file_stat.st_size;

    // Get the filename.
    char *extractFilename = strrchr(path, '/');
    char *filename = extractFilename + 1;

    // Remove the file extension.
    char *extension = strrchr(filename, '.');
    if (extension != NULL)
    {
        strncpy(extension, ".bin", 4);
        //*extension = '\0';
    }
    else
    {
        printf("No file extension found.\n");
    }

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
        return 1;
    }

    printf("Sending file of %ld bytes...\n", fileSize);
    // sendFile(client_socket, filename, fileSize);

    // Read file in chunks: In a loop, read fixed-size chunks of data from the file.
    FILE *file = fopen(path, "rb");
    if (file == NULL)
    {
        perror("Failed to open file!\n");
        return 1;
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
    fclose(file);

    printf("Closing Program...\n");

    close(client_socket);

    return 0;
}