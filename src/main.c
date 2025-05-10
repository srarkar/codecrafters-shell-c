#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);


  printf("$ ");

  // Wait for user input;
  char input[100];
  while (1) {
    // take input and format by removing trailing new line
    fgets(input, 100, stdin);
    input[strlen(input) - 1] = '\0';

    char* temp_input = input;

    // check for exit prompt
    if (!strcmp(input, "exit 0")) {
      break;
    }

    char *token = strtok(temp_input, " ");

    if (!strcmp(token, "echo")) {
      echo_handler(temp_input);
      continue;
    } else if (!strcmp(token, "type")) {

    }

    // built-in commands
    char type[] = "type";
    char *type_check = strstr(input, type);
    if (type_check == input) {
      if (!strcmp(&input[5], "type") || !strcmp(&input[5], "echo") || !strcmp(&input[5], "exit")) {
        printf("%s is a shell builtin\n", &input[5]);
      } else {
        printf("%s: not found\n", &input[5]);
      }
      printf("$ ");
      continue;
    }

    
    printf("%s: command not found\n", input);
    printf("$ ");
  }
  return 0;
}

static void echo_handler(char* input) {
  char *token = strtok(input, " ");
  printf("%s\n", strtok(NULL, "")); // print rest of input, excluding first token (echo)
  printf("$ ");
}
