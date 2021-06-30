#include "apue.h"

#define BUFFSIZE 4096

int main(int argc, char **argv) {
  int n;
  char buf[BUFFSIZE];
  FILE *file = fopen(argv[1],"r");
  int file_descriptor = fileno(file);
  while ((n = read(file_descriptor, buf, BUFFSIZE)) > 0)
    if (write(STDOUT_FILENO, buf, n) != n)
      err_sys("write error");

  if (n < 0)
    err_sys("read error");

  exit(0);
}
