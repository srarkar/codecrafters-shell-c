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
    if (!strstr(input, "exit 0")) {
      break;
    }
    
    // check if echo is the first thing in input
    char *echo_check = strstr(input, "echo");
    if (echo_check == input) {
      printf("%s", &input[5]);
      printf("$ ");
      continue;
    }

    printf("%s: command not found\n", input);
    printf("$ ");
  }
  return 0;
}
