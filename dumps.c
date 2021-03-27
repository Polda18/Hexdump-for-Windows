/**************************************************
 * Hexdump for Windows
 *-------------------------------------------------
 *
 * Command line tool inspired by Linux tool
 * hexdump - stripped to bare hexadecimal dump
 * of given file with option to limit dump length
 *
 * File: dumps.c
 *
 *-------------------------------------------------
 * Made by Marek Pol·Ëek (Polda18)
 **************************************************/

// Global definitions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <direct.h>
#include <errno.h>

// Local definitions
#include "dumps.h"
#include "info.h"

// Print offset - static function
static void print_offset(FILE* file, size_t ndigits, size_t position) {
    // Construct string format for offset display
    char format[100];                       // Init format buffer
    sprintf(format, "%%0%lux", ndigits);    // Get format for offset

    // Return offset hexadecimal value
    fprintf(stdout, format, position);
}

// Calculate number of digits - static function
static size_t get_offset_digits(size_t number) {
    // Get number of digits for offset display
    size_t ndigits = MIN_OFFSET_DIGITS;     // Init number of digits - bare minimum is 7
    register size_t num_copy = number;      // Init operations on given offset value

    // For number greater or equal to 0x10000000 add appropriate number of digits
    while(num_copy >= OVERFLOW_VALUE) {
        num_copy /= 16;     // Hexadecimal division
        ++ndigits;          // Count the digits
    }

    // Return number of digits
    return ndigits;
}

// Dump byte hex
int dump_plain_hex(
    FILE* file,
    bool verbose,
    size_t offset,
    size_t* position,
    size_t endoffset
) {
    // Format:
    // "%07x" or greater for offset at the beginning (aligned to 0x10 for value less than end of file)
    // 1 space
    // "%04x" for each valid 2 bytes of input data (aligned as 8 16bit values, separated by 1 space)
    // If end of file reached, only offset is printed

    // Calculate number of digits based on ending offset
    size_t ndigits = get_offset_digits(endoffset);

    // Initialize buffers (16 bytes, arranged as 8 2-byte values)
    uword_t buffer_current[WORD_ARRAY_NUM];
    uword_t buffer_previous[WORD_ARRAY_NUM];

    ubyte_t remainder = 0;              // Save up for last remaining byte

    bool data_repeat = false;           // Repeating data
    bool line_repeat = false;           // Repeating line

    size_t successfully_read = 0;
    
    // Print out data in plain 2-byte hexadecimal format
    do {
        // Run at least once

        // Check for previous iteration round - full line counts
        if (successfully_read == WORD_ARRAY_NUM) {
            // Previously read full 16 bytes -> copy all current buffer into previous one
            for (int i = 0; i < WORD_ARRAY_NUM; ++i) {
                buffer_previous[i] = buffer_current[i];
            }
        }

        // Read into current buffer -> count number of successful 
        successfully_read = fread(buffer_current, WORD_SIZE, WORD_ARRAY_NUM, file);

        if ((successfully_read < WORD_ARRAY_NUM) && (ftell(file) < endoffset)) {
            // An error occured while reading file

            putchar('\n');      // Divide space between data and error message
            print_file_error("An error occured while file contents output");

            return errno;       // End with specific return code for given error
        }

        // Check for repeating data
        if (*position > offset) {
            // If whole 16 bytes have been read, proceed

            // Mark repeating data initial to true
            data_repeat = true;

            // Iterate through both buffers to find repeating data
            for (int i = 0; i < successfully_read; ++i) {
                // If single element is different, next iteration won't matter
                data_repeat = data_repeat && (buffer_current[i] == buffer_previous[i]);
            }

            if (!data_repeat)           // If data isn't repeating, line isn't repeating eighter
                line_repeat = false;
        }

        // If verbose isn't set and buffers are repeating, then print out single asterisk and skip iteration
        if (!verbose && data_repeat) {
            // Check for repeating line
            if (!line_repeat) {
                fprintf(stdout, "*\n");     // Print out asterisk
                line_repeat = true;         // Set repeating line
            }

            *position = ftell(file);        // Get new position
            continue;                       // Skip iteration
        }
        // Data are different or verbose is enabled - proceed

        // Print current position
        print_offset(file, ndigits, *position);

        // Iterate through current buffer
        for (int i = 0; i < successfully_read; ++i) {
            // Iterate through read elements
            fprintf(stdout, HEX_PLAIN_FORMAT, buffer_current[i]);
        }

        // Get new position from number of successfuly read bytes
        *position += successfully_read * WORD_SIZE;

        // Check for last remanining byte in incomplete 16byte data buffer
        if ((*position == endoffset - BYTE_SIZE) && (successfully_read < 8)) {
            fseek(file, *position, SEEK_SET);                                   // Reset to last known position
            successfully_read += fread(&remainder, BYTE_SIZE, 1, file);         // Read single byte
            fprintf(stdout, HEX_PLAIN_FORMAT, remainder);                       // Print remainder out
            *position = ftell(file);                                            // Update position to finish iteration
        }

        // Fill remaining space with white space
        for (int i = successfully_read; i < WORD_ARRAY_NUM; ++i) {
            fprintf(stdout, "     ");
        }

        // Issue end of line
        putchar('\n');
    } while (*position < endoffset);

    // Reached end of file
    print_offset(file, ndigits, *position);         // Print ending position
    putchar('\n');                                  // Last line feed

    return 0;
}

