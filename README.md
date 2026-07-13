# MiniRBS — Mini Radio Base Station Simulator

A small-scale simulation of a Radio Base Station (RBS) in a mobile network, written in **plain C** on Linux. A personal project built to practice core telecom software engineering skills: socket programming, state machines, Makefile/Autotools, unit testing, debugging with GDB, and memory checking with Valgrind.

*(Bản tiếng Việt: [README.vi.md](README.vi.md))*

## Background

In a mobile network, the RBS (called eNB in 4G, gNB in 5G) is the radio-facing contact point between a UE (User Equipment — a phone or similar device) and the rest of the network. This project simulates, at a very small scale, how an RBS tracks and manages connections with multiple UEs at once, using a simplified text-based message protocol (ATTACH / PING / DETACH) over TCP sockets.

## Current status

🚧 Work in progress.

- [x] Basic build workflow: manual `gcc` compilation, then a `Makefile`
- [x] `struct UE` (id, state) and pointer-based access — the foundation for tracking a list of UEs later
- [x] TCP server: `socket` → `bind` → `listen` → `accept` → `recv`/`send`, with a loop handling multiple messages per connection
- [x] ATTACH / PING / DETACH protocol + state machine (`enum UEState`, each message checked against current state before accept/reject)
- [x] Handling multiple UEs concurrently with `select()` — a fixed-size array of UEs, a "server full" rejection path, and correct cleanup (no leaked file descriptors)
- [x] Unit tests with Check — protocol logic refactored into `protocol.h`/`protocol.c` (decoupled from sockets), 8 test cases covering all messages and accept/reject branches
- [x] Bash script (`build_and_test.sh`) for automated build + test, using `set -e` to stop on the first failure
- [ ] Autotools setup (`configure.ac` / `Makefile.am`) — time permitting
- [ ] Debug journal: one bug found with GDB, one memory leak caught with Valgrind

## Build & run

Requires: `gcc`, `make`, `check` (unit test library), Linux (tested on Linux Mint).

```bash
make          # build the server
./mini-rbs

make test     # build and run the unit test suite

./build_and_test.sh   # or run both steps at once, stops on first failure
```

`make` only recompiles when the source has changed (based on file timestamps), so it skips rebuilding when nothing is new.

## Project structure

- `main.c` — server entry point: socket setup, `select()` loop, I/O
- `protocol.h` / `protocol.c` — protocol/state-machine logic (`process_message`), decoupled from sockets so it can be unit tested directly
- `test_protocol.c` — unit tests (Check framework)
- `build_and_test.sh` — one-command build + test automation
- `docs/notes.md` — technical notes and gotchas found along the way

## Planned tech stack

C, POSIX sockets, `select()`, Makefile (Autotools if time allows), Check (unit testing), Bash, Git, GDB, Valgrind.

## Note

This is a learning/skill-building project, built in a short timeframe to prepare for a radio/telecom software development internship application. It does not implement real 3GPP protocols — it simulates the core ideas (connection, state, message exchange) at a simplified level, enough to explain clearly in an interview.
