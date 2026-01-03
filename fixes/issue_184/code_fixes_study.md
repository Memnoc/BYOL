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

```









```