// Dump byte octal
int dump_byte_octal(
    FILE* file,
    bool verbose,
    size_t offset,
    size_t* position,
    size_t endoffset
) {
    // Format:
    // "%07x" or greater for offset at the beginning (aligned to 0x10 for value less than end of file)
    // 1 space
    // "%03o" for each valid byte of input data (aligned as 16 8bit values) (separated by 1 space)
    // or empty spaces for invalid bytes at the beginning
    // If end of file reached, only offset is printed

    // Calculate number of digits based on ending offset
    size_t ndigits = get_offset_digits(endoffset);

    // Initialize buffers (16 bytes, arranged as 8 2-byte values)
    ubyte_t buffer_current[BYTE_ARRAY_NUM];
    ubyte_t buffer_previous[BYTE_ARRAY_NUM];

    bool data_repeat = false;           // Repeating data
    bool line_repeat = false;           // Repeating line

    size_t successfully_read = 0;

    // Print out data in 1-byte octal format
    do {
        // Run at least once

        // Check for previous iteration round - full line counts
        if (successfully_read == BYTE_ARRAY_NUM) {
            // Previously read full 16 bytes -> copy all current buffer into previous one
            for (int i = 0; i < BYTE_ARRAY_NUM; ++i) {
                buffer_previous[i] = buffer_current[i];
            }
        }

        // Read into current buffer -> count number of successful 
        successfully_read = fread(buffer_current, BYTE_SIZE, BYTE_ARRAY_NUM, file);

        if ((successfully_read < BYTE_ARRAY_NUM) && (ftell(file) < endoffset)) {
            // An error occured while reading file

            putchar('\n');      // Divide space between data and error message
            print_file_error("An error occured while file contents output");

            return errno;       // End with specific return code for given error
        }

        // Check for repeating data
        if (*position > offset) {
            // If whole 16 bytes have been read, proceed

            // Mark repeating data initial to true
            data_repeat = true;

            // Iterate through both buffers to find repeating data
            for (int i = 0; i < successfully_read; ++i) {
                // If single element is different, next iteration won't matter
                data_repeat = data_repeat && (buffer_current[i] == buffer_previous[i]);
            }

            if (!data_repeat)           // If data isn't repeating, line isn't repeating eighter
                line_repeat = false;
        }

        // If verbose isn't set and buffers are repeating, then print out single asterisk and skip iteration
        if (!verbose && data_repeat) {
            // Check for repeating line
            if (!line_repeat) {
                fprintf(stdout, "*\n");     // Print out asterisk
                line_repeat = true;         // Set repeating line
            }

            *position = ftell(file);        // Get new position
            continue;                       // Skip iteration
        }
        // Data are different or verbose is enabled - proceed

        // Print current position
        print_offset(file, ndigits, *position);

        // Iterate through current buffer
        for (int i = 0; i < successfully_read; ++i) {
            // Iterate through read elements
            fprintf(stdout, OCTAL_BYTE_FORMAT, buffer_current[i]);
        }

        // Get new position from number of successfuly read bytes
        *position += successfully_read * BYTE_SIZE;

        // Fill remaining space with white spaces
        for (int i = successfully_read; i < BYTE_ARRAY_NUM; ++i) {
            fprintf(stdout, "    ");
        }

        // Issue end of line
        putchar('\n');
    } while (*position < endoffset);

    // Reached end of file
    print_offset(file, ndigits, *position);         // Print ending position
    putchar('\n');                                  // Last line feed

    return 0;
}

