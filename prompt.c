#include <stdio.h>
#include <stdlib.h>

/* If we are compiling on Windows compile these functions */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* Custom reading line function */
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

/* Otherwise include the editline headers */
#else
#include <editline/readline.h>
#endif

int main(int argc, char *argv[]) {
  /* INFO: REPL starts */
  puts("Lispy Version 0.0.0.1");
  puts("Press Ctrl+c to Exit\n");

  while (1) {

    char *input = readline("lispy> ");
    add_history(input);

    printf("Echo => %s\n", input);
    free(input);
  }
  return 0;
}
