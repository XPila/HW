#include <stdio.h>
#include <stdlib.h>
#include "X51cmn.h"
#include "X51das.h"
#include "X51mod.h"
#include "X51exp.h"
#include "X51asm.h"
#include "X51lnk.h"
#include "X51sim.h"
#include "X51-AT89LPx052.h"
#include "../Hex/Hex.h"
#include <sys/time.h>
#include "../../getch/getch.h"
#include "errno.h"



int SimpleAsm(FILE* pFile, unsigned char* pucCode, int iAddr);


int main(int argc, char** argv)
{
/*	char cExp[] = "ahoj + bhoj * 20";
	int iRes = 0;
	if (X51Exp_Eval(cExp, strlen(cExp), &iRes) == 0)
		printf("%s=%d %04x\n", cExp, iRes, iRes);
	else
		printf("ERROR\n");
	return 0;*/
	
	SX51Lnk* pl = X51Lnk_Init();
	
	char* pcM51FileName = "/mnt/internal_sd/projects/android/c4droid/hw/x51/XProgISP.m51";
	X51Lnk_LoadM51(pl, pcM51FileName, 0);//X51LNK_FLGM51_EXCSFR | X51LNK_FLGM51_EXCLIN | X51LNK_FLGM51_EXCNUM);
	X51Lnk_Sort(pl);
	X51Lnk_PrintMods(pl, 0);
	SX51Mod* pm = X51Lnk_FindModByName(pl, "MAIN");
	printf("%s\n", pm?pm->name:"Null");
//	SStrInt* psi = X51Lnk_FindSymByName(pm, "MAIN_C");
//	printf("%s %06x\n", psi?psi->pc:"NG", psi?psi->i:-1);
//	X51Lnk_PrintSyms(pm, 1);
	X51Lnk_AddMod(pl, "$GLOBAL");
	SX51Mod* pmg = pl->pmod[pl->mod_cnt - 1];
	printf("%s\n", pm?pm->name:"Null");
	for (int i = 0; i < pl->mod_cnt - 1; i++)
	{
		pm = pl->pmod[i];
		for (int j = 0; j < pm->sym_cnt; j++)
		{
			SStrInt* psi = pm->sym_byval + j;
			X51Mod_AddSym(pmg, psi->pc, psi->i);
		}
	}
	X51Mod_Sort(pmg);
	X51Mod_PrintSyms(pmg, 1);
//	SStrInt* psi = X51Mod_FindSymByName(pmg, "MAIN_B_ADDR");
//	SStrInt* psi = X51Mod_FindSymByVal(pmg, 0x210173);
//	printf("%s %06x\n", psi?psi->pc:"NG", psi?psi->i:-1);
	
	
	
//	return 0;
	printf("\033[2J"); //clear
	printf("\033[0;0H"); //goto 0 0
	printf("\033[1;34m"); //blue
	printf("X51 disassembler/simulator\n");
	printf("\033[1;0m"); //no color
	int iRet =  X51_Init();
	if (iRet != 0)
	{
		if (errno) perror("init error");
		else printf("init error");
		return -1;
	}
	printf("init: OK\n");

	X51_AT89LPx052_Init();
	
	SX51Sim* ps = X51Sim_Init(X51_CTYP_AT89LP2052, 20000000);
	if (!ps)
	{
		if (errno) perror("init sim error");
		else printf("init sim error");
		return -2;
	}
	printf("init sim AT89LP2052: OK\n");
	
	char* pcHexFileName = "/mnt/internal_sd/projects/android/c4droid/hw/x51/XProgISP.hex";
	iRet = X51Sim_LoadHex(ps, pcHexFileName);
	if (iRet != 0)
	{
		if (errno) perror("load hex error");
		else printf("load hex error");
		return -3;
	}
	printf("load hex: OK\n");

	
//	X51Das_ToFile(stdout, X51_DAS_FLG_DISASM, ps->Code, 0, 2047, 1000, &X51_AT89LPx052_SFRValue2Name, 0);
	X51Das_ToFile(stdout, X51_DAS_FLG_DISASM, ps->Code, 0, 2047, 1000, &X51Mod_Value2Name, pmg);
	
	X51Sim_Reset(ps);
	X51Sim_Step(ps, 1000);
	X51Sim_Rx(ps, 0x5a);
	X51Sim_Step(ps, 100);
	X51Sim_Tx(ps);
	X51Sim_Print(ps, stdout, &X51Mod_Value2Name, pmg);
	int i = 0;
	while (i = getch())
	{
		X51Sim_Step(ps, 1);
		X51Sim_Print(ps, stdout, &X51Mod_Value2Name, pmg);
	}

	X51Sim_Done(ps);

	X51_Done();
	printf("X51 done\n");
	
	return 0;
	
}

