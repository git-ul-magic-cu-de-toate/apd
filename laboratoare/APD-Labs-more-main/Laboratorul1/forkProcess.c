#include <stdio.h>
#include <stdlib.h>


int main()
{
   
    pid_t pid;
    /* fork a child process */
    pid = fork();
    if (pid < 0)
    { /* error occurred */
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    else if (pid == 0)
    { 
        execlp("/bin/ls", "ls","-l", NULL);
    }
    else
    { /* parent process */
        /* parent will wait for the child to complete */
        printf("2");
        wait(NULL);
        printf("1");
        printf("Child Complete");
    }
    return 0;
}
