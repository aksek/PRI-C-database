#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#include "assist.h"
#include "fileSupport.h"

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

// finds the smallest ID that is bigger than any other ID used 
unsigned int avblId (Section *head) {
	unsigned int id = 0;
	if (head) {
		Section *curS = head->next;
		while (curS) {
			Record *curR = curS->recordHead->nextId;
			while (curR) {
				if (curR->id > id)
					id = curR->id;
				curR = curR->nextId;
			}
			curS = curS->next;
		}
	}
	return id + 1;
}

// calls input functions that modify the given pointer to a record so that it holds infromation typed by the user
bool inputRecord(Record *m, Section **head) {
	if (!(inputStr(m->name, "Name:", false, false) && inputStr(m->formula, "Formula:", true, false) && inputStr(m->country, "Country:", false, false)))
		return false;
	if (!inputInt(&m->id, "Id:")) {
		puts("Incorrect ID was replaced with a generated ID");
		m->id = avblId(*head);
	}
	return true;
}

// fills a Record with data
Record fill(Record mineral, char name[SIZE], char country[SIZE], char formula[SIZE], unsigned int id) {
	mineral.id = id;
	strcpy(mineral.name, name);
	strcpy(mineral.formula, formula);
	strcpy(mineral.country, country);
	return mineral;
}


// returns the record that is previous to the one that is to be inserted when ordered alphabetically 
Record *findPreviousRA(char toInsert[SIZE], Record *head, bool *makeNew) {
	char formattedToInsert[SIZE];
	strcpy(formattedToInsert, toInsert);
	format(formattedToInsert);
	
	char formattedNextCur[SIZE];
	strcpy(formattedNextCur, head->nextAlpha->name);
	format(formattedNextCur);
	
	Record *cur = head;
	while (cur->nextAlpha != NULL && strcmp(formattedNextCur, formattedToInsert) < 0) {
		cur = cur->nextAlpha;
		if (cur->nextAlpha != NULL) {
			strcpy(formattedNextCur, cur->nextAlpha->name);
			format(formattedNextCur);
		}
	}
	if (strcmp(formattedNextCur, formattedToInsert) != 0) {
		*makeNew = true;
	}
	return cur;
}

// returns the section that is previous to the one that is to be created for the new record, ordered alphabetically 
Section *findPreviousS(char toInsert[SIZE], Section *head, bool *makeNew) {
	char formattedToInsert[SIZE];
	strcpy(formattedToInsert, toInsert);
	format(formattedToInsert);
	
	char formattedNextCur[SIZE];
	strcpy(formattedNextCur, head->next->name);
	format(formattedNextCur);
	
	Section *cur = head;
	while (cur->next != NULL && strcmp(formattedNextCur, formattedToInsert) < 0) {
		cur = cur->next;
		if (cur->next != NULL) {
			strcpy(formattedNextCur, cur->next->name);
			format(formattedNextCur);
		}
	}
	if (strcmp(formattedNextCur, formattedToInsert) != 0) {
		*makeNew = true;
	}
	return cur;
}

// places a record in location found by the insert function
void placeR(Record mineral, Record *prevA, Record *prevI) {
	Record *temp = checkAlloc(malloc(sizeof(Record)));
	strcpy(temp->name, mineral.name);
	strcpy(temp->formula, mineral.formula);
	strcpy(temp->country, mineral.country);
	temp->id = mineral.id;
	temp->nextAlpha = prevA->nextAlpha;
	prevA->nextAlpha = temp;
	temp->nextId = prevI->nextId;
	prevI->nextId = temp;
}