// Dump byte characters
int dump_byte_char(
    FILE* file,
    bool verbose,
    size_t offset,
    size_t* position,
    size_t endoffset
) {
    // Format:
    // "%07x" or greater for offset at the beginning (aligned to 0x10 for value less than end of file)
    // "  %c" for each valid 16 bytes of input data (aligned as 8bit values) for printable characters
    // or "\\?" where ? is escape sequence for control characters
    // or "%3o" for other valid values (separated by spaces)
    // or empty spaces for invalid bytes at the beginning
    // If end of file reached, only offset is printed

    // Calculate number of digits based on ending offset
    size_t ndigits = get_offset_digits(endoffset);

    // Initialize buffers (16 bytes, arranged as 8 2-byte values)
    ubyte_t buffer_current[BYTE_ARRAY_NUM];
    ubyte_t buffer_previous[BYTE_ARRAY_NUM];

    bool data_repeat = false;           // Repeating data
    bool line_repeat = false;           // Repeating line

    size_t successfully_read = 0;

    // Print out data in 1-byte char format 
    //  (if valid character, or escape sequence for standard codes or octal number for other values)
    do {
        // Run at least once

        // Check for previous iteration round - full line counts
        if (successfully_read == BYTE_ARRAY_NUM) {
            // Previously read full 16 bytes -> copy all current buffer into previous one
            for (int i = 0; i < BYTE_ARRAY_NUM; ++i) {
                buffer_previous[i] = buffer_current[i];
            }
        }

        // Read into current buffer -> count number of successful 
        successfully_read = fread(buffer_current, BYTE_SIZE, BYTE_ARRAY_NUM, file);

        if ((successfully_read < BYTE_ARRAY_NUM) && (ftell(file) < endoffset)) {
            // An error occured while reading file

            putchar('\n');      // Divide space between data and error message
            print_file_error("An error occured while file contents output");

            return errno;       // End with specific return code for given error
        }

        // Check for repeating data
        if (*position > offset) {
            // If whole 16 bytes have been read, proceed

            // Mark repeating data initial to true
            data_repeat = true;

            // Iterate through both buffers to find repeating data
            for (int i = 0; i < successfully_read; ++i) {
                // If single element is different, next iteration won't matter
                data_repeat = data_repeat && (buffer_current[i] == buffer_previous[i]);
            }

            if (!data_repeat)           // If data isn't repeating, line isn't repeating eighter
                line_repeat = false;
        }

        // If verbose isn't set and buffers are repeating, then print out single asterisk and skip iteration
        if (!verbose && data_repeat) {
            // Check for repeating line
            if (!line_repeat) {
                fprintf(stdout, "*\n");     // Print out asterisk
                line_repeat = true;         // Set repeating line
            }

            *position = ftell(file);        // Get new position
            continue;                       // Skip iteration
        }
        // Data are different or verbose is enabled - proceed

        // Print current position
        print_offset(file, ndigits, *position);

        // Iterate through current buffer
        for (int i = 0; i < successfully_read; ++i) {
            // Iterate through read elements
            ubyte_t value = buffer_current[i];      // Copy value for determining output

            if (value >= ASCII_LOWEST && value <= ASCII_HIGHEST || value >= ASCII_DOS_CP_BEGIN) {
                // Byte is printable ASCII character or equals a value in DOS codepage
                fprintf(stdout, CHAR_BYTE_FORMAT_PRINTABLE, value);
                continue;           // Continue in iteration
            }

            switch (value) {
                // Is not printable nor in DOS codepage
            case CHAR_BYTE_VALUE_NULL:
                // Null byte
                fprintf(stdout, CHAR_BYTE_FORMAT_ESCAPE_NULL);
                break;
            case CHAR_BYTE_VALUE_BELL:
                // Beep
                fprintf(stdout, CHAR_BYTE_FORMAT_ESCAPE_BELL);
                break;
            case CHAR_BYTE_VALUE_BS:
                // Backspace
                fprintf(stdout, CHAR_BYTE_FORMAT_ESCAPE_BS);
                break;
            case CHAR_BYTE_VALUE_TAB:
                // Tabulator
                fprintf(stdout, CHAR_BYTE_FORMAT_ESCAPE_TAB);
                break;
            case CHAR_BYTE_VALUE_LF:
                // Line feed
                fprintf(stdout, CHAR_BYTE_FORMAT_ESCAPE_LF);
                break;
            case CHAR_BYTE_VALUE_VT:
                // Vertical tab
                fprintf(stdout, CHAR_BYTE_FORMAT_ESCAPE_VT);
                break;
            case CHAR_BYTE_VALUE_FF:
                // Form feed
                fprintf(stdout, CHAR_BYTE_FORMAT_ESCAPE_FF);
                break;
            case CHAR_BYTE_VALUE_CR:
                // Carriage return
                fprintf(stdout, CHAR_BYTE_FORMAT_ESCAPE_CR);
                break;
            default:
                // Another value that doesn't fall in standard escape sequence
                fprintf(stdout, OCTAL_BYTE_FORMAT, value);
            }
        }

        // Get new position from number of successfuly read bytes
        *position += successfully_read * BYTE_SIZE;

        // Fill remaining space with white space
        for (int i = successfully_read; i < BYTE_ARRAY_NUM; ++i) {
            fprintf(stdout, "    ");
        }

        // Issue end of line
        putchar('\n');
    } while (*position < endoffset);

    // Reached end of file
    print_offset(file, ndigits, *position);         // Print ending position
    putchar('\n');                                  // Last line feed

    return 0;
}

