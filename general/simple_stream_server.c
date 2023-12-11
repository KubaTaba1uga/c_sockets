/* `send` and `recv` are used for stream communication. */
/* If You are interested in datagram communication check out  */
/* `sendto` and `recvfrom`. */
#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT "8080"
#define CONNECTIONS_LIMIT 5

static struct addrinfo *create_address_info(void);
int process_requests(int socket_descriptor);
int fibbonnaci(char *n);

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

  // Get rid of `Binding socket descriptor to port failed` msg.
  err = 1;
  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &err, sizeof(err));
  if (err == -1) {
    fputs("Creating socket descriptor failed", stderr);
    perror("setsockopt");
    return 3;
  }

  /* BIND SOCKET DESCRIPTOR TO PORT */
  // The port number is used by the kernel to match an incoming
  //  packet to a certain process’s socket descriptor.
  err = bind(sd, servinfo->ai_addr, servinfo->ai_addrlen);

  if (err == -1) {
    fputs("Binding socket descriptor to port failed", stderr);
    return 4;
  }

  err = listen(sd, CONNECTIONS_LIMIT);

  process_requests(sd);

  close(sd);

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
  /* ALLOCATE RESOURCES  */
  struct sockaddr_storage clientinfo;
  socklen_t client_size;
  int client_sd, fib;
  char *n;
  int err;

  client_size = sizeof(clientinfo);

  //
  /* PROCESS NEW CONNECTION */
  while (1) {
    puts("Waiting for connection...");

    err =
        accept(socket_descriptor, (struct sockaddr *)&clientinfo, &client_size);

    if (err == -1) {
      fputs("Accepting new connection failed", stderr);
      return 1;
    }

    client_sd = err;

    //
    /* RECIVE AND SEND DATA VIA CONNECTION */
    while (1) {
      char buffer[255];

      //
      /* RECIVE DATA */
      err = recv(client_sd, &buffer, sizeof(buffer) - 1, 0);

      if (err == -1) {
        fputs("Reciving data failed", stderr);
        perror("recv");
        return 2;

      } else if (err == 0) {
        fputs("Client discconected\n", stderr);
        break;
      }

      //
      /* CALCULATE N'th FIBBONNACI NUMBER */
      n = strdup(buffer);

      fib = fibbonnaci(buffer);

      free(n);

      if (fib == -1) {
        fputs("Counting fibbonnaci failed", stderr);
        return 3;
      }

      //
      /* SEND DATA */
      sprintf(buffer, "%i\n", fib);

      err = send(client_sd, buffer, strlen(buffer) * sizeof(char), 0);
      if (err == -1) {
        fputs("Sending data failed", stderr);
        perror("send");
        return 2;

      } else if (err == 0) {
        fputs("Client discconected\n", stderr);
        break;
      }
    }

    close(client_sd);
  }

  return 0;
}

int _fibbonnaci(int n) {
  if (n <= 1) {
    return n;
  }

  return _fibbonnaci(n - 2) + _fibbonnaci(n - 1);
}

char *sanitize(char *s) {
  char *org = s;

  while (*s) {
    if (isspace(*s))
      *s = *(s + 1);

    s++;
  }

  return org;
}

int fibbonnaci(char *n) {
  int result;
  int n_i;

  n = sanitize(n);

  n_i = atoi(n);

  if (n_i == 0) {
    return -1;
  }

  return _fibbonnaci(n_i);
}
