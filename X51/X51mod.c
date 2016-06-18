////////////////////////////////////////////////////////////////////////////////
// X51mod.c
// X51 module source file


#include "X51cmn.h"
#include "X51mod.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


SX51Mod* X51Mod_Init(char* pcName)
{
	SX51Mod* pm = (SX51Mod*)malloc(sizeof(SX51Mod));
	if (!pm) return 0;
	memset(pm, 0, sizeof(SX51Mod));
	strcpy(pm->name, pcName);
	pm->sym_cnt = 0;
	pm->sym_cap = 0;
	pm->sym_chunk = X51MOD_DEF_SYMCHUNK;
	pm->sym_byval = 0;
	pm->sym_bystr = 0;
	pm->str_len = 0;
	pm->str_cap = 0;
	pm->str_chunk = X51MOD_DEF_STRCHUNK;
	pm->str = 0;
	return pm;
}

void X51Mod_Done(SX51Mod* pm)
{
	if (!pm) return;
	if (pm->sym_byval) free(pm->sym_byval);
	if (pm->sym_bystr) free(pm->sym_bystr);
	if (pm->str) free(pm->str);
	free(pm);
}


void X51Mod_Sort(SX51Mod* pm)
{
	memcpy(pm->sym_bystr, pm->sym_byval, pm->sym_cnt * sizeof(SStrInt));
	qsort(pm->sym_byval, pm->sym_cnt, sizeof(SStrInt), &compare_val);
	qsort(pm->sym_bystr, pm->sym_cnt, sizeof(SStrInt), &compare_str);
}

int X51Mod_AddSym(SX51Mod* pm, char* pcName, int iVal)
{
	int iNameLen = strlen(pcName);
	if (iNameLen > X51_MAX_STR_SYM) return X51MOD_ERR_SNTLNG;
	if (pm->sym_cap < (pm->sym_cnt + 1))
	{
		if (pm->sym_byval) pm->sym_byval = (SStrInt*)realloc(pm->sym_byval, (pm->sym_cap + pm->sym_chunk) * sizeof(SStrInt));
		else pm->sym_byval = (SStrInt*)malloc((pm->sym_cap + pm->sym_chunk) * sizeof(SStrInt));
		if (pm->sym_bystr) pm->sym_bystr = (SStrInt*)realloc(pm->sym_bystr, (pm->sym_cap + pm->sym_chunk) * sizeof(SStrInt));
		else pm->sym_bystr = (SStrInt*)malloc((pm->sym_cap + pm->sym_chunk) * sizeof(SStrInt));
		if (!pm->sym_byval || !pm->sym_bystr)
			return X51_ERR_MALLOC;
		pm->sym_cap += pm->sym_chunk;
	}
	if (pm->str_cap < (pm->str_len + iNameLen + 1))
	{
		char* oldstr = pm->str;
		if (pm->str) pm->str = (char*)realloc(pm->str, (pm->str_cap + pm->str_chunk));
		else pm->str = (char*)malloc(pm->str_cap + pm->str_chunk);
		if (!pm->str)
			return X51_ERR_MALLOC;
		pm->str_cap += pm->str_chunk;
		int shift = oldstr?(pm->str - oldstr):0;
		if (shift) for (int i = 0; i < pm->sym_cnt; i++)
			pm->sym_byval[i].pc += shift;
	}
	char* pcStr = pm->str + pm->str_len;
	strcpy(pcStr, pcName);
	pm->str_len += (iNameLen + 1);
	pm->sym_byval[pm->sym_cnt].pc = pcStr;
	pm->sym_byval[pm->sym_cnt].i = iVal;
	return pm->sym_cnt++;
}

SStrInt* X51Mod_FindSymByName(SX51Mod* pm, char* pcName)
{
	SStrInt si = {pcName, 0};
	SStrInt* psi = (SStrInt*)bsearch(&si, pm->sym_bystr, pm->sym_cnt, sizeof(SStrInt), &compare_str);
	return psi;
}

SStrInt* X51Mod_FindSymByVal(SX51Mod* pm, int iVal)
{
	SStrInt si = {0, iVal};
	SStrInt* psi = (SStrInt*)bsearch(&si, pm->sym_byval, pm->sym_cnt, sizeof(SStrInt), &compare_val);
	return psi;
}

int X51Mod_CheckSyms(SX51Mod* pm)
{
}

void X51Mod_PrintSyms(SX51Mod* pm, int iByName)
{
	char cVType[] = "?CDBN";
	char* pcSType[] = {"???", "SEGMENT", "PUBIC", "SYMBOL", "EXTERN", "LINE#"};
	for (int i = 0; i < pm->sym_cnt; i++)
	{
		int iVal = iByName?pm->sym_bystr[i].i:pm->sym_byval[i].i;
		char* pcName = iByName?pm->sym_bystr[i].pc:pm->sym_byval[i].pc;
		int iSType = iVal >> 20;
		int iVType = (iVal >> 16) & 0xf;
		int iAddr = iVal & 0xffff;
		if (iVType == X51MOD_VTYP_B)
		{
			iVal &= 0xff;
			iAddr = (iVal < 0x80)?((iVal >> 3) + 0x20):(iVal & 0xf8);
			int iBit = iVal & 7;
			printf("  %c:%04XH.%d       %-7s       %s\n", cVType[iVType], iAddr, iBit, pcSType[iSType], pcName);
		}
		else
			printf("  %c:%04XH         %-7s       %s\n", cVType[iVType], iAddr, pcSType[iSType], pcName);
	}
}

//
char* X51Mod_Value2Name(void* pParam, char cType, int iValue)
{
	SStrInt* psi = 0;
	switch (cType)
	{
	case 'C':
		psi = X51Mod_FindSymByVal((SX51Mod*)pParam, (iValue & 0xffff) | (X51MOD_TYP_PUBC << 16));
		if (psi) return psi->pc;
		psi = X51Mod_FindSymByVal((SX51Mod*)pParam, (iValue & 0xffff) | (X51MOD_TYP_SYMC << 16));
		if (psi) return psi->pc;
		break;
	case 'B':
		psi = X51Mod_FindSymByVal((SX51Mod*)pParam, (iValue & 0xff) | (X51MOD_TYP_PUBB << 16));
		if (psi) return psi->pc;
		psi = X51Mod_FindSymByVal((SX51Mod*)pParam, (iValue & 0xff) | (X51MOD_TYP_SYMB << 16));
		if (psi) return psi->pc;
		break;
	case 'D':
		psi = X51Mod_FindSymByVal((SX51Mod*)pParam, (iValue & 0xff) | (X51MOD_TYP_PUBD << 16));
		if (psi) return psi->pc;
		psi = X51Mod_FindSymByVal((SX51Mod*)pParam, (iValue & 0xff) | (X51MOD_TYP_SYMD << 16));
		if (psi) return psi->pc;
		break;
/*	case 'N':
		psi = X51Mod_FindSymByVal((SX51Mod*)pParam, (iValue & 0xff) | (X51MOD_TYP_SYMN << 16));
		if (psi) return psi->pc;
		break;*/
	}
	return 0;
}

//
int X51Mod_Name2Value(void* pParam, char cType, char* pcName)
{
	
}