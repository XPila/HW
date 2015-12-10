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
#include "X51cmnD.h"


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
#define _SFR(a)     (ps->SFR[a - X51_MIN_SFR])
#define _SBIT0(a)   (((SX51Byte*)(&_SFR(a & 0xf8)))->b0)
#define _SBIT1(a)   (((SX51Byte*)(&_SFR(a & 0xf8)))->b1)
#define _SBIT2(a)   (((SX51Byte*)(&_SFR(a & 0xf8)))->b2)
#define _SBIT3(a)   (((SX51Byte*)(&_SFR(a & 0xf8)))->b3)
#define _SBIT4(a)   (((SX51Byte*)(&_SFR(a & 0xf8)))->b4)
#define _SBIT5(a)   (((SX51Byte*)(&_SFR(a & 0xf8)))->b5)
#define _SBIT6(a)   (((SX51Byte*)(&_SFR(a & 0xf8)))->b6)
#define _SBIT7(a)   (((SX51Byte*)(&_SFR(a & 0xf8)))->b7)
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
#define _PCL   	(((unsigned char*)&(ps->PC))[0])
#define _PCH   	(((unsigned char*)&(ps->PC))[1])
#define _ACC       _SFR(X51_SFR_ACC)
#define _B         _SFR(X51_SFR_B)
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
#define _DPTR      (_SFR(X51_SFR_DPL) | (_SFR(X51_SFR_DPH) << 8))

#define _P         _SBIT(X51_SFR_PSW, 0) //parity flag
#define _OV        _SBIT(X51_SFR_PSW, 2) //overflow flag
#define _RS0       _SBIT(X51_SFR_PSW, 3) //register bank select bit 0
#define _RS1       _SBIT(X51_SFR_PSW, 4) //register bank select bit 1
#define _F0        _SBIT(X51_SFR_PSW, 5) //flag 0
#define _AC        _SBIT(X51_SFR_PSW, 6) //auxiliary carry flag (for BCD operations)
#define _CY        _SBIT(X51_SFR_PSW, 7) //carry flag

#define rCODE(a)      X51Sim_Rd(ps, 'C', a)
#define rDATA(a)      X51Sim_Rd(ps, 'D', a)
#define rIDATA(a)     X51Sim_Rd(ps, 'I', a)
#define rXDATA(a)     X51Sim_Rd(ps, 'X', a)
#define rBIT(a)       X51Sim_Rd(ps, 'B', a)
#define rDPTR         (X51Sim_Rd(ps, 'D', X51_SFR_DPL) | (X51Sim_Rd(ps, 'D', X51_SFR_DPH) << 8))
#define wDPTR(v)      { X51Sim_Wr(ps, 'D', X51_SFR_DPL, v & 0xff); X51Sim_Wr(ps, 'D', X51_SFR_DPH, v >> 8); }

#define wDATA(a, v)   X51Sim_Wr(ps, 'D', a, v)
#define wIDATA(a, v)  X51Sim_Wr(ps, 'I', a, v)
#define wXDATA(a, v)  X51Sim_Wr(ps, 'X', a, v)
#define wBIT(a, v)    X51Sim_Wr(ps, 'B', a, v)


int X51Sim_Init(SX51Sim* ps, char* pcChip)
{
	if (!ps) return -1;
	
	ps->core = 1;
	ps->freq = 20000;

	ps->rdSFR = 0;
	ps->wrSFR = 0;

	X51Sim_Reset(ps);
	
	return 0;
}

void X51Sim_Done(SX51Sim* ps)
{
	if (!ps) return;
}

void X51Sim_Reset(SX51Sim* ps)
{
	ps->tick = 0;
	ps->cycle = 0;
	ps->stop = 0;
	memset(ps->Code, 0, X51_MAX_CODE + 1);
	memset(ps->IData, 0, X51_MAX_IDATA + 1);
	memset(ps->XData, 0, X51_MAX_XDATA + 1);
	memset(ps->SFR, 0, X51_MAX_SFR - X51_MIN_SFR + 1);
	_PC = 0;
	_PSW = 0;
	_ACC = 0;
	_B = 0;
	_SP = 0x07;
	_DPL = 0;
	_DPH = 0;
}

