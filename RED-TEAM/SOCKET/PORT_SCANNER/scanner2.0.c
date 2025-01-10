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
int startPort, endPort;

typedef struct {
    const char *targetIPOrHostname;
    int *port_List;
    int start;
    int end;
} ThreadData;

// Function to resolve Hostname to IP Address.
void *resolveHostname(char *hostname) {
    struct hostent *host;
    host = gethostbyname(hostname);
    if (host == NULL) {
        printf("Error: %s\n", strerror(errno));
        exit(1);
    }
    strcpy(targetIPOrHostname, inet_ntoa(*((struct in_addr *)host->h_addr_list[0])));
}

// Port scanning Function for TCP used by threads.
void *threadScan(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    // Declare a TCP socket variable.
    int tcp_Socket;
    int open_Ports = 0;
    int closed_Ports = 0;

    for (int i = data->start; i <= data->end; i++) {
        int port = data->port_List[i];
        tcp_Socket = socket(AF_INET, SOCK_STREAM, 0);

        if (tcp_Socket < 0) {
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
        if (result == 0) {
            printf("Port %d is OPEN!\n", port);
            open_Ports++;
        } else {
            printf("Port %d is CLOSED or FILTERED!\n", port);
            closed_Ports++;
        }
        close(tcp_Socket);
    }

    printf("Thread scanned %d ports. Open: %d, Closed: %d\n",
           data->end - data->start + 1, open_Ports, closed_Ports);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <hostname or IP> <Option> \nOptions: -s <port> (for single port)\n\t -r <startPort> <endPort> (for range of ports)\n\t -l <port> <port> <port>... (for list of ports)\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[2], "-r") == 0) {
        if (argc != 5) {
            printf("Error: Provide exactly two numbers for the range\n");
            return 1;
        }

        strcpy(targetIPOrHostname, argv[1]);
        startPort = atoi(argv[3]);
        endPort = atoi(argv[4]);

        if (startPort > endPort) {
            printf("Error: Start port must be less than or equal to the end port\n");
            return 1;
        }

        if (inet_addr(targetIPOrHostname) == -1) {
            resolveHostname(targetIPOrHostname);
        }

        printf("Target IP: %s\n", targetIPOrHostname);
        printf("Scanning ports %d-%d on %s using TCP protocol...\n", startPort, endPort, targetIPOrHostname);

        int size = endPort - startPort + 1;
        int port_List[size];

        for (int i = 0; i < size; i++) {
            port_List[i] = startPort++;
        }

        int numThreads = 4; // Number of threads
        pthread_t threads[numThreads];
        ThreadData threadData[numThreads];

        int portsPerThread = size / numThreads;
        int remainingPorts = size % numThreads;

        for (int i = 0; i < numThreads; i++) {
            threadData[i].targetIPOrHostname = targetIPOrHostname;
            threadData[i].port_List = port_List;
            threadData[i].start = i * portsPerThread;
            threadData[i].end = (i + 1) * portsPerThread - 1;

            if (i == numThreads - 1) {
                threadData[i].end += remainingPorts;
            }

            pthread_create(&threads[i], NULL, threadScan, (void *)&threadData[i]);
        }

        for (int i = 0; i < numThreads; i++) {
            pthread_join(threads[i], NULL);
        }

        printf("Port scanning completed!\n");

    } else {
        printf("Only range scanning (-r) is supported for multithreading at the moment.\n");
    }

    return 0;
}
