#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"
/* test */

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

  /* NOTE:  Create Some Parsers */
  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Operator = mpc_new("operator");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Starspy = mpc_new("Starspy");

  mpc_err_t *err =
      mpca_lang(MPCA_LANG_DEFAULT, "                                           \
    number   : /-?[0-9]+/ ;                             \
    operator : '+' | '-' | '*' | '/' ;                  \
    expr     : <number> | '(' <operator> <expr>+ ')' ;  \
    Starspy    : /^/ <operator> <expr>+ /$/ ;             \
  ",
                Number, Operator, Expr, Starspy);

  /* FIX: Error handling */
  if (err != NULL) {
    mpc_err_print(err);
    mpc_err_delete(err);
    exit(1);
  }

  /* INFO: REPL starts */
  puts("Starspy Version 0.0.0.0.2");
  puts("Press arrows -> <-  to edit your text\n");
  puts("Press Ctrl+c to Exit\n");

  while (1) {
    char *input = readline("starspy> ");
    add_history(input);

    // WARN: attempt to parse the user input
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Starspy, &r)) {
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    free(input);
  }

  mpc_cleanup(4, Number, Operator, Expr, Starspy);

  return 0;
}
