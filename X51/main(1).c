#include <stdio.h>
#include <stdlib.h>
#include "X51cmn.h"
#include "X51das.h"
#include "X51asm.h"
#include "X51sim.h"
#include "X51-AT89LPx052.h"
#include "../Hex/Hex.h"
#include <sys/time.h>
#include "../../getch/getch.h"

int LoadHex(char* pcFileName);
int LoadSym(char* pcFileName);

extern SX51Ins* g_psInsTab;

unsigned char ucCode[2048];

int SimpleAsm(FILE* pFile, unsigned char* pucCode, int iAddr);

int Gen(unsigned char* pucCode)
{
	unsigned char* puc = pucCode;
	for (int i = 0; i < 256; i++)
	{
		*(puc++) = i;
		puc += (g_psInsTab[i].len - 1);
	}
	return pucCode - puc + 1;
}

int TestIns(SX51Sim* ps,  unsigned char code)
{
	X51Sim_Reset(ps);
	ps->Code[0] = code;
	return X51Sim_Step(ps, 1);
}

int TestAllIns(SX51Sim* ps)
{
	for (int i = 0; i < 256; i++)
	{
		SX51Ins* pi = g_psInsTab + i;
//		if (pi->ins == X51_INS_ORL)// && (pi->op1 == X51_OP_R0))
		{
			if (TestIns(ps, i) < 0)
			{
				printf("INS: %02x (c=%d) %s %s %s %s", i, pi->cyc_sc, X51Das_InsStr(pi->ins), X51Das_OpStr(pi->op0), X51Das_OpStr(pi->op1), X51Das_OpStr(pi->op2));
//				printf("!Unhandled instruction!");
				printf("\n");
			}
		}
	}
}

double Time()
{
	struct timespec sTime;
	clock_gettime(0, &sTime);
	return 1000 * sTime.tv_sec + (double)sTime.tv_nsec / 1000000;
}

unsigned long long time_ns()
{
	struct timespec sTime;
	clock_gettime(0, &sTime);
	return 1000000000 * sTime.tv_sec + sTime.tv_nsec;
}

