/*
 * main.c
 * Create a daemon for Linux
 * author: Garrett Pinto
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
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
#define LOG_FILE "/var/log/matt_daemon.log"
#define PID_FILE "/var/run/matt_daemon.pid"
// run only with root rights
// daemon will list on port 4242
// write logs, create class called Tintin_reporter
// actions should show in matt_daemon.log with [DD/MM/YYYY - HH:MM:SS] timestamp located in /var/log/matt_daemon/
// when the daemon shuts down, the matt_daemon.lock file must be deleted
// "quit" being sent on the opened socket must close the program
// any other character chain is registered in the log file


// void print_usage()

void cleanup() {
    if (unlink(LOCK_FILE) == -1) {
        perror("unlink LOCK_FILE failed");
    } else {
        printf("Lock file removed.\n");
    }

    if (unlink(PID_FILE) == -1) {
        perror("unlink PID_FILE failed");
    } else {
        printf("PID file removed.\n");
    }
}


void write_pid_file() {
/*
	pid_t pid = getpid();
	int pid_fd = open(PID_FILE, O_EXCL | O_CREAT | O_WRONLY | O_TRUNC, 0644);
	assert(pid_fd != -1);
	FILE *pid_file = fdopen(pid_fd, "w");
	assert(pid_file != NULL);
	fprintf(pid_file, "%d", pid);
	fclose(pid_file);
*/
	FILE *pid_file = fopen(PID_FILE, "w");
	if (!pid_file) {
		perror("Failed to write PID file");
		exit(EXIT_FAILURE);
	}
	fprintf(pid_file, "%d\n", getpid());
	fclose(pid_file);
}


int main(void) {
        // Check if running as "root" user
        if (getuid() != 0) {
                printf("This program must be ran as 'root'\n");
                return EXIT_FAILURE;
        }

        struct stat st;
        if (stat(LOCK_DIRECTORY, &st) == -1) {
                if (errno == ENOENT) {
                        printf("Directory doesn't exist.\n");
                        if (mkdir(LOCK_DIRECTORY, 0775) == -1) {
                                perror("Error creating directory");
                                return EXIT_FAILURE;
                        }
                }
                else {
                        perror("Error checking directory existence.\n");
                        return EXIT_FAILURE;
                }
        }

        // Create "matt_daemon.lock" file in /var/lock
        int fd = open(LOCK_FILE, O_CREAT | O_EXCL | O_WRONLY, 0666);
        if (fd == -1) {
          	// If lockfile exists, exit since script is already running, if not; create it and continue
        	// Only 1 daemon should run, if attempting to run a 2nd, pop err message indicating a file opening attempt
		if (errno == EEXIST) {
                        fprintf(stderr, "Another instance is already running.\n");
                }
                else {
                        perror("Error creating lock file...");
                }
                return EXIT_FAILURE;
        }
	close(fd);


        pid_t child_pid = fork();
        assert(child_pid != -1);
        if (child_pid > 0)
                exit(EXIT_SUCCESS);

        // "Double-fork" to create new session
        setsid();
        child_pid = fork();
        assert(child_pid != -1);
        if (child_pid > 0)
                exit(EXIT_SUCCESS);
	
	umask(027);

        atexit(cleanup);
	write_pid_file();
	
	// Create a log file
	// const char *log_file_path = "/var/log/matt_daemon.log";
	FILE *log_file = fopen(LOG_FILE, "a");
	assert(log_file != NULL);
	setlinebuf(log_file);
	setlinebuf(stdout);
	setlinebuf(stderr);
	
	// Redirect stdout/stderr fd to the log file 
	int log_fd = fileno(log_file);
	dup2(log_fd, STDOUT_FILENO);
	dup2(log_fd, STDERR_FILENO);

	while (true) {
                const char *str = "Hello\n";
		printf("%s", str);
		fflush(stdout);
		sleep(1);
        }

        return EXIT_SUCCESS;
}
