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
void *scanPort_TCP(const char *targetIPOrHostname, const int startPort, const int endPort)
{
    // Declare a TCP socket variable.
    int tcp_Socket;

    // Loop through the range of ports specified (start to end).
    for (int i = startPort; i <= endPort; i++)
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
        target_Addr.sin_port = htons(i);

        // Attempt to connect to the target.
        int result;
        result = connect(tcp_Socket, (struct sockaddr *)&target_Addr, sizeof(target_Addr));
        if (result == 0)
        {
            printf("Port %d is OPEN!\n", i);
            close(tcp_Socket);
        }
        else
        {
            printf("Port %d is CLOSED or FILTERED!\n", i);
            // printf("%s\n", strerror(errno));
            close(tcp_Socket);
        }
    }
    printf("=====================================\n");
    printf("Scan Completed! Closing the socket...\n");
}

// Port scanning Function for UDP.
void *scanPort_UDP(const char *targetIPOrHostname, const int startPort, const int endPort)
{

    // Loop through the range of ports specified (start to end).
    for (int i = startPort; i <= endPort; i++)
    {
        // Create a UDP Socket.
        int udp_Socket = socket(AF_INET, SOCK_DGRAM, 0);

        // Define the target server address.
        struct sockaddr_in target_Addr;
        target_Addr.sin_family = AF_INET;
        target_Addr.sin_addr.s_addr = inet_addr(targetIPOrHostname);
        target_Addr.sin_port = htons(i);

        // Send a small data packet to each port.
        char packet[20] = "@#$&";

        ssize_t data_Sent = sendto(udp_Socket, packet, sizeof(packet), 0, (struct sockaddr *)&target_Addr, sizeof(target_Addr));
        if (data_Sent < 0)
        {
            perror("Failed to send data!");
            close(udp_Socket);
            continue;
        }
        else
        {
            printf("Sent %zu bytes of data to the target!\n", data_Sent);
        }

        // Wait for Response: ICMP "Port Unreachable" response.
        char buffer[20];
        ssize_t response = recvfrom(udp_Socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&target_Addr, (socklen_t *)sizeof(target_Addr));

        if (response < 0)
        {
            printf("Port %d is CLOSED!\n%s", i, strerror(errno));
            close(udp_Socket);
        }
        else
        {
            printf("Port %d might be OPEN or FILTERED!", i);
            close(udp_Socket);
        }
    }
    printf("=====================================\n");
    printf("Scan Completed! Closing the socket...\n");
}

// Port scanning Function for Both TCP and UDP.
void *scanPort_TCP_UDP(const char *targetIPOrHostname, const int startPort, const int endPort)
{
    /* code */
}

int main()
{
    pthread_t thread1, thread2; // Threads for parallel scanning
    int thread1_result, thread2_result;

    // Get the target IP or hostname from the user
    printf("Enter the target IP or hostname: ");
    scanf("%s", targetIPOrHostname);

    // Check if the input is a hostname and resolve it to an IP address
    if (inet_addr(targetIPOrHostname) == -1)
    {
        resolveHostname(targetIPOrHostname);
    }

    printf("Target IP: %s\n", targetIPOrHostname);

    // Validate whether the provided IP or resolved IP is valid and reachable.

    /*Code*/

    // Get the port range from the user
    printf("Enter the start port: ");
    scanf("%d", &startPort);
    printf("Enter the end port: ");
    scanf("%d", &endPort);

    // Get the protocol type from the user
    int protocolType;
    printf("Enter the protocol type (1 for TCP, 2 for UDP, 3 for both): ");
    scanf("%d", &protocolType);

    // Validate the user input
    if (protocolType < 1 || protocolType > 3)
    {
        printf("Invalid protocol type\n");
        return 1;
    }

    char *protocol;

    if (protocolType == 1)
    {
        protocol = "TCP";
        printf("Scanning ports %d-%d on %s using %s protocol...\n", startPort, endPort, targetIPOrHostname, protocol);

        // Call TCP scan function.
        scanPort_TCP(targetIPOrHostname, startPort, endPort);
    }
    else if (protocolType == 2)
    {
        protocol = "UDP";
        printf("Scanning ports %d-%d on %s using %s protocol...\n", startPort, endPort, targetIPOrHostname, protocol);

        // Call TCP scan function.
        scanPort_UDP(targetIPOrHostname, startPort, endPort);
    }
    else
    {
        protocol = "BOTH";
        printf("Scanning ports %d-%d on %s using %s protocols...\n", startPort, endPort, targetIPOrHostname, protocol);

        // Call TCP scan function.
        scanPort_TCP_UDP(targetIPOrHostname, startPort, endPort);
    }

    return 0;
}
