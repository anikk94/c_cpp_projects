/*
  multi process server. can deal with multiple client connections 
  at a time. spawns a new process for each new connection with fork()

  do stuff is where the server does something with the communication
  coming from each client
*/
#include <cstdlib>
#include <iostream>
#include <cstring>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

void error(char *msg)
{
  perror(msg);
  exit(1);
}

/******** DOSTUFF() *********************
 There is a separate instance of this function
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
int dostuff(int sock)
{
  cout << __PRETTY_FUNCTION__ << endl;

  int n;

  char buffer[256];

  // bzero(buffer, 256);
  memset(buffer, 0, 256);

  n = read(sock, buffer, 255);

  if (n < 0)
    error("ERROR reading from socket");

  printf("Here is the message %s", buffer);

  n = write(sock, "I got your message", 18);

  if (n < 0)
    error("ERROR writing to socket");
}

int main(int argc, char *argv[])
{
  cout << __PRETTY_FUNCTION__ << endl;

  int sockfd, newsockfd, portno, pid;

  socklen_t clilen;

  struct sockaddr_in serv_addr, cli_addr;

  if (argc < 2)
  {
    fprintf(stderr, "ERROR, no port provided");
    exit(1);
  }

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0)
  {
    error("ERROR opening socket");
  }

  // bzero((char*) &serv_addr, sizeof(serv_addr));
  memset((char *)&serv_addr, 0, sizeof(serv_addr));

  portno = atoi(argv[1]);

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(portno);
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    error("ERROR on binding");
  }

  listen(sockfd, 5);

  clilen = sizeof(cli_addr);

  cout << "waiting for clients" << endl;

  while (1)
  {
    newsockfd = accept(sockfd,
                       (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
      error("ERROR on accept");
    pid = fork();
    if (pid < 0)
      error("ERROR on fork");
    if (pid == 0)
    {
      close(sockfd);
      dostuff(newsockfd);
      exit(0);
    }
    else
      close(newsockfd);
  } /* end of while */

  return 0;
}
