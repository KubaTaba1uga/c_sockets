#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#ifndef ENABLE_TLS
#define ENABLE_TLS 0
#endif

int main(int argc, char *argv[]) {

  if (argc != 2) {
    puts("Usage:\n <exe> <dns address>");
    return 1;
  }

  char *dns_value = argv[1];
  char ip[INET_ADDRSTRLEN];
  const char *result;

  int status;
  struct addrinfo hints;
  struct addrinfo *servinfo;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_UNSPEC;     // don't care IPV6 or IPV4
  hints.ai_socktype = SOCK_STREAM; // TCP
  hints.ai_flags = AI_PASSIVE;     // find IP for me

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
    return 2;
  }

  result = inet_ntop(AF_INET, &(servinfo->ai_addr), ip, INET_ADDRSTRLEN);

  if (!result) {
    fputs("Convertion from addrinfo to string failed", stderr);
    return 3;
  }

  printf("The IPv4 address is: %s\n", ip);

  return 0;
}
