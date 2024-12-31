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

#pragma pack(1) // Disable padding
typedef struct
{
    char *filename;
    long fileSize;
} FileDetails;
#pragma pack()

// int sendFile(int client_socket, char *filename, long fileSize){

//     send(client_socket, filename, strlen(filename), 0);
//     send(client_socket, fileSize, strlen(fileSize), 0);

//     return 0;
// }

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

    // Get the filename.
    char *extractFilename = strrchr(path, '/');
    char *filename = extractFilename++;

    printf("%s\n", filename);
    if (stat(path, &file_stat) < 0)
    {
        perror("Failed to get file description!\n");
        return 1;
    }

    // Get the file size.
    long fileSize;
    fileSize = file_stat.st_size;

    // Allocate memory for the file details struct.
    FileDetails *fileDetails = malloc(sizeof(FileDetails));
    if (fileDetails == NULL)
    {
        perror("Memory allocation failed");
    }

    fileDetails->fileSize = fileSize;
    fileDetails->filename = filename;

    // serialize the file details struct before sending.
    char buffer[sizeof(FileDetails)]; // Buffer to store struct

    memcpy(buffer, fileDetails, sizeof(FileDetails)); // Copy the struct into the buffer

    if (send(client_socket, buffer, sizeof(FileDetails), 0) < 0)
    {
        perror("Sending Failed!");
        return 1;
    }

    printf("Sending file of %ld bytes...\n", fileSize);
    // sendFile(client_socket, filename, fileSize);

    printf("Closing Program...\n");

    close(client_socket);

    return 0;
}