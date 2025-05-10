#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static void echo_handler(char* input) {
  char *token = strtok(input, " "); // this is "echo"
  printf("%s\n", strtok(NULL, "")); // print rest of input, excluding first token (echo)
  printf("$ ");
}

static void type_handler(char* input) {
  char *token = strtok(input, " "); // this is "type"
  char *next_token = strtok(NULL, " ");
  if (!strcmp(next_token, "type") || !strcmp(next_token, "echo") || !strcmp(next_token, "exit")) {
    printf("%s is a shell builtin\n", next_token);
  } else {
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
    } else if (!strcmp(token, "type")) {
      type_handler(temp_input);
    } else {
      printf("%s: command not found\n", input);
      printf("$ ");
    }

  }
  return 0;
}
