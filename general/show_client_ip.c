/* `send` and `recv` are used for stream communication. */
/* If You are interested in datagram communication check out  */
/* `sendto` and `recvfrom`. */
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
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
int get_client_ip(int sockfd, size_t n, char *dst[n]);

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
  int err, bytes_amount, buffer_len;

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
        break;
      } else if (err == 0) {
        fputs("Client discconected\n", stderr);
        break;
      }

      //
      /* GET CLIENT IP */
      n = buffer;
      err = get_client_ip(client_sd, sizeof(buffer), &n);

      if (fib == -1) {
        fputs("Getting client ip failed", stderr);
        break;
      }

      //
      /* SEND DATA */
      while (1) {
        /*  send() returns the number of bytes actually sent out, this might be
           less than the number you told it to send! If the value returned by
           send() doesn’t match the value in len, it’s up to you to send the
           rest of the string.
        */
        buffer_len = strlen(n);
        bytes_amount = send(client_sd, n, buffer_len, 0);
        if (bytes_amount == -1) {
          fputs("Sending data failed", stderr);
          perror("send");
          return 2;
        }

        if (buffer_len == bytes_amount)
          break;

        n = n + bytes_amount;
      }
    }

    close(client_sd);
  }

  return 0;
}

int get_client_ip(int sockfd, size_t n, char *dst[n]) {
  struct sockaddr address_placeholder;
  struct sockaddr_in *local_ipv4;
  struct sockaddr_in6 *local_ipv6;

  void *address;
  int address_size;
  int err;

  address_size = sizeof(struct sockaddr);
  err = getpeername(sockfd, &address_placeholder, &address_size);

  if (err == -1) {
    perror("getpeername");
    return -1;
  }

  // Unpacking ai_addr, is a bit tricky
  if (address_placeholder.sa_family == AF_INET) {
    local_ipv4 = (struct sockaddr_in *)&address_placeholder;
    address = &(local_ipv4->sin_addr);

  } else {
    local_ipv6 = (struct sockaddr_in6 *)&address_placeholder;
    address = &(local_ipv6->sin6_addr);
  }

  address = (void *)inet_ntop(address_placeholder.sa_family, address, *dst, n);

  if (!address) {
    fputs("Convertion from addrinfo to string failed\n", stderr);
    return 4;
  }

  printf("%s\n", *dst);

  return 0;
}
