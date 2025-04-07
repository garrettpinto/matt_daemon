#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define LOCK_DIRECTORY "/var/lock"
#define LOCK_FILE "/var/lock/matt_daemon.lock"
// gcc main.c -o Matt_daemon
// run only with root rights
// run in background
// daemon will list on port 4242
// write logs, create class called Tintin_reporter
// actions should show in matt_daemon.log with [DD/MM/YYYY - HH:MM:SS] timestamp located in /var/log/matt_daemon/
// when the daemon shuts down, the matt_daemon.lock file must be deleted
// "quit" being sent on the opened socket must close the program
// any other character chain is registered in the log file


// void print_usage()

void cleanup() {
        // Cleanup: remove lock file before exit
        unlink(LOCK_FILE);
}

int main(void) {
        // Check if running as "root" user
        if (getuid() != 0) {
                printf("This program must be ran as 'root'\n");
                return 1;
        }

        // create a "matt_daemon.lock" file in /var/lock
        // only one daemon should run at a time, if attempting to run a second, an error message indicating a creation/file opening on matt_daemon.lock attempt must po
        // when the daemon shuts down, the matt_daemon.lock file must be deleted

        // if lockfile exists, exit since script is already running, if not; create it and continue
        // open(O_CREAT | O_EXCL) used instead of flock() since flock() is used to coordinate multiple processes accessing the same file, open() creates a system-wide lock that persists

        struct stat st;
        if (stat(LOCK_DIRECTORY, &st) == -1) {
                if (errno == ENOENT) {
                        printf("Directory doesn't exist.\n");
                        if (mkdir(LOCK_DIRECTORY, 775) == -1) {
                                perror("Error creating directory");
                                return EXIT_FAILURE;
                        }
                }
                else {
                        perror("Error checking directory existence.\n");
                        return EXIT_FAILURE;
                }
        }

        int fd = open(LOCK_FILE, O_CREAT | O_EXCL, 666);

        if (fd == -1) {
                if (errno == EEXIST) {
                        fprintf(stderr, "Another instance is already running.\n");
                }
                else {
                        perror("Error creating lock file...");
                }
                return EXIT_FAILURE;
        }

        printf("Lock file '%s' created.\n", LOCK_FILE);

        atexit(cleanup);

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

        while (true)
        {
                sleep(1);
        }

        return EXIT_SUCCESS;
}