// Dump hex-ascii canonical
int dump_canonical(
    FILE* file,
    bool verbose,
    size_t offset,
    size_t* position,
    size_t endoffset
) {
    // Format:
    // "%07x" or greater for offset at the beginning (aligned to 0x10 for value less than end of file)
    // 2 spaces
    // "%02x" for each first 8 bytes of input data (aligned as 8bit values) (separated by 1 space)
    // 2 spaces
    // "%02x" for each valid 16 bytes of input data (aligned as 8bit values) (separated by 1 space)
    // or empty spaces for invalid bytes at the beginning
    // 2 spaces
    // 16 bytes of valid input data displayed as ASCII text or '.' for non-ASCII printable values,
    // surrounded with '|' at the beginning and the end
    // If end of file reached, only offset is printed

    // Calculate number of digits based on ending offset
    size_t ndigits = get_offset_digits(endoffset);

    // Initialize buffers (16 bytes, arranged as 8 2-byte values)
    ubyte_t buffer_current[BYTE_ARRAY_NUM];
    ubyte_t buffer_previous[BYTE_ARRAY_NUM];

    bool data_repeat = false;           // Repeating data
    bool line_repeat = false;           // Repeating line

    size_t successfully_read = 0;

    // Print out data in canonical 1-byte hex + ASCII notation
    do {
        // Run at least once

        // Check for previous iteration round - full line counts
        if (successfully_read == BYTE_ARRAY_NUM) {
            // Previously read full 16 bytes -> copy all current buffer into previous one
            for (int i = 0; i < BYTE_ARRAY_NUM; ++i) {
                buffer_previous[i] = buffer_current[i];
            }
        }

        // Read into current buffer -> count number of successful 
        successfully_read = fread(buffer_current, BYTE_SIZE, BYTE_ARRAY_NUM, file);

        if ((successfully_read < BYTE_ARRAY_NUM) && (ftell(file) < endoffset)) {
            // An error occured while reading file

            putchar('\n');      // Divide space between data and error message
            print_file_error("An error occured while file contents output");

            return errno;       // End with specific return code for given error
        }

        // Check for repeating data
        if (*position > offset) {
            // If whole 16 bytes have been read, proceed

            // Mark repeating data initial to true
            data_repeat = true;

            // Iterate through both buffers to find repeating data
            for (int i = 0; i < successfully_read; ++i) {
                // If single element is different, next iteration won't matter
                data_repeat = data_repeat && (buffer_current[i] == buffer_previous[i]);
            }

            if (!data_repeat)           // If data isn't repeating, line isn't repeating eighter
                line_repeat = false;
        }

        // If verbose isn't set and buffers are repeating, then print out single asterisk and skip iteration
        if (!verbose && data_repeat) {
            // Check for repeating line
            if (!line_repeat) {
                fprintf(stdout, "*\n");     // Print out asterisk
                line_repeat = true;         // Set repeating line
            }

            *position = ftell(file);        // Get new position
            continue;                       // Skip iteration
        }
        // Data are different or verbose is enabled - proceed

        // Print current position
        print_offset(file, ndigits, *position);

        // Separate position and hex values
        putchar(CAN_HEX_SEPARATOR);

        // Iterate through current buffer
        for (int i = 0; i < successfully_read; ++i) {
            // Check where the iteration is
            if (i == WORD_ARRAY_NUM)
                // Separate second half
                putchar(CAN_HEX_SEPARATOR);
            
            // Print hexadecimal format
            fprintf(stdout, CAN_HEX_FORMAT, buffer_current[i]);
        }

        // Fill remaining space with white space
        for (int i = successfully_read; i < BYTE_ARRAY_NUM; ++i) {
            // Check where the iteration is
            if (i == WORD_ARRAY_NUM)
                // Separate second half
                putchar(CAN_HEX_SEPARATOR);

            // Print spaces
            fprintf(stdout, "   ");
        }

        // Separate hex values from ASCII data
        fprintf(stdout, CAN_ASCII_SEPARATOR);

        // Print ASCII border
        putchar(CAN_ASCII_FORMAT_BORDER);

        // Iterate through data and print their printable ASCII or '.'
        for (int i = 0; i < successfully_read; ++i) {
            // Copy value for comparison
            ubyte_t value = buffer_current[i];

            // Check byte data for printable ASCII values
            if (value < ASCII_LOWEST || value > ASCII_HIGHEST)
                // Not printable -> print '.' instead
                putchar(CAN_ASCII_FORMAT_NONASCII);
            else
                // Printable ASCII value, print out direct value
                putchar(value);
        }

        // Print ASCII border
        putchar(CAN_ASCII_FORMAT_BORDER);

        // Get new position from number of successfuly read bytes
        *position += successfully_read * BYTE_SIZE;

        // Line feed at the end
        putchar('\n');
    } while (*position < endoffset);

    // Reached end of file
    print_offset(file, ndigits, *position);         // Print ending position
    putchar('\n');                                  // Last line feed

    return 0;
}

