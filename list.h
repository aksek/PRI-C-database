#ifndef LIST_H
#define LIST_H

#include<stdbool.h>

#include "list.h"
#include "assist.h"

#define SIZE 40

typedef struct Record Record;
struct Record;

typedef struct Section Section;
struct Section;

enum Mode {ALPHA, ID};
typedef enum Mode Mode;

// finds the smallest ID that is bigger than any other ID used 
unsigned int avblId (Section *head);

// calls input functions that modify the given pointer to a record so that it holds infromation typed by the user
bool inputRecord(Record *m, Section **head);

// fills a Record with data
Record fill(Record mineral, char name[SIZE], char country[SIZE], char formula[SIZE], unsigned int id);

// returns the record that is previous to the one that is to be inserted when ordered alphabetically 
Record *findPreviousRA(char toInsert[SIZE], Record *head, bool *makeNew);

// returns the section that is previous to the one that is to be created for the new record, ordered alphabetically 
Section *findPreviousS(char toInsert[SIZE], Section *head, bool *makeNew);

// places a record in location found by the insert function
void placeR(Record mineral, Record *prevA, Record *prevI);

// adds the element to the list, returns true if successful
int insert(Record mineral, Section **head);

// creates arbitrary data and adds it to the list
void mockInput(Section **head);

//makes changes to a record by erasing it and inserting a new one, returns true if successful
bool alter(Section **head, char comment[]);

// prints all records devided in sections, then either in alphabetical order, or ordered by ID depending on the mode (ALPHA / ID)
void print(Section **head, Mode m);

// frees all the memory
void quit(Section **head);

// interacts with the user
void menu(Section **head);

#endif