#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#include "assist.h"

#define SIZE 40
#define FORMAT(S) "%-" #S "s"
#define S_FORMAT(S) FORMAT(S)

// cleans the buffer
void cleanBuff(void) {
	char c;
	do 
		c = getchar();
	while (c != '\n' && c != EOF);
}

// returns true if a is a letter, space, hyphen or apostrophe
bool legitChar(char a) {
	return ((a >= 'A' && a <= 'Z') || (a >= 'a' && a <= 'z') || a == '-' || a == '\'' || a == ' ');
}

// returns true if a is not a / or a space
bool legitFileName(char a) {
	return (a != '\\' && a != ' ');
}

// modifies array, returns true if successful; if allowNumbers == false, skips all characters disallowed by the legitChar function; if filename == true, skips all \ and spaces
int inputStr(char arr[], char comment[], bool allowNumbers, bool filename) {
	puts(comment);
	bool foundForbidden = false;
	char a;
	int count = 0;
	for (count = 0; count < SIZE - 1; ++count) {
		a = getchar();
		if (a == '\n') {
			arr[count] = '\0';
			return 2;
		} else if (a == EOF) {
			if (count == 0)
				return 0;
			break;
		} else if (!legitChar(a) && !allowNumbers) {
			foundForbidden = true;
			--count;
		} else if (!legitFileName(a) && filename) {
			foundForbidden = true;
			--count;
		} else
			arr[count] = a;
	}
	if (count >= SIZE - 1) {
		puts("This string is too long");
		cleanBuff();
	}
	arr[count] = '\0';
	if (foundForbidden)
		puts("Forbidden characters found and removed. Exptected letters only");
	return 1;
}

// reads characters from a file until encounters \n, ", >, < (if some characters have already been read), EOF or a second space in a row; if allowNumbers == false, skips all characters disallowed by the legitChar function; returns true if successful
int finputStr(char arr[], bool allowNumbers, FILE* filePointer) {
	bool foundForbidden = false;
	char a;
	int count = 0;
	bool secondSpace = false;
	bool read = false;
	for (count = 0; count < SIZE - 1; ++count) {
		a = fgetc(filePointer);
		if (a == '\n') {
			arr[count] = '\0';
			return 2;
		} else if (a == ' ' && read) {
			if (secondSpace) {
				count--;
				break;
			} else {
				secondSpace = true;
				arr[count] = a;
			}
		} else if (a == '\"' || a == '>') {
			break;
		} else if (a == '<' && read) {
			break;
		} else if ((a == '\n' || a == ' ') && !read) {
			count--;
		} else if (a == EOF) {
			if (count == 0)
				return 0;
			break;
		} else if (!legitChar(a) && !allowNumbers) {
			foundForbidden = true;
			--count;
		} else {
			arr[count] = a;
			read = true;
			secondSpace = false;
		}
	}
	if (count >= SIZE - 1) {
		printf("Found a string that was too long: %s", arr);
		cleanBuff();
	}
	arr[count] = '\0';
	if (foundForbidden)
		puts("Forbidden characters found and removed. Exptected letters only");
	return 1;
}

// modifies a pointer to int, returns true if successful
bool inputInt(unsigned int *n, char comment[]) {
	puts(comment);
	if (scanf("%d", n) != 1) {
		puts("Expected a number");
		cleanBuff();
		return false;
	}
	return true;
}

// returns true if manual memory allocation was successful
void *checkAlloc(void *p) {
	if (!p) {
		fputs("ALLOCATIOIN ERROR", stderr);
		exit(EXIT_FAILURE);
	}
	return p;
}

// formats the string so that it only has lowercase letters 
bool format(char a[]) {
	char result[SIZE];
	result[0] = '0';				// if at the end the value remains the same, something went wrong
	int i, j;					// i for *a, j for result
	for (i = 0, j = 0; i < strlen(a) + 1; ++i, ++j) {
		if ((a[i] >= 'a' && a[i] <= 'z') || a[i] == '\0')
			result[j] = a[i];
		else if (a[i] >= 'A' && a[i] <= 'Z')
			result[j] = a[i] + 'a' - 'A';	// converts capitals to lowercase
		else
			--j;				// discards space or character other than a letter
	}
	strcpy(a, result);
	
	if (result[0] == '0')
		return false;
	else 
		return true;
}

// returns true if the file pointer is correct
bool checkPointer(FILE* const filePointer) {
	if (ferror(filePointer)) {
		return false;
	}
	return true;
}

// while writing to a file, writes n spaces
void space(FILE* filePointer, int n) {
	for (int i = 0; i < n; ++i) {
		fputs(" ", filePointer);
	}
}

// while reading a file, skips empty lines, returns the first character of the first non-empty line
char skip(FILE *filePointer) {
	char a;
	do {
		a = fgetc(filePointer);
	} while (a == '\n');
	return a;
}