// Dump word decimal
int dump_word_decimal(
    FILE* file,
    bool verbose,
    size_t offset,
    size_t* position,
    size_t endoffset
) {
    // Format:
    // "%07x" or greater for offset at the beginning (aligned to 0x10 for value less than end of file)
    // 3 spaces
    // "%05d" for each valid 16 bytes of input data (aligned as 16bit values) (separated with 3 spaces)
    // or empty spaces for invalid bytes at the beginning
    // If end of file reached, only offset is printed

    // Calculate number of digits based on ending offset
    size_t ndigits = get_offset_digits(endoffset);

    // Initialize buffers (16 bytes, arranged as 8 2-byte values)
    uword_t buffer_current[WORD_ARRAY_NUM];
    uword_t buffer_previous[WORD_ARRAY_NUM];

    ubyte_t remainder = 0;              // Save up for last remaining byte

    bool data_repeat = false;           // Repeating data
    bool line_repeat = false;           // Repeating line

    size_t successfully_read = 0;

    // Print out data in 2-byte aligned decimal format
    do {
        // Run at least once

        // Check for previous iteration round - full line counts
        if (successfully_read == WORD_ARRAY_NUM) {
            // Previously read full 16 bytes -> copy all current buffer into previous one
            for (int i = 0; i < WORD_ARRAY_NUM; ++i) {
                buffer_previous[i] = buffer_current[i];
            }
        }

        // Read into current buffer -> count number of successful 
        successfully_read = fread(buffer_current, WORD_SIZE, WORD_ARRAY_NUM, file);

        if ((successfully_read < WORD_ARRAY_NUM) && (ftell(file) < endoffset)) {
            // An error occured while reading file

            putchar('\n');      // Divide space between data and error message
            print_file_error("An error occured while file contents output");

            return errno;       // End with specific return code for given error
        }

        // Check for repeating data
        if (*position > offset) {
            // If whole 16 bytes have been read, proceed

            // Mark repeating data initial to true
            data_repeat = true;

            // Iterate through both buffers to find repeating data
            for (int i = 0; i < successfully_read; ++i) {
                // If single element is different, next iteration won't matter
                data_repeat = data_repeat && (buffer_current[i] == buffer_previous[i]);
            }

            if (!data_repeat)           // If data isn't repeating, line isn't repeating eighter
                line_repeat = false;
        }

        // If verbose isn't set and buffers are repeating, then print out single asterisk and skip iteration
        if (!verbose && data_repeat) {
            // Check for repeating line
            if (!line_repeat) {
                fprintf(stdout, "*\n");     // Print out asterisk
                line_repeat = true;         // Set repeating line
            }

            *position = ftell(file);        // Get new position
            continue;                       // Skip iteration
        }
        // Data are different or verbose is enabled - proceed

        // Print current position
        print_offset(file, ndigits, *position);

        // Iterate through current buffer
        for (int i = 0; i < successfully_read; ++i) {
            // Iterate through read elements
            fprintf(stdout, DECIMAL_WORD_FORMAT, buffer_current[i]);
        }

        // Get new position from number of successfuly read bytes
        *position += successfully_read * WORD_SIZE;

        // Check for last remanining byte in incomplete 16byte data buffer
        if ((*position == endoffset - BYTE_SIZE) && (successfully_read < 8)) {
            fseek(file, *position, SEEK_SET);                                   // Reset to last known position
            successfully_read += fread(&remainder, BYTE_SIZE, 1, file);         // Read single byte
            fprintf(stdout, DECIMAL_WORD_FORMAT, remainder);                    // Print remainder out
            *position = ftell(file);                                            // Update position to finish iteration
        }

        // Fill remaining space with white spaces
        for (int i = successfully_read; i < WORD_ARRAY_NUM; ++i) {
            fprintf(stdout, "        ");
        }

        // Issue end of line
        putchar('\n');
    } while (*position < endoffset);

    // Reached end of file
    print_offset(file, ndigits, *position);         // Print ending position
    putchar('\n');                                  // Last line feed

    return 0;
}

