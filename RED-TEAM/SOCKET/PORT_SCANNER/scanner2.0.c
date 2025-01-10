#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>

// Program Variables.
char targetIPOrHostname[100];
int open_Ports = 0;
int closed_Ports = 0;
pthread_mutex_t printMutex, statsMutex;

typedef struct
{
    const char *targetIPOrHostname;
    int *port_List;
    int start;
    int end;
} ThreadData;

// Function to resolve Hostname to IP Address.
void *resolveHostname(char *hostname)
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

// Port scanning Function for TCP used by threads.
void *threadScan(void *arg)
{
    ThreadData *data = (ThreadData *)arg;

    for (int i = data->start; i <= data->end; i++)
    {
        int port = data->port_List[i];
        int tcp_Socket = socket(AF_INET, SOCK_STREAM, 0);

        if (tcp_Socket < 0)
        {
            perror("Socket creation failed!");
            continue; // Skip to the next port
        }

        // Set timeout for receiving and sending.
        struct timeval timeout;
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        setsockopt(tcp_Socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        setsockopt(tcp_Socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

        struct sockaddr_in target_Addr;
        target_Addr.sin_family = AF_INET;
        target_Addr.sin_addr.s_addr = inet_addr(data->targetIPOrHostname);
        target_Addr.sin_port = htons(port);

        int result = connect(tcp_Socket, (struct sockaddr *)&target_Addr, sizeof(target_Addr));
        pthread_mutex_lock(&printMutex);
        if (result == 0)
        {
            printf("Port %d is OPEN!\n", port);
            pthread_mutex_lock(&statsMutex);
            open_Ports++;
            pthread_mutex_unlock(&statsMutex);
        }
        else
        {
            printf("Port %d is CLOSED or FILTERED!\n", port);
            pthread_mutex_lock(&statsMutex);
            closed_Ports++;
            pthread_mutex_unlock(&statsMutex);
        }
        pthread_mutex_unlock(&printMutex);

        close(tcp_Socket);
    }

    pthread_exit(NULL);
}

void performScan(const char *targetIPOrHostname, int *port_List, int size, int numThreads)
{
    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];

    int portsPerThread = size / numThreads;
    int remainingPorts = size % numThreads;

    for (int i = 0; i < numThreads; i++)
    {
        threadData[i].targetIPOrHostname = targetIPOrHostname;
        threadData[i].port_List = port_List;
        threadData[i].start = i * portsPerThread;
        threadData[i].end = (i + 1) * portsPerThread - 1;

        if (i == numThreads - 1)
        {
            threadData[i].end += remainingPorts;
        }

        pthread_create(&threads[i], NULL, threadScan, (void *)&threadData[i]);
    }

    for (int i = 0; i < numThreads; i++)
    {
        pthread_join(threads[i], NULL);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s <hostname or IP> <Option> \nOptions: -s <port> (for single port)\n\t -r <startPort> <endPort> (for range of ports)\n\t -l <port> <port> <port>... (for list of ports)\n", argv[0]);
        return 1;
    }

    pthread_mutex_init(&printMutex, NULL);
    pthread_mutex_init(&statsMutex, NULL);

    if (strcmp(argv[2], "-r") == 0)
    {
        if (argc != 5)
        {
            printf("Error: Provide exactly two numbers for the range\n");
            return 1;
        }

        strcpy(targetIPOrHostname, argv[1]);
        int startPort = atoi(argv[3]);
        int endPort = atoi(argv[4]);

        if (startPort > endPort)
        {
            printf("Error: Start port must be less than or equal to the end port\n");
            return 1;
        }

        if (inet_addr(targetIPOrHostname) == -1)
        {
            resolveHostname(targetIPOrHostname);
        }

        printf("Target IP: %s\n", targetIPOrHostname);
        printf("Scanning ports %d-%d on %s using TCP protocol...\n", startPort, endPort, targetIPOrHostname);

        int size = endPort - startPort + 1;
        int port_List[size];

        for (int i = 0; i < size; i++)
        {
            port_List[i] = startPort++;
        }

        performScan(targetIPOrHostname, port_List, size, 4);
    }
    else if (strcmp(argv[2], "-s") == 0)
    {
        if (argc != 4)
        {
            printf("Error: Provide exactly one port number for single port scan\n");
            return 1;
        }

        strcpy(targetIPOrHostname, argv[1]);
        int port = atoi(argv[3]);

        if (inet_addr(targetIPOrHostname) == -1)
        {
            resolveHostname(targetIPOrHostname);
        }

        printf("Target IP: %s\n", targetIPOrHostname);
        printf("Scanning port %d on %s using TCP protocol...\n", port, targetIPOrHostname);

        int port_List[1] = {port};
        performScan(targetIPOrHostname, port_List, 1, 1);
    }
    else if (strcmp(argv[2], "-l") == 0)
    {
        if (argc < 5)
        {
            printf("Error: Provide a list of port numbers (separated with space)\n");
            return 1;
        }

        strcpy(targetIPOrHostname, argv[1]);
        int size = argc - 3;
        int port_List[size];

        if (inet_addr(targetIPOrHostname) == -1)
        {
            resolveHostname(targetIPOrHostname);
        }

        printf("Target IP: %s\n", targetIPOrHostname);
        printf("Scanning list of ports on %s using TCP protocol...\n", targetIPOrHostname);

        for (int i = 0; i < size; i++)
        {
            port_List[i] = atoi(argv[3 + i]);
        }

        performScan(targetIPOrHostname, port_List, size, 4);
    }

    printf("=====================================\n");
    printf("Scan Summary:\n");
    printf("Open ports: %d\n", open_Ports);
    printf("Closed ports: %d\n", closed_Ports);
    printf("Total ports scanned: %d\n", open_Ports + closed_Ports);
    printf("=====================================\n");

    pthread_mutex_destroy(&printMutex);
    pthread_mutex_destroy(&statsMutex);

    return 0;
}
