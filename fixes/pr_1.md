# Better Error Handling (TBD for the real title)

This PR addresses a bug I have found while following along the book on chapter 5.
https://github.com/orangeduck/BuildYourOwnLisp/issues/184

## Issue: Segfault on parser name mismatch

Program crashes with segfault when `mpca_lang()` references a parser name that doesn't exist (in my case, due to case mismatch). A better quality-of-life improvement for the library users would be to handle this crash more gracefully.

### Reproduce

```c
mpc_parser_t *MyParser = mpc_new("myparser");  // lowercase

mpca_lang(MPCA_LANG_DEFAULT,
  "rule : <MyParser> ;",  // uppercase - won't match
  MyParser);
// Segfault at mpc.c:4087
```

### Root Cause

**Location:** `mpc.c` line ~4105-4120 in `mpca_grammar_find_parser()`

When searching for a non-existent parser:

1. Loop exhausts `va_arg()` looking for matching name
2. `va_arg()` returns garbage when exhausted
3. Code accesses `p->name` before NULL check → segfault

```c
while (1) {
  p = va_arg(*st->va, mpc_parser_t *);
  st->parsers_num++;
  st->parsers = realloc(...);  // Using p before validation

  if (p == NULL || p->name == NULL) {  // Too late
    return mpc_failf("Unknown Parser '%s'!", x);
  }
```

In the book, it occurred while following this part: https://www.buildyourownlisp.com/chapter6_parsing
At the line:

```
mpc_parser_t* Lispy    = mpc_new("lispy");
```

I have made a typo there and passed "Lispy" instead of "lispy" which led to the segfault and took me for ride 💃🏻

### Enhacement

Check pointer validity immediately after `va_arg()` and provide a better error message:

```c
while (1) {
  p = va_arg(*st->va, mpc_parser_t *);

  if (p == NULL || p->name == NULL) {
    // Helpful message with available parser names
    char msg[1024] = "Available: ";
    for (int i = 0; i < st->parsers_num; i++) {
      if (st->parsers[i] && st->parsers[i]->name) {
        strcat(msg, "'"); strcat(msg, st->parsers[i]->name); strcat(msg, "' ");
      }
    }
    return mpc_failf("Parser '<%s>' not found. %s", x, msg);
  }

  st->parsers_num++;
  st->parsers = realloc(...);
  // ... rest
}
```

Happy to send a PR your way if you think it's worth it! 😄

### Environment

- mpc: Latest from GitHub (0.9.0)
- OS: Linux (Arch)
- Compiler: GCC

## The fix

The idea behind the fix is providing a better error message that would help the user understand what is going on. After the fix, the message reads as:

```bash

<mpca_lang>: error: Parser '<lispy>' not fo
und. Did you mean 'Lispy' ?
```

## Additions and changes

`mpc.h`

```C

mpc_err_t *mpca_lang(int flags, const char *language, ...);
```

became

```C
c_err_t *mpca_lang(int flags, const char *language, ...);
```

(explain why)

`mpc.c`

```C

static mpc_parser_t *mpca_grammar_find_parser(char *x, mpca_grammar_st_t *st) {

  int i;
  mpc_parser_t *p;

  /* Case of Number */
  if (is_number(x)) {

    i = strtol(x, NULL, 10);

    while (st->parsers_num <= i) {
      st->parsers_num++;
      st->parsers = realloc(st->parsers, sizeof(mpc_parser_t*) * st->parsers_num);
      st->parsers[st->parsers_num-1] = va_arg(*st->va, mpc_parser_t*);
      if (st->parsers[st->parsers_num-1] == NULL) {
        return mpc_failf("No Parser in position %i! Only supplied %i Parsers!", i, st->parsers_num);
      }
    }

    return st->parsers[st->parsers_num-1];

  /* Case of Identifier */
  } else {

    /* Search Existing Parsers */
    for (i = 0; i < st->parsers_num; i++) {
      mpc_parser_t *q = st->parsers[i];
      if (q == NULL) { return mpc_failf("Unknown Parser '%s'!", x); }
      if (q->name && strcmp(q->name, x) == 0) { return q; }
    }

    /* Search New Parsers */
    while (1) {

      p = va_arg(*st->va, mpc_parser_t*);

      st->parsers_num++;
      st->parsers = realloc(st->parsers, sizeof(mpc_parser_t*) * st->parsers_num);
      st->parsers[st->parsers_num-1] = p;

      if (p == NULL || p->name == NULL) { return mpc_failf("Unknown Parser '%s'!", x); }
      if (p->name && strcmp(p->name, x) == 0) { return p; }

    }

  }

}
```