// Dump word octal
int dump_word_octal(
    FILE* file,
    bool verbose,
    size_t offset,
    size_t* position,
    size_t endoffset
) {
    // Format:
    // "%07x" or greater for offset at the beginning (aligned to 0x10 for value less than end of file)
    // 2 spaces
    // "%06o" for each valid 16 bytes of input data (aligned as 16bit values) (separated with 2 spaces)
    // or empty spaces for invalid bytes at the beginning
    // If end of file reached, only offset is printed

    // Calculate number of digits based on ending offset
    size_t ndigits = get_offset_digits(endoffset);

    // Initialize buffers (16 bytes, arranged as 8 2-byte values)
    uword_t buffer_current[WORD_ARRAY_NUM];
    uword_t buffer_previous[WORD_ARRAY_NUM];

    ubyte_t remainder = 0;              // Save up for last remaining byte

    bool data_repeat = false;           // Repeating data
    bool line_repeat = false;           // Repeating line

    size_t successfully_read = 0;

    // Print out data in 2-byte aligned octal format
    do {
        // Run at least once

        // Check for previous iteration round - full line counts
        if (successfully_read == WORD_ARRAY_NUM) {
            // Previously read full 16 bytes -> copy all current buffer into previous one
            for (int i = 0; i < WORD_ARRAY_NUM; ++i) {
                buffer_previous[i] = buffer_current[i];
            }
        }

        // Read into current buffer -> count number of successful 
        successfully_read = fread(buffer_current, WORD_SIZE, WORD_ARRAY_NUM, file);

        if ((successfully_read < WORD_ARRAY_NUM) && (ftell(file) < endoffset)) {
            // An error occured while reading file

            putchar('\n');      // Divide space between data and error message
            print_file_error("An error occured while file contents output");

            return errno;       // End with specific return code for given error
        }

        // Check for repeating data
        if (*position > offset) {
            // If whole 16 bytes have been read, proceed

            // Mark repeating data initial to true
            data_repeat = true;

            // Iterate through both buffers to find repeating data
            for (int i = 0; i < successfully_read; ++i) {
                // If single element is different, next iteration won't matter
                data_repeat = data_repeat && (buffer_current[i] == buffer_previous[i]);
            }

            if (!data_repeat)           // If data isn't repeating, line isn't repeating eighter
                line_repeat = false;
        }

        // If verbose isn't set and buffers are repeating, then print out single asterisk and skip iteration
        if (!verbose && data_repeat) {
            // Check for repeating line
            if (!line_repeat) {
                fprintf(stdout, "*\n");     // Print out asterisk
                line_repeat = true;         // Set repeating line
            }

            *position = ftell(file);        // Get new position
            continue;                       // Skip iteration
        }
        // Data are different or verbose is enabled - proceed

        // Print current position
        print_offset(file, ndigits, *position);

        // Iterate through current buffer
        for (int i = 0; i < successfully_read; ++i) {
            // Iterate through read elements
            fprintf(stdout, OCTAL_WORD_FORMAT, buffer_current[i]);
        }

        // Get new position from number of successfuly read bytes
        *position += successfully_read * WORD_SIZE;

        // Check for last remanining byte in incomplete 16byte data buffer
        if ((*position == endoffset - BYTE_SIZE) && (successfully_read < 8)) {
            fseek(file, *position, SEEK_SET);                                   // Reset to last known position
            successfully_read += fread(&remainder, BYTE_SIZE, 1, file);         // Read single byte
            fprintf(stdout, OCTAL_WORD_FORMAT, remainder);                      // Print remainder out
            *position = ftell(file);                                            // Update position to finish iteration
        }

        // Fill remaining space with white spaces
        for (int i = successfully_read; i < WORD_ARRAY_NUM; ++i) {
            fprintf(stdout, "        ");
        }

        // Issue end of line
        putchar('\n');
    } while (*position < endoffset);

    // Reached end of file
    print_offset(file, ndigits, *position);         // Print ending position
    putchar('\n');                                  // Last line feed

    return 0;
}

