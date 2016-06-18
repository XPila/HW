////////////////////////////////////////////////////////////////////////////////
// X51lnk.h
// X51 linker header file


#ifndef _X51LNK_H
#define _X51LNK_H

#include "X51cmn.h" //common header
#include "X51mod.h" //module manipulation header
#include "X51lnkD.h" //linker definition header
#include "X51modD.h" //module manipulation definition header
#include <limits.h> //limits header (PATH_MAX..)

typedef struct SX51Lnk
{
	int mod_cnt;
	int mod_cap;
	int mod_chunk;
	SStrInt* mod_byval;
	SStrInt* mod_bystr;
	SX51Mod** pmod;
} SX51Lnk;

//initializes empty linker context
SX51Lnk* X51Lnk_Init();
//appends module to linker context
int X51Lnk_AddMod(SX51Lnk* pl, char* cName);
//check duplicate module names (after SortMods)
int X51Lnk_CheckMods(SX51Lnk* pl);
//finds module by its name (case sensitive)
SX51Mod* X51Lnk_FindModByName(SX51Lnk* pl, char* pcName);
//sorts modules and its symbols
void X51Lnk_Sort(SX51Lnk* pl);
//print modules in linker context
void X51Lnk_PrintMods(SX51Lnk* pl, int iByName);
//loads M51 listing (keil format)
int X51Lnk_LoadM51(SX51Lnk* pl, char* pcFileName, int iFlags);



//
char* X51Lnk_Value2Name(void* pParam, char cType, int iValue);
//
int X51Lnk_Name2Value(void* pParam, char cType, char* pcName);

#endif //_X51LNK_H