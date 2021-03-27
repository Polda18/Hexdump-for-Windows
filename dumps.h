/**************************************************
 * Hexdump for Windows
 *-------------------------------------------------
 *
 * Command line tool inspired by Linux tool
 * hexdump - stripped to bare hexadecimal dump
 * of given file with option to limit dump length
 *
 * File: dumps.h
 *
 *-------------------------------------------------
 * Made by Marek Pol·Ëek (Polda18)
 **************************************************/

#pragma once

#include <stdio.h>                     // Measure for file descriptor
#ifndef __DUMPS_H__
#define __DUMPS_H__

 // Define basic types
typedef unsigned short                  uword_t;
typedef unsigned char                   ubyte_t;

// Define constants
#define MIN_OFFSET_DIGITS               7
#define OVERFLOW_VALUE                  0x10000000
#define BYTE_SIZE                       sizeof(ubyte_t)
#define WORD_SIZE                       sizeof(uword_t)
#define BYTE_ARRAY_NUM                  16 / BYTE_SIZE
#define WORD_ARRAY_NUM                  16 / WORD_SIZE

// Number format constants
#define HEX_PLAIN_FORMAT                " %04x"
#define OCTAL_BYTE_FORMAT               " %03o"
#define OCTAL_WORD_FORMAT               "  %06o"
#define DECIMAL_WORD_FORMAT             "   %05u"
#define HEX_WORD_FORMAT                 "    %04x"

// Canonical format constants
#define CAN_HEX_SEPARATOR               ' '
#define CAN_ASCII_SEPARATOR             "  "
#define CAN_HEX_FORMAT                  " %02x"
#define CAN_ASCII_FORMAT_NONASCII       '.'
#define CAN_ASCII_FORMAT_BORDER         '|'

// ASCII printable values boundaries
#define ASCII_LOWEST                    0x20
#define ASCII_HIGHEST                   0x7e

// National DOS codepage low boundary
#define ASCII_DOS_CP_BEGIN              0x80

// Printable character display
#define CHAR_BYTE_FORMAT_PRINTABLE      "   %c"

// Escape sequences
#define CHAR_BYTE_FORMAT_ESCAPE_NULL    "  \\0"
#define CHAR_BYTE_FORMAT_ESCAPE_BELL    "  \\a"
#define CHAR_BYTE_FORMAT_ESCAPE_BS      "  \\b"
#define CHAR_BYTE_FORMAT_ESCAPE_TAB     "  \\t"
#define CHAR_BYTE_FORMAT_ESCAPE_LF      "  \\n"
#define CHAR_BYTE_FORMAT_ESCAPE_VT      "  \\v"
#define CHAR_BYTE_FORMAT_ESCAPE_FF      "  \\f"
#define CHAR_BYTE_FORMAT_ESCAPE_CR      "  \\r"

// Values for escape sequences
#define CHAR_BYTE_VALUE_NULL            0x00        // null byte
#define CHAR_BYTE_VALUE_BELL            0x07        // beep
#define CHAR_BYTE_VALUE_BS              0x08        // backspace
#define CHAR_BYTE_VALUE_TAB             0x09        // tabulator
#define CHAR_BYTE_VALUE_LF              0x0a        // line feed
#define CHAR_BYTE_VALUE_VT              0x0b        // vertical tab
#define CHAR_BYTE_VALUE_FF              0x0c        // form feed
#define CHAR_BYTE_VALUE_CR              0x0d        // carriage return

// Declare functions
int dump_plain_hex(
    FILE* file,
    bool verbose,
    size_t offset,
    size_t* position,
    size_t endoffset
);       // Dump plain hex
int dump_byte_octal(
    FILE* file,
    bool verbose,
    size_t* position,
    size_t endoffset
);      // Dump byte octal
int dump_byte_char(
    FILE* file,
    bool verbose,
    size_t offset,
    size_t* position,
    size_t endoffset
);       // Dump byte character
int dump_canonical(
    FILE* file,
    bool verbose,
    size_t offset,
    size_t* position,
    size_t endoffset
);       // Dump hex-ascii canonical
int dump_word_decimal(
    FILE* file,
    bool verbose,
    size_t* position,
    size_t endoffset
);    // Dump word decimal
int dump_word_octal(
    FILE* file,
    bool verbose,
    size_t offset,
    size_t* position,
    size_t endoffset
);      // Dump word octal
int dump_word_hex(
    FILE* file,
    bool verbose,
    size_t offset,
    size_t* position,
    size_t endoffset
);        // Dump word hex

#endif
