#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>

int main(int argc, char *argv[]) {
  /* INFO: REPL starts */
  puts("Lispy version 0.0.0.1");
  puts("Press Ctrl+c to Exit\n");

  while (1) {

    /* Prompt output and get input */
    char *input = readline("lispy> ");
    add_history(input);

    printf("Echo => %s\n", input);

    free(input);
  }
  return 0;
}