// adds the element to the list, returns true if successful
int insert(Record mineral, Section **head) {
	int result = 1;		// 1 - success, 2 - ID already used, 3 - name already used, 4 name and ID already used
	Section *tempS = NULL;
	if (head == NULL) {
		checkAlloc(NULL);
		return 0;
	}
	if (*head == NULL) {							// creates the head of the list if there is none, 
		tempS = checkAlloc(malloc(sizeof(Section)));			// the first section, the list within it and inserts 
		strcpy(tempS->name, " ");					// the record
		*head = tempS;
		(*head)->next = checkAlloc(malloc(sizeof(Section)));
		strcpy((*head)->next->name, mineral.country);
		(*head)->next->next = NULL;
		(*head)->next->recordHead = checkAlloc(malloc(sizeof(Record)));
		strcpy((*head)->next->recordHead->name, " ");
		
		placeR(mineral, (*head)->next->recordHead, (*head)->next->recordHead);
		
	} else {
		Section *curSId = (*head)->next;			// searches for the correct section 
		Record *curRId = NULL;
		while (curSId != NULL) {				// checks if a record with a given id already exists
			curRId = curSId->recordHead;
			while (curRId->nextId != NULL && curRId->nextId->id >= mineral.id) {
				if (curRId->nextId != NULL && curRId->nextId->id == mineral.id) {
// 					printf("%d %d ", curRId->nextId->id, mineral.id);
// 					puts("A record with this ID already exists. The ID is replaced with a generated one");
					result += 1;
					mineral.id = avblId(*head);
				}
				curRId = curRId->nextId;
			}
			curSId = curSId->next;
		}
		bool makeNew = false;						// decies whether a new section should be created
		Section *curSection = findPreviousS(mineral.country, *head, &makeNew); 	// modifies makeNew
		
		if (makeNew) {							// creates the section if it does not exist yet
			tempS = checkAlloc(malloc(sizeof(Section)));
			strcpy(tempS->name, mineral.country);
			tempS->next = curSection->next;
			curSection->next = tempS;
			curSection = curSection->next;
			curSection->recordHead = checkAlloc(malloc(sizeof(Record)));
			strcpy(curSection->recordHead->name, " ");
			
			placeR(mineral, curSection->recordHead, curSection->recordHead);
			
		} else {
			Section *curSId = (*head)->next;			// finds the correct position of the record in id ordered list
			Record *curRId = NULL;
			Record *prevId = NULL;
			while (curSId != NULL) {
				curRId = curSId->recordHead;
				while (curRId->nextId != NULL && curRId->nextId->id >= mineral.id) {
					if (curRId->nextId->id > mineral.id)
						curRId = curRId->nextId;
				}
				char formSIdnam[SIZE];		// formatted name of the section while searching by ID
				strcpy(formSIdnam, curSId->name);
				format(formSIdnam);
				char formmincount[SIZE];	// formatted name of the country in whitch the mineral occurs
				strcpy(formmincount, mineral.country);
				format(formmincount);
				if (strcmp(formSIdnam, formmincount) == 0) {
					prevId = curRId;
				}
				curSId = curSId->next;
			}
			
			makeNew = false;					// now makeNew decides whether a new record should be created
			Record *curRecord = findPreviousRA(mineral.name, curSection->next->recordHead, &makeNew);

			if (!makeNew)
				result += 2;
			else 
				placeR(mineral, curRecord, prevId);			// inserts the record
		}
	}
	return result;
}

// creates arbitrary data and adds it to the list
void mockInput(Section **head) {
// 	Record mineral;
// 	insert(fill(mineral, "abelsonite", "Sweden", "C13H32N4Ni", 1), head);
// 	insert(fill(mineral, "acanthite", "Czechia", "Ag2S", 2), head);
// 	insert(fill(mineral, "adamite", "Mexico", "Zn2AsO4OH", 3), head);
// 	insert(fill(mineral, "aegirine", "Greenland", "NAFeSi2o5", 4), head);
// 	insert(fill(mineral, "agrellite", "Canada", "NaCa2Si4O1OF", 5), head);
// 	insert(fill(mineral, "aguilarite", "Mexico", "Ag4SeS", 6), head);
}

//makes changes to a record by erasing it and inserting a new one
bool alter(Section **head, char comment[]) {
	unsigned int id;
	if (!inputInt(&id, comment))
		return false;
	cleanBuff();
	
	Record *toAlter = NULL;
	Record *prev = NULL;
	Record *curRId = NULL;
	Section *prevS = NULL;
	Section *curSId = (*head)->next;
	while (curSId != NULL && toAlter == NULL) {	// finds the requested record
		curRId = curSId->recordHead;
		while (curRId->nextId != NULL && curRId->nextId->id <= id) {
			if (curRId->nextId->id == id) {
				prev = curRId;
				toAlter = curRId->nextId;
			}
			curRId = curRId->nextId;
		}
		if (toAlter == NULL) {
			prevS = curSId;
			curSId = curSId->next;
		}
	}
	if (toAlter == NULL) {
		return false;
	}
	printf(S_FORMAT(SIZE), toAlter->name);
	printf(S_FORMAT(SIZE), toAlter->formula);
	printf(S_FORMAT(SIZE), toAlter->country);
	printf("%u\n", toAlter->id);
	
	prev->nextId = toAlter->nextId;			// erases the record (and the section if erasing the record renders it empty)
	Record *prevA = curSId->recordHead;
	while (prevA->nextAlpha != NULL && prevA->nextAlpha != toAlter) {
		prevA = prevA->nextAlpha;
	}
	prevA->nextAlpha = toAlter->nextAlpha;
	
	free(toAlter);
	if (strcmp(prev->name, " ") == 0 && prev->nextId == NULL) {
		if (prevS)
			prevS->next = curSId->next;
		free(prev);
		free(curSId);
	}
	
	Record new;					// insesrts the corrected record
	if (!inputRecord(&new, head))
		return false;
	if (insert(new, head) == 0)
		return false;
	return true;
}

