////////////////////////////////////////////////////////////////////////////////
// X51lnk.c
// X51 linker source file


#include "X51cmn.h"
#include "X51lnk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

char g_cM51LScan[64] = {0};
char g_cM51SScan[64] = {0};

//Line scan string
//char g_cLScan[X51_MAX_STR_SCAN + 1] = "";

//Token scan string
//char g_cTScan[X51_MAX_STR_SCAN + 1] = "";


SX51Lnk* X51Lnk_Init()
{
	sprintf(g_cM51LScan, "%%%d[^\r\n]%%n", X51_MAX_STR_M51LINE + 1);

	SX51Lnk* pl = (SX51Lnk*)malloc(sizeof(SX51Lnk));
	if (!pl) return 0;
	memset(pl, 0, sizeof(SX51Lnk));
	pl->mod_cnt = 0;
	pl->mod_cap = 0;
	pl->mod_chunk = X51LNK_DEF_MODCHUNK;
	pl->mod_byval = 0;
	pl->mod_bystr = 0;
	pl->pmod = 0;
	return pl;
}

void X51Lnk_Done(SX51Lnk* pl)
{
	if (!pl) return;
	if (pl->pmod)
		for (int i = 0; i < pl->mod_cnt; i++)
			if (pl->pmod[i])
				X51Mod_Done(pl->pmod[i]);
	if (pl->mod_byval) free(pl->mod_byval);
	if (pl->mod_bystr) free(pl->mod_bystr);
	if (pl->pmod) free(pl->pmod);
	free(pl);
}

int X51Lnk_AddMod(SX51Lnk* pl, char* cName)
{
	int iNameLen = strlen(cName);
	if (iNameLen > X51_MAX_STR_MOD) return X51LNK_ERR_MNTLNG;
	if (pl->mod_cap < (pl->mod_cnt + 1))
	{
		if (pl->mod_byval) pl->mod_byval = (SStrInt*)realloc(pl->mod_byval, (pl->mod_cap + pl->mod_chunk) * sizeof(SStrInt));
		else pl->mod_byval = (SStrInt*)malloc((pl->mod_cap + pl->mod_chunk) * sizeof(SStrInt));
		if (pl->mod_bystr) pl->mod_bystr = (SStrInt*)realloc(pl->mod_bystr, (pl->mod_cap + pl->mod_chunk) * sizeof(SStrInt));
		else pl->mod_bystr = (SStrInt*)malloc((pl->mod_cap + pl->mod_chunk) * sizeof(SStrInt));
		if (pl->pmod) pl->pmod = (SX51Mod**)realloc(pl->pmod, (pl->mod_cap + pl->mod_chunk) * sizeof(SX51Mod*));
		else pl->pmod = (SX51Mod**)malloc((pl->mod_cap + pl->mod_chunk) * sizeof(SX51Mod*));
		if (!pl->mod_byval || !pl->mod_bystr || !pl->pmod) X51_ERR_MALLOC;
		pl->mod_cap += pl->mod_chunk;
//		for (int i = 0; i < pl->mod_cnt; i++)
//			pl->mod_byval[i].pc = pl->mod[i].name;
	}
	SX51Mod* pm = X51Mod_Init(cName);
	pl->pmod[pl->mod_cnt] = pm;
	pl->mod_byval[pl->mod_cnt].pc = pm->name;
	pl->mod_byval[pl->mod_cnt].i = pl->mod_cnt;
	return pl->mod_cnt++;
}

SX51Mod* X51Lnk_FindModByName(SX51Lnk* pl, char* pcName)
{
	SStrInt si = {pcName, 0};
	SStrInt* psi = (SStrInt*)bsearch(&si, pl->mod_bystr, pl->mod_cnt, sizeof(SStrInt), &compare_str);
	return psi?(pl->pmod[psi->i]):0;
}

void X51Lnk_Sort(SX51Lnk* pl)
{
	memcpy(pl->mod_bystr, pl->mod_byval, pl->mod_cnt * sizeof(SStrInt));
	qsort(pl->mod_bystr, pl->mod_cnt, sizeof(SStrInt), &compare_str);
	for (int i = 0; i < pl->mod_cnt; i++)
		X51Mod_Sort(pl->pmod[i]);
}

void X51Lnk_PrintMods(SX51Lnk* pl, int iByName)
{
	for (int i = 0; i < pl->mod_cnt; i++)
		if (iByName)
			printf("%02d %s\n", pl->mod_bystr[i].i, pl->mod_bystr[i].pc);
		else
			printf("%02d %s\n", pl->mod_byval[i].i, pl->mod_byval[i].pc);
}