int SimpleAsm(FILE* pFile, unsigned char* pucCode, int iAddr)
{
	int c, n;
	int iLine;
	char cLine[X51_MAX_STR_LINE + 2];
	char cScan[16];
	int iCnt = 0;
	sprintf(cScan, "%%%d[^\r\n]%%n\0", X51_MAX_STR_LINE + 1);
	while ((c = fscanf(pFile, cScan, cLine, &n)) > 0)
	{
		if (n > X51_MAX_STR_LINE) return X51_ASM_ERR_LNTLNG;
		printf("%s %d %d\n", cLine, c, n);
		char cTok0[X51_MAX_STR_TOKEN + 1] = {0};
		char* pc = cLine;
		n = 0; c = sscanf(pc, "%*[ \t]%n", &n); pc += n; //skip white space
		n = 0; c = sscanf(pc, "%16[A-Z]%n", cTok0, &n); pc += n; //parse first token
		if (c == 1) //first token parsed
		{
			if (strcmp(cTok0, "ORG") == 0) //="ORG"
			{
				n = 0; c = sscanf(pc, "%*[ \t]%n", &n); pc += n;//skip white space
				n = 0; c = sscanf(pc, "$%x%n", &iAddr, &n); pc += n; //parse address
				if (c == 1) //address parsed
				{
					fscanf(pFile, "%*[\r\n]%n", &n);
					continue;
				}
			}
		}
		int iAsm = 0;
		int iRes = X51Asm_Ins(0, pucCode, iAddr, cLine, &iAsm, 0, 0);
		if (iRes > 0)
		{
			iCnt += iRes;
			iAddr += iRes;
			printf("Asm: OK %d bytes\n", iRes);
		}
		else
			printf("Asm: %s\n", X51Asm_ErrStr(iRes));
		fscanf(pFile, "%*[\r\n]%n", &n);
	}
	printf("%s %d %d\n", cLine, c, n);
	return iCnt;
}

int LoadSym(char* pcFileName)
{
	printf("loading symbol file %s: ", pcFileName);
	FILE* f = fopen(pcFileName, "ro");
	if (!f)
	{
		perror("ERR %d\n");
		return -1;
	}

	char cLine[1024];
//	for (int i = 0; i < 100; i ++)
//	{
	while (fgets(cLine, 1024, f))
	{
		char cType;
//		int iAddr;
		char cVal[64];
		char cStr1[64];
		char cName[64];
		if (sscanf(cLine, "  %c:%s%s%s", &cType, &cVal, &cStr1, &cName) == 4)
		{
//			if (cType == 'B')
			{
				printf("%c:$%8-s %16-s %s\n", cType, cVal, cStr1, cName);
//				int iAddr, int iBit;
//				if (sscanf(cStr0, "  %c:%s%s%s", &cType, &cStr0, &cStr1, &cStr2) == 4)
//				if ((strcmp(cStr1, "PUBLIC") == 0) ||
//					(strcmp(cStr1, "SYMBOL") == 0))
				{
//					X51_SymTab_Add(&sSymTab_IDataByVal, cStr2, iAddr);
//					printf("%02x %s\n", iAddr, cStr2);
				}
			}
		}
//		if (sscanf(cLine, "%c:%04xH %s %s") == 4)
//B:0020H.0       SEGMENT       SYSTEM_B
//C:06A3H         PUBLIC        SYSTEM_COLDRESET	
	}
	fclose(f);
	printf("OK\n");
	return 0;
}


/*
- Position the Cursor:
  \033[<L>;<C>H
     Or
  \033[<L>;<C>f
  puts the cursor at line L and column C.
- Move the cursor up N lines:
  \033[<N>A
- Move the cursor down N lines:
  \033[<N>B
- Move the cursor forward N columns:
  \033[<N>C
- Move the cursor backward N columns:
  \033[<N>D

- Clear the screen, move to (0,0):
  \033[2J
- Erase to end of line:
  \033[K

- Save cursor position:
  \033[s
- Restore cursor position:
  \033[u*/