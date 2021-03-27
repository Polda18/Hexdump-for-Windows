/**************************************************
 * Hexdump for Windows
 *-------------------------------------------------
 * 
 * Command line tool inspired by Linux tool
 * hexdump - stripped to bare hexadecimal dump
 * of given file with option to limit dump length
 * 
 * File: switches.h
 * 
 *-------------------------------------------------
 * Made by Marek Pol·Ëek (Polda18)
 **************************************************/

#pragma once

#ifndef __SWITCHES_H__
#define __SWITCHES_H__

// Default behaviour
#define SW_DUMP_PLAIN_HEX       0x00000000      // no switch

// Help switch
#define SW_HELP                 0x00000001      // -h switch

// Dump switches: -bcCdox
#define SW_DUMP_BYTE_OCTAL      0x00000002      // -b switch
#define SW_DUMP_BYTE_CHAR       0x00000004      // -c switch
#define SW_DUMP_CANONICAL       0x00000008      // -C switch
#define SW_DUMP_WORD_DECIMAL    0x00000010      // -d switch
#define SW_DUMP_WORD_OCTAL      0x00000020      // -o switch
#define SW_DUMP_WORD_HEX        0x00000040      // -x switch

// File seeking switches -n, -s
// Undefined flags for these (direct input)

// Verbose switch
#define SW_VERBOSE              0x00000080      // -v switch

// Invalid switch found
#define SW_INVALID              0x00100000      // -? switch, where ? is anything but above

// File not specified
#define SW_FILEUNKNOWN          0x00200000		// File wasn't specified (special invalid state)

// File specified elsewhere
#define SW_FILEELSEWHERE        0x00400000      // File was specified in between arguments (not last)

// Length number in incorrect format
#define SW_LENGTHFORMAT         0x00800000

// Offset number in incorrect format
#define SW_OFFSETFORMAT         0x01000000

#endif
