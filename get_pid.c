#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

/**
 * main - PID
 *
 * Return: Always 0.
 */
int main(void) {
  pid_t my_pid, ppid;

  my_pid = getpid();
  ppid = getppid();
  printf("%u\n", my_pid);
  printf("%u\n", ppid);
  return (0);
}