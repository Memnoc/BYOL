# Development notes

## Build your own Lisp

`stdlib.h` provides access to `free()` which is important to de-allocate memory once allocated.
`editline` is something that provides us with access to `editline` functions, `readlin` and `add_history`.

**Preprocessors** is how you call the header files - it's a portability thing, so it makes my code compile in somebody else's machine.
You can also use (like with this book) the preprocessor to detect which operating system the code is being compiled on, and to use this to emit different code!

### Run the files

The book suggests this compile command:

```bash
gcc -std=c99 -Wall main.c -o main
```

Once you start including new headers, add the `-ledit` flag

```bash
gcc -std=c99 -Wall  -g parsing.c mpc.c -o parsing -ledit -lm
```

I use GDB on Linux

```bash
gcc -ggdb main.c -o main
gdb -q ./main
disass main
set disassembly-flavor intel
```

The book makes use of a REPL (read-print-evaluate-loop) system we get to create from scratch!
For the first iteration, it remains something that prompts the user and echoes the answer, the plans are to later extend it to something more like a LISP REPL.

### Omitted code from the book

This is not necessary, as `<editline/reahdline.h>` has a wrapper for history.
`#include <editline/history.h>
`
