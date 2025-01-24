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
#include <netdb.h>


#define PROXY_PORT 8888
#define BUFFER_SIZE 4096
#define MAX_CLIENTS 100
#define BLOCKED_SITES_FILE "./blocked_sites.txt"
#define CACHE_DIR "./cache/"

typedef struct
{
    int client_socket;
    int server_socket;
    struct sockaddr_in client_address;
    socklen_t client_address_len;
} ProxyArgs;

// Client handler function.
void *client_handler(void *args){
    ProxyArgs *proxy_args = (ProxyArgs *)args;
    int client_socket = proxy_args->client_socket;
    char buffer[BUFFER_SIZE];   // Buffer to store client request

    ssize_t bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytesReceived < 0)
    {
        perror("Failed To Receive Request!");
        close(client_socket);
        free(proxy_args);
        return NULL;
    }
    else if (bytesReceived == 0)
    {
        perror("Connection Closed!");
        close(client_socket);
        free(proxy_args);
        return NULL;
    }
    else{
        buffer[bytesReceived] = '\0';
        printf("%s\n", buffer);

        // Extract the target host and resource URI from the HTTP request Header.

        /* code */


        // Forward the request to the target server and send the response back to the client.
        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket < 0)
        {
            perror("Failed To Create Server Socket!");
            close(client_socket);
            free(proxy_args);
            return NULL;
        }

        // Define the address structure for the target server.
        struct sockaddr_in server_address;
        socklen_t server_address_len = sizeof(server_address);

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(80);

        struct hostent *host = gethostbyname(path);
        if (host == NULL)
        {
            perror("Failed To Get Host By Name!");
            close(client_socket);
            close(server_socket);
            free(proxy_args);
            return NULL;
        }

        memcpy(&server_address.sin_addr, host->h_addr_list[0], host->h_length);

        // Connect to the target server.
        if (connect(server_socket, (struct sockaddr *)&server_address, server_address_len) < 0)
        {
            perror("Failed To Connect To Server!");
            close(client_socket);
            close(server_socket);
            free(proxy_args);
            return NULL;
        }

        // Forward the client request to the target server.
        if (send(server_socket, buffer, bytesReceived, 0) < 0)
        {
            perror("Failed To Forward Request To Server!");
            close(client_socket);
            close(server_socket);
            free(proxy_args);
            return NULL;
        }

        // Receive the response from the target server.
        ssize_t bytesSent;
        while ((bytesSent = recv(server_socket, buffer, sizeof(buffer), 0)) > 0)
        {
            if (send(client_socket, buffer, bytesSent, 0) < 0)
            {
                perror("Failed To Forward Response To Client!");
                close(client_socket);
                close(server_socket);
                free(proxy_args);
                return NULL;
            }
        }

        if (bytesSent < 0)
        {
            perror("Failed To Receive Response From Server!");
            close(client_socket);
            close(server_socket);
            free(proxy_args);
            return NULL;
        }

        close(server_socket);

        printf("Request Forwarded Successfully!\n");

    }

    close(client_socket);
    free(proxy_args);
    return NULL;
}

// Main function.
int main()
{
    // Create a TCP socket.
    int proxy_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(proxy_socket < 0){
        perror("Failed To Create Socket!");
        exit(1);
    }

    // Define the address structure.
    struct sockaddr_in proxy_address;
    socklen_t proxy_address_len = sizeof(proxy_address);

    proxy_address.sin_family = AF_INET;
    proxy_address.sin_addr.s_addr = INADDR_ANY;
    proxy_address.sin_port = htons(PROXY_PORT);

    // Bind the socket.
    if (bind(proxy_socket, (struct sockaddr *)&proxy_address, proxy_address_len) < 0)
    {
        perror("Socket Binding Failed!");
        close(proxy_socket);
        exit(1);
    }
    printf("Socket Bound To Port %d\n", PROXY_PORT);

    // Listen for incoming connections.
    if (listen(proxy_socket, MAX_CLIENTS) < 0)
    {
        printf("Failed To Listen On Port %d!\n", PROXY_PORT);
        close(proxy_socket);
        exit(1);
    }
    printf("Proxy Server Listening On Port %d...\n", PROXY_PORT);

    // Accept incoming connections.
    int client_socket;
    pthread_t clientThread;

    while (1)
    {
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);
        client_socket = accept(proxy_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket < 0)
        {
            perror("Failed To Accept Connection!");
            continue;
        }

        // Allocate memory for thread arguments.
        ProxyArgs *proxy_args = malloc(sizeof(ProxyArgs));
        if (proxy_args == NULL)
        {
            perror("Memory Allocation Failed!");
            close(client_socket);
            continue;
        }

        proxy_args->client_socket = client_socket;
        proxy_args->client_address = client_address;
        proxy_args->client_address_len = client_address_len;

        // Create a new thread for each client.
        if(pthread_create(&clientThread, NULL, client_handler, (void *)proxy_args))
        {
            perror("Failed To Create Thread!");
            close(client_socket);
            free(proxy_args);
            continue;
        }

        pthread_detach(clientThread);
    }

    // Close the socket after handling the client
    close(proxy_socket);
    return 0;
}