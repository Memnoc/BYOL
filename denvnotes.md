# Development notes

## Build your own Lisp

### Run the files

The book suggests this compile command
```bash
gcc -std=c99 -Wall main.c -o main
```

I use GDB on Linux
```bash
gcc -ggdb main.c -o main
gdb -q ./main
disass main
set disassembly-flavor intel
```

