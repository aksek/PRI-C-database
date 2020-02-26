#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#include "assist.h"
#include "list.h"

// format for type specifiers in printf
#define SIZE 40
#define FORMAT(S) "%-" #S "s"
#define S_FORMAT(S) FORMAT(S)

// this struct holds information about minerals
typedef struct Record Record;
struct Record{
	char name[SIZE];
	char formula[SIZE];
	char country[SIZE];
	unsigned int id;
	Record *nextAlpha;					// pointer to the next record when ordered alphabetically 
	Record *nextId;						// pointer to the next record when ordered by id
};

// this struct holds a list of minerals occuring in the particular country 
typedef struct Section Section;
struct Section{
	char name[SIZE];
	Record *recordHead;
	Section *next;						// pointer to the next section. sections are ordered alphabetically
};

// enum Mode {ALPHA, ID};
// typedef enum Mode Mode;

// saves the database to a txt file 
void saveTxt(char filename[], Section **head, Mode m) {
	FILE* filePointer = fopen(filename, "w");
	Section *curSection = (*head)->next;
	while (curSection != NULL) {
		fprintf(filePointer, "%s\n", curSection->name);
		Record *curRecord = (m == ALPHA) ? curSection->recordHead->nextAlpha : curSection->recordHead->nextId;
			
		while (curRecord != NULL) {
			fprintf(filePointer, S_FORMAT(SIZE), curRecord->name);
			fprintf(filePointer, S_FORMAT(SIZE), curRecord->formula);
			fprintf(filePointer, "%u", curRecord->id);
			fputs("\n", filePointer);
			curRecord = (m == ALPHA) ? curRecord->nextAlpha : curRecord->nextId;
		}
		fputs("\n", filePointer);
		curSection = curSection->next;
	}
	fclose(filePointer);
}

void saveJSON(char filename[], Section **head) {
	FILE* filePointer = fopen(filename, "w");
	int nrSpaces = 0;
	fputs("{\n", filePointer);
	nrSpaces++;
	Section *curSection = (*head)->next;
	bool firstSection = true;
	while (curSection != NULL) {
		if (!firstSection) 
			fputs(",\n", filePointer);
		firstSection = false;
		space(filePointer, nrSpaces);
		fprintf(filePointer, "\"%s\":[\n", curSection->name);
		nrSpaces++;
		Record *curRecord = curSection->recordHead->nextAlpha;
		bool firstRecord = true;
		while (curRecord != NULL) {
			if (!firstRecord) 
				fputs(",\n", filePointer);
			firstRecord = false;
			space(filePointer, nrSpaces);
			fputs("{\n", filePointer);
			nrSpaces++;
			space(filePointer, nrSpaces);
			fprintf(filePointer, "\"name\":\"%s\",\n", curRecord->name);
			space(filePointer, nrSpaces);
			fprintf(filePointer, "\"formula\":\"%s\",\n", curRecord->formula);
			space(filePointer, nrSpaces);
			fprintf(filePointer, "\"ID\":%u\n", curRecord->id);
			nrSpaces--;
			space(filePointer, nrSpaces);
			fputs("}", filePointer);
			curRecord = curRecord->nextAlpha;
		}
		fputs("\n", filePointer);
		nrSpaces--;
		space(filePointer, nrSpaces);
		fputs("]", filePointer);
		curSection = curSection->next;
	}
	fputs("\n", filePointer);
	nrSpaces--;
	space(filePointer, nrSpaces);
	fputs("}\n", filePointer);
	fclose(filePointer);
}

void saveXML(char filename[], Section **head) {
	FILE* filePointer = fopen(filename, "w");
	fputs("<database>\n\n", filePointer);
	Section *curSection = (*head)->next;
	while (curSection != NULL) {
		fputs("<section>\n", filePointer);
		fprintf(filePointer, "<country>%s</country>\n\n", curSection->name);
		Record *curRecord = curSection->recordHead->nextAlpha;
			
		while (curRecord != NULL) {
			fputs("<record>\n", filePointer);
			fprintf(filePointer, "<name>%s</name>\n", curRecord->name);
			fprintf(filePointer, "<formula>%s</formula>\n", curRecord->formula);
			fprintf(filePointer, "<ID>%u</ID>\n", curRecord->id);
			fputs("</record>\n\n", filePointer);
			curRecord = curRecord->nextAlpha;
		}
		fputs("</section>\n\n", filePointer);
		curSection = curSection->next;
	}
	fputs("</database>\n", filePointer);
	fclose(filePointer);
}

