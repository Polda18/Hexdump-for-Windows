# Hexdump for Windows
A `hexdump` tool for Windows, known from Linux bash. This repository holds source
code for a port of a well known Linux terminal tool called `hexdump` for Windows.
For simplicity, I stripped down `stdin` input for data, only accepting files,
and stripped down functions for custom format, as I personally don't use them
and most people do not need those features, I believe. Plus I would need to learn
more about these features anyway before implementing them in the Windows port. The
programming language of my choice for this project is C for the sake of practice
in low level programming. I also added `-h` switch to replace original Linux manual
page listing for `man hexdump`, which doesn't exist on Windows, or I would have to
create the `man` page separately in order for the Powershell `man` cmdlet to work
with `hexdump`. I recreate the entire tool from scratch, not looking into the original
source code, at least for version `1` of this tool because features I wanted to implement
are fairly simple and weren't much of an issue to get working properly without a source
code reference. I might end up asking for help in future for more features to be supported.

If you want full feature port, let me know and I'll set them up for version `2`. Versions
`1.X` will be intended for bugs hunting if any bugs will be found, I also plan releasing
snapshot versions before version `1` (versions `0.X`) purely for you to test the tool
before an official release is hit, and help me find most important bugs before the tool
is ready for an official launch.

I'll also try and suggest Microsoft to include this tool in the system for future releases.
I think that tool might be super useful for developers that rely on Windows platform and
cannot for some reason access Linux tools (not even from WSL). `hexdump` is essential
tool for debugging and testing. I think that it should be included in Windows, alongside
with a `hexedit` tool, which I guess might be subject for future project as well...

## How to use this tool
Refer to help page given by this command:
```
C:\>hexdump -h
------------------------------------------------------------------------
  Hexadecimal dump command line tool v0.1 ALPHA

  Ported to Windows by Polda18
  https://polda18.github.io/
------------------------------------------------------------------------

This tool displays binary data in human readable format
in console window via standard output pipe.

Usage: hexdump [-h] [-bcCdox] [-v] [-n <length>] [-s <offset>] <path\to\filename.ext>
  You can also use standard output redirection '>' to save dump into file
  or use pipe '|' to redirect standard output to another cmd tool.

Switches you can use:

  -h     Prints out this help. Should be stated as standalone switch.

  -b     One-byte octal display.  Displays the input file offset in hexadecimal,
         followed by sixteen space-separated, three column, zero-filled, bytes
         of input file data per line.

  -c     One-byte character display.  Displays the input file offset in hexa-
         decimal, followed by sixteen space-separated, three column, space-
         filled, characters of input file data per line.

  -C     Canonical hex+ASCII display.  Displays the input file offset in hexa-
         decimal, followed by sixteen space-separated, two column, hexadecimal bytes,
         followed by the same sixteen bytes in %_p format enlosed in '|' characters.

  -d     Two-byte decimal display.  Displays the input file offset in hexadecimal,
         followed by eight space-separated, five column, zero-filled, two-byte units
         of input file data, in unsigned decimal, per line.

  -o     Two-byte octal display.  Displays the input file offset in hexadecimal,
         followed by eight space-separated, six column, zero-filled, two-byte quan-
         tities of input file data, in octal, per line.

  -x     Two-byte hexadecimal display.  Displays the input file offset in hexa-
         decimal, followed by eight, space-separated, four column, zero-filled, two-
         byte quantities of input file data, in hexadecimal, per line.

  -n <length>
         Interpret only <length> bytes of input file data.
         Should be stated as standalone switch.

  -s <offset>
         Skip <offset> bytes from the beginning of the input file.  By default,
         <offset> is interpreted as an unsigned decimal number. With a leading '0x'
         or '0X', <offset> is interpreted as a hexadecimal number, otherwise
         with a leading '0', <offset> is interpreted as an octal number. Appending
         the character 'b', 'k', or 'm' to <offset> causes it to be interpreted
         as a multiple of 512, 1024, or 1048576, respectively.
         Should be stated as standalone switch.

  -v     Causes hexdump to display all input file data.  Without the -v switch,
         any number of groups of output lines, which would be identical to the imme-
         diately preceding group of output lines (except for the input file offsets),
         are replaced with a line comprised of a single asterisk.
         Should be stated as standalone switch.

A tip for Windows users: If you need to hexdump a file with spaces in its name or path,
make sure the path of the file including the file name is surrounded with quotes. Command
line automatically inteprets sequence of characters between quotes as a single argument.
```

Anyone who knows this tool from Linux will be immediately familiar with the function.
Windows port has few minor changes. First of all, output isn't UTF-8, on most systems
output will refer to DOS codepage of your system depending on your language. Bytes above
`0x80` (including) in `-c` output are displayed as is, implying national charset.

Please, report any bugs you find. This software is still in development stage and
though I figured out most of obvious bugs, many of them might still be undiscovered.
That's why I decided to release ALPHA and BETA versions first before a stable release
is ready to be deployed.
