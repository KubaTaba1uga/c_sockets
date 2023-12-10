/* #include <sys/socket.h> */
#include <arpa/inet.h>
#include <stdio.h>

char ip[INET_ADDRSTRLEN] = "10.12.110.57";

int main(void) {
  struct sockaddr_in sa; // IPv4
  const char *ip_cp;
  int err;

  err = inet_pton(AF_INET, ip, &(sa.sin_addr)); // IPv4

  if (err != 1) {
    fputs("Convertion from str to sockaddr_in failed", stderr);
    return 1;
  }

  ip_cp = inet_ntop(AF_INET, &(sa.sin_addr), ip, INET_ADDRSTRLEN);

  if (!ip_cp) {
    fputs("Convertion from sockaddr_in to str failed", stderr);
    return 2;
  }

  printf("The IPv4 address is: %s\n", ip_cp);

  return 0;
}
