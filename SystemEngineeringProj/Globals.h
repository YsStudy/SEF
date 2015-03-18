#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DataTypes.h"

typedef enum {FALSE, TRUE } Bool;
typedef enum { IMMIDIATE, DIRECT, DISTANCE, DIRREG, NULLPARAM=-1 } ParamType;

#define PROGRAM_OFFSET 100

#define OPCODE_LOCATION 6
#define GROUP_LOCATION 10
#define SOURCEOP_LOCATION 4
#define DESTOP_LOCATION 2
#define MAX_LABEL_SIZE 100
#define EXTRAWORD_DATALOC 2

#define TARGET_TYPE_DIST 2
#define TARGET_TYPE_DIST 2
#define TARGET_TYPE_IMID 0

#define ABSOLUTE 0;
#define EXTERNADDRESS 1;
#define RELOACATEABLEADDRESS 2;

#define REGISTER_COUNT 7
#define REGISTER_DATALOC 2

#define MaxCmdLength 5
#define CommandCount 16

//globals:
extern int totalIC;
extern int totalDC;
extern Bool ErrorFound;
extern struct label* headLabelList;
extern struct dataNode* headDataList;
extern struct label* headEntriesList;
extern struct label* headExternList;
extern struct word*headWordList;

extern char CmdList[CommandCount][MaxCmdLength];
typedef enum { mov, cmp, add, sub, not, clr, lea, inc, dec, jmp, bne, red, prn, jsr, rts, stop } opCode;

ParamType ParseParamFirstPass(FILE *fp, int ic);
ParamType ParseParamSecondPass(FILE *fp, int ic);


void InsertToLabelList(char *name, int address, struct label*list);
struct label *GetLabelFromList(char *name);
int CalcLabelAddress(struct label *a);
void insertWord(struct word*a);

void SecondPass(FILE * fp);
unsigned short GetGroup(opCode cmdType);

