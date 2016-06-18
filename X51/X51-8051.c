////////////////////////////////////////////////////////////////////////////////
// X51-8051.c
// source file for chip 8051

#include "X51-8051.h"
#include "X51cmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//Structure for string-int pair
typedef struct SStrInt {char* pc; int i;} SStrInt;

int compare_val(const void* p1, const void* p2);
int compare_str(const void* p1, const void* p2);

#define SFRDTabCnt (29)
SStrInt g_sSFRDTabByVal[SFRDTabCnt] =
{
	{"P0",0x80},
	{"SP",0x81},
	{"DPL",0x82},
	{"DPH",0x83},
	{"PCON",0x87},
	{"TCON",0x88},
	{"TMOD",0x89},
	{"TL0",0x8a},
	{"TL1",0x8b},
	{"TH0",0x8c},
	{"TH1",0x8d},
	{"AUXR",0x8e},
	{"CKCON",0x8f},
	{"P1",0x90},
	{"SCON",0x98},
	{"SBUF",0x99},
	{"P2",0xa0},
	{"IE",0xa8},
	{"P3",0xb0},
	{"IP",0xb8},
	{"T2CON",0xc8},
	{"T2MOD",0xc9},
	{"RCAP2L",0xca},
	{"RCAP2H",0xcb},
	{"TL2",0xcc},
	{"TH2",0xcd},
	{"PSW",0xd0},
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

int X51_8051_Init()
{
	memcpy(&g_sSFRDTabByStr, &g_sSFRDTabByVal, sizeof(g_sSFRDTabByVal));
	qsort(&g_sSFRDTabByStr, SFRDTabCnt, sizeof(char*) + sizeof(int), &compare_str);
}

void X51_8051_Done()
{
}

char* X51_8051_SFRValue2Name(void* pParam, char cType, int iVal)
{
	SStrInt sVal = {0, iVal};
	if ((cType = 'D') && (iVal >= 0x80) && (iVal <= 0xff))
	{
		SStrInt* psVal = (SStrInt*)bsearch(&sVal, &g_sSFRDTabByVal, SFRDTabCnt, sizeof(SStrInt), &compare_val);
		return psVal?psVal->pc:0;
	}
	if ((cType = 'B') && (iVal >= 0x80) && (iVal <= 0xff))
	{
		SStrInt* psVal = (SStrInt*)bsearch(&sVal, &g_sSFRBTabByVal, SFRBTabCnt, sizeof(SStrInt), &compare_val);
		return psVal?psVal->pc:0;
	}
	return 0;
}