// Dump word hex
int dump_word_hex(
    FILE* file,
    bool verbose,
    size_t offset,
    size_t* position,
    size_t endoffset
) {
    // Format:
    // "%07x" or greater for offset at the beginning (aligned to 0x10 for value less than end of file)
    // 4 spaces
    // " %04x" for each valid 16 bytes of input data (aligned as 16bit values) (separated by 4 spaces)
    // or empty spaces for invalid bytes at the beginning
    // If end of file reached, only offset is printed

    // Calculate number of digits based on ending offset
    size_t ndigits = get_offset_digits(endoffset);

    // Initialize buffers (16 bytes, arranged as 8 2-byte values)
    uword_t buffer_current[WORD_ARRAY_NUM];
    uword_t buffer_previous[WORD_ARRAY_NUM];

    ubyte_t remainder = 0;        // Save up for last remaining byte

    bool data_repeat = false;           // Repeating data
    bool line_repeat = false;           // Repeating line

    size_t successfully_read = 0;

    // Print out data in two-byte aligned hexadecimal format
    do {
        // Run at least once

        // Check for previous iteration round - full line counts
        if (successfully_read == WORD_ARRAY_NUM) {
            // Previously read full 16 bytes -> copy all current buffer into previous one
            for (int i = 0; i < WORD_ARRAY_NUM; ++i) {
                buffer_previous[i] = buffer_current[i];
            }
        }

        // Read into current buffer -> count number of successful 
        successfully_read = fread(buffer_current, WORD_SIZE, WORD_ARRAY_NUM, file);

        if ((successfully_read < WORD_ARRAY_NUM) && (ftell(file) < endoffset)) {
            // An error occured while reading file

            putchar('\n');      // Divide space between data and error message
            print_file_error("An error occured while file contents output");

            return errno;       // End with specific return code for given error
        }

        // Check for repeating data
        if (*position > offset) {
            // If whole 16 bytes have been read, proceed

            // Mark repeating data initial to true
            data_repeat = true;

            // Iterate through both buffers to find repeating data
            for (int i = 0; i < successfully_read; ++i) {
                // If single element is different, next iteration won't matter
                data_repeat = data_repeat && (buffer_current[i] == buffer_previous[i]);
            }

            if (!data_repeat)           // If data isn't repeating, line isn't repeating eighter
                line_repeat = false;
        }

        // If verbose isn't set and buffers are repeating, then print out single asterisk and skip iteration
        if (!verbose && data_repeat) {
            // Check for repeating line
            if (!line_repeat) {
                fprintf(stdout, "*\n");     // Print out asterisk
                line_repeat = true;         // Set repeating line
            }

            *position = ftell(file);        // Get new position
            continue;                       // Skip iteration
        }
        // Data are different or verbose is enabled - proceed

        // Print current position
        print_offset(file, ndigits, *position);

        // Iterate through current buffer
        for (int i = 0; i < successfully_read; ++i) {
            // Iterate through read elements
            fprintf(stdout, HEX_WORD_FORMAT, buffer_current[i]);
        }

        // Get new position from number of successfuly read bytes
        *position += successfully_read * WORD_SIZE;

        // Check for last remanining byte in incomplete 16byte data buffer
        if ((*position == endoffset - BYTE_SIZE) && (successfully_read < 8)) {
            fseek(file, *position, SEEK_SET);                                   // Reset to last known position
            successfully_read += fread(&remainder, BYTE_SIZE, 1, file);         // Read single byte
            fprintf(stdout, HEX_WORD_FORMAT, remainder);                        // Print remainder out
            *position = ftell(file);                                            // Update position to finish iteration
        }

        // Fill remaining space with white spaces
        for (int i = successfully_read; i < WORD_ARRAY_NUM; ++i) {
            fprintf(stdout, "        ");
        }

        // Issue end of line
        putchar('\n');
    } while (*position < endoffset);

    // Reached end of file
    print_offset(file, ndigits, *position);         // Print ending position
    putchar('\n');                                  // Last line feed

    return 0;
}
