#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/stat.h>

#define SERVER_PORT 8080
#define DOCUMENT_ROOT "../static/"
#define HTTP_BUFFER_SIZE 1024
#define THREAD_POOL_SIZE 10
#define MAX_CLIENTS 10

typedef struct
{
    int client_socket;
    char client_request;
} ClientArgs;

void *client_handler(void *args)
{
    ClientArgs *client_args = (ClientArgs *) args;
    int client_socket = client_args->client_socket;
    char buffer[HTTP_BUFFER_SIZE];

    ssize_t bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytesReceived < 0)
    {
        perror("Failed To Receive Request!");
        close(client_socket);
    }
    else
    {
        buffer[bytesReceived] = '\0';
        printf("%s\n", buffer);

        // Extract the requested file path from the HTTP request
        char method[10], path[50], http_version[10];
        sscanf(buffer, "%s %s %s", method, path, http_version);

        // Construct the full file path
        char full_path[100];
        if(strcmp(path, "/") == 0)
            snprintf(full_path, sizeof(full_path), "%s%s", DOCUMENT_ROOT, "/index.html");
        else
            snprintf(full_path, sizeof(full_path), "%s%s", DOCUMENT_ROOT, path);

        // Open the requested file
        FILE *file = fopen(full_path, "rb");
        if (file == NULL)
        {
            perror("Failed To Open File!");
            close(client_socket);
            return NULL;
        }

        // Get the file size
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        // Allocate memory for the file content
        char *file_buffer = (char *)malloc(file_size);
        if (file_buffer == NULL)
        {
            perror("Memory Allocation Failed!");
            close(client_socket);
            return NULL;
        }

        // Read the file content into the buffer
        fread(file_buffer, file_size, 1, file);
        fclose(file);

        // Determine the content type based on the file extension
        char *content_type;
        if (strstr(full_path, ".html"))
            content_type = "text/html";
        else if (strstr(full_path, ".css"))
            content_type = "text/css";
        else if (strstr(full_path, ".js"))
            content_type = "application/javascript";
        else if (strstr(full_path, ".jpg") || strstr(full_path, ".jpeg"))
            content_type = "image/jpeg";
        else if (strstr(full_path, ".png"))
            content_type = "image/png";
        else if (strstr(full_path, ".webp"))
            content_type = "image/webp";
        else if (strstr(full_path, ".svg"))
            content_type = "image/svg+xml";
        else if (strstr(full_path, ".ico"))
            content_type = "image/x-icon";
        else if(strstr(full_path, ".json"))
            content_type = "application/json";
        else if(strstr(full_path, ".pdf"))
            content_type = "application/pdf";
        else if(strstr(full_path, ".xml"))
            content_type = "application/xml";
        else if(strstr(full_path, ".zip"))
            content_type = "application/zip";
        else if(strstr(full_path, ".mp3"))
            content_type = "audio/mpeg";
        else if(strstr(full_path, ".mp4"))
            content_type = "video/mp4";
        else
            content_type = "application/octet-stream";

        // Serve the file as response to the client with the necessary HTTP headers
        char http_response[HTTP_BUFFER_SIZE];
        snprintf(http_response, sizeof(http_response), "HTTP/1.1 200 OK\nContent-Type: %s\nContent-Length: %ld\n\n", content_type, file_size);
        send(client_socket, http_response, strlen(http_response), 0);
        send(client_socket, file_buffer, file_size, 0);

        free(file_buffer);
        printf("Web Content Served Successfully!\n\n%s\n", http_response);
    }

    close(client_socket);
    return NULL;
}

int main()
{

    pthread_t clientThread;

    // Create a socket.
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Socket Creation Failed!");
        exit(1);
    }
    
    // Define the address structure.
    struct sockaddr_in address;
    socklen_t address_len = sizeof(address);

    address.sin_family = AF_INET;
    address.sin_port = htons(SERVER_PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket.
    if (bind(server_socket, (struct sockaddr *)&address, address_len) < 0)
    {
        perror("Failed To Bind Socket!");
        close(server_socket);
        exit(1);
    }
    printf("Socket Bound To Port %d\n", SERVER_PORT);

    // Listen for incoming connections.
    if (listen(server_socket, MAX_CLIENTS) < 0)
    {
        perror("Failed To Listen!");
        close(server_socket);
        exit(1);
    }
    printf("Server Listening On Port %d\n", SERVER_PORT);
    
    // Accept incoming client connections.
    int client_socket;
    while (1)
    {
        client_socket = accept(server_socket, (struct sockaddr *)&address, &address_len);
        if (client_socket < 0)
        {
            perror("Failed To Accept Client!");
            close(client_socket);
            continue;
        }
        printf("Connection Successful! _FD: %d\n", client_socket);

        // Initialize thread Arguments.
        ClientArgs *client_args = malloc(sizeof(ClientArgs));
        if (client_args == NULL)
        {
            perror("Memory Allocation Failed!");
            close(client_socket);
            continue;
        }
        client_args->client_socket = client_socket;

        //Create new thread for the client.
        if(pthread_create(&clientThread, NULL, client_handler, client_args) < 0)
        {
            perror("Failed To Create Thread!");
            free(client_args);
            exit(1);
        }

        pthread_detach(clientThread);    
    }
    
    // Close the socket after handling the client
    close(server_socket);
    return 0;
}