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

#define PROXY_PORT 8888
#define BUFFER_SIZE 4096
#define MAX_CLIENTS 100
#define BLOCKED_SITES_FILE "./blocked_sites.txt"
#define CACHE_DIR "./cache/"

typedef struct
{
    int client_socket;
    int server_socket;
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
    }
    else if (bytesReceived == 0)
    {
        perror("Connection Close!");
        close(client_socket);
    }
    else{
        buffer[bytesReceived] = '\0';
        printf("%s\n", buffer);

        // Extract the target server and resource from the HTTP request
        char method[10], path[50], http_version[10];
        sscanf(buffer, "%s %s %s", method, path, http_version);
        printf("Host: %s\nResource: %s\nMethod: %s\n", path, http_version, method);

    }
    

}

// Main function.
int main()
{
    // Create a TCP socket.
    int proxy_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(proxy_socket == NULL){
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
        client_socket = accept(proxy_socket, (struct sockaddr *)&proxy_address, &proxy_address_len);

        // Allocate memory for thread arguments.
        ProxyArgs *proxy_args = malloc(sizeof(ProxyArgs));
        if (proxy_args == NULL)
        {
            perror("Memory Allocation Failed!");
            close(proxy_socket);
            exit(1);
        }

        proxy_args->client_socket = client_socket;

        // Create a new thread for each client.
        if(pthread_create(&clientThread, NULL, client_handler, (void *)proxy_args))
        {
            perror("Failed To Create Thread!");
            free(proxy_args);
            exit(1);
        }

        pthread_detach(clientThread);
    }


    // Close the socket after handling the client
    close(proxy_socket);
    return 0;
}