#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(void)
{
        pid_t child_pid = fork();
        assert(child_pid != -1);
        if (child_pid > 0)
                exit(EXIT_SUCCESS);
        // Create new session
        setsid();
        child_pid = fork();
        assert(child_pid != -1);
        if (child_pid > 0)
                exit(EXIT_SUCCESS);
        //


        while (true)
        {
                sleep(1);
        }


        return 0;
}