#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#define PATH_MAX 1000




// checks if a token is a builtin command or not
// returns 1 if token is builtin and 0 otherwise
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

static void echo_handler(char* input) {
  printf("%s\n", input); // print rest of input, excluding first token (echo)
}

static void type_handler(char* input, char** paths, int path_count) {

  char *next_token = strtok(input, " "); // grab token after "type"
  if (!next_token) {
    printf("$ ");
    return;
  }
  
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


int main(int argc, char *argv[], char * envp[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  printf("$ ");

  // Wait for user input;
  char input[100];

  
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
    fgets(input, 100, stdin);
    input[strlen(input) - 1] = '\0';
    char* temp_input = input;

    // check for exit prompt
    if (!strcmp(input, "exit 0")) {
      break;
    }

    char *token = strtok_r(temp_input, " ", &temp_input);
    char* search_path = find_in_path(token, paths, path_count);

    if (!strcmp(token, "echo")) {
      echo_handler(temp_input);

    } else if (!strcmp(token, "type")) {
      type_handler(temp_input, paths, path_count);

    } else if (!strcmp(token, "pwd")) {
      char cwd[PATH_MAX];
      if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
      } else {
        printf("Error retrieving current working directory");
        return 1;
      }

    } else if (!strcmp(token, "cd")) {
    int cd;
    if (!strcmp(temp_input, "~")) { // return to home directory
       cd = chdir(find_in_env(envp, "HOME="));
    } else {
      cd = chdir(temp_input);
    }
    if (cd != 0) {
      printf("cd: %s: No such file or directory\n", temp_input);
    }

    } else if (strcmp(search_path, "") != 0){
      // handle external command execution
      // use execve to run command
      // use fork to create copy of my own shell so it's not lost
      // use wait so my shell waits until command (child) process finishes. 
      char* args[100]; // array to hold args
      args[0] = token;
      int argc = 1;
      while ((args[argc] = strsep(&temp_input, " ")) != NULL) {
       if (*args[argc] != '\0') // skip empty tokens (due to multiple spaces)
         argc++;
      }
      args[argc] = NULL;

      
      pid_t parent = getpid();
      pid_t pid = fork();

      if (pid == -1) {
          printf("error, failed to fork");
          return 1;
      } else if (pid > 0) {
          int status;
          waitpid(pid, &status, 0);
      } else {
        char *complete_path = malloc(strlen(search_path) + strlen(token) + 2); // "/" and "\0"
        if (!complete_path) {
            printf("malloc failed to allocate string");
            return 1;
        }
        sprintf(complete_path, "%s/%s", search_path, token);
        execve(complete_path, args, envp);
        _exit(EXIT_FAILURE);   // exec never returns
        return 1;
      }
    } else {
      printf("%s: command not found\n", input);
    }
    printf("$ ");
  }
  return 0;
}