became

```C

static mpc_parser_t *mpca_grammar_find_parser(char *x, mpca_grammar_st_t *st) {

  int i;
  mpc_parser_t *p;

  /* Case of Number */
  if (is_number(x)) {

    i = strtol(x, NULL, 10);

    while (st->parsers_num <= i) {
      st->parsers_num++;
      st->parsers =
          realloc(st->parsers, sizeof(mpc_parser_t *) * st->parsers_num);
      st->parsers[st->parsers_num - 1] = va_arg(*st->va, mpc_parser_t *);
      if (st->parsers[st->parsers_num - 1] == NULL) {
        return mpc_failf("No Parser in position %i! Only supplied %i Parsers!",
                         i, st->parsers_num);
      }
    }

    return st->parsers[st->parsers_num - 1];

    /* Case of Identifier */
  } else {

    /* Search Existing Parsers */
    for (i = 0; i < st->parsers_num; i++) {
      mpc_parser_t *q = st->parsers[i];
      if (q == NULL) {
        return mpc_failf("Unknown Parser '%s'!", x);
      }
      if (q->name && strcmp(q->name, x) == 0) {
        return q;
      }
    }

    /* Search New Parsers */
    while (1) {

      p = va_arg(*st->va, mpc_parser_t *);

      /* FIX: could not find and exact match */
      if (p == NULL) {
        char msg[1024] = "Available parsers: ";
        for (int j = 0; j < st->parsers_num; j++) {
          if (st->parsers[j] && st->parsers[j]->name) {
            // FIX: make a comparison ignoring cases
            if (strcasecmp(st->parsers[j]->name, x) == 0) {
              return mpc_failf("Parser '<%s>' not found. Did you mean '%s' ?",
                               x, st->parsers[j]->name);
            }
            strcat(msg, "'");
            strcat(msg, st->parsers[j]->name);
            strcat(msg, "' ");
          }
        }
        return mpc_failf("Parser '<%s>' not found. %s!", x, msg);
      }
      st->parsers_num++;
      st->parsers =
          realloc(st->parsers, sizeof(mpc_parser_t *) * st->parsers_num);
      st->parsers[st->parsers_num - 1] = p;

      if (strcmp(p->name, x) == 0) {
        return p;
      }
    }
  }
}

static mpc_val_t *mpcaf_grammar_id(mpc_val_t *x, void *s) {

  mpca_grammar_st_t *st = s;
  mpc_parser_t *p = mpca_grammar_find_parser(x, st);
  free(x);

  if (p->name) {
    return mpca_state(mpca_root(mpca_add_tag(p, p->name)));
  } else {
    return mpca_state(mpca_root(p));
  }
}
```

(explain why)

`mpc.c`

```C

static mpc_val_t *mpca_stmt_list_apply_to(mpc_val_t *x, void *s) {

  mpca_grammar_st_t *st = s;
  mpca_stmt_t *stmt;
  mpca_stmt_t **stmts = x;
  mpc_parser_t *left;

  while(*stmts) {
    stmt = *stmts;
    left = mpca_grammar_find_parser(stmt->ident, st);
    if (st->flags & MPCA_LANG_PREDICTIVE) { stmt->grammar = mpc_predictive(stmt->grammar); }
    if (stmt->name) { stmt->grammar = mpc_expect(stmt->grammar, stmt->name); }
    mpc_optimise(stmt->grammar);
    mpc_define(left, stmt->grammar);
    free(stmt->ident);
    free(stmt->name);
    free(stmt);
    stmts++;
  }

  free(x);

  return NULL;
}
```

