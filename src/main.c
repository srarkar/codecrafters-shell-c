#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>



static void echo_handler(char* input) {
  printf("%s\n", input); // print rest of input, excluding first token (echo)
  printf("$ ");
}

static void type_handler(char* input, char** paths, int path_count) {
  
  char *builtins = "type echo exit"; // EDIT THIS WHEN ADDING BUILTINS
  char found = 0;
  DIR* dir;
  struct dirent *entry;
  char *next_token = strtok(input, " "); // grab token after "type"
  if (strstr(builtins, next_token) != NULL) {
    printf("%s is a shell builtin\n", next_token);
  } else {
    // search for next_token in PATH
    for (int i = 0; i < path_count; i++) {
      dir = opendir(paths[i]);
      while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, next_token) == 0) {
          printf("%s is %s/%s", next_token, paths[i], next_token);
          found = 1;
          closedir(dir);
          break;
        }
      }
      closedir(dir);
    }
  }
  if (!found) {
    printf("%s: not found\n", next_token);
  }
  printf("$ ");
}


int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  printf("$ ");

  // Wait for user input;
  char input[100];

  
  // grab PATH using getenv(). Alternatively, use parameter "char *envp[]" for main().
  char* path = strdup(getenv("PATH"));

   // allocate array of strings
  char** paths  = calloc((1000), sizeof(char*)); // not accepting a PATH with any more than 999 paths
  if (!paths) {
    printf("String array allocation: Out of memory");
    return 0;
  }

  // separate PATH into tokens and store into string array 
  char *token;
  int path_count = 0;
  char  delimiter = ':';
  while ((token = strsep(&path, &delimiter)) != NULL) {
    paths[path_count] = strdup(token); 

    if (!paths[path_count]) {
      printf("Path allocation: Out of memory");
      for (int j = 0; j < path_count; j++) {
        free(paths[j]); // free all previous elements if strdup fails
      }
      return 0;
    }

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
