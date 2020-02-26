#ifndef ASSIST_H
#define ASSIST_H

#include<stdbool.h>

// cleans the buffer
void cleanBuff(void);

// returns true if a is a letter, space, hyphen or apostrophe
bool legitChar(char a);

// returns true if a is not a / or a space
bool legitFileName(char a);

// modifies array, returns true if successful; if allowNumbers == false, skips all characters disallowed by the legitChar function; if filename == true, skips all \ and spaces
int inputStr(char arr[], char comment[], bool allowNumbers, bool filename);

// reads characters from a file until encounters \n, ", >, < (if some characters have already been read), EOF or a second space in a row; if allowNumbers == false, skips all characters disallowed by the legitChar function; returns true if successful
int finputStr(char arr[], bool allowNumbers, FILE* filePointer);

// modifies a pointer to int, returns true if successful
bool inputInt(unsigned int *n, char comment[]);

// returns true if manual memory allocation was successful
void *checkAlloc(void *p);

// formats the string so that it only has lowercase letters 
bool format(char a[]);

// returns true if the file pointer is correct
bool checkPointer(FILE* const filePointer);

// while writing to a file, writes n spaces
void space(FILE* filePointer, int n);

// while reading a file, skips empty lines, returns the first character of the first non-empty line
char skip(FILE* filePointer);

#endif