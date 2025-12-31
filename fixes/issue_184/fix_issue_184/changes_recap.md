# A recap of the changes for issue_184

## mpc.h

- Added `mpca_lang`\_internal function declaration
- Added `mpca_lang` macro wrapper that appends NULL sentinel

## mpc.c

**Struct change:**

- `mpca_grammar_st_t` — added `va_exhausted` and `error_msg` fields

**Functions modified:**

`mpca_grammar_find_parser` — added va_exhausted checks, error_msg capture, case-insensitive suggestions
`mpca_grammar` — added `st.va_exhausted = 0` and `st.error_msg = NULL` initialization, error_msg cleanup
`mpca_stmt_list_apply_to` — (verify if you changed this one)
`mpca_lang_file` — added initialization, error_msg check after `mpca_lang_st`
`mpca_lang_pipe`` — added initialization, error_msg check after `mpca_lang_st`
`mpca_lang_internal`— renamed from mpca_lang, added initialization, error_msg check after`mpca_lang_st`
`mpca_lang_contents`— added initialization, error_msg check after`mpca_lang_st`

**tests/grammar.c**

- `test_missingrule` — updated to expect fail-fast behavior (error at definition time)

**tests/test.c**

- Added `suite_issue_184` declaration and registration

**tests/issue_184.c**

New file — test for case-insensitive parser name suggestion

**Makefile**

Added `-Wno-variadic-macros` flag

## Possible Commit naming

```bash
git add mpc.c mpc.h tests/grammar.c tests/test.c tests/issue_184.c
git commit -m "Fix segfault on parser name mismatch in mpca_lang
```

- Add NULL sentinel via macro wrapper for mpca_lang
- Track va_list exhaustion to prevent reading garbage pointers
- Capture first error message for proper reporting
- Fail fast at grammar definition time instead of parse time
- Add test for issue #184 (case-insensitive parser suggestion)
- Update test_missingrule to expect new fail-fast behavior"

```

```
