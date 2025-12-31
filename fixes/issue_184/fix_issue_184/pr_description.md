# Title: Fix segfault on parser name mismatch in mpca_lang (#184)

## Description:

- Fixes #184

## Problem

When a parser name in the grammar string doesn't match any supplied parser (e.g., `Lispy`` vs `mpc_new("lispy"))`, `mpca_lang`would segfault. The`va_arg` call reads past the end of the argument list, returning garbage pointers that crash when dereferenced.

## Fix

- Add `NULL` sentinel via macro wrapper — `mpca_lang` now auto-appends NULL
- Track `va_exhausted` flag to prevent reading garbage after sentinel
- Capture first error message in `error_msg` field for better reporting
- Fail fast at grammar definition time instead of silently deferring to parse time
- Quality of life improvement - provide case-insensitive suggestions (e.g., "Did you mean 'lispy'?")

## Behavior Change

- Errors are now caught at `mpca_lang` call time rather than during `mpc_parse`. This is by design - seems to me a fast failure it's a better outcome than a silent corruption.

## Tests

- Updated `test_missingrule` for new fail-fast behavior
- Added `issue_184.c` test suite for case-insensitive detection
