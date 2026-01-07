# Fixes explained logically

`mpc.h`
This is the header file of the mpc library. Contains mainly function signatures, type definitions and macros - it tells the library that things exist and how to use them. `mpc.c` is the actual implementation of `mpc.h` (a bit loosely)

This fix:

```C
/* adding sentinel NULL macro to enforce the fix in mpc.c */
mpc_err_t *mpca_lang_internal(int flags, const char *language, ...);
#define mpca_lang(flags, language, ...)                                        \
  mpca_lang_internal(flags, language, __VA_ARGS__, NULL)
```

Conceptually, this code tells me when I am grabbing something that is not a parser (i.e. garbage)
`mpac_lang` already takes a variable number of parsers:

```C
mpca_lang(FLAGS, "grammar...", Parser1, Parser2, Parser3);
```

Internally, the function uses `va_arg` to read each parser one at a time. But when I called `va_arg` more times than we have arguments available, then it just returns whatever happens to be in memory at that location (i.e. [undefined](https://en.cppreference.com/w/c/variadic/va_arg.html)); so, the idea was to instead have it return a `NULL` intentionally, to avoid the program to segfault when a garbage pointer is de-referenced.

A sample of the debugging flow:

```C
DEBUG: va_arg call #4 returned 0x557361724430, looking for 'Lispy'  // -> valid heap pointer
DEBUG: va_arg call #5 returned 0x7ffc7166ee28, looking for 'Lispy'  // ->  stack garbage
DEBUG: va_arg call #6 returned 0x100000000, looking for 'Lispy'     // ->  unmapped address
[1]    2963963 segmentation fault
```

I have explored other possible approaches: counting parameters, re-implementing `mcpa_lang` with a fixed amount of parameters,, using the [builder pattern](https://stackoverflow.com/questions/17604811/builder-pattern-in-c), and many more impractical ideas. So in the end, the easiest way was the Sentinel `NULL` check.

```

```

`mpc.c`
This is the main library file. All the functions, structs and global variables with their implementation is found here.

This fix:

```C
/* adding tracking fields */

typedef struct {
  va_list *va;
  int parsers_num;
  mpc_parser_t **parsers;
  int flags;
  int va_exhausted;
  char *error_msg;
} mpca_grammar_st_t;
```

- The `error_msg` field it's used to store the error we pass to the wrapper functions later on in the program.
- The `va_exhausted` field int is used as boolean (logically) to track `va_arg` return:

```bash
Call 1: looking for "number"    → found, va_exhausted still 0
Call 2: looking for "operator"  → found, va_exhausted still 0
Call 3: looking for "expr"      → found, va_exhausted still 0
Call 4: looking for "Starspy"   → not found, hit NULL, set va_exhausted = 1
Call 5: looking for "rule"      → va_exhausted is 1, skip va_arg entirely

```

This fix:
High level, the function, given a parser name `x`(like `number` or `Lispy`), finds and returns the corresponding parser from the valid list.

Two main cases I have found:

- Numeric reference (`<0>`, `<1>`) - e.g. If is_number -> finds parser by position
- Named reference (`<number>`, `<Lispy>`) - e.g. else search already retrieved parsers, and if `va_exhausted`, build error message and do so while(1): keep retrieving from `va_arg` until found `NULL`

```C

static mpc_parser_t *mpca_grammar_find_parser(char *x, mpca_grammar_st_t *st) {
  int i;
  mpc_parser_t *p;

  if (is_number(x)) {
    i = strtol(x, NULL, 10);
    while (st->parsers_num <= i) {
      if (st->va_exhausted) {
        return mpc_failf("No Parser in position %i! Only supplied %i Parsers!", i, st->parsers_num);
      }
      st->parsers_num++;
      st->parsers = realloc(st->parsers, sizeof(mpc_parser_t*) * st->parsers_num);
      st->parsers[st->parsers_num-1] = va_arg(*st->va, mpc_parser_t*);
      if (st->parsers[st->parsers_num-1] == NULL) {
        st->va_exhausted = 1;
        return mpc_failf("No Parser in position %i! Only supplied %i Parsers!", i, st->parsers_num);
      }
    }
    return st->parsers[st->parsers_num-1];
  } else {
    for (i = 0; i < st->parsers_num; i++) {
      mpc_parser_t *q = st->parsers[i];
      if (q == NULL) { return mpc_failf("Unknown Parser '%s'!", x); }
      if (q->name && strcmp(q->name, x) == 0) { return q; }
    }

    if (st->va_exhausted) {
      char msg[1024];
      strcpy(msg, "");
      for (i = 0; i < st->parsers_num; i++) {
        if (st->parsers[i] && st->parsers[i]->name) {
          if (mpc_strcasecmp(st->parsers[i]->name, x) == 0) {
            return mpc_failf("Unknown Parser '%s'! Did you mean '%s'?", x, st->parsers[i]->name);
          }
          if (strlen(msg) + strlen(st->parsers[i]->name) + 5 < 1024) {
            strcat(msg, "'"); strcat(msg, st->parsers[i]->name); strcat(msg, "' ");
          }
        }
      }
      if (strlen(msg) == 0) { return mpc_failf("Unknown Parser '%s'!", x); }
      return mpc_failf("Unknown Parser '%s'! Available: %s", x, msg);
    }

    while (1) {
      p = va_arg(*st->va, mpc_parser_t*);
      if (p == NULL) {
        int j;
        char msg[1024];
        /* Tracking */
        st->va_exhausted = 1;
        strcpy(msg, "");
        for (j = 0; j < st->parsers_num; j++) {
          if (st->parsers[j] && st->parsers[j]->name) {
            if (mpc_strcasecmp(st->parsers[j]->name, x) == 0) {
              /*  capturing error */
              if(st->error_msg == NULL) {
                st->error_msg = malloc(strlen(x) + strlen(st->parsers[j]->name) + 50);
                sprintf(st->error_msg, "Unknown Parser '%s'! Did you mean '%s'?", x, st->parsers[j]->name);
              }
              return mpc_failf("Unknown Parser '%s'! Did you mean '%s'?", x, st->parsers[j]->name);
            }
            if (strlen(msg) + strlen(st->parsers[j]->name) + 5 < 1024) {
              strcat(msg, "'"); strcat(msg, st->parsers[j]->name); strcat(msg, "' ");
            }
          }
        }
        if (st->error_msg == NULL) {
          if (strlen(msg) ==0) {
            st->error_msg = malloc(strlen(x) + 30);
            sprintf(st->error_msg, "Unknown Parser '%s'!", x);
          } else {
            st->error_msg = malloc(strlen(x) + strlen(msg) + 40);
            sprintf(st->error_msg, "Unknown Parser '%s'! Available: %s", x, msg);
          }
        }
        if (strlen(msg) == 0) { return mpc_failf("Unknown Parser '%s'!", x); }
        return mpc_failf("Unknown Parser '%s'! Available: %s", x, msg);
      }
      st->parsers_num++;
      st->parsers = realloc(st->parsers, sizeof(mpc_parser_t*) * st->parsers_num);
      st->parsers[st->parsers_num-1] = p;
      if (strcmp(p->name, x) == 0) { return p; }
    }
  }
}
```

## Initialization sites

Five functions initialise `mpca_grammar_st_t` and call the grammar parser. They all follow the same pattern. We can use `mpca_lang_internal` as primary example and because it's the one my macro calls anyway.

The pattern is pretty much the same in all of them:

```text
1. Declare struct and variables
2. Initialize all struct fields (including new ones)
3. Call mpca_lang_st to parse grammar
4. Check st.error_msg and surface error if present  ← FIX
5. Clean up and return
```
