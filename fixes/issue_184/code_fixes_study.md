# Fixes explained logically

`mpc.h`
This is the header file of the mpc library. Contains mainly function signatures, type definitions and macros - it tells the library that things exist and how to use them. `mpc.c` is the actual implementation of `mpc.h` (a bit loosely)

This fix:

```C
/* FIX: adding sentinel NULL macro to enforce the fix in mpc.c */
mpc_err_t *mpca_lang_internal(int flags, const char *language, ...);
#define mpca_lang(flags, language, ...)                                        \
  mpca_lang_internal(flags, language, __VA_ARGS__, NULL)
```

Conceptually, this code tells me when I am grabbing something that is not a parser (i.e. garbage)
