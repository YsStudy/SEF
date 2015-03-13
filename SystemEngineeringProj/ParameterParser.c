#include "Globals.h"


void ParseCalcDistanceParam(FILE *fp, int ic)
{
	char labelNameA[MAX_LABEL_SIZE], labelNameB[MAX_LABEL_SIZE];
	struct label*labelA, *labelB;
	struct word*result;
	int distLabels, distCmdA, distCmdB, labelAAddress, labelBAddress;

	result = malloc(sizeof(struct word));
	result->next = NULL;
	result->address = ic;

	/*Scan labels and check there are indeed two labels.*/
	if (fscanf(fp, "(%s,%s)", labelNameA) != 2)
	{
		printf("Not enough arguments for distance..");
		ErrorFound = TRUE;
		return;
	}
	/*Assign and assert struct labelA*/
	if (!(labelA = GetLabelFromList(labelNameA)))
	{
		printf("The first argument in a distance Op is invalid, not a label\n");
		ErrorFound = TRUE;
		return;
	}
	/*Assign and assert struct labelB*/
	else if (!(labelB = GetLabelFromList(labelNameB)))
	{
		printf("The second argument in a distance Op is invalid, not a label\n");
		ErrorFound = TRUE;
		return;
	}
	labelAAddress = CalcLabelAddress(labelA);
	labelBAddress = CalcLabelAddress(labelB);
	/*Distance between labels is the absolute value of the difference between struct labelA address and struct labelB address*/
	distLabels = abs(labelAAddress - labelBAddress);
	distCmdA = abs(labelAAddress - ic);
	distCmdB = abs(labelBAddress - ic);

	/*if extern*/
	if (labelA->address == 0)
		InsertToLabelList(labelA->name, ic, headExternList);
	if (labelB->address == 0)
		InsertToLabelList(labelB->name, ic, headExternList);

	result->instructionBinary = max(max(distLabels, distCmdA), distCmdB) << EXTRAWORD_DATALOC;
	insertWord(result);
}

void ParseDirectValue(FILE *fp, int ic)
{
	int parsedValue;
	struct word*result;
	result = malloc(sizeof(struct word));
	result->next = NULL;
	result->address = ic;
	if (fscanf(fp, "%d", &parsedValue))
		insertWord(result);
	else
	{
		printf("Error parsing direct value..\n");
		ErrorFound = TRUE;
	}
}

Bool TryParseRegister(char *param, int ic)
{
	struct word* result;
	/*Check if it is not a valid register*/
	if (strlen(param) < 2
		|| param[0] != 'r'
		|| param[1] - '0' < 0 || param[1] - '0' > REGISTER_COUNT
		&& (param[2] != ',' && param[2] != NULL))
		return FALSE;
	result = malloc(sizeof(struct word));
	result->address = ic;
	result->instructionBinary = (param[1] - '0') << REGISTER_DATALOC;
	result->next = NULL;
	insertWord(result);
	return TRUE;
}

Bool TryParseLabel(char *name, int ic)
{
	struct word*result;
	struct label*lab = GetLabelFromList(name);
	if (lab == NULL)
		return FALSE;
	result = malloc(sizeof(struct word));
	result->next = NULL;
	result->address = ic;
	/*shift address 2 bits left.*/
	result->instructionBinary = (lab->address) << EXTRAWORD_DATALOC;
	/*assign A.R.E value*/
	result->instructionBinary |= RELOACATEABLE;
	insertWord(result);
	return TRUE;
}

/*
Parase a single parameter, return the target type (direct/immediate/dist/
*/
ParamType ParseParam(FILE *fp, int ic)
{
	char currentChar;
	char currentString[MAX_LABEL_SIZE];

	/*Skip all non relevant characters*/
	do
		currentChar = fgetc(fp);
	while (currentChar == ' ' || currentChar == '\t');

	/*If distance*/
	if (currentChar == '~')
	{
		ParseCalcDistanceParam(fp, ic);
		return DISTANCE;
	}
	/*if immediate*/
	else if (currentChar == '#')
	{
		ParseDirectValue(fp, ic);
		return IMMIDIATE;
	}
	/*Unget the character, we will read the rest as string.*/
	else
		ungetc(currentChar, fp);

	if (!fscanf(fp, "%s", currentString))
	{
		ErrorFound = TRUE;
		printf("Param expected!\n");
	}
	if (TryParseRegister(currentString, ic))
		return DIRREG;
	else if (TryParseLabel(currentString, ic))
		return DIRECT;

	ErrorFound = TRUE;
	printf("Invalid parameter of unknown type!\n");
	/*we basically just return whatever to keep going..*/
	return DIRECT;
}