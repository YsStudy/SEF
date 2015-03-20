#include "Globals.h"

void printWord(struct word W)
{
	int i;
	printf("Word:\n");
	for (i = 1 << 11; i > 0; i >>= 1)
	{
		if (W.instructionBinary & i)
			printf("1");
		else
			printf("0");
	}
	printf("\n");
}

void InsertExtern(char *name, int address)
{
	struct extLabel* cntPtr = headExternList;
	while (cntPtr->next)
		cntPtr = cntPtr->next;
	cntPtr->next = (struct label*)malloc(sizeof(struct label));
	cntPtr = cntPtr->next;
	cntPtr->address = address;
	cntPtr->name = (char*)malloc(sizeof(char*)*(strlen(name) + 1));
	strcpy(cntPtr->name, name);
	cntPtr->next = NULL;
}

void InsertWord(struct word*a)
{
	struct word*cWord = headWordList;
	while (cWord->next)
		cWord = cWord->next;
	cWord->next = a;
	printWord(*a);
}

/*Calculates the final label's address (data rows appear after instruction rows, and address is relative to type)*/
int CalcLabelAddress(struct label*a)
{
	struct rowComp type = a->rowType;
	if (type.isData)
		return PROGRAM_OFFSET + totalIC + a->address;
	else if (type.isExtern)
		return NULL;
	else if (type.isInstruction)
		return PROGRAM_OFFSET + a->address;
}

/*Iterates over all labels, returns matching struct labelor NULL if not found.*/
struct label*GetLabelFromList(char *name)
{
	struct label*cLabel = headLabelList->next;
	/*Iterate over list until reaching the last "next" member*/
	while (cLabel)
	{		
		if (!strcmp(cLabel->name, name))
			return cLabel;
		cLabel = cLabel->next;
	}
	return NULL;
}

void InsertLabel(char* c, int cnt, struct rowComp rc)
{
	struct label* cntPtr = headLabelList;
	while (cntPtr->next)
		cntPtr = cntPtr->next;
	cntPtr->next = (struct label*)malloc(sizeof(struct label));
	cntPtr = cntPtr->next;
	cntPtr->address = cnt;
	cntPtr->name = (char*)malloc(sizeof(char*)*(strlen(c) + 1));
	strcpy(cntPtr->name, c);
	cntPtr->rowType = rc;
	cntPtr->next = NULL;
}

void InsertDataOrString(int cnt, int n)
{
	struct dataNode* cntPtr = headDataList;

	while (cntPtr->next)
		cntPtr = cntPtr->next;
	cntPtr->next = (struct dataNode*)malloc(sizeof(struct dataNode));
	cntPtr = cntPtr->next;
	cntPtr->address = cnt;
	cntPtr->data = n;
	cntPtr->next = NULL;
}