bool loadTxt(char filename[], Section **head) {
	FILE* filePointer = fopen(filename, "r");
	if (!checkPointer(filePointer)) {
		return false;
	}
	Record mineral;
	char country[SIZE];
	char buf[SIZE];
	char temp[SIZE];
	char a;
	int what = finputStr(buf, false, filePointer);
	while (what) {
		if (what == 2) {
			strcpy(country, buf);
			finputStr(buf, false, filePointer);
		}
		strcpy(mineral.name, buf);
		finputStr(buf, true, filePointer);
		strcpy(mineral.formula, buf);
		if (fscanf(filePointer, "%u", &mineral.id) != 1) {
			cleanBuff();
			mineral.id = avblId(*head);
		}
		strcpy(mineral.country, country);
		insert(mineral, head);
		a = skip(filePointer);
		temp[0] = a;
		temp[1] = '\0';
		what = finputStr(buf, false, filePointer);
		strcat(temp, buf);
		strcpy(buf, temp);
	}
	fclose(filePointer);
	return true;
}

bool readChar(char a, FILE* const filePointer) {
	char res;
	do
		res = fgetc(filePointer);
	while (res == ' ' || res == '\n');
	if (res == a)
		return true;
	return false;
}

bool readElementStrJSON(char key[], char element[], FILE* const filePointer, bool allowNumbers) {
	char buf[SIZE];
	if (!readChar('\"', filePointer))
		return false;
	if (!finputStr(buf, false, filePointer))
		return false;
	if (strcmp(buf, key) != 0)
		return false;
	if (!readChar(':', filePointer) || !readChar('\"', filePointer))
		return false;
	if (!finputStr(element, allowNumbers, filePointer))
		return false;
	return true;
}

bool readElementIntJSON(char key[], unsigned int *element, FILE* const filePointer) {
	char buf[SIZE];
	if (!readChar('\"', filePointer))
		return false;
	if (!finputStr(buf, false, filePointer))
		return false;
	if (strcmp(buf, key) != 0)
		return false;
	if (!readChar(':', filePointer))
		return false;
	if (1 != fscanf(filePointer, "%u", element))
		return false;
	
	return true;
}

bool loadJSON(char filename[], Section **head) {
	FILE* filePointer = fopen(filename, "r");
	if (!checkPointer(filePointer) || !readChar('{', filePointer))
		return false;
	char country[SIZE];
	do {
		if (!readChar('\"', filePointer))
			return false;
		if (!finputStr(country, false, filePointer))
			return false;
		if (!readChar(':', filePointer) || !readChar('[', filePointer))
			return false;
		do {
			Record mineral;
			if (!readChar('{', filePointer))
				return false;
			if (!readElementStrJSON("name", mineral.name, filePointer, false) || !readChar(',', filePointer))
				return false;
			if (!readElementStrJSON("formula", mineral.formula, filePointer, true) || !readChar(',', filePointer))
				return false;
			if (!readElementIntJSON("ID", &mineral.id, filePointer))
				return false;
			if (!readChar('}', filePointer))
				return false;
			strcpy(mineral.country, country);
			insert(mineral, head);
		} while (readChar(',', filePointer));
	} while (readChar(',', filePointer));
	fclose(filePointer);
	return true;
}

bool readKeyXML(char key[], FILE* const filePointer) {
	char buf[SIZE];
	if (!finputStr(buf, true, filePointer))
		return false;
	if (strcmp(buf, key) != 0)
		return false;
	return true;
}

bool readElementStrXML(char key[], char element[], FILE* const filePointer, bool allowNumbers) {
	skip(filePointer);
	char closeKey[SIZE] = "/";
	strcat(closeKey, key);
	if (!readKeyXML(key, filePointer))
		return false;
	if (!finputStr(element, allowNumbers, filePointer))
		return false;
	if (!readKeyXML(closeKey, filePointer))
		return false;
	return true;
}

bool readElementIntXML(char key[],  unsigned int *element, FILE* const filePointer) {
	skip(filePointer);
	char closeKey[SIZE] = "</";
	strcat(closeKey, key);
	if (!readKeyXML(key, filePointer))
		return false;
	if (fscanf(filePointer, "%u", element) != 1)
		return false;
	if (!readKeyXML(closeKey, filePointer))
		return false;
	return true;
}

bool loadXML(char filename[], Section **head) {
	FILE* filePointer = fopen(filename, "r");
	if (!checkPointer(filePointer))
		return false;
	if (!readKeyXML("<database", filePointer))
		return false;
	skip(filePointer);
	if (!readKeyXML("section", filePointer))
		return false;
	do {
		char country[SIZE];
		if (!readElementStrXML("country", country, filePointer, false))
			return false;
		skip(filePointer);
		Record mineral;
		if (!readKeyXML("record", filePointer))
			return false;
		do {
			if (!readElementStrXML("name", mineral.name, filePointer, false) || !readElementStrXML("formula", mineral.formula, filePointer, true))
				return false;
			if (!readElementIntXML("ID", &mineral.id, filePointer))
				return false;
			skip(filePointer);
			if (!readKeyXML("/record", filePointer))
				return false;
			skip(filePointer);
			strcpy(mineral.country, country);
			insert(mineral, head);
			
		} while (readKeyXML("record", filePointer));
		skip(filePointer);
	} while (readKeyXML("section", filePointer));
	fclose(filePointer);
	return true;
}