/**************************************************
 * Hexdump for Windows
 *-------------------------------------------------
 *
 * Command line tool inspired by Linux tool
 * hexdump - stripped to bare hexadecimal dump
 * of given file with option to limit dump length
 *
 * File: main.c
 *
 *-------------------------------------------------
 * Made by Marek Pol·Ëek (Polda18)
 **************************************************/

// Global definitions
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <math.h>

// Local definitions
#include "switches.h"
#include "dumps.h"
#include "info.h"

// Main program
int main(int argc, char** argv) {
    int retcode = 0;                    // Initializing return code
    FILE* file;                         // Declare file descriptor
    unsigned int switches = 0;          // Switches flag register
    bool verbose = false;               // Checks a verbose flag
    size_t length, offset;              // Offset and length of file to read
    length = offset = 0;                // Init length and offset to be zero (default value)
                                        // Zero in length means until end of file,
                                        // zero in offset means from file beginning

    for (int i = 1; i < argc; ++i) {
        // Iterate through command line arguments
        bool offset_set, length_set, help_set, verbose_set;
        offset_set = length_set = help_set = verbose_set = false;
        // Check variables for if offset length, help or verbose was set

        // Set up character index
        int j = 0;
        while (argv[i][j] != 0) {       // Null-byte terminated string
            // Loop through characters in current argument
            if (j == 0 && argv[i][j] != '-')
                break;      // Exit the while loop if the first character isn't dash

            if (j > 0) {
                switch (argv[i][j]) {
                    // Iterate through switches

                case 'h':
                    // Help switch stated
                    if (j == 1) {
                        // Can only be single
                        switches |= SW_HELP;
                        help_set = true;
                    }
                    else
                        // Otherwise it's invalid
                        switches |= SW_INVALID;
                    break;

                case 'b':
                    // Octal byte switch stated
                    if (j > 1 && (offset_set || length_set || help_set))
                        // Offset, lenght or help was already set -> invalid
                        switches |= SW_INVALID;
                    else
                        // Proceed
                        switches |= SW_DUMP_BYTE_OCTAL;
                    break;
                case 'c':
                    // Character byte switch stated
                    if (j > 1 && (offset_set || length_set || help_set))
                        // Offset, lenght or help was already set -> invalid
                        switches |= SW_INVALID;
                    else
                        // Proceed
                        switches |= SW_DUMP_BYTE_CHAR;
                    break;
                case 'C':
                    // Canonical HEX+ASCII switch stated
                    if (j > 1 && (offset_set || length_set || help_set))
                        // Offset, lenght or help was already set -> invalid
                        switches |= SW_INVALID;
                    else
                        // Proceed
                        switches |= SW_DUMP_CANONICAL;
                    break;
                case 'd':
                    // Decimal word switch stated
                    if (j > 1 && (offset_set || length_set || help_set))
                        // Offset, lenght or help was already set -> invalid
                        switches |= SW_INVALID;
                    else
                        // Proceed
                        switches |= SW_DUMP_WORD_DECIMAL;
                    break;
                case 'o':
                    // Octal word switch stated
                    if (j > 1 && (offset_set || length_set || help_set))
                        // Offset, lenght or help was already set -> invalid
                        switches |= SW_INVALID;
                    else
                        // Proceed
                        switches |= SW_DUMP_WORD_OCTAL;
                    break;
                case 'x':
                    // Hex word switch stated
                    if (j > 1 && (offset_set || length_set || help_set))
                        // Offset, lenght or help was already set -> invalid
                        switches |= SW_INVALID;
                    else
                        // Proceed
                        switches |= SW_DUMP_WORD_HEX;
                    break;

                case 'n':
                    // Length switch stated
                    if (j == 1) {
                        // Can only be single
                        length_set = true;
                    }
                    else
                        // Otherwise it's invalid
                        switches |= SW_INVALID;
                    break;
                case 's':
                    // Offset switch stated
                    if (j == 1) {
                        // Can only be single
                        offset_set = true;
                    }
                    else
                        // Otherwise it's invalid
                        switches |= SW_INVALID;
                    break;
                case 'v':
                    // Verbose switch stated
                    if (j == 1) {
                        // Can only be single
                        switches |= SW_VERBOSE;
                        verbose_set = true;
                    }
                    else
                        // Otherwise it's invalid
                        switches |= SW_INVALID;
                    break;

                default:
                    // Invalid switch stated
                    switches |= SW_INVALID;
                }
            }

            ++j;    // Propagate to next character
        }

        if (switches & SW_INVALID)
            // An invalid switch is flagged => cancel iteration
            break;

        if (offset_set && (i == argc - 1 || argv[i + 1][0] == '-'))
            // Offset is set and next argument is missing or starts with a dash (another switch)
            switches |= SW_INVALID;
        else if (offset_set) {
            // Try to parse next argument as an unsigned size value
            size_t modifier = 1;    // Set up a modifier

            // Get number specification string length
            int modlen = strlen(argv[i + 1]);

            if (modlen < 1) {
                // Empty string (unlikely to happen, but still to be sure)
                switches |= SW_OFFSETFORMAT;
                continue;
            }

            switch (argv[i + 1][modlen - 1]) {
                // Read the last character
            case 'b':
                // .5k modifier
                modifier *= 512;
                break;
            case 'k':
                // 1k modifier
                modifier *= 1024;
                break;
            case 'm':
                // 1M modifier
                modifier *= 1048576;
                break;
            }

            // Set a space for an argument copy based on modifier
            char* number_copy = malloc((modifier > 1) ? modlen - 1 : modlen);

            // Copy bytes over
            for (int l = 0; l < modlen; ++l) {
                // Set up condition
                bool modified = (modifier > 1 && l == modlen - 1);

                // Copy over bytes of the argument (omit last byte if modifier is upgraded)
                number_copy[l] = (modified)? 0 : argv[i + 1][l];

                if (modified)
                    // Modifier applied -> exit the loop
                    break;
            }

            if (number_copy[0] == '0') {
                // Number starts with a predecing 0 -> can be hex or octal
                if (number_copy[1] == 'x') {
                    // Number starts with a preceding 0x -> hexadecimal
                    if (!sscanf_s((number_copy + 2), "%lx", &offset) && !sscanf_s((number_copy + 2), "%lX", &offset))
                        // Cannot be interpreted
                        switches |= SW_OFFSETFORMAT;
                }
                else {
                    // Number starts only with a preceding 0 -> octal
                    if (!sscanf_s((number_copy + 1), "%lo", &offset))
                        // Cannot be interpreted
                        switches |= SW_OFFSETFORMAT;
                }
            }
            else {
                // Number isn't preceeded by a format modifier -> unsigned decimal value
                if (!sscanf_s(number_copy, "%lu", &offset))
                    // Cannot be interpreted
                    switches |= SW_OFFSETFORMAT;
            }

            // Modify the offset by a modifier value
            offset *= modifier;

            // Tidy up
            free(number_copy);

            // Increment counter
            ++i;
        }

        if (length_set && (i == argc - 1 || argv[i + 1][0] == '-'))
            // Length is set and next argument is missing or starts with a dash (another switch)
            switches |= SW_INVALID;
        else if (length_set) {
            // Try to parse next argument as an unsigned size value
            if (!sscanf_s(argv[++i], "%lu", &length))
                // Cannot be interpreted
                switches |= SW_LENGTHFORMAT;
        }

        if (!offset_set && !length_set && i == argc - 1 && argv[i][0] == '-')
            // File wasn't specified at the end of the command line
            switches |= SW_FILEUNKNOWN;

        if (!offset_set && !length_set && i < argc - 1 && argv[i][0] != '-')
            // File was specified in between arguments (not last) => invalid argument
            switches |= SW_FILEELSEWHERE;
    }

    // Check arguments list after iteration
    if (argc < 2)
        // No arguments were provided (iteration was skipped) => no file specified
        switches |= SW_FILEUNKNOWN;

    // Top priority: check if help switch was specified
    if (switches & SW_HELP) {
        // Print out help page (similar to "man hexdump" from Linux)
        print_help();

        return 0;
    }

    // Top second priority: check if an invalid switch has been specified
    if (switches & SW_INVALID) {
        // Print out error message and guide user to help command
        print_error("Invalid switch detected");

        return 1;
    }

    // Top third priority: check if a length switch format has been misspelled
    if (switches & SW_LENGTHFORMAT) {
        // Print out error message and guide user to help command
        print_error("Length must be a decimal number");

        return 2;
    }

    // Top fourth priority: check if an offset switch format has been misspelled
    if (switches & SW_OFFSETFORMAT) {
        // Print out error message and guide user to help command
        print_error("Offset must be a formatted number, followed by an optional modifier");

        return 3;
    }

    // Top fifth priority: check if a file path/name is ommited
    if (switches & SW_FILEUNKNOWN) {
        // Print out error message and guide user to help command
        print_error("File was not specified");

        return 4;
    }

    // Top sixth priority: check if a file path/name is specified in between arguments
    if (switches & SW_FILEELSEWHERE) {
        // Print out error message and guide user to help command
        print_error("File must be specified as a last argument");

        return 5;
    }

    // Path is clear, just check for a verbose switch
    if (switches & SW_VERBOSE)
        verbose = true;     // Verbose has been enabled

    // Path is clear, let's try to open the file for reading in binary mode
    file = fopen(argv[argc - 1], "rb");

    if (!file) {
        // File couldn't be opened for some reason
        print_file_error("File couldn\'t be opened");

        return errno;       // Exit with specific system return code
    }

    // Get the file size
    fseek(file, 0L, SEEK_END);
    size_t fsize = ftell(file);

    // If beginning offset is greater than the file size, set it to reach end of file
    if (offset > fsize)
        offset = fsize;

    // If length isn't specified or is 0, set it to reach end of file
    if (length == 0)
        length = fsize - offset;

    // Get end offset
    size_t endoffset = offset + length;

    // If end offset is greater than the file size, set it to the end of file
    if (endoffset > fsize)
        endoffset = fsize;

    // Set current position to the initial offset
    size_t position = offset;

    // Set position on file to given offset
    fseek(file, offset, SEEK_SET);

    // File can be opened, now check for dump switches and dump binary contents
    if ((switches & ~SW_VERBOSE) == SW_DUMP_PLAIN_HEX) {    // Don't interpret a verbose switch
        // Plain hexadecimal two-byte system-endian representation
        retcode = dump_plain_hex(file, verbose, offset, &position, endoffset);

        goto end_procedure;     // End the program after done
    }

    if (switches & SW_DUMP_BYTE_OCTAL) {
        // Octal one-byte representation
        retcode = dump_byte_octal(file, verbose, offset, &position, endoffset);

        goto end_procedure;     // End the program after done
    }

    if (switches & SW_DUMP_BYTE_CHAR) {
        // Character one-byte ASCII representation
        retcode = dump_byte_char(file, verbose, offset, &position, endoffset);

        goto end_procedure;     // End the program after done
    }

    if (switches & SW_DUMP_CANONICAL) {
        // Canonical HEX+ASCII one-byte representation
        retcode = dump_canonical(file, verbose, offset, &position, endoffset);

        goto end_procedure;     // End the program after done
    }

    if (switches & SW_DUMP_WORD_DECIMAL) {
        // Decimal aligned two-byte system-endian representation
        retcode = dump_word_decimal(file, verbose, offset, &position, endoffset);

        goto end_procedure;     // End the program after done
    }

    if (switches & SW_DUMP_WORD_OCTAL) {
        // Octal aligned two-byte system-endian representation
        retcode = dump_word_octal(file, verbose, offset, &position, endoffset);

        goto end_procedure;     // End the program after done
    }

    if (switches & SW_DUMP_WORD_HEX) {
        // Hexadecimal aligned two-byte system-endian representation
        retcode = dump_word_hex(file, verbose, offset, &position, endoffset);

        // End the program after done -> last branch, doesn't require goto statement
    }
    
    end_procedure:
    fclose(file);               // Tidy up

    return retcode;             // Return the retcode
}
