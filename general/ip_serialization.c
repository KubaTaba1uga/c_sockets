/* #include <sys/socket.h> */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>

char ip[INET_ADDRSTRLEN] = "10.12.110.57";

int main(void) {
  struct sockaddr_in sa; // IPv4
  char ip_cp[INET_ADDRSTRLEN];
  const char *result;
  int err;

  err = inet_pton(AF_INET,
                  // STRING
                  ip,
                  // RESULT PLACEHOLDER
                  &(sa.sin_addr)); // IPv4

  if (err != 1) {
    fputs("Convertion from str to sockaddr_in failed", stderr);
    return 1;
  }

  result = inet_ntop(AF_INET,
                     // IP
                     &(sa.sin_addr),
                     // RESULT PLACEHOLDER
                     ip_cp,
                     // PLACEHOLDER LENGTH
                     INET_ADDRSTRLEN);

  if (!result) {
    fputs("Convertion from sockaddr_in to str failed", stderr);
    return 2;
  }

  printf("The IPv4 address is: %s\n", ip_cp);

  return 0;
}
