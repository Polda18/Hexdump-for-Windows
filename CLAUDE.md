# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

Windows C port of the Linux `hexdump` CLI. Intentionally a *stripped* port: only file input is supported (no stdin), and custom-format strings (`-e`, `-f`) from the GNU original are deliberately omitted. Adds a `-h` switch in place of the `man hexdump` page that doesn't exist on Windows. Built from scratch without referencing the original sources, so don't assume behavioral parity beyond what's documented in `README.md` / `info.c`'s help text.

## Build

MSBuild project (`hexdump.sln` / `hexdump.vcxproj`), Platform Toolset **v142** (Visual Studio 2019). Four configurations: `{Debug,Release} x {Win32,x64}`.

```powershell
# from a Developer PowerShell / VS dev cmd
msbuild hexdump.sln /p:Configuration=Release /p:Platform=x64
# or open hexdump.sln in Visual Studio and build
```

Output binary: `<Platform>\<Configuration>\hexdump.exe` (e.g. `x64\Release\hexdump.exe`).

There are no tests, no linter config, and no CI. Validate changes by running the built binary against sample files and comparing output to the expected format documented in `info.c::print_help`.

**Charset quirk:** `Debug|Win32` passes `/execution-charset:cp852` to cl. The other three configurations don't. Source files contain Czech characters in author-attribution comments (`Marek Polášek`) — if you save them in a different encoding the Win32 Debug build can break in surprising ways. Prefer not to re-save source files unless you know what encoding they're in.

## Architecture

Three translation units, dispatched from `main.c`:

- **`main.c`** — argv parsing as a character-by-character state machine. Sets bitflags from `switches.h` (`SW_DUMP_BYTE_OCTAL`, `SW_VERBOSE`, `SW_INVALID`, `SW_FILEUNKNOWN`, `SW_OFFSETFORMAT`, ...) into a single `unsigned int switches` register. After parsing, error flags are checked in **priority order** (help → invalid → length format → offset format → missing file → misplaced file), each producing a specific exit code (1–5). Then the appropriate `dump_*` function is dispatched based on the dump flag bits.
- **`dumps.c` / `dumps.h`** — one `dump_*` function per output mode (`dump_plain_hex`, `dump_byte_octal`, `dump_byte_char`, `dump_canonical`, `dump_word_decimal`, `dump_word_octal`, `dump_word_hex`). All read 16-byte rows from `FILE*`, format them, and update `*position`. The non-verbose collapse-to-asterisk behavior (suppress runs of identical lines) is implemented inside each dump function via `buffer_current` / `buffer_previous` comparison. Format constants (column widths, escape sequences for `-c`, ASCII printable boundaries) live in `dumps.h`.
- **`info.c` / `info.h`** — `print_help` (the `man` replacement), `print_error`, `print_file_error` (formats `errno` via `strerror_s`), `print_error_memory`. All prepend a header banner. `VERSION` macro lives here.

### Argument-parsing rules baked into `main.c`

- The file path must be the **last** argument; appearing earlier triggers `SW_FILEELSEWHERE`.
- `-h`, `-n`, `-s`, `-v` must each be a **standalone** switch (not combined like `-hv`); detected by checking `j == 1` in the per-character loop. Dump-mode flags (`-bcCdox`) can be combined.
- `-s <offset>` parses C-style numeric prefixes itself: leading `0x`/`0X` → hex, leading `0` → octal, otherwise decimal; trailing `b`/`k`/`m` multiplies by 512 / 1024 / 1048576.

### Version bumps

`VERSION` string lives in **two** places that must stay in sync: `info.h` (`#define VERSION`) and `resources.rc` (`FILEVERSION`, `PRODUCTVERSION`, and the `StringFileInfo` block).

### Output encoding

Console output is **not** UTF-8 — it follows the active OEM/DOS codepage. Bytes ≥ `0x80` in `-c` mode are emitted raw, so they render per the user's national codepage. This is intentional; don't "fix" it to UTF-8 without considering the design note in `README.md`.

## Known rough edges (to be aware of when editing)

- `dumps.h` declares `dump_byte_octal` and `dump_word_decimal` without an `offset` parameter while the other dump prototypes take one — easy to mis-call. Check signatures before adding new dump variants.
- `print_file_error` reads `errno` after a `malloc` call; if the malloc itself fails, `errno` may have been overwritten before `strerror_s` runs. Be careful when reordering allocations near error paths.
