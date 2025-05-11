#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>



// checks if a token is a builtin command or not
// returns 1 if token is builtin and 0 otherwise
static char check_builtin(char *token) {
  char* builtins[] = {"type", "echo", "exit"};
  int num_builtins = sizeof(builtins) / sizeof(builtins[0]);
  for (int i = 0; i < num_builtins; i++) {
    if (strcmp(builtins[i], token) == 0) {
      return 1; // builtin found
    }
  }
  return 0; // builtin not found
}



static void echo_handler(char* input) {
  printf("%s\n", input); // print rest of input, excluding first token (echo)
  printf("$ ");
}

static void type_handler(char* input, char** paths, int path_count) {
  
  char found = 0;
  DIR* dir;
  struct dirent *entry;
  char *next_token = strtok(input, " "); // grab token after "type"
  if (check_builtin(next_token)) {
    printf("%s is a shell builtin\n", next_token);
    found = 1;
  } else {

    // search for next_token in PATH
    for (int i = 0; i < path_count; i++) {
      dir = opendir(paths[i]);
      while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, next_token) == 0) {
          printf("%s is %s/%s\n", next_token, paths[i], next_token);
          found = 1;
          break;
        }
      }
      closedir(dir);
      if (found) {
        break;
      }
    }

  }
  if (!found) {
    printf("%s: not found\n", next_token);
  }
  printf("$ ");
}


int main(int argc, char *argv[]) {

  // use execve to run command
  // use fork to create copy of my own shell so it's not lost
  // use wait so my shell waits until command (child) process finishes. 
  // how to know if a command is actually external? 

  // Flush after every printf
  setbuf(stdout, NULL);
  printf("$ ");

  // Wait for user input;
  char input[100];

  
  // grab PATH using getenv(). Alternatively, use parameter "char *envp[]" for main().
  char* path = strdup(getenv("PATH"));
  if (!path) {
    printf("No PATH");
    return 0;
  }

   // allocate array of strings
  char** paths  = calloc((1000), sizeof(char*)); // not accepting a PATH with any more than 999 paths
  if (!paths) {
    printf("String array allocation: Out of memory");
    return 0;
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

    if (!strcmp(token, "echo")) {
      echo_handler(temp_input);
    } else if (!strcmp(token, "type")) {
      type_handler(temp_input, paths, path_count);
    } else {
      printf("%s: command not found\n", input);
      printf("$ ");
    }

  }
  return 0;
}
