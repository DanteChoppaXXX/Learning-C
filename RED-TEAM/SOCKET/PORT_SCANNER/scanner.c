/*
ROBUST PORT SCANNER PROGRAM BY DANTECHOPPAXXX.
==============================================
This program scans a specific range of ports on a target IP or hostname to determine which ports are open, closed, or filtered, then saves the results in a txt file.

*/

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

enum Protocol_Type
{
    TCP = 1,
    UDP = 2,
    BOTH = 3 // Work on this later; if(both){ create two separate threads to scan tcp and udp concurrently }
};

enum Port_Status
{
    OPEN = 1,
    CLOSED = 2,
    FILTERED = 3
};

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

// Port scanning Function for TCP.
void *scanPort(const char *targetIPOrHostname, int *port_List, int size)
{
    // Declare a TCP socket variable.
    int tcp_Socket;

    // Port count variables.
    int total_Ports_Scanned = 0;
    int open_Ports = 0;
    int closed_Ports = 0;

    // Loop through the range of ports specified (start to end).
    for (int i = 0; i < size; i++)
    {
        // For each port create a TCP socket.
        tcp_Socket = socket(AF_INET, SOCK_STREAM, 0);

        // Check if socket was created successfully.
        if (tcp_Socket < 0)
        {
            perror("Socket creation failed!");
            exit(1);
        }

        // printf("TCP socket created! Socket descriptor: %d\n", tcp_Socket);

        // Set timeout for receiving and sending.
        struct timeval timeout;

        timeout.tv_sec = 3;  // 3 seconds timeout
        timeout.tv_usec = 0; // 0 microseconds
        setsockopt(tcp_Socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        setsockopt(tcp_Socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

        // Define the target server address.
        struct sockaddr_in target_Addr;
        target_Addr.sin_family = AF_INET;
        target_Addr.sin_addr.s_addr = inet_addr(targetIPOrHostname);
        target_Addr.sin_port = htons(port_List[i]);

        // Attempt to connect to the target.
        int result;
        result = connect(tcp_Socket, (struct sockaddr *)&target_Addr, sizeof(target_Addr));
        if (result == 0)
        {
            printf("Port %d is OPEN!\n", port_List[i]);
            close(tcp_Socket);
            total_Ports_Scanned++;
            open_Ports++;
        }
        else
        {
            printf("Port %d is CLOSED or FILTERED!\n", port_List[i]);
            // printf("%s\n", strerror(errno));
            close(tcp_Socket);
            total_Ports_Scanned++;
            closed_Ports++;
        }
    }
    printf("=====================================\n");
    printf("Scan Completed! Closing the socket...\n");
    printf("=====================================\n");
    printf("Open ports: %d\n", open_Ports);
    printf("Closed ports: %d\n", closed_Ports);
    printf("Total ports scanned: %d\n", total_Ports_Scanned);
}

int main(int argc, char *argv[])
{
    // Check for sufficient number of arguments.
    if (argc < 3)
    {
        printf("Usage: %s <hostname or IP> <Option> \nOptions: -s <port> (for single port)\n\t -r <startPort> <endPort> (for range of ports)\n\t -l <port> <port> <port>... (for list of ports)\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[2], "-r") == 0)
    {
        // Check for correct number of arguments.
        if (argc != 5)
        {
            printf("Error: Provide exactly two numbers for the range\n");
            return 1;
        }

        strcpy(targetIPOrHostname, argv[1]);
        startPort = atoi(argv[3]);
        endPort = atoi(argv[4]);

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

        // Populate the port list.
        for (int i = 0; i < size; i++)
        {
            port_List[i] = startPort;
            startPort++;
        }

        // Validate whether the provided IP or resolved IP is valid and reachable.

        /*Code*/

        // Call TCP scan function.
        scanPort(targetIPOrHostname, port_List, size);
    }
    else if (strcmp(argv[2], "-s") == 0)
    {
        // Check for correct number of arguments.
        if (argc != 4)
        {
            printf("Error: Provide exactly one port number for single port scan\n");
            return 1;
        }

        strcpy(targetIPOrHostname, argv[1]);
        int port = atoi(argv[3]);

        if (port < 0)
        {
            printf("Error: Port number must be between 0-65535\n");
            return 1;
        }

        // Check if the input is a hostname and resolve it to an IP address
        if (inet_addr(targetIPOrHostname) == -1)
        {
            resolveHostname(targetIPOrHostname);
        }

        printf("Target IP: %s\n", targetIPOrHostname);

        printf("Scanning port %d on %s using TCP protocol...\n", port, targetIPOrHostname);

        int size = 1;

        // Call TCP scan function.
        scanPort(targetIPOrHostname, &port, size);
    }
    else if (strcmp(argv[2], "-l") == 0)
    {
        // Check for correct number of arguments.
        if (argc < 5)
        {
            printf("Error: Provide a list of port numbers (separated with space)\n");
            return 1;
        }

        strcpy(targetIPOrHostname, argv[1]);

        int size = argc - 3;
        printf("%d\n", size);
        int port_List[size];

        // Check if the input is a hostname and resolve it to an IP address
        if (inet_addr(targetIPOrHostname) == -1)
        {
            resolveHostname(targetIPOrHostname);
        }

        printf("Target IP: %s\n", targetIPOrHostname);

        printf("Scanning lists of ports on %s using TCP protocol...\n", targetIPOrHostname);

        // Populate the port list.
        for (int i = 0; i < size; i++)
        {
            port_List[i] = atoi(argv[3 + i]);
        }

        // Validate whether the provided IP or resolved IP is valid and reachable.

        /*Code*/

        // Call TCP scan function.
        scanPort(targetIPOrHostname, port_List, size);
    }

    // pthread_t thread1, thread2; // Threads for parallel scanning
    // int thread1_result, thread2_result;

    // Check if the input is a hostname and resolve it to an IP address

    return 0;
}
