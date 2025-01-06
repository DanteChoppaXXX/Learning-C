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
int timeout;

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

// Port scanning Function.
void *scanPort(const char *targetIPOrHostname, const int startPort, const int endPort)
{

    /*Code*/
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
    }
    else if (protocolType == 2)
    {
        protocol = "UDP";
    }
    else
    {
        protocol = "BOTH";
    }

    // Get the timeout value from the user
    printf("Enter the timeout value (in seconds): ");
    scanf("%d", &timeout);

    printf("Scanning ports %d-%d on %s using %s protocol, with %d seconds timeout...\n", startPort, endPort, targetIPOrHostname, protocol, timeout);

    return 0;
}