int X51Sim_Tick(SX51Sim* ps, int iCnt)
{
	int iTicks = 0;
	if (ps->core == X51_CORE_X1)
	{
		unsigned long long tick = ps->tick;
		while (iCnt--)
			if ((tick++ % 12) == 0)
			{
				if (X51Sim_Cycle(ps, 1))
					iTicks += 12;
			}
		ps->tick = tick;
	}
	else if (ps->core == X51_CORE_SC)
	{
		iCnt = X51Sim_Cycle(ps, iCnt);
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
/*			if (ps->cycle == 0)
			{
				char cTxt[128];
				printf("PC=%04x ACC=%02x PSW=%02x DPTR=%04x\n", _PC, _ACC, _PSW, rDPTR);
			}*/
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

void X51Sim_Stop(SX51Sim* ps)
{
	ps->stop = 1;
}

void X51Sim_Print(SX51Sim* ps, void* pFile)
{
	char cPSW[9] = "CAF01O-P";
	for (int i = 0; i < 8; i++)
		if (!(_PSW & (1 << i))) cPSW[i] = '-';
	fprintf((FILE*)pFile, "PC:%04x ACC:%02x PSW:%02x(%s) B=%02x SP=%02x DPTR=%04x\n", _PC, _ACC, _PSW, cPSW, _B, _SP, _DPTR);
	fprintf((FILE*)pFile, "R0-7:%02x %02x %02x %02x %02x %02x %02x %02x (RS=%d)\n", _R0, _R1, _R2, _R3, _R4, _R5, _R6, _R7, (_PSW & 0x18) >> 3);
	int iAddr = _PC; for (int i = 0; i < 6; i++)
	{
		char cIns[128];
		iAddr += X51Das_Ins(X51_DAS_FLG_DISASM, ps->Code, iAddr, cIns, 0, 0, 0);
		fprintf((FILE*)pFile, "%s\n", cIns);
	}
	//	fprintf((FILE*)pFile, "CODE[PC]:%02x %02x %02x %02x \n", _CODE(_PC+0), _CODE(_PC+1), _CODE(_PC+2), _CODE(_PC+3));
}

unsigned char X51Sim_Rd(SX51Sim* ps, char t, unsigned short a)
{
	switch (t)
	{
	case 'C': //Code memory
		if (a < X51_MAX_CODE) return _CODE(a);
		return 0;
	case 'D': //Data - direct (IData or SFR)
		if (a < X51_MIN_SFR) return _IDATA(a);
		if (a <= X51_MAX_SFR) return _SFR(a);
		return 0;
	case 'I': //IData memory - indirect
		return _IDATA(a);
	case 'B': //Bit (IData or SFR)
		if (a < X51_MIN_SFR) return (_IDATA(0x20 + ((a & 0x78) >> 3)) & (1 << (a & 7)))?1:0;
		if (a <= X51_MAX_SFR) return (_SFR((a & 0xf8)) & (1 << (a & 7)))?1:0;
		return 0;
	}
	return 0;
}

void X51Sim_Wr(SX51Sim* ps, char t, unsigned short a, unsigned char v)
{
	switch (t)
	{
	case 'D': //Data - direct (IData or SFR)
		if (a < X51_MIN_SFR) _IDATA(a) = v;
		else if (a <= X51_MAX_SFR) _SFR(a) = v;
		break;
	case 'I': //IData memory - indirect
		_IDATA(a) = v;
		break;
	case 'B': //Bit (IData or SFR)
		if (a < X51_MIN_SFR)
		{
			if (v) _IDATA(0x20 + ((a & 0x78) >> 3)) |= (1 << (a & 7));
			else _IDATA(0x20 + ((a & 0x78) >> 3)) &= ~(1 << (a & 7));
		}
		else if (a <= X51_MAX_SFR)
		{
			if (v) _SFR((a & 0xf8)) |= (1 << (a & 7));
			else _SFR((a & 0xf8)) &= ~(1 << (a & 7));
		}
		break;
	}
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
	switch(pi->ins)
	{
	case X51_INS_CLR:
		switch (opv) {
		case X51_OPV_C: _CY = 0; return 1;
		case X51_OPV_A: _ACC = 0; return 1;
		} break;
	case X51_INS_CPL:
		switch (opv) {
		case X51_OPV_C: _CY = !_CY; return 1;
		case X51_OPV_A: _ACC = ~_ACC; return 1;
		} break;
	case X51_INS_DEC:
		switch (opv) {
		case X51_OPV_A: _ACC--; return 1;
		case X51_OPV_R0:
		case X51_OPV_R1:
		case X51_OPV_R2:
		case X51_OPV_R3:
		case X51_OPV_R4:
		case X51_OPV_R5:
		case X51_OPV_R6:
		case X51_OPV_R7:
			_Rn(pi->op0 - X51_OP_R0)--; return 1;
		} break;
	case X51_INS_INC:
		switch (opv) {
		case X51_OPV_A: _ACC++; return 1;
		case X51_OPV_R0:
		case X51_OPV_R1:
		case X51_OPV_R2:
		case X51_OPV_R3:
		case X51_OPV_R4:
		case X51_OPV_R5:
		case X51_OPV_R6:
		case X51_OPV_R7:
			_Rn(pi->op0 - X51_OP_R0)++; return 1;
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
//			ps->ACC = X51Sim_RdIData(ps, (ps->PSW & 0x18) + (pi->op1 - X51_OP_R0)); return 1;
		}
		break;
	case X51_INS_NOP: return 1;*/
	case X51_INS_SETB:
		switch (opv) {
		case X51_OPV_C: _CY = 1; return 1;
		} break;
	}
	return 1;
}

int X51Sim_SC_1(SX51Sim* ps)
{
	unsigned char code = rCODE(_PC);
	SX51Ins* pi = ps->pi;
	if (!pi) return 0;
	unsigned long opv = *((unsigned long*)pi) & 0xffffff00;
	switch(pi->ins)
	{
	case X51_INS_CLR:
		switch (opv) {
		case X51_OPV_Bit: wBIT(code, 0); return 1;
		} break;
	case X51_INS_CPL:
		switch (opv) {
		case X51_OPV_Bit: wBIT(code, !rBIT(code)); return 1;
		} break;
	case X51_INS_DEC:
		switch (opv) {
		case X51_OPV_Dir: wDATA(code, rDATA(code) - 1); return 1;
		case X51_OPV_iR0: wDATA(_R0, rDATA(_R0) - 1); return 1;
		case X51_OPV_iR1: wDATA(_R1, rDATA(_R1) - 1); return 1;
		} break;
	case X51_INS_INC:
		switch (opv) {
		case X51_OPV_Dir: wDATA(code, rDATA(code) + 1); return 1;
		case X51_OPV_iR0: wDATA(_R0, rDATA(_R0) + 1); return 1;
		case X51_OPV_iR1: wDATA(_R1, rDATA(_R1) + 1); return 1;
		case X51_OPV_DPTR: wDPTR(rDPTR + 1); return 1;
		} break;
		
	case X51_INS_MOV:
		switch (opv) {
		case X51_OPV_A_Data8: _ACC = rCODE(_PC); return 1;
		case X51_OPV_A_Dir: _ACC = rIDATA(rCODE(_PC)); return 1;
		} break;
	case X51_INS_SETB:
		switch (opv) {
		case X51_OPV_Bit: wBIT(code, 1); return 1;
		} break;
	}
	return 1;
}
#define _PUSH(v) wIDATA(_SP++, v)
#define _POP rIDATA(--_SP)

int X51Sim_SC_2(SX51Sim* ps)
{
	SX51Ins* pi = ps->pi;
	if (!pi) return 0;
	unsigned long opv = *((unsigned long*)pi) & 0xffffff00;
	switch(pi->ins)
	{
	case X51_INS_ACALL: _PUSH(_PCL); _PUSH(_PCH); _PC = ((pi->op0 - X51_OP_P0) << 8) + rCODE(_PC); return 0;
	case X51_INS_AJMP:  _PC = ((pi->op0 - X51_OP_P0) << 8) + rCODE(_PC); return 0;
	case X51_INS_SJMP:  _PC += (signed char)rCODE(_PC - 1); return 0;
	}
	return 1;
}


int X51Sim_SC_3(SX51Sim* ps)
{
	SX51Ins* pi = ps->pi;
	if (!pi) return 0;
	unsigned long opv = *((unsigned long*)pi) & 0xffffff00;
	switch(pi->ins)
	{
	case X51_INS_LCALL: _PUSH(_PCL); _PUSH(_PCH); _PC = rCODE(_PC - 1) + (rCODE(_PC - 2) << 8); return 0;
	case X51_INS_LJMP:  _PC = rCODE(_PC - 1) + (rCODE(_PC - 2) << 8); return 0;
	case X51_INS_RET:  _PCH = _POP; _PCL = _POP; return 0;
	case X51_INS_RETI:  _PCH = _POP; _PCL = _POP; return 0;
	}
	return 1;
}

/*
*#define X51_INS_ACALL 0x01
#define X51_INS_ADD   0x02
#define X51_INS_ADDC  0x03
*#define X51_INS_AJMP  0x04
#define X51_INS_ANL   0x05
#define X51_INS_CJNE  0x06
*#define X51_INS_CLR   0x07
*#define X51_INS_CPL   0x08
#define X51_INS_DA    0x09
*#define X51_INS_DEC   0x0a
#define X51_INS_DIV   0x0b
#define X51_INS_DJNZ  0x0c
*#define X51_INS_INC   0x0d
#define X51_INS_JB    0x0e
#define X51_INS_JBC   0x0f
#define X51_INS_JC    0x10
#define X51_INS_JMP   0x11
#define X51_INS_JNB   0x12
#define X51_INS_JNC   0x13
#define X51_INS_JNZ   0x14
#define X51_INS_JZ    0x15
*#define X51_INS_LCALL 0x16
*#define X51_INS_LJMP  0x17
#define X51_INS_MOV   0x18
#define X51_INS_MOVC  0x19
#define X51_INS_MOVX  0x1a
#define X51_INS_MUL   0x1b
#define X51_INS_NOP   0x1c
#define X51_INS_ORL   0x1d
#define X51_INS_POP   0x1e
#define X51_INS_PUSH  0x1f
*#define X51_INS_RET   0x20
*#define X51_INS_RETI  0x21
#define X51_INS_RL    0x22
#define X51_INS_RLC   0x23
#define X51_INS_RR    0x24
#define X51_INS_RRC   0x25
*#define X51_INS_SETB  0x26
*#define X51_INS_SJMP  0x27
#define X51_INS_SUBB  0x28
#define X51_INS_SWAP  0x29
#define X51_INS_XCH   0x2a
#define X51_INS_XCHD  0x2b
#define X51_INS_XRL   0x2c
*/
