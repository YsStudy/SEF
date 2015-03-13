#include "Globals.h"


void InsertToLabelList (char *name, int address, struct label*list)
{
	struct label*ext, *clabel = list;
	ext = malloc(sizeof(struct label));
	ext->next = NULL;
	ext->address = address;
	strcpy(ext->name, name);

	while (!clabel->next)
		clabel = clabel->next;
	clabel->next = ext;
}

void insertWord(struct word*a)
{
	struct word*cWord = headWordList;
	while (!cWord->next)
		cWord = cWord->next;
	cWord->next = a;
}

/*Calculates the final label's address (data rows appear after instruction rows, and address is relative to type)*/
int CalcLabelAddress(struct label*a)
{
	struct rowComp type = a->rowType;
	if (type.isData)
		return totalIC + a->address;
	else if (type.isExtern)
		return NULL;
	else if (type.isInstruction)
		return a->address;
}

/*Iterates over all labels, returns matching struct labelor NULL if not found.*/
struct label*GetLabelFromList(char *name)
{
	struct label*cLabel = headLabelList;
	/*Iterate over list until reaching the last "next" member*/
	while (cLabel)
	{		
		if (strcmp(cLabel->name, name))
			return cLabel;
		cLabel = cLabel->next;
	}
	return NULL;
}