became:

```C

static mpc_val_t *mpca_stmt_list_apply_to(mpc_val_t *x, void *s) {

  mpca_grammar_st_t *st = s;
  mpca_stmt_t *stmt;
  mpca_stmt_t **stmts = x;
  mpc_parser_t *left;

  while (*stmts) {
    stmt = *stmts;
    left = mpca_grammar_find_parser(stmt->ident, st);
    /* FIX: Check if the parser found is actually a Failure object */
    if (left->type == MPC_TYPE_FAIL) {
      free(stmt->ident);
      free(stmt->name);
      mpc_soft_delete(stmt->grammar);
      free(stmt);

      stmts++;
      while (*stmts) {
        stmt = *stmts;
        free(stmt->ident);
        free(stmt->name);
        mpc_soft_delete(stmt->grammar);
        free(stmt);
        stmts++;
      }
      free(x);

      return left;
    }
    if (st->flags & MPCA_LANG_PREDICTIVE) {
      stmt->grammar = mpc_predictive(stmt->grammar);
    }
    if (stmt->name) {
      stmt->grammar = mpc_expect(stmt->grammar, stmt->name);
    }
    mpc_optimise(stmt->grammar);
    mpc_define(left, stmt->grammar);
    free(stmt->ident);
    free(stmt->name);
    free(stmt);
    stmts++;
  }

  free(x);

  return NULL;
}
```

(explain why)

`mpc.c`

```C

static mpc_err_t *mpca_lang_st(mpc_input_t *i, mpca_grammar_st_t *st) {

  mpc_result_t r;
  mpc_err_t *e;
  mpc_parser_t *Lang, *Stmt, *Grammar, *Term, *Factor, *Base;

  Lang    = mpc_new("lang");
  Stmt    = mpc_new("stmt");
  Grammar = mpc_new("grammar");
  Term    = mpc_new("term");
  Factor  = mpc_new("factor");
  Base    = mpc_new("base");

  mpc_define(Lang, mpc_apply_to(
    mpc_total(mpc_predictive(mpc_many(mpca_stmt_fold, Stmt)), mpca_stmt_list_delete),
    mpca_stmt_list_apply_to, st
  ));

  mpc_define(Stmt, mpc_and(5, mpca_stmt_afold,
    mpc_tok(mpc_ident()), mpc_maybe(mpc_tok(mpc_string_lit())), mpc_sym(":"), Grammar, mpc_sym(";"),
    free, free, free, mpc_soft_delete
  ));

  mpc_define(Grammar, mpc_and(2, mpcaf_grammar_or,
      Term,
      mpc_maybe(mpc_and(2, mpcf_snd_free, mpc_sym("|"), Grammar, free)),
      mpc_soft_delete
  ));

  mpc_define(Term, mpc_many1(mpcaf_grammar_and, Factor));

  mpc_define(Factor, mpc_and(2, mpcaf_grammar_repeat,
    Base,
      mpc_or(6,
        mpc_sym("*"),
        mpc_sym("+"),
        mpc_sym("?"),
        mpc_sym("!"),
        mpc_tok_brackets(mpc_int(), free),
        mpc_pass()),
    mpc_soft_delete
  ));

  mpc_define(Base, mpc_or(5,
    mpc_apply_to(mpc_tok(mpc_string_lit()), mpcaf_grammar_string, st),
    mpc_apply_to(mpc_tok(mpc_char_lit()),   mpcaf_grammar_char, st),
    mpc_tok(mpc_and(3, mpcaf_fold_regex, mpc_regex_lit(), mpc_many(mpcf_strfold, mpc_oneof("ms")), mpc_lift_val(st), free, free)),
    mpc_apply_to(mpc_tok_braces(mpc_or(2, mpc_digits(), mpc_ident()), free), mpcaf_grammar_id, st),
    mpc_tok_parens(Grammar, mpc_soft_delete)
  ));

  mpc_optimise(Lang);
  mpc_optimise(Stmt);
  mpc_optimise(Grammar);
  mpc_optimise(Term);
  mpc_optimise(Factor);
  mpc_optimise(Base);

  if (!mpc_parse_input(i, Lang, &r)) {
    e = r.error;
  } else {
    e = NULL;
  }

  mpc_cleanup(6, Lang, Stmt, Grammar, Term, Factor, Base);

  return e;
}

```