// prints all records devided in sections, then either in alphabetical order, or ordered by ID depending on the mode (ALPHA / ID)
void print(Section **head, Mode m) {
	Section *curSection = (*head)->next;
	while (curSection != NULL) {
		printf("%s\n", curSection->name);
		Record *curRecord = (m == ALPHA) ? curSection->recordHead->nextAlpha : curSection->recordHead->nextId;
			
		while (curRecord != NULL) {
			printf(S_FORMAT(SIZE), curRecord->name);
			printf(S_FORMAT(SIZE), curRecord->formula);
			printf("%u", curRecord->id);
			puts("");
			curRecord = (m == ALPHA) ? curRecord->nextAlpha : curRecord->nextId;
		}
		puts("");
		curSection = curSection->next;
	}
}

// frees all the memory
void quit(Section **head) {
	Record *nextR = NULL;
	Section *nextS = NULL;
	while ((*head)->next != NULL) {
		while ((*head)->next->recordHead->nextAlpha != NULL) {
			nextR = (*head)->next->recordHead->nextAlpha;
			(*head)->next->recordHead->nextAlpha = (*head)->next->recordHead->nextAlpha->nextAlpha;
			free(nextR);
		}
		free((*head)->next->recordHead);
		nextS = (*head)->next;
		(*head)->next = (*head)->next->next;
		free(nextS);
	}
	free(*head);
}




// interacts with the user
void menu(Section **head) {
	puts("Type:\n1 - add record   2 - change data   3 - see all records in alphabetical order   4 - see all records sorted by ID   5 - save as   6 - load file   7 - quit");
	short int choice = 0;
	scanf("%hu", &choice);
	switch (choice) {
		Record new;
		case 1:
			cleanBuff();
			if (inputRecord(&new, head)) {
				int insertResult = insert(new, head);
				if (insertResult == 2 || insertResult == 4)
					puts("A record with this ID already exists. The ID has been replaced with a generated one");
				if (insertResult == 3 || insertResult == 4)
					puts("A record with this name already exists");
			} else
				puts("Could not add record");
			menu(head);
			break;
		case 2:
			cleanBuff();
			if (!alter(head, "Type the ID of the record that you would like to alter")) 
				puts("Could not change record");
			menu(head);
			break;
		case 3:
			print(head, ALPHA);
			cleanBuff();
			menu(head);
			break;
		case 4:
			print(head, ID);
			cleanBuff();
			menu(head);
			break;
		case 5:
			puts("Choose file format: 1 - txt  2 - XML  3 - JSON");
			short int type1;
			scanf("%hu", &type1);
			cleanBuff();
			char filename1[SIZE];
			if (!inputStr(filename1, "Filename:", true, true))
				puts("Incorrect filename");
			switch (type1) {
				case 1:
					strcat(filename1, ".txt");
					saveTxt(filename1, head, ALPHA);
					break;
				case 2:
					strcat(filename1, ".XML");
					saveXML(filename1, head);
					break;
				case 3:
					strcat(filename1, ".JSON");
					saveJSON(filename1, head);
					break;
				default:
					puts("Unknown format");
					break;
			}
			menu(head);
			break;
		case 6:
			puts("Choose file format: 1 - txt  2 - XML  3 - JSON");
			short int type2;
			scanf("%hu", &type2);
			cleanBuff();
			char filename2[SIZE];
			if (!inputStr(filename2, "Filename:", true, true))
				puts("Incorrect filename");
			switch (type2) {
				case 1:
					strcat(filename2, ".txt");
					if (!loadTxt(filename2, head))
						puts("Could not load file");
					else 
						printf("Added records from %s\n", filename2);
					break;
				case 2:
					strcat(filename2, ".XML");
					if (!loadXML(filename2, head))
						puts("Could not load file");
					else 
						printf("Added records from %s\n", filename2);
					break;
				case 3:
					strcat(filename2, ".JSON");
					if (!loadJSON(filename2, head))
						puts("Could not load file");
					else 
						printf("Added records from %s\n", filename2);
					break;
				default:
					puts("Unknown format");
					break;
			}
			menu(head);
			break;		
		case 7:
			cleanBuff();
			puts("Are you sure you want to quit? (y)");
			char a;
			scanf("%c", &a);
			switch(a) {
				case 'y':
					quit(head);
					break;
				default:
					menu(head);
					break;
			}
			break;
		default:
			cleanBuff();
			puts("Unknown instruction. Try again");
			menu(head);
			break;
	}
}

int main(void) {
	Section *head = NULL;
	mockInput (&head);
	menu(&head);
}
