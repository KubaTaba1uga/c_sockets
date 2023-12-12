#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
  const size_t buffer_size = 255;
  char buffer[buffer_size];
  int err;
  err = gethostname(buffer, buffer_size);

  if (err == -1) {
    puts("Getting hostname failed");
    perror("gethostname");
    return 1;
  }

  puts(buffer);

  return 0;
}
