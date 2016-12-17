#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char* argv[])
{
  int fd;
  char* ptr;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <device>\n", argv[0]);
    exit(1);
  }
  printf("1");
  if ((fd = open(argv[1], O_RDONLY, 0)) < 0) {
    perror(argv[1]);
    exit(1);
  }
  printf("2");
  ptr = mmap(NULL, 32, PROT_READ, MAP_SHARED, fd, 0);
  if (ptr == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
  printf("3");
  printf("OK : start");
  while (1) {
    fprintf(stderr, "%s", ptr);
    usleep(100000);
  }
  return 0;
}
