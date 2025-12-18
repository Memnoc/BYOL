#include <stdio.h>
/* Declare a buffer for user input of size 2048 */
static char input[2048];

int main(int argc, char *argv[]) {
  /* REPL */
  /* Print version and Exit information */
  puts("Lispy version 0.0.0.1");
  puts("Press Ctrl+c to Exit\n");

  /* Infinite loop */
  while (1) {

    /* Prompt output */
    fputs("lispy>", stdout);

    /* Read a line of user input of maximum size 2048 */
    fgets(input, 2048, stdin);

    /* Echo input back to the user */
    printf("No you're a %s", input);
  }
  return 0;
}
