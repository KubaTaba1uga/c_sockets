/* `fibbonnaci` function meant to be copied into each server. */
#include <stdio.h>
#include <stdlib.h>

int fibbonnaci(int n);

int main(int argc, char *argv[]) {
  int n;
  //
  /* PARSE CLI ARGS */
  if (argc != 2) {
    puts("Usage:\n <exe> <number>");
    return 1;
  }

  n = atoi(argv[1]);

  n = _fibbonnaci(n);

  printf("%i\n", n);

  return 0;
}

int fibbonnaci(int n) {

  if (n <= 1) {
    return n;
  }

  return fibbonnaci(n - 2) + fibbonnaci(n - 1);
}
