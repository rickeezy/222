
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKENS 64

char prompt[] = "> ";

char delimiters[] = " \t\r\n";
extern char **environ;


// Function to tokenize the input string
int tokenize(char *input, char **tokens) {
    const char *delimiters = " \t\r\n";
    char *token = strtok(input, delimiters);
    int count = 0;

    while (token != NULL) {
        if (token[0] == '$' && strlen(token) > 1) {
            char *var_name = &token[1];
            char *var_value = getenv(var_name);
            if (var_value != NULL) {
                tokens[count] = strdup(var_value);
            } else {
                fprintf(stderr, "Variable not found: %s\n", var_name);
                tokens[count] = NULL; // Set it to NULL to indicate an error
            }
        } else {
            tokens[count] = strdup(token);
        }

        token = strtok(NULL, delimiters);
        count++;
    }

    tokens[count] = NULL;
    return count;
}

// Function to execute built-in commands

void execute_builtin(char *tokens[], int token_count) {
    
    if (strcmp(tokens[0], "cd") == 0) {
        if (token_count >= 2) {
            if (chdir(tokens[1]) != 0) {
                perror("chdir");
            }
        } else {
            fprintf(stderr, "Usage: cd <directory>\n");
        }
    } else if (strcmp(tokens[0], "pwd") == 0) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("getcwd");
        }
    } else if (strcmp(tokens[0], "echo") == 0) {
        int i;
        for (i = 1; tokens[i] != NULL; i++) {
            char *top = tokens[i];
            if (top[0] == '$'){
              char *var_name = top + 1;
              char *var_val = getenv(var_name);
              printf("%s ", var_val);
            }
            else {
              printf("%s ", top); //tokens[i]
            }
        }
        printf("\n");
    } 
     else if (strcmp(tokens[0], "exit") == 0) {
        exit(0);
    } 
    else if (strcmp(tokens[0], "clear") == 0) {
				// Clear screen
				printf("\033[2J\033[H");

    } else if (strcmp(tokens[0], "env") == 0) {
        if (tokens[1] != NULL) {
            char **env = environ;
            char **var_name = tokens[1];
            while (*env != NULL) {
                if (strncmp(*env, var_name, strlen(var_name)) == 0 && (*env)[strlen(var_name)] == '='){
                    printf("%s\n", *env);
                    break;
                }
                env++;
              }
              if (*env == NULL){
                  printf("%s: not found\n", *var_name);
              }
        }
        else {
          int i;
          for (i = 0; environ[i] != NULL; i++) {
            printf("%s\n", environ[i]);
          }
        }
    } else if (strcmp(tokens[0], "setenv") == 0) {
        if (token_count != 2) {
            fprintf(stderr, "Usage: setenv <variable> <value>\n");
        } else {
            char *var_name = strtok(tokens[1], "=");
            char *var_value = strtok(NULL, "=");

            if (var_name != NULL && var_value != NULL) {
                if (setenv(var_name, var_value, 1) != 0) {
                    perror("setenv");
                }
            } else {
                fprintf(stderr, "Invalid setenv syntax\n");
            }
        }
    } else {
        fprintf(stderr, "Command not found: %s\n", tokens[0]);
    }
}

void execute_command(char *tokens[], int token_count) {
    pid_t pid, wpid;
    int status;

    // Fork a new process
    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // This is the child process
        // Load and execute the program
        execvp(tokens[0], tokens);

        // If execvp returns, it means an error occurred
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        // This is the parent process
        do {
            // Wait for the child process to terminate
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

// void getOSTYPE() {
//     pid_t pid;
//     int status;

//     pid = fork();

//     if (pid < 0) {
//         perror("fork");
//         exit(1);
//     } else if (pid == 0) {
//         // This is the child process
//         char *command[] = { "echo", "$OSTYPE", NULL };
//         execvp(command[0], command);
//         // If execvp returns, it means an error occurred
//         perror("execvp");
//         exit(1);
//     } else {
//         // This is the parent process
//         wait(&status);
//     }
// }


int main() {
    char input[MAX_INPUT_SIZE];
    char *tokens[MAX_TOKENS];
    char cwd[PATH_MAX];

    while (1) {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s> ", cwd);
        } else {
            perror("getcwd");
            break;
        }

        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("fgets");
            continue;
        }

        if (feof(stdin)) {
            printf("\n");
            break;
        }

        int token_count = tokenize(input, tokens);

        if (tokens[0] != NULL) {
            execute_builtin(tokens, token_count);
        }
    }

    return 0;
}

// #include <stdio.h>
// #include <stdbool.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/wait.h>
// #include <sys/types.h>
// #include <fcntl.h>
// #include <signal.h>

// #define MAX_COMMAND_LINE_LEN 1024
// #define MAX_COMMAND_LINE_ARGS 128

// char prompt[] = "> ";
// char delimiters[] = " =\t\r\n";
// extern char **environ;
// bool background = false;
// int chld_term = 0;
// int timer_set = 0;

// void chldsig_handler(int signum) {
// 	int status;
// 	pid_t pid = waitpid(-1, &status, WNOHANG);
// 	while (pid > 0) {
// 		if (status == 0) {
// 			printf("Child process %d exited\n", pid);
// 		}
// 		else if (status > 0) {
// 			printf("Child process %d terminated by command\n", pid);
// 		}
// 	}
// 	chld_term = status;
// }

