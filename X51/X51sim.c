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
#include "X51simM.h"
#include "../Hex/Hex.h"


extern SX51Ins* g_psInsTab;

typedef int (FX51SimC)(SX51Sim* ps);


//cycle process routines for X1 core
int X51Sim_X1_0(SX51Sim* ps);
int X51Sim_X1_1(SX51Sim* ps);
int X51Sim_X1_2(SX51Sim* ps);
int X51Sim_X1_3(SX51Sim* ps);

//cycle process routines for SC core
int X51Sim_SC_0(SX51Sim* ps);
int X51Sim_SC_1(SX51Sim* ps);
int X51Sim_SC_2(SX51Sim* ps);
int X51Sim_SC_3(SX51Sim* ps);

//cycle process routines pointer array for X1 core (4 elements)
FX51SimC* g_pFCycleX1[X51_MAX_CYCLE + 1] = {&X51Sim_X1_0, &X51Sim_X1_1, &X51Sim_X1_2, &X51Sim_X1_3};

//cycle process routines pointer array for SC core (4 elements)
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

SX51Sim* X51Sim_Init(int iChip, int iFreq)
{
	switch (iChip >> 16) //chip manufacturer
	{
	case X51_CMNF_Atmel:
		switch ((iChip >> 8) & 0xff) //chip family
		{
//		case X51_CFAM_AT89C5x:
//		case X51_CFAM_AT89Cx051:
//		case X51_CFAM_AT89S5x:
//		case X51_CFAM_AT89Sx051:
		case X51_CFAM_AT89LPx052: return X51_AT89LPx052_SimInit(iChip & 0xff, iFreq);
		}
		break;
	}
	return 0;
}

void X51Sim_Done(SX51Sim* ps)
{
	if (!ps) return;
	free(ps);
}

void X51Sim_Reset(SX51Sim* ps)
{
//	if (ps->Code && ps->code_sz) memset(ps->Code, 0, ps->code_sz);
	if (ps->IData && ps->idata_sz) memset(ps->IData, 0, ps->idata_sz);
	if (ps->XData && ps->xdata_sz) memset(ps->XData, 0, ps->xdata_sz);
	if (ps->SFR && ps->sfr_sz) memset(ps->SFR, 0, ps->sfr_sz);
	ps->tick = 0;
	ps->cycle = 0;
	ps->stop = 0;
	ps->pi = 0;
	ps->code_q = 0;
	_PC = 0x0000;
	_PSW = 0x00;
	_ACC = 0x00;
	_B = 0x00;
	_SP = 0x07;
	_DPL = 0x00;
	_DPH = 0x00;
}

int X51Sim_Tick(SX51Sim* ps, int iCnt)
{
	int iRet = 0;
	int iTicks = 0;
	if (ps->core == X51_CORE_X1)
	{
		unsigned long long tick = ps->tick;
		while (iCnt--)
			if ((tick++ % 12) == 0)
			{
				if ((iRet = X51Sim_Cycle(ps, 1)) < 0) return iRet;
				if (iRet) iTicks += 12;
			}
		ps->tick = tick;
	}
	else if (ps->core == X51_CORE_SC)
	{
		if ((iRet = X51Sim_Cycle(ps, iCnt)) < 0) return iRet;
		iTicks += iRet;
	}
	return iTicks;
}

int X51Sim_Cycle(SX51Sim* ps, int iCnt)
{
	int iRet = 0;
	int iCycles = 0;
	while (!ps->stop && iCnt--)
	{
		if (ps->core == X51_CORE_X1)
		{
			iRet = (*(g_pFCycleX1[ps->cycle]))(ps);
			if (iRet < 0) return iRet;
			if (iRet) _PC++;
			ps->tick += 12;
			if ((ps->pi) && (++(ps->cycle) == ps->pi->cyc_x1))
				ps->cycle = 0;
		}
		else if (ps->core == X51_CORE_SC)
		{
			iRet = (*(g_pFCycleSC[ps->cycle]))(ps);
			if (iRet < 0) return iRet;
			if (iRet) _PC++;
			ps->tick++;
			if ((ps->pi) && (++(ps->cycle) == ps->pi->cyc_sc))
				ps->cycle = 0;
		}
		iCycles++;
	}
	return iCycles;
}

