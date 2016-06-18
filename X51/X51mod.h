////////////////////////////////////////////////////////////////////////////////
// X51mod.h
// X51 module header file (used in assembler and linker)


#ifndef _X51MOD_H
#define _X51MOD_H

#include "X51cmn.h" //common header
#include "X51modD.h" //module definition header
#include <limits.h> //limits header (PATH_MAX..)


typedef struct SX51Seg
{
	char name[X51_MAX_STR_SEG + 1];
	char type;
	unsigned short addr;
	int len;
} SX51Seg;


typedef struct SX51Mod
{
	char filename[PATH_MAX + 1];
	char name[X51_MAX_STR_MOD + 1];
	int sym_cnt;
	int sym_cap;
	int sym_chunk;
	SStrInt* sym_byval;
	SStrInt* sym_bystr;
	int str_len;
	int str_cap;
	int str_chunk;
	char* str;
} SX51Mod;


//initializes empty module context and return its pointer
SX51Mod* X51Mod_Init(char* pcName);

//frees the module context
void X51Mod_Done(SX51Mod* pm);

//appends symbol to module context
int X51Mod_AddSym(SX51Mod* pm, char* pcName, int iVal);
//sorts symbols in module context
void X51Mod_Sort(SX51Mod* pm);
//finds symbol by its name, in module context (case sensitive)
SStrInt* X51Mod_FindSymByName(SX51Mod* pm, char* pcName);
//finds symbol by its value, in module context (case sensitive)
SStrInt* X51Mod_FindSymByVal(SX51Mod* pm, int iVal);

//check duplicate symbol names (after Sort)
int X51Mod_CheckSyms(SX51Mod* pm);
//print symbols in module context
void X51Mod_PrintSyms(SX51Mod* pm, int iByName);

//
char* X51Mod_Value2Name(void* pParam, char cType, int iValue);
//
int X51Mod_Name2Value(void* pParam, char cType, char* pcName);


#endif //_X51MOD_H