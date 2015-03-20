#include "DataStructUtilities.h"
#include "Globals.h"

typedef enum { NOTDATA, DATA, STRING, EXTERN, ENTRY } DataType;

DataType TryParseDataTypes(char *str)
{
	if (!strcmp(str, ".data"))
		return DATA;
	else if (!strcmp(str, ".string"))
		return STRING;
	else if (!strcmp(str, ".extern"))
		return EXTERN;
	else if (!strcmp(str, ".entry"))
		return ENTRY;
	else
		return NOTDATA;
}

void FirstPass(FILE *fp)
{
	int ic = 0, dc = 0;
	char tempScan[MAX_LABEL_SIZE];
	char tempScanLabel[MAX_LABEL_SIZE];
	struct rowComp rowCompTemp = { 0 };
	DataType dataType;
	int cData;
	int i;
	opCode cmdType;
	short numOfParams;
	ParamType aType, bType;

	printf("%u\n", headExternList->next);

	while (!feof(fp))	/* while not end of file*/
	{
		/* every time i do scanf, its like i just continue scan (no white chars)*/
		fscanf(fp, "%s", &tempScan);
		printf("Reading: %s IC: %d DC: %d\n", tempScan, ic, dc);
		if (strrchr(tempScan, ':'))
		{
			rowCompTemp.hasLabel = 1;
			strcpy(tempScanLabel, tempScan);
			/*Remove the trailing ':' */
			tempScanLabel[strlen(tempScanLabel) - 1] = NULL;
			fscanf(fp, "%s", &tempScan);
		}

		dataType = TryParseDataTypes(tempScan);
		if (dataType == DATA || dataType == STRING)
		{
			rowCompTemp.isData = TRUE;
			if (rowCompTemp.hasLabel)
				InsertLabel(tempScanLabel, dc, rowCompTemp);

			if (dataType == DATA)
			{
				while (!feof(fp) && fscanf(fp, "%d", &cData))
				{
					InsertDataOrString(dc, cData);
					dc++;

					/*Read next non whitespace char*/
					fscanf(fp, "%c", &cData);
				}
				if (!feof(fp))
					ungetc(cData, fp);
			}
			else if (dataType == STRING)
			{
				fscanf(fp, "%s", &tempScan);
				for (i = 1; i < strlen(tempScan) - 1; i++)
				{
					printf("inserting: %c\n", tempScan[i]);
					InsertDataOrString(dc, tempScan[i]);
					dc++;
				}
				/*Insert end of string indicator*/
				InsertDataOrString(dc, NULL);
				dc++;
			}
		}
		else if (dataType == EXTERN)
		{
			rowCompTemp.isExtern = 1;
			fscanf(fp, "%s", &(tempScan));
			InsertLabel(tempScan, NULL, rowCompTemp);
		}
		else if ((cmdType = GetCommandType(tempScan)) != -1)
		{
			rowCompTemp.isInstruction = 1;
			InsertLabel(tempScanLabel, ic, rowCompTemp);
			ic = ParseParamsFirstPass(fp, cmdType, ic);
		}
		else if (dataType == ENTRY)
		{
			fscanf(fp, "%s", &tempScan);
		}
		else
		{
			printf("\nUnidentified input, current input:  %s\n", tempScan);
			ErrorFound = TRUE;
		}

		rowCompTemp.hasLabel = 0;
		rowCompTemp.isData = 0;
		rowCompTemp.isExtern = 0;
		rowCompTemp.isInstruction = 0;
		rowCompTemp.isEntry = 0;
	}

	totalIC = ic;
	totalDC = dc;
	return 0;
}