/**************************************************
 * Hexdump for Windows
 *-------------------------------------------------
 *
 * Command line tool inspired by Linux tool
 * hexdump - stripped to bare hexadecimal dump
 * of given file with option to limit dump length
 *
 * File: info.c
 *
 *-------------------------------------------------
 * Made by Marek Pol·Ëek (Polda18)
 **************************************************/

// Global libraries
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

// Local libraries
#include "info.h"

// Header print - static function
static void print_header(void) {
    fprintf(stdout, "------------------------------------------------------------------------\n");
    fprintf(stdout, "  Hexadecimal dump command line tool v%s\n", VERSION);
    putchar('\n');
    fprintf(stdout, "  Ported to Windows by Polda18\n");
    fprintf(stdout, "  https://polda18.github.io/\n");
    fprintf(stdout, "------------------------------------------------------------------------\n\n");
}

// Help page print
void print_help(void) {
    print_header();     // Print header first

    fprintf(stdout, "This tool displays binary data in human readable format\n");
    fprintf(stdout, "in console window via standard output pipe.\n\n");

    fprintf(stdout, "Usage: hexdump [-h] [-bcCdox] [-v] [-n <length>] [-s <offset>] <path\\to\\filename.ext>\n");
    fprintf(stdout, "  You can also use standard output redirection \'>\' to save dump into file\n");
    fprintf(stdout, "  or use pipe \'|\' to redirect standard output to another cmd tool.\n\n");

    fprintf(stdout, "Switches you can use:\n\n");

    fprintf(stdout, "  -h     Prints out this help. Should be stated as standalone switch.\n\n");

    fprintf(stdout, "  -b     One-byte octal display.  Displays the input file offset in hexadecimal,\n");
    fprintf(stdout, "         followed by sixteen space-separated, three column, zero-filled, bytes\n");
    fprintf(stdout, "         of input file data per line.\n\n");

    fprintf(stdout, "  -c     One-byte character display.  Displays the input file offset in hexa-\n");
    fprintf(stdout, "         decimal, followed by sixteen space-separated, three column, space-\n");
    fprintf(stdout, "         filled, characters of input file data per line.\n\n");

    fprintf(stdout, "  -C     Canonical hex+ASCII display.  Displays the input file offset in hexa-\n");
    fprintf(stdout, "         decimal, followed by sixteen space-separated, two column, hexadecimal bytes,\n");
    fprintf(stdout, "         followed by the same sixteen bytes in %%_p format enlosed in \'|\' characters.\n\n");

    fprintf(stdout, "  -d     Two-byte decimal display.  Displays the input file offset in hexadecimal,\n");
    fprintf(stdout, "         followed by eight space-separated, five column, zero-filled, two-byte units\n");
    fprintf(stdout, "         of input file data, in unsigned decimal, per line.\n\n");

    fprintf(stdout, "  -o     Two-byte octal display.  Displays the input file offset in hexadecimal,\n");
    fprintf(stdout, "         followed by eight space-separated, six column, zero-filled, two-byte quan-\n");
    fprintf(stdout, "         tities of input file data, in octal, per line.\n\n");

    fprintf(stdout, "  -x     Two-byte hexadecimal display.  Displays the input file offset in hexa-\n");
    fprintf(stdout, "         decimal, followed by eight, space-separated, four column, zero-filled, two-\n");
    fprintf(stdout, "         byte quantities of input file data, in hexadecimal, per line.\n\n");

    fprintf(stdout, "  -n <length>\n");
    fprintf(stdout, "         Interpret only <length> bytes of input file data.\n");
    fprintf(stdout, "         Should be stated as standalone switch.\n\n");
    
    fprintf(stdout, "  -s <offset>\n");
    fprintf(stdout, "         Skip <offset> bytes from the beginning of the input file.  By default,\n");
    fprintf(stdout, "         <offset> is interpreted as an unsigned decimal number. With a leading \'0x\'\n");
    fprintf(stdout, "         or \'0X\', <offset> is interpreted as a hexadecimal number, otherwise\n");
    fprintf(stdout, "         with a leading \'0\', <offset> is interpreted as an octal number. Appending\n");
    fprintf(stdout, "         the character \'b\', \'k\', or \'m\' to <offset> causes it to be interpreted\n");
    fprintf(stdout, "         as a multiple of 512, 1024, or 1048576, respectively.\n");
    fprintf(stdout, "         Should be stated as standalone switch.\n\n");

    fprintf(stdout, "  -v     Causes hexdump to display all input file data.  Without the -v switch,\n");
    fprintf(stdout, "         any number of groups of output lines, which would be identical to the imme-\n");
    fprintf(stdout, "         diately preceding group of output lines (except for the input file offsets),\n");
    fprintf(stdout, "         are replaced with a line comprised of a single asterisk.\n");
    fprintf(stdout, "         Should be stated as standalone switch.\n\n");

    fprintf(stdout, "A tip for Windows users: If you need to hexdump a file with spaces in its name or path,\n");
    fprintf(stdout, "make sure the path of the file including the file name is surrounded with quotes. Command\n");
    fprintf(stdout, "line automatically inteprets sequence of characters between quotes as a single argument.\n");

    // TODO: Help page of hexdump
}

// Error message print
void print_error(char* msg) {
    print_header();     // Print header first

    fprintf(stderr, "ERROR! %s\n", msg);
    fprintf(stderr, "If you need to learn how to use this tool, run hexdump -h\n");
}

// File error message print
void print_file_error(char* msg) {
    char* system_msg = malloc(MAX_MSG);
    if (!system_msg) {
        print_error_memory("Out of memory");
        return;         // If malloc for some reason doesn't work
    }

    print_header();     // Print header first

    strerror_s(system_msg, MAX_MSG, errno);

    fprintf(stderr, "ERROR! %s\n", msg);
    fprintf(stderr, "System message: %s\n", system_msg);

    free(system_msg);   // Tidy up
}

// Memory error print
void print_error_memory(char* msg) {
    print_header();     // Print header first

    // Memory error occured
    fprintf(stderr, "ERROR! %s\n", msg);
    fprintf(stderr, "Consider freeing up your memory space!\n");
}