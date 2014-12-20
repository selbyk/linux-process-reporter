#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

/**
[
  {
    "name" : "processes",
    "columns" : ["pid", "uname", "pcpu", "pmem", "comm"],
    "points" : [
      [23.2, "serverA", "/mnt"]
    ]
  }
]
**/

// Counter to test that intervals are correct
int counter = 0;

// Function to get all processes using `ps`
int get_processes()
{
    FILE *fp;
    FILE *json_out;
    char process_str[1035];

    /* Open the command for reading. */
    fp = popen("ps -e -o pid,uname,pcpu,pmem,comm", "r");
    if (fp == NULL) {
      printf("Failed to run command\n" );
      exit(1);
    }

    json_out = fopen("processes_post.json", "w");

    fprintf(json_out, "[{\"name\" : \"processes\",\"columns\" : [\"pid\", \"uname\", \"pcpu\", \"pmem\", \"comm\"],\"points\" : [");

    /* Read the output a line at a time - output it. */
    bool contin = fgets(process_str, sizeof(process_str)-1, fp) != NULL;
    while (contin) {
      int pid;
      char uname[20];
      double pcpu, pmem;
      char comm[20];

      sscanf(process_str, "  %d %s      %lf  %lf %s", &pid, uname, &pcpu, &pmem, comm);
      //printf("%s\n", process_str);
      fprintf(json_out, "[%d, \"%s\", %0.1lf, %0.1lf, \"%s\"]", pid, uname, pcpu, pmem, comm);
      contin = fgets(process_str, sizeof(process_str)-1, fp) != NULL;
      if(contin)
        fprintf(json_out, ",\n");
    }

    fprintf(json_out, "]}]");
    fclose(json_out);

    /* close */
    pclose(fp);

    // curl -X POST -d @processes_post.json 'http://localhost:8086/db/system/series?u=root&p=root'
    system("curl -X POST -d @processes_post.json 'http://localhost:8086/db/system/series?u=root&p=root'");

    return 0;
}

// Thread to start fetching process information on interval
void *start()
{
    while(1)
    {
        // 4 times a second
        usleep((int)(1000000*10));
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
