/*"ID" or "Component" of the row*/
struct rowComp
{
	int hasLabel : 1;
	int isData : 1;				/* .Data or / .string*/
	int isInstruction : 1;		/* mov...*/
	int isExtern : 1;
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
	int data;
	int address;
	struct dataNode* next;
};

struct word
{
	unsigned short instructionBinary : 12;
	int address;
	struct word* next;
};