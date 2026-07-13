# Debug Journal

*(Bản tiếng Việt: [DEBUG_JOURNAL.vi.md](DEBUG_JOURNAL.vi.md))*

Two bugs were deliberately introduced and then found using GDB and Valgrind, to demonstrate real debugging workflow rather than just describing the tools.

## Bug 1 — Missing `break`: one new connection duplicated across every UE slot (found with GDB)

**How the bug was introduced:** commented out the `break;` statement in the loop that finds an empty slot for a newly accepted connection.

**Symptom:** connecting a single `nc` client caused the server to log 5 separate "UE connected" lines, all reporting the same file descriptor.

**Root cause:** without `break`, the loop kept scanning the rest of the (still-empty) `clients[]` array after finding the first free slot, assigning the same `new_fd` to every remaining slot instead of stopping after the first match.

**How it was found with GDB:**

```
(gdb) break main.c:54
(gdb) run
# ... connected once via nc ...
Breakpoint 1, main () at main.c:54
54          clients[i].socket = new_fd;
(gdb) print i
$1 = 0
(gdb) continue
Breakpoint 1, main () at main.c:54
(gdb) print i
$2 = 1
(gdb) continue
...
$5 = 4
```

A single incoming connection caused the breakpoint to hit five times in a row, with `i` incrementing from 0 to 4 each time — clear proof the loop wasn't stopping where it should.

**Fix:** restored the `break;` statement.

**Verification:** re-ran the same GDB session after the fix. The breakpoint at `main.c:54` was hit exactly once per connection, then execution moved on normally (into the ATTACH_REQUEST handling) without hitting it again.

## Bug 2 — Memory leak: `log_message` allocated but never freed (found with Valgrind)

**How the bug was introduced:** added a `malloc(100)` call to build a small per-connection log message, intentionally without a matching `free()`.

```c
char *log_message = malloc(100);
sprintf(log_message, "Log for UE connected at fd=%d", new_fd);
printf("%s\n", log_message);
// intentionally missing: free(log_message);
```

**How it was found with Valgrind:**

```
valgrind --leak-check=full ./mini-rbs
```

After connecting a couple of UEs and stopping the server (Ctrl+C), Valgrind reported:

```
200 bytes in 2 blocks are definitely lost in loss record 2 of 3
   at 0x4846828: malloc (...)
   by 0x10979E: main (main.c:69)

LEAK SUMMARY:
   definitely lost: 200 bytes in 2 blocks
```

`definitely lost` is Valgrind's most severe category — memory with no remaining pointer to it, unrecoverable. The report pointed to the exact `malloc` call responsible (`main.c:69`).

**Fix:** added `free(log_message);` right after using it, before the connection-handling block ends.

**Verification:** re-ran under Valgrind after the fix:

```
total heap usage: 5 allocs, 4 frees, 1,424 bytes allocated

LEAK SUMMARY:
   definitely lost: 0 bytes in 0 blocks
   indirectly lost: 0 bytes in 0 blocks
   possibly lost: 0 bytes in 0 blocks
   still reachable: 1,024 bytes in 1 blocks
```

All 4 of our `malloc(100)` calls (one per connected UE) were matched with a `free()`. The one remaining "still reachable" block is an internal glibc allocation unrelated to our code — Valgrind does not count it as a leak.

## Takeaway

Both bugs were reproducible, diagnosed with the actual tool (not guesswork via `printf`), and verified fixed by re-running the same tool afterward — the standard debug-fix-verify loop expected in real development work.
