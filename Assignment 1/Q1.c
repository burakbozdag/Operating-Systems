#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
  int c = 0;
  int child = fork();
  c++;
  if(child == 0) {
    child = fork();
    c += 2;
    if(child) c = c*3;
  } else {
    c += 4;
    fork();
  }
  printf("%d %d\n", c, child);
  return 0;
}
