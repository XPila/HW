////////////////////////////////////////////////////////////////////////////////
// X51sim.c
// X51 simulator source file


#include "X51sim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "X51-AT89LPx052.h"
#include "X51das.h"


extern SX51Ins* g_psInsTab;

typedef int (FX51SimC)(SX51Sim* ps);

#define X51_MAX_CYCLE 3

int X51Sim_X1_0(SX51Sim* ps);
int X51Sim_X1_1(SX51Sim* ps);
int X51Sim_X1_2(SX51Sim* ps);
int X51Sim_X1_3(SX51Sim* ps);

int X51Sim_SC_0(SX51Sim* ps);
int X51Sim_SC_1(SX51Sim* ps);
int X51Sim_SC_2(SX51Sim* ps);
int X51Sim_SC_3(SX51Sim* ps);

FX51SimC* g_pFCycleX1[X51_MAX_CYCLE + 1] = {&X51Sim_X1_0, &X51Sim_X1_1, &X51Sim_X1_2, &X51Sim_X1_3};
FX51SimC* g_pFCycleSC[X51_MAX_CYCLE + 1] = {&X51Sim_SC_0, &X51Sim_SC_1, &X51Sim_SC_2, &X51Sim_SC_3};

//structure for bit memory access
typedef struct SX51Byte
{
	char b0:1;
	char b1:1;
	char b2:1;
	char b3:1;
	char b4:1;
	char b5:1;
	char b6:1;
	char b7:1;
} SX51Byte;

unsigned char X51Sim_Rd(SX51Sim* ps, char t, unsigned short a);
void X51Sim_Wr(SX51Sim* ps, char t, unsigned short a, unsigned char v);

//macros for smart access and more readability of the code
#define _CODE(a)    (ps->Code[a])
#define _IDATA(a)   (ps->IData[a])
#define _XDATA(a)   (ps->XData[a])
#define _SFR(a)     (ps->SFR[X51_MIN_SFR + a])
#define _SBIT0(a)   (((SX51Byte*)(ps->SFR + a))->b0)
#define _SBIT1(a)   (((SX51Byte*)(ps->SFR + a))->b1)
#define _SBIT2(a)   (((SX51Byte*)(ps->SFR + a))->b2)
#define _SBIT3(a)   (((SX51Byte*)(ps->SFR + a))->b3)
#define _SBIT4(a)   (((SX51Byte*)(ps->SFR + a))->b4)
#define _SBIT5(a)   (((SX51Byte*)(ps->SFR + a))->b5)
#define _SBIT6(a)   (((SX51Byte*)(ps->SFR + a))->b6)
#define _SBIT7(a)   (((SX51Byte*)(ps->SFR + a))->b7)
#define _SBIT(a, b) _SBIT##b(a)
#define _IBIT0(a)   (((SX51Byte*)(ps->IData + 0x20 + a))->b0)
#define _IBIT1(a)   (((SX51Byte*)(ps->IData + 0x20 + a))->b1)
#define _IBIT2(a)   (((SX51Byte*)(ps->IData + 0x20 + a))->b2)
#define _IBIT3(a)   (((SX51Byte*)(ps->IData + 0x20 + a))->b3)
#define _IBIT4(a)   (((SX51Byte*)(ps->IData + 0x20 + a))->b4)
#define _IBIT5(a)   (((SX51Byte*)(ps->IData + 0x20 + a))->b5)
#define _IBIT6(a)   (((SX51Byte*)(ps->IData + 0x20 + a))->b6)
#define _IBIT7(a)   (((SX51Byte*)(ps->IData + 0x20 + a))->b7)
#define _IBIT(a, b) _IBIT##b(a)

#define _PC    	(ps->PC)
#define _ACC       _SFR(X51_SFR_ACC)
#define _PSW       _SFR(X51_SFR_PSW)
#define _DPL       _SFR(X51_SFR_DPL)
#define _DPH       _SFR(X51_SFR_DPH)
#define _SP        _SFR(X51_SFR_SP)
#define _R0        _IDATA(_PSW & 0x18 + 0)
#define _R1        _IDATA(_PSW & 0x18 + 1)
#define _R2        _IDATA(_PSW & 0x18 + 2)
#define _R3        _IDATA(_PSW & 0x18 + 3)
#define _R4        _IDATA(_PSW & 0x18 + 4)
#define _R5        _IDATA(_PSW & 0x18 + 5)
#define _R6        _IDATA(_PSW & 0x18 + 6)
#define _R7        _IDATA(_PSW & 0x18 + 7)
#define _Rn(n)     _IDATA(_PSW & 0x18 + n)

