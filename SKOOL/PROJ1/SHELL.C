#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

char prompt[] = "> ";
char delimiters[] = " =\t\r\n";
extern char **environ;
bool background = false;
int chld_term = 0;
int timer_set = 0;

void chldsig_handler(int signum) {
	int status;
	pid_t pid = waitpid(-1, &status, WNOHANG);
	while (pid > 0) {
		if (status == 0) {
			printf("Child process %d exited\n", pid);
		}
		else if (status > 0) {
			printf("Child process %d terminated by command\n", pid);
		}
	}
	chld_term = status;
}

void sigint_handler(int signum) {
	if (chld_term == 0) {
		signal(SIGINT, sigint_handler);
		printf("\n");
		char current_dir[MAX_COMMAND_LINE_LEN];
		if (getcwd(current_dir, sizeof(current_dir)) != NULL) {
			printf("%s> ", current_dir);
		} 
		else {
			fprintf(stderr, "getcwd error");
			printf("> ");
		}
		fflush(stdout);
		fflush(stderr);
	}
}
void sigalrm_handler(int signum) {
	if (timer_set) {
		printf("\nTimer expired. Terminating process.\n");
		kill(0, SIGINT);
	}
}

void set_timer() {
	timer_set = 1;
	alarm(10);
}

void cancel_timer() {
	timer_set = 0;
	alarm(0);
}

void tokenize(char *command_line, char **arguments) {
    int i = 0;
    char *token = strtok(command_line, delimiters);

    while (token != NULL) {
        if (token[0] == '$') {
            char *var_name = token + 1;
            char *var_value = getenv(var_name);
            if (var_value != NULL) {
                arguments[i++] = var_value;
            } else {
                arguments[i++] = ""; // Set to empty string if variable is not found
            }
        }
				else {
          arguments[i++] = token;
        }
        token = strtok(NULL, delimiters);
    }

    arguments[i] = NULL; // Set the last element to NULL for execvp
}

void execute_command(char **arguments, bool background) {
  pid_t pid = fork();
  if (pid == 0) { // Child process
		signal(SIGINT, SIG_DFL);
    // Execute the command
    execvp(arguments[0], arguments);
    perror("execvp");
    exit(EXIT_FAILURE); // Exit if execvp fails
    } 
	else if (pid < 0) { // Error forking
    perror("fork");
  } 
	else { // Parent process
    if (!background) {
			set_timer();
      int status;
      if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
      }
			cancel_timer();
    }
  }
}

int main() {
    char command_line[MAX_COMMAND_LINE_LEN];
    char current_dir[MAX_COMMAND_LINE_LEN];
    char *arguments[MAX_COMMAND_LINE_ARGS];

		signal(SIGINT, sigint_handler);
		signal(SIGCHLD, chldsig_handler);
		signal(SIGALRM, sigalrm_handler);

    while (true) {
			do { 
				if (getcwd(current_dir, sizeof(current_dir)) != NULL) {
					printf("%s> ", current_dir);
				} 
				else {
					fprintf(stderr, "getcwd error");
					printf("> ");
				}

				if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
					fprintf(stderr, "fgets error");
					exit(0);
				}
			} while (command_line[0] == 0x0A);

			if (feof(stdin))  {
				printf("\n");
				fflush(stdout);
				fflush(stderr);
				return 0;
			}

			tokenize(command_line, arguments);

			bool background = false;
			int i = 0;
			for (i; arguments[i] != NULL; i++) {
				if (strcmp(arguments[i], "&") == 0) {
					background = true;
					arguments[i] = NULL;
					break;
				}
			}

			//echo
			if (strcmp(arguments[0], "echo") == 0) {
				int i = 1;
				for (i; arguments[i] != NULL; i++) {
					char *arg = arguments[i];
					if (arg[0] == '$') {
						char *var_name = arg + 1;
						char *var_value = getenv(var_name);
						if (var_value != NULL) {
							printf("%s ", var_value);
						}
					} 
					else {
						printf("%s ", arg);
					}
				}
				printf("\n");
			} 
			//cd
			else if (strcmp(arguments[0], "cd") == 0) {
				if (arguments[1] != NULL) {
					if (chdir(arguments[1]) != 0) {
						printf("%s not found.\n", arguments[1]);
					}
				}
			} 
			//setenv
			else if (strcmp(arguments[0], "setenv") == 0) {
				if (arguments[1] != NULL && arguments[2] != NULL) {
					if (setenv(arguments[1], arguments[2], 1) != 0) {
						perror("setenv");
					}
				} 
				else {
					printf("Missing arguments.\nsetenv <variable>=<value>\n");
				}
			} 
			//pwd
			else if (strcmp(arguments[0], "pwd") == 0) {
				printf("%s\n", current_dir);
			} 
			//env
			else if (strcmp(arguments[0], "env") == 0) {
				if (arguments[1] != NULL) {
					char **env = environ;
					char *var_name = arguments[1];
					while (*env != NULL) {
						if (strncmp(*env, var_name, strlen(var_name)) == 0 && (*env)[strlen(var_name)] == '=') {
							printf("%s\n", *env);
							break;
						}
						env++;
					}
					if (*env == NULL) {
						printf("%s: not found\n", var_name);
					}
				} 
				else {
					char **env = environ;
					while (*env != NULL) {
						printf("%s\n", *env);
						env++;
					}
				}
			} 
			//clear
			else if (strcmp(arguments[0], "clear") == 0) {
				// Clear screen
				printf("\033[2J\033[H");
			} 
			//exit
			else if (strcmp(arguments[0], "exit") == 0) {
				fflush(stdout);
				fflush(stderr);
				return 0;
			}

			else {
				// Execute external command
				execute_command(arguments, background);
			}
	}

	return -1;
}
