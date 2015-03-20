#include "Globals.h"

#define OPCODE_LOCATION 6
#define GROUP_LOCATION 10
#define SOURCEOP_LOCATION 4
#define DESTOP_LOCATION 2

opCode GetCommandType(char * cmd);
unsigned short GetGroup(opCode cmdType);
int ParseCmd(FILE *fp, int ic);
Bool ValidateParameterType(int isDestination, ParamType param, opCode cmd);
void MergeDirectRegisterParams();

void SecondPass(FILE * fp)
{
	headWordList = (struct word*)malloc(sizeof(struct word));
	headWordList->address = NULL;
	headWordList->next = NULL;

	int ic = 0;
	while (!feof(fp))
	{
		/*Reads a command, or does nothing if it cannot find a command.
		Returns the IC after advancing it accordingly*/
		ic = ParseCmd(fp, ic);
	}
}

void ScanAndMarkEntry(FILE *fp)
{
	char labelName[MAX_LABEL_SIZE];
	struct label* label;
	fscanf(fp, "%s", labelName);
	if (label = GetLabelFromList(labelName))
	{
		label->rowType.isEntry = 1;
	}
	else
	{
		printf("An entry was declared on a non-existant label.\n");
		ErrorFound = TRUE;
	}
}

int ParseCmd(FILE *fp, int ic)
{
	opCode cmdType;
	unsigned short group;
	struct word* result;
	ParamType paramA, paramB;
	char cmd[MAX_LABEL_SIZE];
	result = (struct word*)malloc(sizeof(struct word));
	result->address = ic;
	result->instructionBinary = 0;
	result->next = NULL;

	/*Skip all non-command*/
	do
	{
		if (feof(fp))
			return ic;
		else if (!fscanf(fp, "%s", &cmd))
			cmdType = - 1;
		else
		{
			cmdType = GetCommandType(cmd);
		}
		if (cmdType == -1 && (!strcmp(cmd, ".entry")))
		{
			ScanAndMarkEntry(fp);
		}
	} while (cmdType == -1);

	group = GetGroup(cmdType);
	printf("Debug: group %d\n", group);

	result->instructionBinary |= cmdType << OPCODE_LOCATION;
	result->instructionBinary |= group << GROUP_LOCATION;

	printf("inserting incomplete word");
	InsertWord(result);
	ic = ParseParamsSecondPass(fp, cmdType, ic, result);
	printf("complete word: ");
	printWord(*result);
	return ic;
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

opCode GetCommandType(char *cmd)
{
	short cmdCount, cmdIterator = 0;
	while (cmdIterator < CommandCount)
	{
		if (!strcmp(CmdList[cmdIterator], cmd))
			return cmdIterator;
		cmdIterator++;
	}
	return -1;
}
