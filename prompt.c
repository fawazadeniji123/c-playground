#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
  char *line = NULL;
  size_t len = 0;
  ssize_t nread;

  printf("$ ");
  nread = getline(&line, &len, stdin);
  if (nread == -1) {
    perror("Error:");
  }

  printf("%s", line);

  free(line);

  return 0;
}
