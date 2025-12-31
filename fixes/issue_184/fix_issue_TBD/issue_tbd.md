# Future PR: Pre-existing Memory Leak

## Title: Fix memory leak in mpc_parse_fold/mpc_export

### Description:

```bash
Valgrind reports 256 bytes leaked in 4 blocks from `mpc_export` called via `mpc_parse_fold`.

Reproduction:
valgrind --leak-check=full ./build/test-file

```

**Stack trace:**

```bash
==802188== 256 bytes in 4 blocks are definitely lost in loss record 1 of 1 ==802188== at 0x4A727A8: malloc (vg_replace_malloc.c:446) ==802188== by 0x400A713: mpc_export (mpc.c:302) ==802188== by 0x400A713: mpc_parse_fold (mpc.c:1024) ==802188== by 0x4014FF3: mpc_parse_run (mpc.c:1265) ==802188== by 0x4014FF3: mpc_parse_input (mpc.c:1358) ==802188== by 0x401513F: mpc_parse (mpc.c:1371) ==802188== by 0x4003C88: test_sepby (core.c:264)

```

The leak originates from `test_sepby` in core.c and exists on master branch prior to any recent changes.
