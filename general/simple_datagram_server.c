/* `sendto` and `recvfrom` are used for datagram communication. */
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

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM; // UDP
  hints.ai_flags = AI_PASSIVE;    // bind to localhost

  //
  /* GET ADDRESSES DATA */
  status = getaddrinfo(NULL, PORT, &hints, &servinfo);

  if (status != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    return NULL;
  }

  return servinfo;
}

int process_requests(int socket_descriptor) { /* ALLOCATE RESOURCES  */

  const size_t buffer_size = 255;
  char buffer[buffer_size];
  struct sockaddr_in client_address; // IPv4
  char *n;
  int err, fib;
  int client_size;

  /* client_address.sin_family = AF_INET; */
  /* client_address.sin_port = htons(atoi(PORT)); */

  while (1) {
    client_size = sizeof(client_address);

    err = recvfrom(socket_descriptor, buffer, buffer_size, 0,
                   (struct sockaddr *)&client_address, &client_size);

    if (err == -1) {
      fputs("Reciving data failed", stderr);
      perror("recvfrom");
      return -1;
    }

    printf("Recfrom: %i\n", err);

    /* puts(buffer); */

    /* err = inet_pton(AF_INET, */
    /*                 // STRING */
    /*                 buffer, */
    /*                 // RESULT PLACEHOLDER */
    /*                 &(client_address.sin_addr)); // IPv4 */

    /* if (err == -1) { */
    /*   fputs("Convertion from str to sockaddr_in failed\n", stderr); */
    /*   perror("inet_pton"); */
    /*   return 1; */
    /* } */

    //
    /* CALCULATE N'th FIBBONNACI NUMBER */
    n = strdup(buffer);

    fib = fibbonnaci(buffer);

    free(n);

    sprintf(buffer, "%i\n", fib);

    puts(buffer);

    if (fib == -1) {
      fputs("Counting fibbonnaci failed", stderr);
      return 3;
    }
    n = buffer;

    err = sendto(socket_descriptor, (const char *)n, strlen(n), 0,
                 (const struct sockaddr *)&client_address, client_size);

    if (err == -1) {
      fputs("Sending data failed\n", stderr);
      perror("sendto");
      return 2;
    }

    printf("Sendto: %i\n", err);

    puts(buffer);
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
