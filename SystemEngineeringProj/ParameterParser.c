#include "Globals.h"
#include "DataTypes.h"
#define REGISTER_SIZE 5

#define DST_PARAMTYPE_LOCATION 2
#define SRC_PARAMTYPE_LOCATION 4
#define REGISTER_DATALOC 2

#define REGISTER_COUNT 7

#define EXTRAWORD_DATALOC 2

#define RELOACATEABLEADDRESS 2
#define EXTERNADDRESS 1

struct word* ParseCalcDistanceParam(FILE *fp, int ic, Bool isSecondPass)
{
	char labelNameA[MAX_LABEL_SIZE], labelNameB[MAX_LABEL_SIZE];
	struct label*labelA, *labelB;
	struct word*result;
	int distLabels, distCmdA, distCmdB, labelAAddress, labelBAddress, i, j;
	Bool gotBrackets;
	char cChar;

	/*Get to first character in label*/
	do
	{
		cChar = fgetc(fp);
		if (cChar == '(')
		{
			gotBrackets = TRUE;
			cChar = fgetc(fp);
		}
	} while (cChar == ' ' || cChar == '\t');

	/*Read labelA and assign string end indicator*/
	for (i = 0; cChar != ','; i++)
	{
		labelNameA[i] = cChar;
		cChar = fgetc(fp);
	}
	labelNameA[i] = NULL;

	/*Read to next non blank*/
	do
		cChar = fgetc(fp);
	while (cChar == ' ' || cChar == '\t');

	/*Read label B, assign string end indicator*/
	for (i = 0; cChar != ')'; i++)
	{
		labelNameB[i] = cChar;
		cChar = fgetc(fp);
	}
	labelNameB[i] = NULL;

	if (!isSecondPass)
		return NULL;

	/*Assign and assert struct labelA*/
	if (!(labelA = GetLabelFromList(labelNameA)))
	{
		printf("The first argument in a distance Op is invalid, not a label\n");
		ErrorFound = TRUE;
		return NULL;
	}
	/*Assign and assert struct labelB*/
	else if (!(labelB = GetLabelFromList(labelNameB)))
	{
		printf("The second argument in a distance Op is invalid, not a label\n");
		ErrorFound = TRUE;
		return NULL;
	}

	result = malloc(sizeof(struct word));
	result->next = NULL;

	labelAAddress = CalcLabelAddress(labelA);
	labelBAddress = CalcLabelAddress(labelB);
	/*Distance between labels is the absolute value of the difference between struct labelA address and struct labelB address*/
	distLabels = abs(labelAAddress - labelBAddress);
	distCmdA = abs(labelAAddress - ic - PROGRAM_OFFSET);
	distCmdB = abs(labelBAddress - ic - PROGRAM_OFFSET);

	/*if extern*/
	if (labelA->rowType.isExtern || labelB->rowType.isExtern)
	{
		printf("Error, extern used in a distance operation");
		ErrorFound = TRUE;
	}

	result->instructionBinary = max(max(distLabels, distCmdA), distCmdB) << EXTRAWORD_DATALOC;
	return result;
}

struct word* ParseDirectValue(FILE *fp)
{
	int parsedValue;
	struct word*result;
	if (fscanf(fp, "%d", &parsedValue))
	{
		result = malloc(sizeof(struct word));
		result->next = NULL;
		result->instructionBinary = parsedValue << EXTRAWORD_DATALOC;
		return result;
	}
	else
	{
		printf("Error parsing direct value..\n");
		ErrorFound = TRUE;
		return NULL;
	}
}

struct word* TryParseRegister(char *param)
{
	struct word* result;
	/*Check if it is not a valid register*/
	if (strlen(param) < 2
		|| param[0] != 'r'
		|| param[1] - '0' < 0 || param[1] - '0' > REGISTER_COUNT
		&& (param[2] != ',' && param[2] != NULL))
		return NULL;
	result = malloc(sizeof(struct word));
	result->instructionBinary = (param[1] - '0') << REGISTER_DATALOC;
	result->next = NULL;
	return result;
}

struct word* TryParseLabel(char *name, int ic, Bool isSecondPass, Bool isSecondParam)
{
	struct word*result;
	struct label*lab = GetLabelFromList(name);
	if (lab == NULL)
		return NULL;
	result = malloc(sizeof(struct word));
	result->next = NULL;
	/*shift address 2 bits left.*/
	result->instructionBinary = (CalcLabelAddress(lab)) << EXTRAWORD_DATALOC;
	/*assign A.R.E value*/
	if (isSecondPass && lab->rowType.isExtern)
	{
		result->instructionBinary |= EXTERNADDRESS;
		InsertExtern(lab->name, ic + PROGRAM_OFFSET + 1 + isSecondParam);
	}
	else
		result->instructionBinary |= RELOACATEABLEADDRESS;
	return result;
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

/*
Parase a single parameter, return the word, and the target type via the parameter.
*/
struct word* ParseParamWordAndType(FILE *fp, int ic, Bool isSecondPass, Bool isSecondParam, ParamType* typeOutput)
{
	char currentChar;
	char currentString[MAX_LABEL_SIZE];
	struct word * result;
	int i;

