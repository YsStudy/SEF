#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DataTypes.h"

typedef enum {FALSE, TRUE } Bool;
typedef enum { IMMIDIATE, DIRECT, DISTANCE, DIRREG, DIRECTEXT, NULLPARAM=-1 } ParamType;

#define PROGRAM_OFFSET 100

#define MAX_LABEL_SIZE 100

#define MaxCmdLength 5
#define CommandCount 16

//globals:
extern int totalIC;
extern int totalDC;
extern Bool ErrorFound;
extern struct label* headLabelList;
extern struct dataNode* headDataList;
extern struct extLabel* headExternList;
extern struct word*headWordList;

extern char CmdList[CommandCount][MaxCmdLength];
typedef enum { mov, cmp, add, sub, not, clr, lea, inc, dec, jmp, bne, red, prn, jsr, rts, stop } opCode;

