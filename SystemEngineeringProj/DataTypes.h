#ifndef DATA_TYPES
#define DATA_TYPES

#define WORD_SIZE 12

/*"ID" or "Component" of the row*/
struct rowComp
{
	unsigned int hasLabel : 1;
	unsigned int isData : 1;				/* .Data or / .string*/
	unsigned int isInstruction : 1;		/* mov...*/
	unsigned int isExtern : 1;
	unsigned int isEntry : 1;
};

struct label
{
	char* name;
	int address;
	struct rowComp rowType;			/*"hasLAbel" must be "1" in this case*/
	struct label * next;
};


struct extLabel
{
	char* name;
	int address;
	struct extLabel * next;
};

/*"Data / Sring Table" (will be contained in linked list)*/
struct dataNode
{
	int data : WORD_SIZE;
	int address;
	struct dataNode* next;
};

struct word
{
	unsigned short instructionBinary : WORD_SIZE;
	int address;
	struct word* next;
};

#endif