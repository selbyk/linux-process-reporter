#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Counter to test that intervals are correct
int counter = 0;

// Function to test printing from thread
void say_hey()
{
    printf("Hey!\n");
    return;
}

// Function to get all processes using `ps`
void get_processes()
{
    FILE *fp;
    char path[1035];

    /* Open the command for reading. */
    fp = popen("ps -e", "r");
    if (fp == NULL) {
      printf("Failed to run command\n" );
      exit(1);
    }

    /* Read the output a line at a time - output it. */
    /*while (fgets(path, sizeof(path)-1, fp) != NULL) {
      printf("%s", path);
    }*/

    /* close */
    pclose(fp);
    return;
}

// Thread to start fetching process information on interval
void *start()
{
    while(1)
    {
        // 4 times a second
        usleep((int)(1000000/4));
        get_processes();
        ++counter;
    }
    return 0;
}

int main( int argc, char *argv[] )
{
  // Create a thread to start fetching process information on an interval
  pthread_t tid;
  pthread_create(&tid, NULL, &start, NULL);

  while(1)
  {
      sleep(5);
      printf("Counter: %d\n", counter);
  }

  return 0;
}
