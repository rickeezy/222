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

int tokenize(char *input, char **tokens) {
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
                tokens[count] = NULL;
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

void execute_command(char *tokens[], int background) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // This is the child process
        execvp(tokens[0], tokens);
        perror("execvp");
        exit(1);
    } else {
        // This is the parent process
        if (!background) {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

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
            if (top[0] == '$') {
                char *var_name = top + 1;
                char *var_val = getenv(var_name);
                printf("%s ", var_val);
            } else {
                printf("%s ", top);
            }
        }
        printf("\n");
    } else if (strcmp(tokens[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(tokens[0], "clear") == 0) {
        // Clear screen
        printf("\033[2J\033[H");
    } else if (strcmp(tokens[0], "env") == 0) {
        if (tokens[1] != NULL) {
            char **env = environ;
            char *var_name = tokens[1];
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
        } else {
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
        execute_command(tokens, 0);
    }
}

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
            int background = 0;
            int i;
            for (i = 0; tokens[i] != NULL; i++) {
                if (strcmp(tokens[i], "&") == 0) {
                    background = 1;
                    tokens[i] = NULL;
                    break;
                }
            }
            execute_builtin(tokens, token_count, background);
        }
    }

    return 0;
}
