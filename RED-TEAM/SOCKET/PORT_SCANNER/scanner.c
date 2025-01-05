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
#include <sys/socket.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>

// Program Variables.
char targetIPOrHostname[100];
int startPort, endPort;
int timeout = 3;

enum Protocol_Type
{
    TCP = 1,
    UDP = 2,
    BOTH = 3 // Work on this later; if(both){ create two separate threads to scan tcp and udp concurrently }
};

int main()
{
    pthread_t thread1, thread2; // Threads for parallel scanning
    int thread1_result, thread2_result;
    // Get the target IP or hostname from the user
    printf("Enter the target IP or hostname: ");

    return 0;
}
