# Development notes

## Build your own Lisp

### Run the files

```bash
gcc -std=c99 -Wall main.c -o main
```

```bash
gcc -ggdb main.c -o main
gdb -q ./main
disass main
set disassembly-flavor intel
```

Prologue

```bash
 0x0000000000001139 <+0>:     push   rbp
   0x000000000000113a <+1>:     mov    rbp,rsp
   0x000000000000113d <+4>:     sub    rsp,0x10

```

Epilogue

```bash
 0x0000000000001157 <+30>:    mov    eax,0x0
   0x000000000000115c <+35>:    leave
   0x000000000000115d <+36>:    ret

```
