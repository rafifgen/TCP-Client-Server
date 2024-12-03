#include <stdio.h> // printf
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h> // sockaddr_in, INADDR_ANY
#include <sys/socket.h> // socket, bind, listen, accept, AF_INET, SOCK_STREAM
#include <sys/types.h> // htonl, htons, socklen_t
#include <unistd.h> // read, write, close

#define MAX_MESSAGE_SIZE 256

void error(const char *msg)
{
  perror(msg);
  exit(1);
}
int main(int argc, char *argv[])
{
  int sockfd, newsockfd, portno;
  int cli_no, pid, buf_size;
  socklen_t cli_len;
  char buffer[MAX_MESSAGE_SIZE];
  struct sockaddr_in serv_addr, cli_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) error("ERROR opening socket");

  // inisialisasi dengan '\0'
  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = 8080;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  // binding
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error("ERROR on binding");
  if (listen(sockfd, 5));
  cli_len = sizeof(cli_addr);

  while (1) {
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
    if (newsockfd < 0) error("ERROR on accept");

    pid = fork();
    if (pid < 0) error("ERROR in new process creation");

    if (pid == 0) {
      close(sockfd);
      
      // do whatever you want
      bzero(buffer, MAX_MESSAGE_SIZE);
      buf_size = read(newsockfd, buffer, 255);
      if (buf_size < 0) error("ERROR reading from socket");
      printf("Here is the message:\n%s\n", buffer);

      // mengirim balasan
      buf_size = write(newsockfd, "I got your message", 18);
      if (buf_size < 0) error("ERROR writing to socket");
      close(newsockfd);
      exit(1);
    } else {
      close(newsockfd);
    }
  }
}
