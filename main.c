#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// Counter to test that intervals are correct
int counter = 0;

typedef struct process {
    char * str;
    int pid;
    struct process * next;
} process_t;

process_t * head = NULL;

// Function to get all processes using `ps`
int get_processes()
{
    FILE *fp;
    char process_str[1035];

    /* Open the command for reading. */
    fp = popen("ps -e", "r");
    if (fp == NULL) {
      printf("Failed to run command\n" );
      exit(1);
    }

    process_t * current = NULL;

    /* Read the output a line at a time - output it. */
    while (fgets(process_str, sizeof(process_str)-1, fp) != NULL) {
      if(current == NULL) {
          if (head == NULL) {
              head = malloc(sizeof(process_t));
              if (head == NULL) {
                  printf("Oh shit, can't get memory. I'm bailin', yo.\n");
                  return 1;
              }
          }
          current = head;
      } else {
        if (current->next == NULL) {
            current->next = malloc(sizeof(process_t));
            if (current->next == NULL) {
                printf("Oh shit, can't get memory. I'm bailin', yo.\n");
                return 1;
            }
        }
        current = current->next;
      }
      if(current->str != NULL)
        free(current->str);
      current->str = malloc(1035*sizeof(*current->str));
      strcpy(current->str, process_str);
      current->next = NULL;
      //printf("%s", process_str);
    }

    /* close */
    pclose(fp);
    return 0;
}

// Function to test printing from thread
void print_processes()
{
    process_t * current = head;

    while (current != NULL) {
        printf("%s\n", current->str);
        current = current->next;
    }
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
      print_processes();
      printf("Counter: %d\n", counter);
  }

  return 0;
}
