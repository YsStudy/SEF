#include <stdio.h>

#define OPCODE_LOCATION 6;
#define GROUP_LOCATION 10;
#define SOURCEOP_LOCATION 4;
#define DESTOP_LOCATION 2;

//Order critical enumeration of the commands.
const char *CmdList[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
typedef enum { mov, cmp, add, sub, not, clr, lea, inc, dec, jmp, bne, red, prn, jsr, rts, stop } opCode;


typedef struct wordStruct word;
typedef struct wordStruct
{
	unsigned short instructionBinary : 12;
	int address;
	word* next;
};
void printWord(word W);


opCode GetCommandType(char * command);
unsigned short GetGroup(opCode cmdType);
word* ParseCmd(char * command);

int main()
{
	char command[] = "cmp	~(K,END),W";
	ParseCmd(command);
	return 0;
}

word* ParseCmd(char * command)
{
	opCode cmdType;
	unsigned short group;
	word* result;

	result = malloc(sizeof(word));

	result->instructionBinary = 0;
	result->next = NULL;

	cmdType = GetCommandType(command);
	printf("Debug: CMDType %d\n", cmdType);
	if (cmdType == -1)
		return NULL;

	group = GetGroup(cmdType);
	printf("Debug: group %d\n", group);

	result->instructionBinary |= cmdType << OPCODE_LOCATION;
	result->instructionBinary |= group << GROUP_LOCATION;

	//Group has 0 words, so we are done here, opCode and group are set, the rest of the fields are zeroes.
	if (!group)
		return result;

	//Todo: advance char iterator, parse params

	printWord(*result);

	getchar();
	return result;
}

void printWord(word W)
{
	int i;
	printf("Word:\n");
	for (i = 1<<11; i > 0; i >>= 1)
	{
		if (W.instructionBinary & i)
			printf("1");
		else
			printf("0");
	}
	printf("\n");
}

unsigned short GetGroup(opCode cmdType)
{
	switch (cmdType)
	{
	case mov:
	case cmp:
	case add:
	case sub:
	case lea:
		return 2;
	case not:
	case clr:
	case inc:
	case dec:
	case jmp:
	case bne:
	case red:
	case prn:
	case jsr:
		return 1;
	default:
		return 0;
	}
}

opCode GetCommandType(char * command)
{
	short cmdIterator = 0;
	short charIterator = 0;
	short arrayLen = sizeof(CmdList) / sizeof(char*);
	printf("%d\n", arrayLen);

	//iterate over all commands..
	for (cmdIterator = 0; cmdIterator < arrayLen; cmdIterator++)
	{
		//increase char iterator until the next non-letter character or until the first non-matching character
		for (charIterator = 0; (isalpha(command[charIterator]) && (command[charIterator] == CmdList[cmdIterator][charIterator]) ); charIterator++)
			printf("DEBUG: %d, %d\n", command[charIterator], CmdList[cmdIterator][charIterator]);
		printf("DDEBUG: %d, %d\n", command[charIterator], CmdList[cmdIterator][charIterator]);

		//if both the input command's end (non-letter character) and the matching command's end have been reached, return the matching command index.
		if (!isalpha(command[charIterator]) && !isalpha(CmdList[cmdIterator][charIterator]))
			return cmdIterator;
	}
	return -1;
}

