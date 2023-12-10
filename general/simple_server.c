#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT "8080"
#define CONNECTIONS_LIMIT 5

static struct addrinfo *create_address_info(void);
int process_requests(int socket_descriptor);
int fibbonnaci(int n);

int main(void) {
  /* ALLOCATE RESOURCES  */
  struct addrinfo *servinfo;

  int sd; // socket descriptor
  int err;

  servinfo = create_address_info();

  if (!servinfo) {
    return 1;
  }

  /* CREATE SOCKET DESCRIPTOR */
  sd =
      socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

  if (sd == -1) {
    fputs("Creating socket descriptor failed", stderr);
    return 2;
  }

  /* BIND SOCKET DESCRIPTOR TO PORT */
  // The port number is used by the kernel to match an incoming
  //  packet to a certain process’s socket descriptor.
  err = bind(sd, servinfo->ai_addr, servinfo->ai_addrlen);

  if (err == -1) {
    fputs("Binding socket descriptor to port failed", stderr);
    return 3;
  }

  err = listen(sd, CONNECTIONS_LIMIT);

  process_requests(sd);

  return 0;
}

struct addrinfo *create_address_info(void) {
  /* ALLOCATE RESOURCES  */
  struct addrinfo hints;
  struct addrinfo *servinfo, *servinfo_cp;
  void *result;
  int status;

  //
  /* PREPARE FOR ADDRESSES DATA RETRIVAL */
  result = memset(&hints, 0, sizeof(hints));

  if (!result) {
    fputs("Creating addrinfo failed", stderr);
    return NULL;
  }

  hints.ai_socktype = SOCK_STREAM; // TCP
  hints.ai_flags = AI_PASSIVE;     // bind to localhost

  //
  /* GET ADDRESSES DATA */
  status = getaddrinfo(NULL, PORT, &hints, &servinfo);

  if (status != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    return NULL;
  }

  return servinfo;
}

int process_requests(int socket_descriptor) {
  struct sockaddr_storage clientinfo;
  socklen_t client_size;
  int client_sd;
  int err;

  client_size = sizeof(clientinfo);
  err = accept(socket_descriptor, (struct sockaddr *)&clientinfo, &client_size);
  if (err == -1) {
    fputs("Accepting new connection failed", stderr);
    return 1;
  }

  client_sd = err;

  while (1) {
    char buffer[255];

    err = recv(client_sd, &buffer, sizeof(buffer) - 1, 0);

    if (err == -1) {
      fputs("Reciving data failed", stderr);
      perror("recv");
      return 2;
    } else if (err == 0) {
      // Client disconnected
      break;
    }

    printf("Received: %s", buffer);
  }

  return 0;
}

int _fibbonnaci(int n) {

  if (n <= 1) {
    return n;
  }

  return fibbonnaci(n - 2) + fibbonnaci(n - 1);
}
