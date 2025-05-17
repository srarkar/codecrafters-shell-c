#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_NUM_TOKENS 100
#define PATH_MAX 1000


static int tokenize_input (char* rest, char* args[]) {
  // 2-D array: each token has its own array, used to build the full token that will go into args
  static char token_bufs[MAX_NUM_TOKENS][1000]; // max length of tokens: 999 chars
  int argc = 0;
  int token_i = 0;
  if (!rest) {
    args[argc] = NULL;
    return argc;
  }

  while (*rest) {
    // Skip leading spaces
    while (*rest == ' ') rest++;
    if (*rest == '\0') break;

    char *buf = token_bufs[token_i]; // 1 row of token_bufs
    int j = 0;

    while (*rest) {
      if (*rest == ' ') {
        break;
      }
      if (*rest == '\\') {
        rest++;
        if (*rest) {
          buf[j] = *rest;
          j++;
          rest++;
        }
        continue;
      }

      if (*rest == '\'') {
        rest++;  // skip starting quote
        while (*rest && *rest != '\'') {
          if (j < 999) {
            buf[j++] = *rest++;
          } else {
            rest++;
          }
        }
        if (*rest == '\'') {
          if (*(rest + 1) == '\'') {
            rest+=2;
            continue;
          }
          rest++;  // skip ending quote
          break;
        }
      } else if (*rest == '\"'){
        rest++;  // skip starting quote
        while (*rest && *rest != '\"') {
          if (j < 999) {
            // handle exceptions preceded by backslash
            if (*rest == '\\' && ((*(rest + 1) == '\\') || (*(rest + 1) == '$') || (*(rest + 1) == (int)10) || (*(rest + 1) == '\"'))) {
              rest++;
            }
            buf[j++] = *rest++;
          } else {
            rest++;
          }
        }
        if (*rest == '\"') {
          rest++;  // skip ending quote
        }
      }
      else {
        if (j < 999) { 
          buf[j++] = *rest++;
        } else {
          rest++;
        }
      }
    }

    buf[j] = '\0';
    args[argc++] = buf;
    token_i++;
  }

  args[argc] = NULL;
  return argc;
}


// checks if a token is a builtin command or not
// returns 0 if token is builtin and 1 otherwise (either invalid or an external)
static char check_builtin(char *token) {
  char* builtins[] = {"type", "echo", "exit", "pwd", "cd"};
  int num_builtins = sizeof(builtins) / sizeof(builtins[0]);
  for (int i = 0; i < num_builtins; i++) {
    if (strcmp(builtins[i], token) == 0) {
      return 0; // builtin found
    }
  }
  return 1; // builtin not found
}

static char* find_in_path(char* token, char** paths, int path_count) {
  DIR* dir;
  struct dirent* entry;

  for (int i = 0; i < path_count; i++) {
    dir = opendir(paths[i]);
    if (!dir) continue;    // skip invalid/unopenable PATH entry
    while ((entry = readdir(dir)) != NULL) {
      if (strcmp(entry->d_name, token) == 0) {
        return(paths[i]);
      }
    }
    closedir(dir);
  }
  return "";
}

static char* find_in_env(char* envp[], char *token) {
  int token_len = strlen(token);
  for (int i = 0; envp[i] != NULL; i++) {
    if (!strncmp(envp[i], token, token_len)) {
        return envp[i] + token_len;
    }
}
return 0; // not found
}

static void echo_handler(char* args[], int argc) {
  for (int i = 1; i < argc; i++) { 
    printf("%s ", args[i]); // print rest of input, excluding first token (echo)
  }
  printf("\n");
}

static void type_handler(char* args[], int argc, char** paths, int path_count) {
  char *next_token;
  for (int i = 0; i < argc - 1; i++) {
    next_token = args[i];
    if (!check_builtin(next_token)) {
      printf("%s is a shell builtin\n", next_token); // check if next_token is a builtin command
    } else {
      char* search_path = find_in_path(next_token, paths, path_count); // search for next_token in PATH
      if (strcmp(search_path, "")) {
        printf("%s is %s/%s\n", next_token, search_path, next_token);
      } else {
        printf("%s: not found\n", next_token);
      }
    }
  }
}

static void cd_handler(char* args[], int argc, char* envp[]) {
  int cd;
  if (!strcmp(args[1], "~")) { // return to home directory
      cd = chdir(find_in_env(envp, "HOME="));
  } else {
    cd = chdir(args[1]);
  }
  if (cd != 0) {
    printf("cd: %s: No such file or directory\n", args[1]);
  }
}

static void pwd_handler(void){
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("%s\n", cwd);
  } else {
    printf("Error retrieving current working directory\n");
  }
}

static int external_handler(char* args[], char* envp[], char* search_path) {
  pid_t parent = getpid();
  pid_t pid = fork();

  if (pid == -1) {
      printf("error, failed to fork");
      return 1;
  } else if (pid > 0) {
      int status;
      waitpid(pid, &status, 0);
  } else {
    char *complete_path = malloc(strlen(search_path) + strlen(args[0]) + 2); // "/" and "\0"
    if (!complete_path) {
        return 1;
    }
    sprintf(complete_path, "%s/%s", search_path, args[0]);
    execve(complete_path, args, envp);
    _exit(EXIT_FAILURE);   // exec never returns
    return 2;
  } 
}

int main(int argc, char *argv[], char * envp[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  printf("$ ");
  
  // Wait for user input;
  char input[MAX_NUM_TOKENS];

  
  // grab PATH using getenv(). Alternatively, use parameter "char *envp[]" for main().
  char* path = strdup(find_in_env(envp, "PATH="));
  if (!path) {
    printf("No PATH provided");
    return 1;
  }

   // allocate array of strings
  char** paths  = calloc((PATH_MAX), sizeof(char*)); // not accepting a PATH with any more than 999 paths
  if (!paths) {
    printf("String array allocation: Out of memory");
    return 1;
  }

  // separate PATH into tokens and store into string array 
  char *token;
  int path_count = 0;

  while ((token = strsep(&path, ":")) != NULL) {
    paths[path_count] = token; 
    path_count++;
  }


  while (1) {
    // take input and format by removing trailing new line
    fgets(input, MAX_NUM_TOKENS, stdin);
    input[strlen(input) - 1] = '\0';
    char* temp_input = input;

    // check for exit prompt
    if (!strcmp(input, "exit 0")) {
      break;
    }

    /////
    // TODO: tokenize input (accounting for single quotes)
    // Then, store into args[] array and update builtins to use args[] instead of temp_input
    /////
    char* args[MAX_NUM_TOKENS]; // array to hold args
    argc = tokenize_input(temp_input, args);
    char* search_path = find_in_path(args[0], paths, path_count);

    if (!strcmp(args[0], "echo")) {
      echo_handler(args, argc);

    } else if (!strcmp(args[0], "type")) {
      type_handler(args + 1, argc, paths, path_count);

    } else if (!strcmp(args[0], "pwd")) {
      pwd_handler();

    } else if (!strcmp(args[0], "cd")) {
      cd_handler(args, argc, envp);

    } else if (strcmp(search_path, "") != 0) {
      int err = external_handler(args, envp, search_path);
      if (err == 1) {
        printf("malloc failure");
      }
      if (err == 2) {
        printf("exec failure");
      }

    } else {
      printf("%s: command not found\n", args[0]);
    }

    // print $ at start of next line
    printf("$ ");
  }
  return 0;
}