#define _C         _SBIT(X51_SFR_PSW, 0)

#define rCODE(a)      X51Sim_Rd(ps, 'C', a)
#define rDATA(a)      X51Sim_Rd(ps, 'D', a)
#define rIDATA(a)     X51Sim_Rd(ps, 'I', a)
#define rXDATA(a)     X51Sim_Rd(ps, 'X', a)
#define rBIT(a)       X51Sim_Rd(ps, 'B', a)

#define wDATA(a, v)   X51Sim_Wr(ps, 'D', a, v)
#define wIDATA(a, v)  X51Sim_Wr(ps, 'I', a, v)
#define wXDATA(a, v)  X51Sim_Wr(ps, 'X', a, v)
#define wBIT(a, v)    X51Sim_Wr(ps, 'B', a, v)


SX51Sim* X51Sim_Init(char* pcChip)
{
	SX51Sim* ps = (SX51Sim*)malloc(sizeof(SX51Sim));
	if (!ps) return 0;
	ps->Code_len = 0x10000;
	ps->IData_len = 0x100;
	ps->XData_len = 0;
	ps->SFR_len = 0x80;
	ps->Code = (ps->Code_len)?(unsigned char*)malloc(ps->Code_len):0;
	ps->IData = (ps->IData_len)?(unsigned char*)malloc(ps->IData_len):0;
	ps->XData = (ps->XData_len)?(unsigned char*)malloc(ps->XData_len):0;
	ps->SFR = (ps->SFR_len)?(unsigned char*)malloc(ps->SFR_len):0;
	if (ps->Code) memset(ps->Code, ps->Code_empty, ps->Code_len);
	if (ps->IData) memset(ps->IData, ps->IData_empty, ps->IData_len);
	if (ps->XData) memset(ps->XData, ps->XData_empty, ps->XData_len);
	if (ps->SFR) memset(ps->SFR, ps->SFR_empty, ps->SFR_len);
	
	ps->core = 1;
	ps->freq = 20000;

	ps->rdSFR = 0;
	ps->wrSFR = 0;
	ps->tick = 0;
	ps->cycle = 0;
	ps->stop = 0;
	
	_PC = 0;
	_DPL = 0;
	_DPH = 0;
	_ACC = 0;
	_PSW = 0;
	_SP = 0;
	
	return ps;
}

void X51Sim_Done(SX51Sim* ps)
{
	if (!ps) return;
	if (ps->Code) free(ps->Code);
	if (ps->IData) free(ps->IData);
	if (ps->XData) free(ps->XData);
	if (ps->SFR) free(ps->SFR);
	free(ps);
}

int X51Sim_Tick(SX51Sim* pSim, int iCnt)
{
	int iTicks = 0;
	if (pSim->core == X51_CORE_X1)
	{
		unsigned long long tick = pSim->tick;
		while (iCnt--)
			if ((tick++ % 12) == 0)
			{
				if (X51Sim_Cycle(pSim, 1))
					iTicks += 12;
			}
		pSim->tick = tick;
	}
	else if (pSim->core == X51_CORE_SC)
	{
		iCnt = X51Sim_Cycle(pSim, iCnt);
		iTicks += iCnt;
	}
	return iTicks;
}

int X51Sim_Cycle(SX51Sim* ps, int iCnt)
{
	int iCycles = 0;
	while (!ps->stop && iCnt--)
	{
		if (ps->core == X51_CORE_X1)
		{
			if ((*(g_pFCycleX1[ps->cycle]))(ps)) _PC++;
			ps->tick += 12;
			if ((ps->pi) && (++(ps->cycle) == ps->pi->cyc_x1))
				ps->cycle = 0;
		}
		else if (ps->core == X51_CORE_SC)
		{
			if ((*(g_pFCycleSC[ps->cycle]))(ps)) _PC++;
			ps->tick++;
			if ((ps->pi) && (++(ps->cycle) == ps->pi->cyc_sc))
				ps->cycle = 0;
			if (ps->cycle == 0)
			{
				char cTxt[128];
				printf("PC=%04x ACC=%02x PSW=%02x\n", _PC, _ACC, _PSW);
			}
		}
		iCycles++;
	}
	return iCycles;
}

