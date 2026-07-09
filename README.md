# MiniRBS — Mini Radio Base Station Simulator

A small-scale simulation of a Radio Base Station (RBS) in a mobile network, written in **plain C** on Linux. A personal project built to practice core telecom software engineering skills: socket programming, state machines, Makefile/Autotools, unit testing, debugging with GDB, and memory checking with Valgrind.

*(Bản tiếng Việt: [README.vi.md](README.vi.md))*

## Background

In a mobile network, the RBS (called eNB in 4G, gNB in 5G) is the radio-facing contact point between a UE (User Equipment — a phone or similar device) and the rest of the network. This project simulates, at a very small scale, how an RBS tracks and manages connections with multiple UEs at once, using a simplified text-based message protocol (ATTACH / PING / DETACH) over TCP sockets.

## Current status

🚧 Work in progress.

- [x] Basic build workflow: manual `gcc` compilation, then a `Makefile`
- [x] `struct UE` (id, state) and pointer-based access — the foundation for tracking a list of UEs later
- [ ] TCP echo server/client (in progress)
- [ ] ATTACH / PING / DETACH protocol + state machine
- [ ] Handling multiple UEs concurrently with `select()`
- [ ] Unit tests with Check
- [ ] Bash script for automated build + test
- [ ] Autotools setup (`configure.ac` / `Makefile.am`) — time permitting
- [ ] Debug journal: one bug found with GDB, one memory leak caught with Valgrind

## Build & run

Requires: `gcc`, `make`, Linux (tested on Linux Mint).

```bash
make
./mini-rbs
```

`make` only recompiles when the source has changed (based on file timestamps), so it skips rebuilding when nothing is new.

## Planned tech stack

C, POSIX sockets, `select()`, Makefile (Autotools if time allows), Check (unit testing), Bash, Git, GDB, Valgrind.

## Note

This is a learning/skill-building project, built in a short timeframe to prepare for a radio/telecom software development internship application. It does not implement real 3GPP protocols — it simulates the core ideas (connection, state, message exchange) at a simplified level, enough to explain clearly in an interview.