int X51Lnk_LoadM51(SX51Lnk* pl, char* pcFileName, int iFlags)
{
	int iRet = 0; //return value
	int c = 0, n = 0; //variables for scanf
	char cLine[X51_MAX_STR_M51LINE + 2] = {0}; //line buffer
	FILE* pFile = fopen(pcFileName, "r"); //open the file
	if (!pFile) return X51_ERR_FILEIO; //Error "File I/O error"
//	strcpy(pAsm->filename[pAsm->depth], pcFileName); //copy current filename to asm context
	SX51Mod* pm = 0; //module pointer initialized by "MODULE" keyword
	int linenum = 0; //reset line counter
	while (!feof(pFile)) //loop until end of file
	{
		//parse line with scan string g_cLScan (generated in X51_Init)
		if ((c = fscanf(pFile, g_cM51LScan, cLine, &n)) < 0)
		{
			//not eof means other io error, eof is ok
			if (!feof(pFile)) iRet = X51_ERR_FILEIO; //Error "File I/O error"
			break;
		}
		//check line length
		if (n > X51_MAX_STR_M51LINE)
		{
			iRet = X51LNK_ERR_LNTLNG; //Error "Line too long"
			break;
		}
		//process line
		if (c > 0)
		{
			char cVType = 0;
			char cVal[9] = {0};
			char cType[16] = {0};
			char cName[33] = {0};
			if (cLine[2] == '-')
			{
				c = sscanf(cLine, "  -------%*[ \t]%6[A-Z]%*[ \t]%1[A-Z]%32[0-9A-Z_]", cType, cName, cName + 1);
				if (c == 3)
				{
					if (strcmp(cType, "MODULE") == 0)
					{
						//pm = X51Lnk_FindMod(cName);
						if (pm == 0)
						{
							int iMod = X51Lnk_AddMod(pl, cName);
//							printf("MODULE %d %s\n", iMod, cName);
							pm = pl->pmod[iMod];
						}
						else
						{
							printf("error - not closed module %s\n", cName);
							break;
						}
					}
					else if (strcmp(cType, "ENDMOD") == 0)
					{
						if (pm != 0)
						{
//							printf("ENDMOD %s\n", pm->name);
							pm = 0;
						}
						else
						{
							printf("error - not opened module\n");
							break;
						}
					}
				}
			}
			else
			{
				c = sscanf(cLine, "  %c:%7[--0-9A-FH.]%*[ \t]%15[#A-Z]%*[ \t]%32[0-9A-Z_]", &cVType, cVal, cType, cName);
				if (c == 4)
				{
					//parse the value type
					int iVType = 0;
					switch (cVType)
					{
					case 'B': iVType = X51MOD_VTYP_B; break;
					case 'C': iVType = X51MOD_VTYP_C; break;
					case 'D': iVType = X51MOD_VTYP_D; break;
					case 'N': iVType = X51MOD_VTYP_N; break;
					}
					if (iVType == 0)
					{
						printf("error - invalid value type\n");
						break;
					}
					//parse value
					int iAddr = 0;
					int iBit = 0;
					c = sscanf(cVal, "%4XH.%d", &iAddr, &iBit);
					int iVal = 0;
					if (c == 1)
						iVal = iAddr;
					else if (c == 2)
					{
//						printf("!BIT %02x.%d\n", iAddr, iBit);
						if ((iBit < 0) || (iBit > 7))
						{
							printf("error - invalid bit index\n");
							break;
						}
						if ((iAddr >= 0x20) && (iAddr <= 0x2f))
							iVal = (((iAddr - 0x20) << 3) | iBit);
						else if ((iAddr >= 0x80) && (iAddr <= 0xff) && ((iAddr & 7) == 0))
							iVal = (iAddr | iBit);
						else
						{
							printf("error - invalid bit address\n");
							break;
						}
					}
					else
					{
						printf("error - invalid format\n");
						break;
					}
					//parse symbol type
					int iSType = 0;
					switch (cType[0])
					{
					case 'L':
						if (strcmp(cType + 1, "INE#") == 0)
							iSType = X51MOD_STYP_LIN;
						break;
					case 'P':
						if (strcmp(cType + 1, "UBLIC") == 0)
							iSType = X51MOD_STYP_PUB;
						break;
					case 'S':
						switch (cType[1]) {
						case 'E':
							if (strcmp(cType + 2, "GMENT") == 0)
								iSType = X51MOD_STYP_SEG;
							break;
						case 'Y':
							if (strcmp(cType + 2, "MBOL") == 0)
								iSType = X51MOD_STYP_SYM;
							break;
						} break;
					}
					if (iSType == 0)
					{
						printf("error - invalid symbol type\n");
						break;
					}
					int iType = (iSType << 4) | iVType;
					iVal |= iType << 16;
					if (pm != 0)
					{
						int iAdd = 1;
						if (iFlags & X51LNK_FLGM51_EXCSFR)
							if ((iVType == X51MOD_VTYP_D) || (iVType == X51MOD_VTYP_B))
								if (iAddr >= X51_MIN_SFR)
									iAdd = 0;
						if (iFlags & X51LNK_FLGM51_EXCLIN)
							if (iSType == X51MOD_STYP_LIN)
								iAdd = 0;
						if (iFlags & X51LNK_FLGM51_EXCNUM)
							if (iVType == X51MOD_VTYP_N)
								iAdd = 0;
						if (iAdd)
						{
//							printf("%c%d %04x %s%d %s\n", cVType, iVType, iVal, cType, iSType, cName);
//							if (iVType == X51LNK_VTYP_B)
							X51Mod_AddSym(pm, cName, iVal);
						}
					}
					else
					{
						printf("error - not opened module\n");
						break;
					}
				}
			}
		}
		linenum++; //increment line number counter
		//skip one CR and one LF char (or CRLF sequence)
		if ((c = fscanf(pFile, "%1*[\r]")) < 0) //skip CR
		{
			//not eof means other io error, eof is ok
			if (!feof(pFile)) iRet = X51_ERR_FILEIO; //Error "File I/O error"
			break;
		}
		if ((c = fscanf(pFile, "%1*[\n]")) < 0) //skip LF
		{
			//not eof means other io error, eof is ok
			if (!feof(pFile)) iRet = X51_ERR_FILEIO; //Error "File I/O error"
			break;
		}
	}
	fclose(pFile); //close the file
	return iRet;
}