#include "Globals.h"

#define REGISTER_SIZE 5
#define DST_PARAMTYPE_LOCATION 2
#define SRC_PARAMTYPE_LOCATION 4

opCode GetCommandType(char * cmd);
unsigned short GetGroup(opCode cmdType);
int ParseCmd(FILE *fp, int ic);
Bool ValidateParameterType(int isDestination, ParamType param, opCode cmd);
void MergeDirectRegisterParams();

void SecondPass(fp)
{
	int ic = 0;
	while (!feof(fp))
	{
		/*Reads a command, or does nothing if it cannot find a command.
		Returns the IC after advancing it accordingly*/
		ic = ParseCmd(fp, ic);
	}
}


int ParseCmd(FILE *fp, int ic)
{
	opCode cmdType;
	unsigned short group;
	struct word* result;
	ParamType paramA, paramB;
	char cmd[MaxCmdLength];
	result = (struct word*)malloc(sizeof(struct word));

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
			printf("Debug: CMDType %d\n", cmdType);
		}
	} while (cmdType == -1);

	group = GetGroup(cmdType);
	printf("Debug: group %d\n", group);

	result->instructionBinary |= cmdType << OPCODE_LOCATION;
	result->instructionBinary |= group << GROUP_LOCATION;

	insertWord(result);
	ic++;
	/*Group has 0 words, so we are done here, opCode and group are set, the rest of the fields are zeroes.*/
	if (!group) /* TODO: READ TO END - CHECK IF ANY CHARACTERS, REPORT ERROR IF SO*/
		return ic;
	paramA = ParseParam(fp, ic++);

	if (group == 1)
	{
		ValidateParameterType(1, paramA, cmdType);
		result->instructionBinary |= (paramA << DST_PARAMTYPE_LOCATION);
		/* TODO: READ TO END - CHECK IF ANY CHARACTERS, REPORT ERROR IF SO*/
		return ic;
	}
	else
		ValidateParameterType(0, paramA, cmdType);

	paramB = ParseParam(fp, ic); 
	ic++;
	ValidateParameterType(1, paramB, cmdType);

	/*check if both are register, in which case they need to be joined.*/
	if (paramA == DIRREG && paramB == DIRREG)
	{
		ic--;
		MergeDirectRegisterParams(); 
	}
	
	result->instructionBinary |= (paramA << SRC_PARAMTYPE_LOCATION);
	result->instructionBinary |= (paramB << DST_PARAMTYPE_LOCATION);
	/* TODO: READ TO END - CHECK IF ANY CHARACTERS, REPORT ERROR IF SO*/
	return ic;
}

void printWord(struct word W)
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

opCode GetCommandType(char *cmd)
{
	short cmdCount, cmdIterator = 0;

	cmdCount = sizeof(CmdList) / sizeof(char);
	while (cmdIterator++ < cmdCount)
	{
		if (!strcmp(CmdList[cmdIterator], cmd))
			return cmdIterator;
	}
	return -1;
}

Bool ValidateParameterType(int isDestination, ParamType param, opCode cmd)
{
	if (!isDestination)
	{
		if (cmd < 4 || (param == DIRECT && cmd == lea)) //First four commands work with all types lea works with DIRECT only.
			return TRUE;
		else
		{
			ErrorFound = TRUE;
			printf("Parameter is of invalid type..");
			return FALSE;
		}
	}
	
	switch (cmd)
	{
		case cmp:
		case prn: 
			return TRUE; /*these work with all types.*/
		case jmp:
		case bne:
		case red:
			if (param) /*these work with 1,2,3 (non zeroes)..*/
				return TRUE;
			break;
		case jsr:
			if (param == DIRECT)
				return TRUE;
			break;
		default: 
			if (param == DIRECT || param == DIRREG) /*these work with 1,3..*/
				return TRUE;	
			break;
	}
	ErrorFound = TRUE;
	printf("Parameter is of invalid type..");
	return FALSE;
}

/*Merges the last two words - assumes they are registers which need to be placed side by side in the same work.*/
void MergeDirectRegisterParams()
{
	struct word*cWord = headWordList;
	
	/*Advance to the second-to-last word.*/
	while ((cWord->next)->next)
		cWord = cWord->next;

	/*merge the two words*/
	cWord->instructionBinary |= ( ((cWord->next)->instructionBinary) >> REGISTER_SIZE);

	/*Free resources, and remove from list.*/
	free(cWord->next);
	cWord->next = NULL;
}