	/*Skip all non relevant characters*/
	do
		currentChar = fgetc(fp);
	while (currentChar == ' ' || currentChar == '\t');

	/*If distance*/
	if (currentChar == '~')
	{
		*typeOutput = DISTANCE;
		return ParseCalcDistanceParam(fp, ic, isSecondPass);
	}
	/*if immediate*/
	else if (currentChar == '#')
	{
		*typeOutput = IMMIDIATE;
		return ParseDirectValue(fp);
	}
	/*Unget the character, we will read the rest as string.*/
	
	/*Read until next ','*/
	for (i = 0; currentChar != ',' && currentChar != '\n' && currentChar != ' '; i++, currentChar = fgetc(fp))
	{
		currentString[i] = currentChar;
	}
	currentString[i] = NULL;
	/* Unget the last character, if it was ',' we want to handle it later*/
	ungetc(currentChar, fp);

	if (result = TryParseRegister(currentString))
	{
		*typeOutput = DIRREG;
		return result;
	}
	/*If it is the first pass, we assume it is a label, as we dont know which labels exist yet..*/
	else if (!isSecondPass)
	{
		*typeOutput = DIRECT;
		return NULL;
	}
	else if (result = TryParseLabel(currentString, ic, isSecondPass, isSecondParam))
	{
		*typeOutput = DIRECT;
		return result;
	}

	ErrorFound = TRUE;
	printf("Invalid parameter of unknown type!\n");

	return NULL;
}

int ParseParamsTypeAndLength(FILE *fp, opCode cmd, int ic, Bool isSecondPass, ParamType *aType, ParamType *bType)
{
	short numOfParams;
	*aType = *bType = NULLPARAM;
	struct word *paramA = NULL, *paramB = NULL;
	char tempChar;

	numOfParams = GetGroup(cmd);
	if (numOfParams == 0)
	{
		for (tempChar = fgetc(fp); tempChar != '\n'; tempChar = fgetc(fp))
		{
			if (tempChar != ' ' && tempChar != '\t')
			{
				ErrorFound = TRUE;
				printf("\nExcess characters found after all parameters have been parsed...\n");
			}
		}
		return ++ic;
	}

	paramA = ParseParamWordAndType(fp, ic, isSecondPass, FALSE, aType);

	if (numOfParams == 2)
	{
		/*Read until the ','*/
		do
		{
			tempChar = fgetc(fp);
		} while (tempChar != ',');
		paramB = ParseParamWordAndType(fp, ic, isSecondPass, TRUE, bType);
	}

	//Advance IC (command takes 1 word).
	ic++;
	if (*aType != NULLPARAM)
		ic++;

	if (paramA)
	{
		if (isSecondPass)
		{
			paramA->address = ic;
			InsertWord(paramA);
		}
		else
			free(paramA);
	}

	/*If both params are reg they only take one word - no need to advance IC.*/
	if (*bType != NULLPARAM && (DIRREG != *aType || DIRREG != *bType))
		ic++;

	if (paramB)
	{
		if (isSecondPass)
		{
			/*If there is a second param, a should be shifted to the source param.*/
			if (DIRREG == *aType)
			{
				paramA->instructionBinary <<= REGISTER_SIZE;
				/*If both params are reg they only take one word.*/
				if (DIRREG == *bType)
					paramA->instructionBinary |= paramB->instructionBinary;
				else
				{
					paramB->address = ic;
					InsertWord(paramB);
				}
			}
			else
			{
				paramB->address = ic;
				InsertWord(paramB);
			}
		}
		else
		{
			free(paramB);
		}
	}
	return ic;
}

int ParseParamsSecondPass(FILE *fp, opCode cmd, int ic, struct word *instructionWord)
{
	ParamType aType, bType;
	ic = ParseParamsTypeAndLength(fp, cmd, ic, TRUE, &aType, &bType);
	if (bType == DIRREG || aType == DIRREG)
		printf("we did get here... %d, %d\n", aType, bType);

	if (aType != NULLPARAM)
	{ 
		if (bType != NULLPARAM)
		{/*Both params are defined*/
			ValidateParameterType(FALSE, aType, cmd);
			ValidateParameterType(TRUE, bType, cmd);
			instructionWord->instructionBinary |= (aType << SRC_PARAMTYPE_LOCATION);
			instructionWord->instructionBinary |= (bType << DST_PARAMTYPE_LOCATION);
		}
		else
		{/*There is only one param, hence it is the destination param */
			ValidateParameterType(TRUE, aType, cmd);
			instructionWord->instructionBinary |= (aType << DST_PARAMTYPE_LOCATION);
		}
	}
	return ic;
}


int ParseParamsFirstPass(FILE *fp, opCode cmd, int ic)
{
	ParamType aType, bType;
	return ParseParamsTypeAndLength(fp, cmd, ic, FALSE, &aType, &bType);
}