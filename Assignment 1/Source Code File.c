#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
  int level = 1;
  int pid = getpid();
  int child[3] = {0, 0, 0};
  child[0] = fork(); //Child of L1 (main process)
  if (!child[0]) {
    ++level;
    pid = getpid();
    child[0] = fork(); //Child of L2_1
    if (!child[0]) {
      ++level;
      pid = getpid();
      printf("<PID: %d, Children_PID(s): None, level: %d>\n",
      pid, level); //End of L3_1
      return 0;
    }
    child[1] = fork(); //Child of L2_1
    if (!child[1]) {
      ++level;
      pid = getpid();
      child[0] = fork(); //Child of L3_2
      if (!child[0]) {
        ++level;
        pid = getpid();
        printf("<PID: %d, Children_PID(s): None, level: %d>\n",
        pid, level); //End of L4_1
        return 0;
      }
      printf("<PID: %d, Children_PID(s): %d, %d, level: %d>\n",
      pid, child[0], child[1], level); //End of L3_2
      return 0;
    }
    printf("<PID: %d, Children_PID(s): %d, %d, level: %d>\n",
    pid, child[0], child[1], level); //End of L2_1
    return 0;
  }
  child[1] = fork(); //Child of L1 (main process)
  if (!child[1]) {
    ++level;
    pid = getpid();
    child[0] = fork(); //Child of L2_2
    if (!child[0]) {
      ++level;
      pid = getpid();
      printf("<PID: %d, Children_PID(s): None, level: %d>\n",
      pid, level); //End of L3_3
      return 0;
    }
    child[1] = fork(); //Child of L2_2
    if (!child[1]) {
      ++level;
      pid = getpid();
      child[0] = fork(); //Child of L3_4
      if (!child[0]) {
        ++level;
        pid = getpid();
        printf("<PID: %d, Children_PID(s): None, level: %d>\n",
        pid, level); //End of L4_2
        return 0;
      }
      printf("<PID: %d, Children_PID(s): %d, level: %d>\n",
      pid, child[0], level); //End of L3_4
      return 0;
    }
    printf("<PID: %d, Children_PID(s): %d, %d, level: %d>\n",
    pid, child[0], child[1], level); //End of L2_2
    return 0;
  }
  child[2] = fork(); //Child of L1 (main process)
  if (!child[2]) {
    ++level;
    pid = getpid();
    child[0] = fork(); //Child of L2_3
    if (!child[0]) {
      ++level;
      pid = getpid();
      printf("<PID: %d, Children_PID(s): None, level: %d>\n",
      pid, level); //End of L3_5
      return 0;
    }
    child[1] = fork(); //Child of L2_3
    if (!child[1]) {
      ++level;
      pid = getpid();
      child[0] = fork(); //Child of L3_6
      if (!child[0]) {
        ++level;
        pid = getpid();
        printf("<PID: %d, Children_PID(s): None, level: %d>\n",
        pid, level); //End of L4_3
        return 0;
      }
      printf("<PID: %d, Children_PID(s): %d, level: %d>\n",
      pid, child[0], level); //End of L3_6
      return 0;
    }
    printf("<PID: %d, Children_PID(s): %d, %d, level: %d>\n",
    pid, child[0], child[1], level); //End of L2_3
    return 0;
  }
  printf("<PID: %d, Children_PID(s): %d, %d, %d, level: %d>\n",
  pid, child[0], child[1], child[2], level); //End of L1 (main process)

  getchar();
  return 0;
}
