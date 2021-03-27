/**************************************************
 * Hexdump for Windows
 *-------------------------------------------------
 *
 * Command line tool inspired by Linux tool
 * hexdump - stripped to bare hexadecimal dump
 * of given file with option to limit dump length
 *
 * File: info.h
 *
 *-------------------------------------------------
 * Made by Marek Pol·Ëek (Polda18)
 **************************************************/

#pragma once

#ifndef __INFO_H__
#define __INFO_H__

// Defined constants
#define MAX_MSG         200					// Maximum length of an error message
#define VERSION         "0.1 ALPHA"			// Version

void print_help(void);
void print_error(char* msg);
void print_file_error(char* msg);
void print_error_memory(char* msg);

#endif
