////////////////////////////////////////////////////////////////////////////////
// X51-AT89LPx052.c
// source file for chip family AT89LPx052

#include "X51-AT89LPx052.h"
#include "X51cmn.h"
#include "X51sim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define SFRDTabCnt (37)
SStrInt g_sSFRDTabByVal[SFRDTabCnt] =
{
	{"SP",0x81},
	{"DPL",0x82},
	{"DPH",0x83},
	{"SPDR",0x86},
	{"PCON",0x87},
	{"TCON",0x88},
	{"TMOD",0x89},
	{"TL0",0x8a},
	{"TL1",0x8b},
	{"TH0",0x8c},
	{"TH1",0x8d},
	{"P1",0x90},
	{"TCONB",0x91},
	{"RL0",0x92},
	{"RL1",0x93},
	{"RH0",0x94},
	{"RH1",0x95},
	{"ACSR",0x97},
	{"SCON",0x98},
	{"SBUF",0x99},
	{"WDTRST",0xa6},
	{"WDTCON",0xa7},
	{"IE",0xa8},
	{"SADDR",0xa9},
	{"SPSR",0xaa},
	{"P3",0xb0},
	{"IPH",0xb7},
	{"IP",0xb8},
	{"SADEN",0xb9},
	{"P1M0",0xc2},
	{"P1M1",0xc3},
	{"P3M0",0xc6},
	{"P3M1",0xc7},
	{"PSW",0xd0},
	{"SPCR",0xd5},
	{"ACC",0xe0},
	{"B",0xf0}
};
SStrInt g_sSFRDTabByStr[SFRDTabCnt] = {};

#define SFRBTabCnt (8+8+7+6+7)
SStrInt g_sSFRBTabByVal[SFRBTabCnt] =
{
	//TCON
	{"IT0",0x88},
	{"IE0",0x89},
	{"IT1",0x8a},
	{"IE1",0x8b},
	{"TR0",0x8c},
	{"TF0",0x8d},
	{"TR1",0x8e},
	{"TF1",0x8f},
	//SCON
	{"RI",0x98},
	{"TI",0x99},
	{"RB8",0x9a},
	{"TB8",0x9b},
	{"REN",0x9c},
	{"SM2",0x9d},
	{"SM1",0x9e},
	{"SM0",0x9f},
	//IE
	{"EX0",0xa8},
	{"ET0",0xa9},
	{"EX1",0xaa},
	{"ET1",0xab},
	{"ES",0xac},
	{"ET2",0xad},
	{"EA",0xaf},
	//IP
	{"PX0",0xb8},
	{"PT0",0xb9},
	{"PX1",0xba},
	{"PT1",0xbb},
	{"PS",0xbc},
	{"PC",0xbe},
	//PSW
	{"P",0xd0},
	{"OV",0xd2},
	{"RS0",0xd3},
	{"RS1",0xd4},
	{"F0",0xd5},
	{"AC",0xd6},
	{"CY",0xd7},
};
SStrInt g_sSFRBTabByStr[SFRBTabCnt] = {};

int X51_AT89LPx052_Init()
{
	memcpy(&g_sSFRDTabByStr, &g_sSFRDTabByVal, sizeof(g_sSFRDTabByVal));
	qsort(&g_sSFRDTabByStr, SFRDTabCnt, sizeof(char*) + sizeof(int), &compare_str);
}

void X51_AT89LPx052_Done()
{
}

char* X51_AT89LPx052_SFRValue2Name(void* pParam, char cType, int iVal)
{
	SStrInt sVal = {0, iVal};
	if ((cType == 'D') && (iVal >= 0x80) && (iVal <= 0xff))
	{
		SStrInt* psVal = (SStrInt*)bsearch(&sVal, &g_sSFRDTabByVal, SFRDTabCnt, sizeof(SStrInt), &compare_val);
		return psVal?psVal->pc:0;
	}
	if ((cType == 'B') && (iVal >= 0x80) && (iVal <= 0xff))
	{
		SStrInt* psVal = (SStrInt*)bsearch(&sVal, &g_sSFRBTabByVal, SFRBTabCnt, sizeof(SStrInt), &compare_val);
		return psVal?psVal->pc:0;
	}
	return 0;
}

int X51_AT89LPx052_SFRName2Value(void* pParam, char cType, char* pcName)
{
	SStrInt sVal = {pcName, 0};
	if (cType == 'D')
	{
		SStrInt* psVal = (SStrInt*)bsearch(&sVal, &g_sSFRDTabByStr, SFRDTabCnt, sizeof(SStrInt), &compare_str);
		return psVal?psVal->i:-1;
	}
	if (cType == 'B')
	{
		SStrInt* psVal = (SStrInt*)bsearch(&sVal, &g_sSFRBTabByStr, SFRBTabCnt, sizeof(SStrInt), &compare_str);
		return psVal?psVal->i:-1;
	}
	return -1;
}

void* X51_AT89LPx052_SimInit(char cChipType, int iFreq)
{
	if ((cChipType < 1) || (cChipType > 3)) return 0;
	int iCodeSize = 0;
	switch (cChipType)
	{
	case 1: iCodeSize = 1024; break; //1kb - AT89LP1052
	case 2: iCodeSize = 2048; break; //2kb - AT89LP2052
	case 3: iCodeSize = 4096; break; //4kb - AT89LP4052
	}
	int iIDataSize = 256;
	int iSFRSize = 128;
	int iTotalSize = sizeof(SX51Sim) + iCodeSize + iIDataSize + iSFRSize;
	SX51Sim* ps = (SX51Sim*)malloc(iTotalSize);
	if (!ps) return 0;
	ps->core = 1; //core type (0=X1, 1=SC)
	ps->freq = iFreq; //Xtal frequency in Hz
	ps->PC = 0x0000; //PC - program counter
	ps->Code = ((unsigned char*)ps) + sizeof(SX51Sim); //Code memory pointer
	ps->IData = ps->Code + iCodeSize; //IData memory pointer
	ps->XData = 0; //XData memory pointer
	ps->SFR = ps->IData + iIDataSize; //SFR space pointer
	ps->code_msk = iCodeSize - 1; //Code address mask (e.g. 0x3ff for 1kb, 0x7ff for 2kb, etc)
	ps->idata_msk = iIDataSize - 1; //IData address mask (0x7f for 128byte or 0xff for 256 byte)
	ps->xdata_msk = 0x0000; //XData address mask
	ps->sfr_msk = iSFRSize - 1; //SFR address mask (always 0x7f)
	ps->code_sz = iCodeSize; //Code memory size (in bytes)
	ps->idata_sz = iIDataSize; //IData memory size (128 or 256)
	ps->xdata_sz = 0; //XData memory size (in bytes)
	ps->sfr_sz = iSFRSize; //SFR space size (always 128)
	ps->rdSFR = 0; //SFR read function
	ps->wrSFR = 0; //SFR write function
	ps->tick = 0; //Xtal tick (from reset)
	ps->cycle = 0; //instruction cycle (0-3)
	ps->stop = 0; //stop flag
	ps->pi = 0; //instruction structure pointer (updated in cycle0)
	ps->code_q = 0; //code queue - contain last four values readed from code memory
	return ps;	
}
