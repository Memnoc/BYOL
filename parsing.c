#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"

/* Functions for Windows compilation */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* Custom readline function */
char *readline(char *prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char *cpy = malloc(strlen(buffer) + 1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy) - 1] = '\0';
  return cpy;
}

/* Custom add_history function */
void add_history(char *unused) {}

/* Include editline headers for Linux and MacOS */
#else
#include <editline/readline.h>
#endif

int main(int argc, char *argv[]) {
  /* INFO: REPL starts */
  puts("Starspy Version 0.0.0.1");
  puts("Press arrows -> <-  to edit your text\n");
  puts("Press Ctrl+c to Exit\n");

  while (1) {

    char *input = readline("starspy> ");
    add_history(input);

    printf("Starspy says => %s\n", input);
    free(input);
  }
  return 0;
}
