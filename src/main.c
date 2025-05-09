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

    // check for exit prompt
    if (!strcmp(input, "exit 0")) {
      break;
    }
    
    // check if echo is the first thing in input
    char echo[] = "echo";
    char *echo_check = strstr(input, echo);
    if (echo_check == input) {
      printf("%s\n", &input[5]);
      printf("$ ");
      continue;
    }

    // built-in commands
    char type[] = "type";
    char *type_check = strstr(input, type);
    if (type_check == input) {
      if (!strcmp(&input[5], "type") || !strcmp(&input[5], "echo") || !strcmp(&input[5], "exit")) {
        printf("%s is a shell builtin\n", &input[5]);
      } else {
        printf("%s: command not found\n", &input[5]);
      }
      printf("$ ");
      continue;
    }

    
    printf("%s: command not found\n", input);
    printf("$ ");
  }
  return 0;
}
