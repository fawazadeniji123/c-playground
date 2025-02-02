#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * main - fork example
 *
 * Return: Always 0.
 */
int main(void) {
  pid_t my_pid;
  pid_t pid;

  printf("Before fork\n");
  pid = fork();
  if (pid == -1) {
    perror("Error:");
    return (1);
  }
  printf("After fork\n");
  printf("gh repo clone fawazadeniji123/simple_shell\n");
  my_pid = getpid();
  printf("My pid is %u\n", my_pid);
  return (0);
}