became:

```C

static mpc_err_t *mpca_lang_st(mpc_input_t *i, mpca_grammar_st_t *st) {

  mpc_result_t r;
  mpc_err_t *e;
  mpc_parser_t *Lang, *Stmt, *Grammar, *Term, *Factor, *Base;

  Lang = mpc_new("lang");
  Stmt = mpc_new("stmt");
  Grammar = mpc_new("grammar");
  Term = mpc_new("term");
  Factor = mpc_new("factor");
  Base = mpc_new("base");

  mpc_define(Lang, mpc_apply_to(
                       mpc_total(mpc_predictive(mpc_many(mpca_stmt_fold, Stmt)),
                                 mpca_stmt_list_delete),
                       mpca_stmt_list_apply_to, st));

  mpc_define(Stmt,
             mpc_and(5, mpca_stmt_afold, mpc_tok(mpc_ident()),
                     mpc_maybe(mpc_tok(mpc_string_lit())), mpc_sym(":"),
                     Grammar, mpc_sym(";"), free, free, free, mpc_soft_delete));

  mpc_define(Grammar, mpc_and(2, mpcaf_grammar_or, Term,
                              mpc_maybe(mpc_and(2, mpcf_snd_free, mpc_sym("|"),
                                                Grammar, free)),
                              mpc_soft_delete));

  mpc_define(Term, mpc_many1(mpcaf_grammar_and, Factor));

  mpc_define(
      Factor,
      mpc_and(2, mpcaf_grammar_repeat, Base,
              mpc_or(6, mpc_sym("*"), mpc_sym("+"), mpc_sym("?"), mpc_sym("!"),
                     mpc_tok_brackets(mpc_int(), free), mpc_pass()),
              mpc_soft_delete));

  mpc_define(
      Base,
      mpc_or(5,
             mpc_apply_to(mpc_tok(mpc_string_lit()), mpcaf_grammar_string, st),
             mpc_apply_to(mpc_tok(mpc_char_lit()), mpcaf_grammar_char, st),
             mpc_tok(mpc_and(3, mpcaf_fold_regex, mpc_regex_lit(),
                             mpc_many(mpcf_strfold, mpc_oneof("ms")),
                             mpc_lift_val(st), free, free)),
             mpc_apply_to(
                 mpc_tok_braces(mpc_or(2, mpc_digits(), mpc_ident()), free),
                 mpcaf_grammar_id, st),
             mpc_tok_parens(Grammar, mpc_soft_delete)));

  mpc_optimise(Lang);
  mpc_optimise(Stmt);
  mpc_optimise(Grammar);
  mpc_optimise(Term);
  mpc_optimise(Factor);
  mpc_optimise(Base);

  if (!mpc_parse_input(i, Lang, &r)) {
    e = r.error;
  } else {
    /* FIX: Check if the output contains a returned Failure parser */
    if (r.output) {
      mpc_parser_t *err_parser = (mpc_parser_t *)r.output;
      e = mpc_err_fail(i, err_parser->data.fail.m);
      // Preventing use-after-free bug by moving the error to the heap
      e = mpc_err_export(i, e);

      mpc_delete(err_parser);
    } else {
      e = NULL;
    }
  }

  mpc_cleanup(6, Lang, Stmt, Grammar, Term, Factor, Base);

  return e;
}
```

