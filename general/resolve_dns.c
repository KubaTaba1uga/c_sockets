#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#ifndef ENABLE_TLS
#define ENABLE_TLS 0
#endif

int main(int argc, char *argv[]) {
  /* ALLOCATE RESOURCES  */
  char *dns_value;
  char ip[INET6_ADDRSTRLEN];
  const char *result;

  int status;
  void *address;
  struct addrinfo hints;
  struct addrinfo *servinfo, *servinfo_cp;
  struct sockaddr_in *local_ipv4;
  struct sockaddr_in6 *local_ipv6;

  //
  /* PARSE CLI ARGS */
  if (argc != 2) {
    puts("Usage:\n <exe> <dns address>");
    return 1;
  }
  dns_value = argv[1];

  //
  /* PREPARE CONNECTION DATA */
  result = memset(&hints, 0, sizeof(hints));

  if (!result) {
    fputs("Creating addrinfo failed", stderr);
    return 2;
  }

  hints.ai_family = AF_UNSPEC;     // don't care IPV6 or IPV4
  hints.ai_socktype = SOCK_STREAM; // TCP
  hints.ai_flags = AI_PASSIVE;     // find IP for me

  //
  /* GET CONNECTION DATA */
  status = getaddrinfo(
      // IP or DNS
      dns_value,
      // PROTO or PORT NUMBER
      ENABLE_TLS ? "443" : "80",
      // CONNECTION SETTINGS (connection is not established yet!)
      &hints,
      // RESULT PLACEHOLDER
      &servinfo);

  if (status != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    return 3;
  }

  //
  /* SHOW CONNECTION DATA */
  servinfo_cp = servinfo;
  while (servinfo) {

    // Unpacking ai_addr, is a bit tricky
    if (servinfo->ai_family == AF_INET) {
      local_ipv4 = (struct sockaddr_in *)servinfo->ai_addr;
      address = &(local_ipv4->sin_addr);

    } else {
      local_ipv6 = (struct sockaddr_in6 *)servinfo->ai_addr;
      address = &(local_ipv6->sin6_addr);
    }

    result = inet_ntop(servinfo->ai_family, address, ip, sizeof(ip));

    if (!result) {
      fputs("Convertion from addrinfo to string failed\n", stderr);
      return 4;
    }

    printf("The IPv4 address is: %s\n", ip);

    servinfo = servinfo->ai_next;
  }

  //
  /* CLEANUP */
  freeaddrinfo(servinfo_cp);

  return 0;
}
