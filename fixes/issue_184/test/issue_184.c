#include "../mpc.h"
#include "ptest.h"

void test_missing_parser_detection(void) {
  mpc_parser_t *Lispy;
  mpc_err_t *err;
  char *err_msg;

  Lispy = mpc_new("lispy");

  /* Use the macro version to ensure NULL sentinel is added */
  err = mpca_lang(MPCA_LANG_DEFAULT, " rule : <Lispy> ; ", Lispy);

  PT_ASSERT(err != NULL);
  /* Convert error to string for inspection */
  err_msg = mpc_err_string(err);
  /* Check for our fuzzy suggestion */
  PT_ASSERT(strstr(err_msg, "Unknown Parser 'Lispy'!") != NULL);
  PT_ASSERT(strstr(err_msg, "Did you mean 'lispy'") != NULL);

  /* Cleanup */
  free(err_msg);
  mpc_err_delete(err);
  mpc_cleanup(1, Lispy);
}

void suite_issue_184(void) {
  pt_add_test(test_missing_parser_detection,
              "Test Missing Parser Detection (#184)", "Suite Issue 184");
}
