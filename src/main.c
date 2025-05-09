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
    fgets(input, 100, stdin);
    input[strlen(input) - 1] = '\0'; // set null terminator to remove trailing new line 
    if (!strcmp(input, "exit 0")) {
      break;
    }
    printf("%s: command not found\n", input);
    printf("$ ");
  }
  return 0;
}
