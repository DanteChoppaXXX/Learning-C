#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <pthread.h>

// Program Variables.
char targetIPOrHostname[100];
int startPort, endPort, threadCount = 4, timeout = 3;
int openPorts = 0, closedPorts = 0, totalPorts = 0;
pthread_mutex_t lock;

// Function to resolve Hostname to IP Address.
void resolveHostname(char *hostname)
{
    struct hostent *host;
    host = gethostbyname(hostname);
    if (host == NULL)
    {
        printf("Error: %s\n", strerror(errno));
        exit(1);
    }
    strcpy(targetIPOrHostname, inet_ntoa(*((struct in_addr *)host->h_addr_list[0])));
}

// Service detection function.
const char *detectService(int port)
{
    switch (port)
    {
    case 22:
        return "SSH";
    case 80:
        return "HTTP";
    case 443:
        return "HTTPS";
    case 21:
        return "FTP";
    case 25:
        return "SMTP";
    default:
        return "Unknown";
    }
}

// Save result to file.
void *saveResult(const char *buffer){
    FILE *file;
    char filename[100];
    sprintf(filename, ".%s.txt", targetIPOrHostname);
    file = fopen(filename, "a");
    fprintf(file, buffer);
    fclose(file);
}

// Port scanning function for TCP.
void *scanPort(void *arg)
{
    int *portList = (int *)arg;
    char *buffer = malloc(1024);

    for (int i = 0; portList[i] != -1; i++)
    {
        int port = portList[i];

        // Create TCP socket.
        int tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (tcpSocket < 0)
        {
            perror("Socket creation failed");
            continue;
        }

        // Set timeout for the socket.
        struct timeval timeoutVal;
        timeoutVal.tv_sec = timeout;
        timeoutVal.tv_usec = 0;
        setsockopt(tcpSocket, SOL_SOCKET, SO_RCVTIMEO, &timeoutVal, sizeof(timeoutVal));
        setsockopt(tcpSocket, SOL_SOCKET, SO_SNDTIMEO, &timeoutVal, sizeof(timeoutVal));

        // Define target address.
        struct sockaddr_in targetAddr;
        targetAddr.sin_family = AF_INET;
        targetAddr.sin_addr.s_addr = inet_addr(targetIPOrHostname);
        targetAddr.sin_port = htons(port);

        // Attempt to connect to the port.
        if (connect(tcpSocket, (struct sockaddr *)&targetAddr, sizeof(targetAddr)) == 0)
        {
            pthread_mutex_lock(&lock);
            printf("Port %d is OPEN (Service: %s)\n", port, detectService(port));
            sprintf(buffer, "Port %d is OPEN (Service: %s)\n", port, detectService(port));
            saveResult(buffer);
            openPorts++;
            pthread_mutex_unlock(&lock);
        }
        else
        {
            pthread_mutex_lock(&lock);
            printf("Port %d is CLOSED or FILTERED\n", port);
            sprintf(buffer, "Port %d is CLOSED or FILTERED\n", port);
            saveResult(buffer);
            closedPorts++;
            pthread_mutex_unlock(&lock);
        }

        close(tcpSocket);

        pthread_mutex_lock(&lock);
        totalPorts++;
        pthread_mutex_unlock(&lock);
    }
    free(buffer);
    return NULL;
}

// Divide ports among threads.
void dividePortsAndScan(int *portList, int size)
{
    pthread_t threads[threadCount];
    int portsPerThread = size / threadCount;
    int remainder = size % threadCount;

    for (int i = 0; i < threadCount; i++)
    {
        int chunkSize = portsPerThread + (i < remainder ? 1 : 0);
        int *chunk = malloc((chunkSize + 1) * sizeof(int));
        if (chunk == NULL)
        {
            perror("Memory allocation failed");
            exit(1);
        }
        
        for (int j = 0; j < chunkSize; j++)
        {
            chunk[j] = portList[i * portsPerThread + j + (i < remainder ? i : remainder)];
        }
        chunk[chunkSize] = -1; // Sentinel value to mark end of chunk.

        if (pthread_create(&threads[i], NULL, scanPort, (void *)chunk) != 0)
        {
            perror("Thread creation failed");
        }
    }

    for (int i = 0; i < threadCount; i++)
    {
        pthread_join(threads[i], NULL);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s <hostname or IP> <Option>\nOptions: -s <port>\n\t-r <startPort> <endPort>\n\t-l <port1> <port2> ...\n\t--timeout <seconds>\n\t--threads <number>\n", argv[0]);
        return 1;
    }

    // Parse additional arguments.
    for (int i = 3; i < argc; i++)
    {
        if (strcmp(argv[i], "--timeout") == 0 && i + 1 < argc)
        {
            timeout = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--threads") == 0 && i + 1 < argc)
        {
            threadCount = atoi(argv[++i]);
        }
    }

    pthread_mutex_init(&lock, NULL);

    if (strcmp(argv[2], "-r") == 0)
    {
        if (argc < 5)
        {
            printf("Error: Provide start and end ports\n");
            return 1;
        }

        strcpy(targetIPOrHostname, argv[1]);
        startPort = atoi(argv[3]);
        endPort = atoi(argv[4]);

        if (startPort > endPort)
        {
            printf("Error: Start port must be <= end port\n");
            return 1;
        }

        if (inet_addr(targetIPOrHostname) == -1)
        {
            resolveHostname(targetIPOrHostname);
        }

        printf("Target IP: %s\n", targetIPOrHostname);
        printf("Scanning ports %d-%d using %d threads and timeout %d seconds\n", startPort, endPort, threadCount, timeout);

        int size = endPort - startPort + 1;
        int portList[size];

        for (int i = 0; i < size; i++)
        {
            portList[i] = startPort + i;
        }

        dividePortsAndScan(portList, size);
    }
    else if (strcmp(argv[2], "-s") == 0)
    {
        if (argc < 4)
        {
            printf("Error: Provide a port\n");
            return 1;
        }

        strcpy(targetIPOrHostname, argv[1]);
        int port = atoi(argv[3]);

        if (inet_addr(targetIPOrHostname) == -1)
        {
            resolveHostname(targetIPOrHostname);
        }

        printf("Target IP: %s\n", targetIPOrHostname);
        printf("Scanning port %d using timeout %d seconds\n", port, timeout);

        int portList[2] = {port, -1};
        dividePortsAndScan(portList, 1);
    }
    else if (strcmp(argv[2], "-l") == 0)
    {
        if (argc < 4)
        {
            printf("Error: Provide a list of ports\n");
            return 1;
        }

        strcpy(targetIPOrHostname, argv[1]);

        int size = argc - 3;
        int portList[size];

        for (int i = 0; i < size; i++)
        {
            portList[i] = atoi(argv[i + 3]);
        }

        if (inet_addr(targetIPOrHostname) == -1)
        {
            resolveHostname(targetIPOrHostname);
        }

        printf("Target IP: %s\n", targetIPOrHostname);
        printf("Scanning %d ports using %d threads and timeout %d seconds\n", size, threadCount, timeout);

        dividePortsAndScan(portList, size);
    }

    printf("\nSummary:\n");
    printf("Open ports: %d\n", openPorts);
    printf("Closed ports: %d\n", closedPorts);
    printf("Total ports scanned: %d\n", totalPorts);

    pthread_mutex_destroy(&lock);
    return 0;
}
