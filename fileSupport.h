#ifndef FILESUPPORT_H
#define FILESUPPORT_H

#include<stdbool.h>

#include "list.h"
#include "assist.h"

// writes all data into a txt file
void saveTxt(char filename[], Section **head, Mode m);

// writes all data into a JSON file
void saveJSON(char filename[], Section **head);

// writes all data into an XML file
void saveXML(char filename[], Section **head);

// reads data from an txt file, inserts records that haven't been added yet to the database, returns true if the file was read successfully 
bool loadTxt(char filename[], Section **head);

// used by all functions used for reading JSON files, checks if the next character in the file is the one expected
bool readChar(char a, FILE* const filePointer);

// used by the loadJSON function, reads a line, containing a key and a string value
bool readElementStrJSON(char key[], char element[], FILE* const filePointer, bool allowNumbers);

// used by the loadJSON function, reads a line containing  a key and an in value
bool readElementIntJSON(char key[], unsigned int *element, FILE* const filePointer);

// reads data from an JSON file, inserts records that haven't been added yet to the database, returns true if the file was read successfully 
bool loadJSON(char filename[], Section **head);

// used by the loadXML and readElementsStrXML, reads a key and checks whether it is correct
bool readKeyXML(char key[], FILE* const filePointer);

// used by the loadXML function, reads a line, containing a key and a string value
bool readElementStrXML(char key[], char element[], FILE* const filePointer, bool alloNumbers);

// used by the loadXML function, reads a line, containing a key and an int value
bool readElementIntXML(char key[], unsigned int *element, FILE* const filePointer);

// reads data from an XML file, inserts records that haven't been added yet to the database, returns true if the file was read successfully 
bool loadXML(char filename[], Section **head);

#endif