int main(int argc, char** argv)
{
/*	X51_AT89LPx052_Init();
	printf("%d", X51_AT89LPx052_SFRName2Value(0, 'B', "RI"));
	printf("%s", X51_AT89LPx052_SFRValue2Name(0, 'B', X51_SFB_RI));
	return 0;/**/

/*	int iToken = X51_Asm_Str2Tok(0, "$INCLUDE");
	int iDir = iToken & 0xff; 
	printf("%s\n", X51_Asm_TkTStr(iToken >> 8));
	return 0;
/*	int c, n; char cb[100];
	c = sscanf("1231321", "%[0-9^3]", cb);
	printf("%s", cb); return 0;*/
//	printf("%%%d[^\\r\\n]%%n", X51_MAX_STR_LINE + 1);
//	return 0;
	printf("X51 disassembler/simulator\n");
	if (X51_Init() != 0)
	{
		printf("X51 init error\n");
		perror("");
		return -1;
	}
	printf("X51 init: OK\n");
	X51_AT89LPx052_Init();
/*
	memset(ucCode, 0, 2048);
	int ic = Gen(ucCode);
//	char* pcFileNameAsm1 = "/mnt/sdcard/external_sd/projects/android/c4droid/hw/x51/test.asm";
	char* pcFileNameAsm1 = "/mnt/internal_sd/projects/android/c4droid/hw/x51/all.asm";
	FILE* f1 = fopen(pcFileNameAsm1, "w+");
	X51Das_ToFile(f1, 0, ucCode, 0, 2048, 256, 0, 0);
	fclose(f1);
	return 0;
*/
	
//	char* pcFileNameAsm = "/mnt/sdcard/external_sd/projects/android/c4droid/hw/x51/test.asm";
	char* pcFileNameAsm = "/mnt/internal_sd/projects/android/c4droid/hw/x51/test.asm";
	FILE* f = fopen(pcFileNameAsm, "r");
//	int iCnt = SimpleAsm(f, ucCode, 0);
	fclose(f);
/*	char* pcHex = HexText(ucCode, iCnt);
	printf("HEX: %s\n", pcHex);
	free(pcHex);
//	X51Das_ToFile(stdout, X51_DAS_FLG_DISASM, ucCode, 0, 10, 10, 0, 0);*/
	

	
	SX51Sim* ps X51Sim_Init("AT89LP2052");
	memcpy(sSim.Code, ucCode, 2048);


	printf("\n");

//	TestAllIns(&sSim);

	printf("\n");
	
//	X51Sim_Print(&sSim, stdout);
/*
	printf("PSW=%02x\n", X51Sim_Rd(&sSim, 'D', X51_SFR_PSW));
	printf("CY=%d\n", X51Sim_Rd(&sSim, 'B', X51_SFB_CY));
	X51Sim_Wr(&sSim, 'B', X51_SFB_CY, 1);
	printf("PSW=%02x\n", X51Sim_Rd(&sSim, 'D', X51_SFR_PSW));
	printf("CY=%d\n", X51Sim_Rd(&sSim, 'B', X51_SFB_CY));
	X51Sim_Wr(&sSim, 'B', X51_SFB_CY, 0);
	printf("PSW=%02x\n", X51Sim_Rd(&sSim, 'D', X51_SFR_PSW));
	printf("CY=%d\n", X51Sim_Rd(&sSim, 'B', X51_SFB_CY));
	X51Sim_Wr(&sSim, 'B', X51_SFB_CY, 1);
	printf("PSW=%02x\n", X51Sim_Rd(&sSim, 'D', X51_SFR_PSW));
	printf("CY=%d\n", X51Sim_Rd(&sSim, 'B', X51_SFB_CY));
	
	char* pcHex = HexText(sSim.SFR, 128);
	printf("HEX:\n%s", pcHex);
	free(pcHex);
*/	
//	char* pcFileNameSym = "/mnt/internal_sd/projects/android/c4droid/hw/x51/XProgISP.m51";
	char* pcFileNameHex = "/mnt/internal_sd/projects/android/c4droid/hw/x51/XProgISP.hex";

//	char* pcFileNameSym = "/mnt/sdcard/external_sd/projects/android/c4droid/hw/x51/XProgISP.m51";
//	char* pcFileNameHex = "/mnt/sdcard/external_sd/projects/android/c4droid/hw/x51/XProgISP.hex";

//	if (LoadSym(pcFileNameSym)) return -1;
/*	if (LoadHex(pcFileNameHex)) return -1;
	memcpy(sSim.Code, ucCode, 2048);
	
	double dT1 = Time();
	int time_fac = 1000000000 / sSim.freq;
	time_fac *= 10;
	int time_dif = 60000;
	int chunk = 200;
	int count = 200000;
*/
/*	for (int i = 0; i < count / chunk; i++)
	{
//		printf("\nticks=%lld time=%0.9lf\n", sSim.tick, (double)sSim.tick/sSim.freq);
//		X51Sim_Print(&sSim, stdout);
		unsigned long long tr0_ns = time_ns();
		unsigned long long ts0 = sSim.tick;
		int c = X51Sim_Step(&sSim, chunk);
		unsigned long long tr1_ns = time_ns();
		unsigned long long ts1 = sSim.tick;
		unsigned long long dts = ts1 - ts0;
		unsigned long long dtr_ns = tr1_ns - tr0_ns;
		unsigned long long dts_ns = dts * time_fac;
		if (dts_ns > (dtr_ns + time_dif)) usleep((dts_ns - dtr_ns - time_dif) / 1000);
	}
	double dT2 = Time();
	double dT = dT2 - dT1;
	
	printf("\nticks=%lld time=%0.9lf timer=%0.9lf\n", sSim.tick, 1000*(double)sSim.tick/sSim.freq, dT);
	X51Sim_Print(&sSim, stdout);

//	X51Sim_Int(&sSim, X51_INTV_S);
	X51Sim_Rx(&sSim, 0xa5);
	int c = X51Sim_Step(&sSim, 6);
	X51Sim_Print(&sSim, stdout);
	while (1)
	{
		getch();
		int c = X51Sim_Step(&sSim, 1);
		X51Sim_Print(&sSim, stdout);
	}
*/
	
	X51Sim_Done(&sSim);
	
/*	
//	SX51Asm* pAsm = X51_Asm_Init("/mnt/internal_sd/projects/android/c4droid/hw/x51/cpp/MAIN.a51");
	SX51Asm* pAsm = X51Asm_Init("/mnt/internal_sd/projects/android/c4droid/hw/x51/cpp/test.asm");
	if (!pAsm)
	{
		printf("ASM init error\n");
		perror("");
		return -1;
	}
	printf("ASM init asm: OK\n");
	int iRet = X51Asm_Make(pAsm, stdout);
	if (iRet == X51_ASM_ERR_FILEIO)
	{
		printf("ASM make error\n");
		perror("");
	}
	else if (iRet != 0)
	{
		printf("ASM make error %s\n", X51Asm_ErrStr(iRet));
		char* pcFileName = 0;
		char* pcLine = 0;
		char* pcToken = 0;
		int iLine = 0;
		int iPos = 0;
		X51Asm_GetErr(pAsm, &pcFileName, &pcLine, &pcToken, &iLine, &iPos);
		printf("line %d, pos %d, token '%s'\n%s\n", iLine, iPos, pcToken, pcLine);
		
	}
	X51Asm_Done(pAsm);
	printf("ASM done\n");
/**/	

//	unsigned char* pucCode = (unsigned char*)&cCodeData;
//	unsigned short usAddr = 0x0038;
//	Asm(stdin, pucCode, usAddr);

	
//	FILE* f = fopen("/mnt/internal_sd/projects/android/c4droid/hw/x51/table.c", "w+");
	

	X51_Done();
	printf("X51 done\n");
	
	return 0;
	
}
#define X51_MAX_LINE 255

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

int LoadHex(char* pcFileName)
{
	printf("loading hex file %s: ", pcFileName);
	int iBlockCnt = 0;
	unsigned short* pusBlockAddr = 0;
	unsigned short* pusBlockSize = 0;
	unsigned char** ppucBlockData = 0;
	unsigned short usStartAddr = -1;
	int iResHex = HexIntelLoadFromFile(pcFileName, &iBlockCnt, &pusBlockAddr, &pusBlockSize, &ppucBlockData, &usStartAddr);
//	if (iResHex == HEX_RES_ERR) exiterror();
	if (iResHex != HEX_RES_OK)
	{
		printf("ERR %d\n", iResHex);
		return -1;
	}
	printf("OK\n");
	memset(ucCode, 0xff, 2048);
	int iSize = HexBlocksToMemory(iBlockCnt, pusBlockAddr, pusBlockSize, ppucBlockData, ucCode);
	HexBlocksFree(iBlockCnt, pusBlockAddr, pusBlockSize, ppucBlockData);
//	printf(HexText(cCodeData, 2048));
	return 0;
}