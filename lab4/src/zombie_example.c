#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid > 0) {
        printf("Parent process PID: %d\n", getpid());
        printf("Child process PID: %d\n", pid);

        sleep(20); 
        
        wait(NULL);
    } 
    else if (pid == 0) {
        printf("Child process exiting.\n");
        exit(0); 
    } 
    else {
        perror("fork failed");
        return 1;
    }

    return 0;
}