## PR draft

### Pull Request: Graceful Error Handling for Undefined Parsers in mpca_lang

1. Issue Summary

When using mpca_lang to define a grammar, referencing a parser name that does not exist (e.g., due to a case mismatch like starspy vs Starspy) currently results in a Segmentation Fault. The library attempts to access memory beyond the provided variadic arguments and fails to propagate error messages correctly, leading to a hard crash instead of a helpful error message. 2. Root Cause Analysis

The failure occurs due to a chain of three distinct issues within mpc.c:

    Unbounded Argument Traversal: mpca_grammar_find_parser iterates through va_arg looking for a matching parser. Without a NULL sentinel at the end of the argument list, the loop continues into garbage memory, causing a crash when accessing p->name.

    Swallowed Errors: When a parser is not found, a "Failure" parser is created. However, mpca_stmt_list_apply_to attempts to use this Failure object in mpc_define, which is illegal. mpc_define overwrites the specific "Not Found" error with a generic "Unretained Parser" error, hiding the root cause.

    Use-After-Free: Even if the error is caught, the error message resides in the mpc_input_t scratchpad memory. When mpca_lang returns and cleans up the input, the error message is destroyed. Attempting to print this error in main results in a crash.

3. Implementation Details & Fixes
   A. Safety Sentinel (mpc.h)

We introduced a macro to wrap mpca_lang. Change:

```C
#define mpca_lang(flags, language, ...) \
 mpca_lang_internal(flags, language, **VA_ARGS**, NULL)
```

Why: This enforces a `NULL` sentinel at the end of the variadic argument list. This allows the internal loops to safely detect when they have exhausted all provided parsers, preventing the initial invalid memory access.
B. Fuzzy Matching & Error Reporting (mpc.c: mpca_grammar_find_parser)

We updated the parser search logic to handle the "Not Found" state gracefully. Change: Instead of crashing on p == NULL, we now iterate through the known parsers using strcasecmp. Why:

    1. Crash Prevention: It handles the end of the list safely.

    2. UX Improvement: It detects case-sensitivity typos (e.g., "lispy" vs "Lispy") and returns a helpful "Did you mean...?" error message, or lists all available parsers if no close match is found.

C. Error Propagation (mpc.c: mpca_stmt_list_apply_to)

We added a check for MPC_TYPE_FAIL. Change:

```C
if (left->type == MPC_TYPE_FAIL) {
// cleanup logic...
return left;
}
```

Why: Previously, this function blindly attempted to define the grammar using the returned object. If the object was a Failure (because the parser wasn't found), this would cause a logic error. We now detect the failure, clean up memory to prevent leaks, and bubble the error up the stack immediately.
D. Memory Safety (mpc.c: mpca_lang_st)

We added heap export logic before returning. Change:

```C
if (r.output) {
// ... extract error ...
e = mpc_err_export(i, e); // Move to heap
// ... cleanup ...
}
```

Why: The error object created internally lives in the "scratchpad" memory of mpc_input_t \*i. When mpca_lang finishes, i is deleted, and the error becomes a dangling pointer. mpc_err_export performs a deep copy of the error to the heap, ensuring it survives the return to the user's main function. 4. Verification

Reproduction Code:

```C
mpc_parser_t\* Lispy = mpc_new("lispy");
mpca_lang(MPCA_LANG_DEFAULT, "rule : <Lispy> ;", Lispy); // Case match
mpca_lang(MPCA_LANG_DEFAULT, "rule : <lispy> ;", Lispy); // Case mismatch
```

Before Fix: Segmentation Fault (core dumped).

After Fix: The program exits gracefully with the message:

```bash
<mpca_lang>: error: Parser '<lispy>' not found. Did you mean 'Lispy' ?
```