// void sigint_handler(int signum) {
// 	if (chld_term == 0) {
// 		signal(SIGINT, sigint_handler);
// 		printf("\n");
// 		char current_dir[MAX_COMMAND_LINE_LEN];
// 		if (getcwd(current_dir, sizeof(current_dir)) != NULL) {
// 			printf("%s> ", current_dir);
// 		} 
// 		else {
// 			fprintf(stderr, "getcwd error");
// 			printf("> ");
// 		}
// 		fflush(stdout);
// 		fflush(stderr);
// 	}
// }
// void sigalrm_handler(int signum) {
// 	if (timer_set) {
// 		printf("\nTimer expired. Terminating process.\n");
// 		kill(0, SIGINT);
// 	}
// }

// void set_timer() {
// 	timer_set = 1;
// 	alarm(10);
// }

// void cancel_timer() {
// 	timer_set = 0;
// 	alarm(0);
// }

// void tokenize(char *command_line, char **arguments) {
//     int i = 0;
//     char *token = strtok(command_line, delimiters);

//     while (token != NULL) {
//         if (token[0] == '$') {
//             char *var_name = token + 1;
//             char *var_value = getenv(var_name);
//             if (var_value != NULL) {
//                 arguments[i++] = var_value;
//             } else {
//                 arguments[i++] = ""; // Set to empty string if variable is not found
//             }
//         }
// 				else {
//           arguments[i++] = token;
//         }
//         token = strtok(NULL, delimiters);
//     }

//     arguments[i] = NULL; // Set the last element to NULL for execvp
// }

// void execute_command(char **arguments, bool background) {
//   pid_t pid = fork();
//   if (pid == 0) { // Child process
// 		signal(SIGINT, SIG_DFL);
//     // Execute the command
//     execvp(arguments[0], arguments);
//     perror("execvp");
//     exit(EXIT_FAILURE); // Exit if execvp fails
//     } 
// 	else if (pid < 0) { // Error forking
//     perror("fork");
//   } 
// 	else { // Parent process
//     if (!background) {
// 			set_timer();
//       int status;
//       if (waitpid(pid, &status, 0) == -1) {
//         perror("waitpid");
//       }
// 			cancel_timer();
//     }
//   }
// }

// int main() {
//     char command_line[MAX_COMMAND_LINE_LEN];
//     char current_dir[MAX_COMMAND_LINE_LEN];
//     char *arguments[MAX_COMMAND_LINE_ARGS];

// 		signal(SIGINT, sigint_handler);
// 		signal(SIGCHLD, chldsig_handler);
// 		signal(SIGALRM, sigalrm_handler);

//     while (true) {
// 			do { 
// 				if (getcwd(current_dir, sizeof(current_dir)) != NULL) {
// 					printf("%s> ", current_dir);
// 				} 
// 				else {
// 					fprintf(stderr, "getcwd error");
// 					printf("> ");
// 				}

// 				if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
// 					fprintf(stderr, "fgets error");
// 					exit(0);
// 				}
// 			} while (command_line[0] == 0x0A);

// 			if (feof(stdin))  {
// 				printf("\n");
// 				fflush(stdout);
// 				fflush(stderr);
// 				return 0;
// 			}

// 			tokenize(command_line, arguments);

// 			bool background = false;
// 			int i = 0;
// 			for (i; arguments[i] != NULL; i++) {
// 				if (strcmp(arguments[i], "&") == 0) {
// 					background = true;
// 					arguments[i] = NULL;
// 					break;
// 				}
// 			}

// 			//echo
// 			if (strcmp(arguments[0], "echo") == 0) {
// 				int i = 1;
// 				for (i; arguments[i] != NULL; i++) {
// 					char *arg = arguments[i];
// 					if (arg[0] == '$') {
// 						char *var_name = arg + 1;
// 						char *var_value = getenv(var_name);
// 						if (var_value != NULL) {
// 							printf("%s ", var_value);
// 						}
// 					} 
// 					else {
// 						printf("%s ", arg);
// 					}
// 				}
// 				printf("\n");
// 			} 
// 			//cd
// 			else if (strcmp(arguments[0], "cd") == 0) {
// 				if (arguments[1] != NULL) {
// 					if (chdir(arguments[1]) != 0) {
// 						printf("%s not found.\n", arguments[1]);
// 					}
// 				}
// 			} 
// 			//setenv
// 			else if (strcmp(arguments[0], "setenv") == 0) {
// 				if (arguments[1] != NULL && arguments[2] != NULL) {
// 					if (setenv(arguments[1], arguments[2], 1) != 0) {
// 						perror("setenv");
// 					}
// 				} 
// 				else {
// 					printf("Missing arguments.\nsetenv <variable>=<value>\n");
// 				}
// 			} 
// 			//pwd
// 			else if (strcmp(arguments[0], "pwd") == 0) {
// 				printf("%s\n", current_dir);
// 			} 
// 			//env
// 			else if (strcmp(arguments[0], "env") == 0) {
// 				if (arguments[1] != NULL) {
// 					char **env = environ;
// 					char *var_name = arguments[1];
// 					while (*env != NULL) {
// 						if (strncmp(*env, var_name, strlen(var_name)) == 0 && (*env)[strlen(var_name)] == '=') {
// 							printf("%s\n", *env);
// 							break;
// 						}
// 						env++;
// 					}
// 					if (*env == NULL) {
// 						printf("%s: not found\n", var_name);
// 					}
// 				} 
// 				else {
// 					char **env = environ;
// 					while (*env != NULL) {
// 						printf("%s\n", *env);
// 						env++;
// 					}
// 				}
// 			} 
// 			//clear
// 			else if (strcmp(arguments[0], "clear") == 0) {
// 				// Clear screen
// 				printf("\033[2J\033[H");
// 			} 
// 			//exit
// 			else if (strcmp(arguments[0], "exit") == 0) {
// 				fflush(stdout);
// 				fflush(stderr);
// 				return 0;
// 			}

// 			else {
// 				// Execute external command
// 				execute_command(arguments, background);
// 			}
// 	}

// 	return -1;
// }