int X51Sim_Step(SX51Sim* pSim, int iCnt)
{
	int iSteps = 0;
	while (!pSim->stop && iCnt--)
	{
		X51Sim_Cycle(pSim, 1);
		while (pSim->cycle)
			X51Sim_Cycle(pSim, 1);
		iSteps++;
	}
	return iSteps;
}

int X51Sim_Run(SX51Sim* pSim, float fTimeMultiply, int iChunk)
{
	int iCycles = 0;
	while (!pSim->stop)
		iCycles += X51Sim_Cycle(pSim, iChunk);
	return iCycles;
}

int X51Sim_RunAsync(SX51Sim* pSim, float fTimeMultiply, int iChunk)
{
}

void X51Sim_Stop(SX51Sim* pSim)
{
	pSim->stop = 1;
}

unsigned char X51Sim_Rd(SX51Sim* ps, char t, unsigned short a)
{
	switch (t)
	{
	case 'C': //Code memory
		return (a < ps->Code_len)?_CODE(a):ps->Code_empty;
/*	case 'D': //Data - direct (IData or SFR)
		if (usAddr < X51_MIN_SFR)
			return (usAddr < pSim->IData_len)?pSim->IData[usAddr]:pSim->IData_empty;
		return (usAddr <= X51_MAX_SFR)?pSim->SFR[usAddr]:pSim->IData_empty;
	case 'I': //IData memory - indirect
		return (usAddr < pSim->IData_len)?pSim->IData[usAddr]:pSim->IData_empty;*/
	}
	return 0;
}

void X51Sim_Wr(SX51Sim* ps, char t, unsigned short a, unsigned char v)
{
}

int X51Sim_X1_0(SX51Sim* ps) {}
int X51Sim_X1_1(SX51Sim* ps) {}
int X51Sim_X1_2(SX51Sim* ps) {}
int X51Sim_X1_3(SX51Sim* ps) {}


int X51Sim_SC_0(SX51Sim* ps)
{
	ps->pi = g_psInsTab + rCODE(_PC);
	SX51Ins* pi = ps->pi;
	unsigned long opv = *((unsigned long*)pi) & 0xffffff00;
	
	char cIns[128];
	X51Das_Ins(X51_DAS_FLG_DISASM, ps->Code, ps->PC, cIns, 0, 0, 0);
	printf("%d %s\n", (int)ps->tick, cIns);
	
	switch(pi->ins)
	{
	case X51_INS_CLR:
		switch (opv) {
		case X51_OPV_C: _C = 0; return 1;
		case X51_OPV_A: _ACC = 0; return 1;
		} break;
/*	case X51_INS_MOV:
		switch (opv)
		{
		case X51_OPV_A_R0:
		case X51_OPV_A_R1:
		case X51_OPV_A_R2:
		case X51_OPV_A_R3:
		case X51_OPV_A_R4:
		case X51_OPV_A_R5:
		case X51_OPV_A_R6:
		case X51_OPV_A_R7:
//			ps->ACC = X51Sim_RdIData(ps, (ps->PSW & 0x18) + (pi->op0 - X51_OP_R0)); return 1;
		}
		break;
	case X51_INS_NOP: return 1;*/
	}
	return 1;
}

int X51Sim_SC_1(SX51Sim* ps)
{
	SX51Ins* pi = ps->pi;
	if (!pi) return 0;
	unsigned long opv = *((unsigned long*)pi) & 0xffffff00;
	switch(pi->ins)
	{
	case X51_INS_MOV:
		switch (opv) {
		case X51_OPV_A_Data8: _ACC = rCODE(_PC); return 1;
		case X51_OPV_A_Dir: _ACC = rIDATA(rCODE(_PC)); return 1;
		} break;
	}
	return 1;
}

int X51Sim_SC_2(SX51Sim* ps)
{
	SX51Ins* pi = ps->pi;
	if (!pi) return 0;
	unsigned long opv = *((unsigned long*)pi) & 0xffffff00;
	switch(pi->ins)
	{
	case X51_INS_ACALL: _PC = ((pi->op0 - X51_OP_P0) << 8) + rCODE(_PC); return 0;
	case X51_INS_AJMP:  _PC = ((pi->op0 - X51_OP_P0) << 8) + rCODE(_PC); return 0;
	case X51_INS_SJMP:  _PC += (signed char)rCODE(_PC - 1); return 0;
	}
	return 1;
}

int X51Sim_SC_3(SX51Sim* ps)
{
	return 1;
}