int X51Sim_Step(SX51Sim* pSim, int iCnt)
{
	int iRet = 0;
	int iSteps = 0;
	while (!pSim->stop && iCnt--)
	{
		if ((iRet = X51Sim_Cycle(pSim, 1)) < 0) return iRet;
		while (pSim->cycle)
			if ((iRet = X51Sim_Cycle(pSim, 1)) < 0) return iRet;
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

void X51Sim_Int(SX51Sim* ps, char cIntV)
{
	_PUSH(_PCL); _PUSH(_PCH); _PC = cIntV;
}

void X51Sim_Rx(SX51Sim* ps, char cRx)
{
	wBIT(X51_SFB_RI, 1);
	printf("SCON = %02x\n", rDATA(X51_SFR_SCON));
	wDATA(X51_SFR_SBUF, cRx);
	_PUSH(_PCL); _PUSH(_PCH); _PC = X51_INTV_S;
}

void X51Sim_Tx(SX51Sim* ps)
{
	wBIT(X51_SFB_TI, 1);
	printf("SCON = %02x\n", rDATA(X51_SFR_SCON));
//	wDATA(X51_SFR_SBUF, cRx);
	_PUSH(_PCL); _PUSH(_PCH); _PC = X51_INTV_S;
}

void X51Sim_Print(SX51Sim* ps, void* pFile, FX51Value2Name* pValue2Name, void* pParam)
{
	char cPSW[9] = "CAF01O-P";
	for (int i = 0; i < 8; i++)
		if (!(_PSW & (1 << i))) cPSW[7 - i] = '-';
	fprintf((FILE*)pFile, "PC:%04x ACC:%02x PSW:%02x(%s) B=%02x SP=%02x DPTR=%04x\n", _PC, _ACC, _PSW, cPSW, _B, _SP, _DPTR);
	fprintf((FILE*)pFile, "R0-7:%02x %02x %02x %02x %02x %02x %02x %02x (RS=%d)\n", _R0, _R1, _R2, _R3, _R4, _R5, _R6, _R7, (_PSW & 0x18) >> 3);
	fprintf((FILE*)pFile, "IDATA:%02x %02x %02x %02x %02x %02x %02x %02x (RS=%d)\n", _IDATA(0), _IDATA(1), _IDATA(2), _IDATA(3), _IDATA(4), _IDATA(5), _IDATA(6), _IDATA(7));
	int iAddr = _PC; for (int i = 0; i < 6; i++)
	{
		char cIns[128];
		iAddr += X51Das_Ins(X51_DAS_FLG_DISASM, ps->Code, iAddr, cIns, 0, pValue2Name, pParam);
		fprintf((FILE*)pFile, "%s\n", cIns);
	}
	//	fprintf((FILE*)pFile, "CODE[PC]:%02x %02x %02x %02x \n", _CODE(_PC+0), _CODE(_PC+1), _CODE(_PC+2), _CODE(_PC+3));
}

int X51Sim_LoadHex(SX51Sim* ps, char* pcFileName)
{
	int iBlockCnt = 0;
	unsigned short* pusBlockAddr = 0;
	unsigned short* pusBlockSize = 0;
	unsigned char** ppucBlockData = 0;
	unsigned short usStartAddr = -1;
	int iRes = HexIntelLoadFromFile(pcFileName, &iBlockCnt, &pusBlockAddr, &pusBlockSize, &ppucBlockData, &usStartAddr);
	if (iRes != HEX_RES_OK) return iRes;
	unsigned short usMinAddr = 0;
	unsigned short usMaxAddr = 0;
	HexBlocksAnalyze(iBlockCnt, pusBlockAddr, pusBlockSize, ppucBlockData, &usMinAddr, &usMaxAddr);
	if (usMaxAddr > ps->code_sz - 1) return -1;
	memset(ps->Code, 0xff, ps->code_sz);
	int iSize = HexBlocksToMemory(iBlockCnt, pusBlockAddr, pusBlockSize, ppucBlockData, ps->Code);
	HexBlocksFree(iBlockCnt, pusBlockAddr, pusBlockSize, ppucBlockData);
	return 0;
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

#define _UF_P            _P = 0
#define _UF_OV_P(v, o1)  _OV = ((~v & _ACC & o1 & 128) || (v & ~_ACC & ~o1 & 128))
#define _UF_OV_N(v, o1)  _OV = ((~v & _ACC & o1 & 128) || (v & ~_ACC & ~o1 & 128))
#define _UF_AC(v)        _AC = 0
#define _UF_CY(v)        _CY = ((v > 256) | (v < 0))
#define _UF_CY_P(v)      _CY = (v > 256)
#define _UF_CY_N(v)      _CY = (v < 0)

int X51Sim_SC_0(SX51Sim* ps)
{
	ps->code_q <<= 8; //shift code queue
	unsigned char* code = (unsigned char*)&ps->code_q; //pointer to code queue
	*code = rCODE(_PC); //read current code
	ps->pi = g_psInsTab + *code; //"decode" instruction
	SX51Ins* pi = ps->pi; //current instruction
	unsigned long opv = *((unsigned long*)pi) & 0xffffff00; //instruction variant
	char* op = ((char*)&opv) + 1; //operands pointer
	int ret = 1; //return value != 0 means increment PC (default)
	int tmp; //temporary variable
	switch(pi->ins)
	{
	case X51_INS_A5: break;
	case X51_INS_DA: break;
	case X51_INS_ADD:
		switch (opv)
		{
		case X51_OPV_A_R0: tmp = _ACC + _R0; _ACC = tmp; _UF_P; _UF_OV_P(tmp, _R0); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_R1: tmp = _ACC + _R1; _ACC = tmp; _UF_P; _UF_OV_P(tmp, _R1); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_R2: tmp = _ACC + _R2; _ACC = tmp; _UF_P; _UF_OV_P(tmp, _R2); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_R3: tmp = _ACC + _R3; _ACC = tmp; _UF_P; _UF_OV_P(tmp, _R3); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_R4: tmp = _ACC + _R4; _ACC = tmp; _UF_P; _UF_OV_P(tmp, _R4); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_R5: tmp = _ACC + _R5; _ACC = tmp; _UF_P; _UF_OV_P(tmp, _R5); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_R6: tmp = _ACC + _R6; _ACC = tmp; _UF_P; _UF_OV_P(tmp, _R6); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_R7: tmp = _ACC + _R7; _ACC = tmp; _UF_P; _UF_OV_P(tmp, _R7); _UF_AC(tmp); _UF_CY_P(tmp); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_ADDC:
		switch (opv)
		{
		case X51_OPV_A_R0: tmp = _ACC + _R0 + _CY; _ACC = tmp; _UF_P; _UF_OV_P(tmp, _R0); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_R1: tmp = _ACC + _R1 + _CY; _ACC = tmp; _UF_P; _UF_OV_P(tmp, _R1); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_R2: tmp = _ACC + _R2 + _CY; _ACC = tmp; _UF_P; _UF_OV_P(tmp, _R2); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_R3: tmp = _ACC + _R3 + _CY; _ACC = tmp; _UF_P; _UF_OV_P(tmp, _R3); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_R4: tmp = _ACC + _R4 + _CY; _ACC = tmp; _UF_P; _UF_OV_P(tmp, _R4); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_R5: tmp = _ACC + _R5 + _CY; _ACC = tmp; _UF_P; _UF_OV_P(tmp, _R5); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_R6: tmp = _ACC + _R6 + _CY; _ACC = tmp; _UF_P; _UF_OV_P(tmp, _R6); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_R7: tmp = _ACC + _R7 + _CY; _ACC = tmp; _UF_P; _UF_OV_P(tmp, _R7); _UF_AC(tmp); _UF_CY_P(tmp); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_ANL:
		switch (opv) {
		case X51_OPV_A_R0: _ACC &= _R0; _UF_P; break;
		case X51_OPV_A_R1: _ACC &= _R1; _UF_P; break;
		case X51_OPV_A_R2: _ACC &= _R2; _UF_P; break;
		case X51_OPV_A_R3: _ACC &= _R3; _UF_P; break;
		case X51_OPV_A_R4: _ACC &= _R4; _UF_P; break;
		case X51_OPV_A_R5: _ACC &= _R5; _UF_P; break;
		case X51_OPV_A_R6: _ACC &= _R6; _UF_P; break;
		case X51_OPV_A_R7: _ACC &= _R7; _UF_P; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_CLR:
		switch (opv) {
		case X51_OPV_C: _CY = 0; break;
		case X51_OPV_A: _ACC = 0; _UF_P; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_CPL:
		switch (opv) {
		case X51_OPV_C: _CY = !_CY; break;
		case X51_OPV_A: _ACC = ~_ACC; _UF_P; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_DEC:
		switch (opv) {
		case X51_OPV_A: _ACC--; _UF_P; break;
		case X51_OPV_R0: _R0--; break;
		case X51_OPV_R1: _R1--; break;
		case X51_OPV_R2: _R2--; break;
		case X51_OPV_R3: _R3--; break;
		case X51_OPV_R4: _R4--; break;
		case X51_OPV_R5: _R5--; break;
		case X51_OPV_R6: _R6--; break;
		case X51_OPV_R7: _R7--; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_INC:
		switch (opv) {
		case X51_OPV_A: _ACC++; _UF_P; break;
		case X51_OPV_R0: _R0++; break;
		case X51_OPV_R1: _R1++; break;
		case X51_OPV_R2: _R2++; break;
		case X51_OPV_R3: _R3++; break;
		case X51_OPV_R4: _R4++; break;
		case X51_OPV_R5: _R5++; break;
		case X51_OPV_R6: _R6++; break;
		case X51_OPV_R7: _R7++; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_MOV:
		switch (opv)
		{
		case X51_OPV_A_R0: _ACC = _R0; _UF_P; break;
		case X51_OPV_A_R1: _ACC = _R1; _UF_P; break;
		case X51_OPV_A_R2: _ACC = _R2; _UF_P; break;
		case X51_OPV_A_R3: _ACC = _R3; _UF_P; break;
		case X51_OPV_A_R4: _ACC = _R4; _UF_P; break;
		case X51_OPV_A_R5: _ACC = _R5; _UF_P; break;
		case X51_OPV_A_R6: _ACC = _R6; _UF_P; break;
		case X51_OPV_A_R7: _ACC = _R7; _UF_P; break;
		case X51_OPV_R0_A: _R0 = _ACC; break;
		case X51_OPV_R1_A: _R1 = _ACC; break;
		case X51_OPV_R2_A: _R2 = _ACC; break;
		case X51_OPV_R3_A: _R3 = _ACC; break;
		case X51_OPV_R4_A: _R4 = _ACC; break;
		case X51_OPV_R5_A: _R5 = _ACC; break;
		case X51_OPV_R6_A: _R6 = _ACC; break;
		case X51_OPV_R7_A: _R7 = _ACC; break;
		case X51_OPV_iR0_A: wIDATA(_R0, _ACC); break;
		case X51_OPV_iR1_A: wIDATA(_R1, _ACC); break;
//		case X51_OPV_A_iR0: _ACC = rIDATA(_R0); break; //?possible bug in doc
//		case X51_OPV_A_iR1: _ACC = rIDATA(_R1); break; //?possible bug in doc
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_NOP: break;
	case X51_INS_ORL:
		switch (opv) {
		case X51_OPV_A_R0: _ACC |= _R0; _UF_P; break;
		case X51_OPV_A_R1: _ACC |= _R1; _UF_P; break;
		case X51_OPV_A_R2: _ACC |= _R2; _UF_P; break;
		case X51_OPV_A_R3: _ACC |= _R3; _UF_P; break;
		case X51_OPV_A_R4: _ACC |= _R4; _UF_P; break;
		case X51_OPV_A_R5: _ACC |= _R5; _UF_P; break;
		case X51_OPV_A_R6: _ACC |= _R6; _UF_P; break;
		case X51_OPV_A_R7: _ACC |= _R7; _UF_P; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_RL: _ACC = (_ACC << 1) | (_ACC >> 7); _UF_P; break;
	case X51_INS_RLC: tmp = (_ACC << 1) | _CY; _CY = _ACC_7; _ACC = tmp; _UF_P; break;
	case X51_INS_RR: _ACC = (_ACC >> 1) | (_ACC << 7); _UF_P; break;
	case X51_INS_RRC: tmp = (_ACC >> 1) | (_CY << 7); _CY = _ACC_0; _ACC = tmp; _UF_P; break;
	case X51_INS_SETB:
		switch (opv) {
		case X51_OPV_C: _CY = 1; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_SUBB:
		switch (opv)
		{
		case X51_OPV_A_R0: tmp = _ACC - _R0 - _CY; _ACC = tmp; _UF_P; _UF_OV_N(tmp, _R0); _UF_AC(tmp); _UF_CY_N(tmp); break;
		case X51_OPV_A_R1: tmp = _ACC - _R1 - _CY; _ACC = tmp; _UF_P; _UF_OV_N(tmp, _R1); _UF_AC(tmp); _UF_CY_N(tmp); break;
		case X51_OPV_A_R2: tmp = _ACC - _R2 - _CY; _ACC = tmp; _UF_P; _UF_OV_N(tmp, _R2); _UF_AC(tmp); _UF_CY_N(tmp); break;
		case X51_OPV_A_R3: tmp = _ACC - _R3 - _CY; _ACC = tmp; _UF_P; _UF_OV_N(tmp, _R3); _UF_AC(tmp); _UF_CY_N(tmp); break;
		case X51_OPV_A_R4: tmp = _ACC - _R4 - _CY; _ACC = tmp; _UF_P; _UF_OV_N(tmp, _R4); _UF_AC(tmp); _UF_CY_N(tmp); break;
		case X51_OPV_A_R5: tmp = _ACC - _R5 - _CY; _ACC = tmp; _UF_P; _UF_OV_N(tmp, _R5); _UF_AC(tmp); _UF_CY_N(tmp); break;
		case X51_OPV_A_R6: tmp = _ACC - _R6 - _CY; _ACC = tmp; _UF_P; _UF_OV_N(tmp, _R6); _UF_AC(tmp); _UF_CY_N(tmp); break;
		case X51_OPV_A_R7: tmp = _ACC - _R7 - _CY; _ACC = tmp; _UF_P; _UF_OV_N(tmp, _R7); _UF_AC(tmp); _UF_CY_N(tmp); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_SWAP:  _ACC = (_ACC << 4) | (_ACC >> 4); break;
	case X51_INS_XCH:
		switch (opv) {
		case X51_OPV_A_R0: tmp = _ACC; _ACC = _R0; _R0 = tmp; _UF_P; break;
		case X51_OPV_A_R1: tmp = _ACC; _ACC = _R1; _R1 = tmp; _UF_P; break;
		case X51_OPV_A_R2: tmp = _ACC; _ACC = _R2; _R2 = tmp; _UF_P; break;
		case X51_OPV_A_R3: tmp = _ACC; _ACC = _R3; _R3 = tmp; _UF_P; break;
		case X51_OPV_A_R4: tmp = _ACC; _ACC = _R4; _R4 = tmp; _UF_P; break;
		case X51_OPV_A_R5: tmp = _ACC; _ACC = _R5; _R5 = tmp; _UF_P; break;
		case X51_OPV_A_R6: tmp = _ACC; _ACC = _R6; _R6 = tmp; _UF_P; break;
		case X51_OPV_A_R7: tmp = _ACC; _ACC = _R7; _R7 = tmp; _UF_P; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_XRL:
		switch (opv) {
		case X51_OPV_A_R0: _ACC ^= _R0; _UF_P; break;
		case X51_OPV_A_R1: _ACC ^= _R1; _UF_P; break;
		case X51_OPV_A_R2: _ACC ^= _R2; _UF_P; break;
		case X51_OPV_A_R3: _ACC ^= _R3; _UF_P; break;
		case X51_OPV_A_R4: _ACC ^= _R4; _UF_P; break;
		case X51_OPV_A_R5: _ACC ^= _R5; _UF_P; break;
		case X51_OPV_A_R6: _ACC ^= _R6; _UF_P; break;
		case X51_OPV_A_R7: _ACC ^= _R7; _UF_P; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
	}
	return ret;
}

int X51Sim_SC_1(SX51Sim* ps)
{
	SX51Ins* pi = ps->pi; //current instruction
	if (!pi) return -1; //error - no instruction - inconsistent state
	ps->code_q <<= 8; //shift code queue
	unsigned char* code = (unsigned char*)&ps->code_q; //pointer to code queue
	*code = rCODE(_PC); //read current code
	unsigned long opv = *((unsigned long*)pi) & 0xffffff00; //instruction variant
	char* op = ((char*)&opv) + 1; //operands pointer
	int ret = 1; //return value != 0 means increment PC (default)
	int tmp; //temporary variable
	switch(pi->ins)
	{
	case X51_INS_ADD:
		switch (opv) {
		case X51_OPV_A_Data8: tmp = _ACC + *code; _ACC = tmp; _UF_P; _UF_OV_P(tmp, *code); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_Dir: tmp = _ACC + rDATA(*code); _ACC = tmp; _UF_P; _UF_OV_P(tmp, rDATA(*code)); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_iR0: tmp = _ACC + rIDATA(_R0); _ACC = tmp; _UF_P; _UF_OV_P(tmp, rIDATA(_R0)); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_iR1: tmp = _ACC + rIDATA(_R1); _ACC = tmp; _UF_P; _UF_OV_P(tmp, rIDATA(_R1)); _UF_AC(tmp); _UF_CY_P(tmp); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_ADDC:
		switch (opv) {
		case X51_OPV_A_Data8: tmp = _ACC + *code + _CY; _ACC = tmp; _UF_P; _UF_OV_P(tmp, *code); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_Dir: tmp = _ACC + rDATA(*code) + _CY; _ACC = tmp; _UF_P; _UF_OV_P(tmp, rDATA(*code)); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_iR0: tmp = _ACC + rIDATA(_R0) + _CY; _ACC = tmp; _UF_P; _UF_OV_P(tmp, rIDATA(_R0)); _UF_AC(tmp); _UF_CY_P(tmp); break;
		case X51_OPV_A_iR1: tmp = _ACC + rIDATA(_R1) + _CY; _ACC = tmp; _UF_P; _UF_OV_P(tmp, rIDATA(_R1)); _UF_AC(tmp); _UF_CY_P(tmp); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_ANL:
		switch (opv) {
		case X51_OPV_C_Bit: _CY &= rBIT(*code); break;
		case X51_OPV_A_iR0: _ACC &= rIDATA(_R0); _UF_P; break;
		case X51_OPV_A_iR1: _ACC &= rIDATA(_R1); _UF_P; break;
		case X51_OPV_A_Dir: _ACC &= rDATA(*code); _UF_P; break;
		case X51_OPV_A_Data8: _ACC &= *code; _UF_P; break;
		case X51_OPV_Dir_A: wDATA(*code, rDATA(*code) & _ACC); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_ANLN:
		switch (opv) {
		case X51_OPV_C_Bit: _CY &= !rBIT(*code); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_CLR:
		switch (opv) {
		case X51_OPV_Bit: wBIT(*code, 0); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_CPL:
		switch (opv) {
		case X51_OPV_Bit: wBIT(*code, !rBIT(*code)); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_DEC:
		switch (opv) {
		case X51_OPV_Dir: wDATA(*code, rDATA(*code) - 1); break;
		case X51_OPV_iR0: wDATA(_R0, rDATA(_R0) - 1); break;
		case X51_OPV_iR1: wDATA(_R1, rDATA(_R1) - 1); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_DJNZ:
		switch (opv) {
		case X51_OPV_R0_Rel: if (--_R0 == 0) ps->cycle++; break;
		case X51_OPV_R1_Rel: if (--_R1 == 0) ps->cycle++; break;
		case X51_OPV_R2_Rel: if (--_R2 == 0) ps->cycle++; break;
		case X51_OPV_R3_Rel: if (--_R3 == 0) ps->cycle++; break;
		case X51_OPV_R4_Rel: if (--_R4 == 0) ps->cycle++; break;
		case X51_OPV_R5_Rel: if (--_R5 == 0) ps->cycle++; break;
		case X51_OPV_R6_Rel: if (--_R6 == 0) ps->cycle++; break;
		case X51_OPV_R7_Rel: if (--_R7 == 0) ps->cycle++; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_INC:
		switch (opv) {
		case X51_OPV_Dir: wDATA(*code, rDATA(*code) + 1); break;
		case X51_OPV_iR0: wDATA(_R0, rDATA(_R0) + 1); break;
		case X51_OPV_iR1: wDATA(_R1, rDATA(_R1) + 1); break;
		case X51_OPV_DPTR: wDPTR(rDPTR + 1); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_JNC:
		switch (opv) {
		case X51_OPV_Rel: if (_CY == 0) ps->cycle++; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_JMP: _PC = _ACC + _DPTR; ret = 0; break;
	case X51_INS_JC:
		switch (opv) {
		case X51_OPV_Rel: if (_CY != 0) ps->cycle++; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_JNZ:
		switch (opv) {
		case X51_OPV_Rel: if (_ACC == 0) ps->cycle++; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_JZ:
		switch (opv) {
		case X51_OPV_Rel: if (_ACC != 0) ps->cycle++; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_MOV:
		switch (opv) {
		case X51_OPV_A_Data8: _ACC = *code; _UF_P; break;
		case X51_OPV_A_Dir: _ACC = rDATA(*code); _UF_P; break;
		case X51_OPV_A_iR0: _ACC = rIDATA(_R0); _UF_P; break;
		case X51_OPV_A_iR1: _ACC = rIDATA(_R1); _UF_P; break;
		case X51_OPV_C_Bit: _CY = rBIT(*code); break;
		case X51_OPV_Bit_C: wBIT(*code, _CY); break;
		case X51_OPV_Dir_A: wDATA(*code, _ACC); break;
		case X51_OPV_R0_Dir: _R0 = rDATA(*code); break;
		case X51_OPV_R1_Dir: _R1 = rDATA(*code); break;
		case X51_OPV_R2_Dir: _R2 = rDATA(*code); break;
		case X51_OPV_R3_Dir: _R3 = rDATA(*code); break;
		case X51_OPV_R4_Dir: _R4 = rDATA(*code); break;
		case X51_OPV_R5_Dir: _R5 = rDATA(*code); break;
		case X51_OPV_R6_Dir: _R6 = rDATA(*code); break;
		case X51_OPV_R7_Dir: _R7 = rDATA(*code); break;
		case X51_OPV_R0_Data8: _R0 = *code; break;
		case X51_OPV_R1_Data8: _R1 = *code; break;
		case X51_OPV_R2_Data8: _R2 = *code; break;
		case X51_OPV_R3_Data8: _R3 = *code; break;
		case X51_OPV_R4_Data8: _R4 = *code; break;
		case X51_OPV_R5_Data8: _R5 = *code; break;
		case X51_OPV_R6_Data8: _R6 = *code; break;
		case X51_OPV_R7_Data8: _R7 = *code; break;
		case X51_OPV_iR0_Dir: wIDATA(_R0, rDATA(*code)); break;
		case X51_OPV_iR1_Dir: wIDATA(_R0, rDATA(*code)); break;
		case X51_OPV_iR0_Data8: wIDATA(_R0, *code); break;
		case X51_OPV_iR1_Data8: wIDATA(_R0, *code); break;
		case X51_OPV_Dir_R0: wDATA(*code, _R0); break;
		case X51_OPV_Dir_R1: wDATA(*code, _R1); break;
		case X51_OPV_Dir_R2: wDATA(*code, _R2); break;
		case X51_OPV_Dir_R3: wDATA(*code, _R3); break;
		case X51_OPV_Dir_R4: wDATA(*code, _R4); break;
		case X51_OPV_Dir_R5: wDATA(*code, _R5); break;
		case X51_OPV_Dir_R6: wDATA(*code, _R6); break;
		case X51_OPV_Dir_R7: wDATA(*code, _R7); break;
		case X51_OPV_Dir_iR0: wDATA(*code, rIDATA(_R0)); break;
		case X51_OPV_Dir_iR1: wDATA(*code, rIDATA(_R1)); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_MUL: tmp = _ACC * _B; _B = (tmp >> 8); _ACC = tmp; break;
	case X51_INS_ORL:
		switch (opv) {
		case X51_OPV_C_Bit: _CY |= rBIT(*code); break;
		case X51_OPV_A_iR0: _ACC |= rIDATA(_R0); _UF_P; break;
		case X51_OPV_A_iR1: _ACC |= rIDATA(_R1); _UF_P; break;
		case X51_OPV_A_Dir: _ACC |= rDATA(*code); _UF_P; break;
		case X51_OPV_A_Data8: _ACC |= *code; _UF_P; break;
		case X51_OPV_Dir_A: wDATA(*code, rDATA(*code) | _ACC); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_ORLN:
		switch (opv) {
		case X51_OPV_C_Bit: _CY |= !rBIT(*code); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_POP:
		switch (opv) {
		case X51_OPV_Dir: wDATA(*code, _POP); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_PUSH:
		switch (opv) {
		case X51_OPV_Dir: _PUSH(rDATA(*code)); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_SETB:
		switch (opv) {
		case X51_OPV_Bit: wBIT(*code, 1); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_SUBB:
		switch (opv) {
		case X51_OPV_A_Data8: tmp = _ACC - *code - _CY; _ACC = tmp; _UF_P; _UF_OV_N(tmp, *code); _UF_AC(tmp); _UF_CY_N(tmp); break;
		case X51_OPV_A_Dir: tmp = _ACC - rDATA(*code) - _CY; _ACC = tmp; _UF_P; _UF_OV_N(tmp, rDATA(*code)); _UF_AC(tmp); _UF_CY_N(tmp); break;
		case X51_OPV_A_iR0: tmp = _ACC - rIDATA(_R0) - _CY; _ACC = tmp; _UF_P; _UF_OV_N(tmp, rIDATA(_R0)); _UF_AC(tmp); _UF_CY_N(tmp); break;
		case X51_OPV_A_iR1: tmp = _ACC - rIDATA(_R1) - _CY; _ACC = tmp; _UF_P; _UF_OV_N(tmp, rIDATA(_R1)); _UF_AC(tmp); _UF_CY_N(tmp); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_XCH:
		switch (opv) {
		case X51_OPV_A_iR0: tmp = _ACC; _ACC = rIDATA(_R0); wIDATA(_R0, tmp); _UF_P; break;
		case X51_OPV_A_iR1: tmp = _ACC; _ACC = rIDATA(_R1); wIDATA(_R1, tmp); _UF_P; break;
		case X51_OPV_A_Dir: tmp = _ACC; _ACC = rDATA(*code); wDATA(*code, tmp); _UF_P; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_XCHD:
		switch (opv) {
		case X51_OPV_A_iR0: tmp = rIDATA(_R0); wIDATA(_R0, (tmp & 0xf0) | (_ACC & 0x0f)); _ACC = (_ACC & 0xf0) | (tmp & 0x0f); _UF_P; break;
		case X51_OPV_A_iR1: tmp = rIDATA(_R1); wIDATA(_R1, (tmp & 0xf0) | (_ACC & 0x0f)); _ACC = (_ACC & 0xf0) | (tmp & 0x0f); _UF_P; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_XRL:
		switch (opv) {
		case X51_OPV_A_iR0: _ACC ^= rIDATA(_R0); _UF_P; break;
		case X51_OPV_A_iR1: _ACC ^= rIDATA(_R1); _UF_P; break;
		case X51_OPV_A_Dir: _ACC ^= rDATA(*code); _UF_P; break;
		case X51_OPV_A_Data8: _ACC ^= *code; _UF_P; break;
		case X51_OPV_Dir_A: wDATA(*code, rDATA(*code) ^ _ACC); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
	}
	return ret;
}

int X51Sim_SC_2(SX51Sim* ps)
{
	SX51Ins* pi = ps->pi; //current instruction
	if (!pi) return -1; //error - no instruction - inconsistent state
	ps->code_q <<= 8; //shift code queue
	unsigned char* code = (unsigned char*)&ps->code_q; //pointer to code queue
	*code = rCODE(_PC); //read current code
	unsigned long opv = *((unsigned long*)pi) & 0xffffff00; //instruction variant
	char* op = ((char*)&opv) + 1; //operands pointer
	int ret = 1; //return value != 0 means increment PC (default)
	int tmp; //temporary variable
	switch(pi->ins)
	{
	case X51_INS_ACALL: _PUSH(_PCL); _PUSH(_PCH); _PC = ((op[0] - X51_OP_P0) << 8) + rCODE(_PC); ret = 0; break;
	case X51_INS_AJMP:  _PC = ((op[0] - X51_OP_P0) << 8) + rCODE(_PC); ret = 0; break;
	case X51_INS_ANL:
		switch (opv) {
		case X51_OPV_Dir_Data8: wDATA(code[1], rDATA(code[1]) & *code); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_CJNE:
		switch (opv) {
		case X51_OPV_A_Dir_Rel: if (_ACC == rDATA(code[1])) ps->cycle++; break;
		case X51_OPV_A_Data8_Rel: if (_ACC == code[1]) ps->cycle++; break;
		case X51_OPV_iR0_Data8_Rel: if (rIDATA(_R0) == code[1]) ps->cycle++; break;
		case X51_OPV_iR1_Data8_Rel: if (rIDATA(_R1) == code[1]) ps->cycle++; break;
		case X51_OPV_R0_Data8_Rel: if (_R0 == code[1]) ps->cycle++; break;
		case X51_OPV_R1_Data8_Rel: if (_R1 == code[1]) ps->cycle++; break;
		case X51_OPV_R2_Data8_Rel: if (_R2 == code[1]) ps->cycle++; break;
		case X51_OPV_R3_Data8_Rel: if (_R3 == code[1]) ps->cycle++; break;
		case X51_OPV_R4_Data8_Rel: if (_R4 == code[1]) ps->cycle++; break;
		case X51_OPV_R5_Data8_Rel: if (_R5 == code[1]) ps->cycle++; break;
		case X51_OPV_R6_Data8_Rel: if (_R6 == code[1]) ps->cycle++; break;
		case X51_OPV_R7_Data8_Rel: if (_R7 == code[1]) ps->cycle++; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_DJNZ:
		switch (opv) {
		case X51_OPV_R0_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_R1_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_R2_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_R3_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_R4_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_R5_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_R6_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_R7_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_Dir_Rel: wDATA(code[1], tmp = rDATA(code[1])); if (tmp == 0) ps->cycle++; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_JB:
		switch (opv) {
		case X51_OPV_Bit_Rel: if (rBIT(code[1]) == 0) ps->cycle++; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_JBC:
		switch (opv) {
		case X51_OPV_Bit_Rel: if (rBIT(code[1]) == 0) ps->cycle++; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_JNB:
if (_PC == 0x0392) printf("JNB rBIT(%02x)=%d\n", code[1], rBIT(code[1]));
		switch (opv) {
		case X51_OPV_Bit_Rel: if (rBIT(code[1]) != 0) ps->cycle++; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_JC:
		switch (opv) {
		case X51_OPV_Rel: _PC += (signed char)code[1]; ret = 0; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_JNC:
		switch (opv) {
		case X51_OPV_Rel: _PC += (signed char)code[1]; ret = 0; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_JNZ:
		switch (opv) {
		case X51_OPV_Rel: _PC += (signed char)code[1]; ret = 0; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_JZ:
		switch (opv) {
		case X51_OPV_Rel: _PC += (signed char)code[1]; ret = 0; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_MOV:
		switch (opv) {
		case X51_OPV_Dir_Dir: wDATA(code[1], rDATA(*code)); break;
		case X51_OPV_Dir_Data8: wDATA(code[1], *code); break;
		case X51_OPV_DPTR_Data16: _DPL = *code; _DPH = code[1]; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_MOVC:
		switch (opv) {
		case X51_OPV_A_iAPC: _ACC = rCODE(_ACC + _PC); _UF_P; break;
		case X51_OPV_A_iADPTR: _ACC = rCODE(_ACC + _DPTR); _UF_P; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_MOVX:
		switch (opv) {
		case X51_OPV_A_iR0: _ACC = rXDATA(_R0); _UF_P; break;
		case X51_OPV_A_iR1: _ACC = rXDATA(_R1); _UF_P; break;
		case X51_OPV_A_iDPTR: _ACC = rXDATA(_DPTR); _UF_P; break;
		case X51_OPV_iR0_A: wXDATA(_R0, _ACC); break;
		case X51_OPV_iR1_A: wXDATA(_R1, _ACC); break;
		case X51_OPV_iDPTR_A: wXDATA(_DPTR, _ACC); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_ORL:
		switch (opv) {
		case X51_OPV_Dir_Data8: wDATA(code[1], rDATA(code[1]) | *code); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_SJMP: _PC += (signed char)code[1]; ret = 0; break;
	case X51_INS_XRL:
		switch (opv) {
		case X51_OPV_Dir_Data8: wDATA(code[1], rDATA(code[1]) ^ *code); break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
	}
	return ret;
}


int X51Sim_SC_3(SX51Sim* ps)
{
	SX51Ins* pi = ps->pi; //current instruction
	if (!pi) return -1; //error - no instruction - inconsistent state
	ps->code_q <<= 8; //shift code queue
	unsigned char* code = (unsigned char*)&ps->code_q; //pointer to code queue
	*code = rCODE(_PC); //read current code
	unsigned long opv = *((unsigned long*)pi) & 0xffffff00; //instruction variant
	char* op = ((char*)&opv) + 1; //operands pointer
	int ret = 1; //return value != 0 means increment PC (default)
	int tmp; //temporary variable
	switch(pi->ins)
	{
	case X51_INS_CJNE:
		switch (opv) {
		case X51_OPV_A_Dir_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_A_Data8_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_iR0_Data8_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_iR1_Data8_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_R0_Data8_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_R1_Data8_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_R2_Data8_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_R3_Data8_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_R4_Data8_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_R5_Data8_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_R6_Data8_Rel: _PC += (signed char)code[1]; ret = 0; break;
		case X51_OPV_R7_Data8_Rel: _PC += (signed char)code[1]; ret = 0; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_DJNZ:
		switch (opv) {
		case X51_OPV_Dir_Rel: _PC += (signed char)code[1]; ret = 0; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_JB:
		switch (opv) {
		case X51_OPV_Bit_Rel: _PC += (signed char)code[1]; ret = 0; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_JBC:
		switch (opv) {
		case X51_OPV_Bit_Rel: _PC += (signed char)code[1]; ret = 0; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_JNB:
		switch (opv) {
		case X51_OPV_Bit_Rel: _PC += (signed char)code[1]; ret = 0; break;
		default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
		} break;
	case X51_INS_LCALL: _PUSH(_PCL); _PUSH(_PCH); _PC = code[1] + (code[2] << 8); ret = 0; break;
	case X51_INS_LJMP:  _PC = code[1] + (code[2] << 8); ret = 0; break;
	case X51_INS_RET:   _PCH = _POP; _PCL = _POP; ret = 0; break;
	case X51_INS_RETI:  _PCH = _POP; _PCL = _POP; ret = 0; break;
	case X51_INS_DIV:   tmp = _B?(_ACC / _B):0; _B = _B?(_ACC % _B):0; _ACC = tmp; break;
	default: if (ps->cycle == pi->cyc_sc - 1) ret = -2; break;
	}
	return ret;
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
*#define X51_INS_DIV   0x0b
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
*#define X51_INS_MOV   0x18
#define X51_INS_MOVC  0x19
#define X51_INS_MOVX  0x1a
#define X51_INS_MUL   0x1b
*#define X51_INS_NOP   0x1c
#define X51_INS_ORL   0x1d
*#define X51_INS_POP   0x1e
*#define X51_INS_PUSH  0x1f
*#define X51_INS_RET   0x20
*#define X51_INS_RETI  0x21
*#define X51_INS_RL    0x22
*#define X51_INS_RLC   0x23
*#define X51_INS_RR    0x24
*#define X51_INS_RRC   0x25
*#define X51_INS_SETB  0x26
*#define X51_INS_SJMP  0x27
#define X51_INS_SUBB  0x28
*#define X51_INS_SWAP  0x29
#define X51_INS_XCH   0x2a
#define X51_INS_XCHD  0x2b
#define X51_INS_XRL   0x2c
*/