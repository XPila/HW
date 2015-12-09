////////////////////////////////////////////////////////////////////////////////
// X51cmn.c
// X51 common source file


#include "X51cmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "X51-AT89LPx052.h"


//Table of instruction definitions for every opcodes (256 entries)
SX51Ins* g_psInsTab = 0;

//Table of operand masks and variants for every instructions (X51_INS_MAX + 1 entries)
SX51InsOp* g_psInsOpTab = 0;


//Structure for string-int pair
typedef struct SStrInt {char* pc; int i;} SStrInt;

int compare_val(const void* p1, const void* p2);
int compare_str(const void* p1, const void* p2);


int X51_Init()
{
	int iRet = -1;
	if (!(g_psInsTab = (SX51Ins*)malloc(256 * sizeof(SX51Ins))));// goto e0;
	memset(g_psInsTab, 0, 256 * sizeof(SX51Ins));
	//Generate table of every instruction opcodes
	for (int iCode = 0; iCode < 256; iCode++)
	{
		int iIns, iOp0, iOp1, iOp2, iBytes, iCyclesX1, iCyclesSC;
		//Decode the instruction - X51_DecIns function is used just for generating this table
		X51_DecIns(iCode, &iIns, &iOp0, &iOp1, &iOp2, &iBytes, &iCyclesX1, &iCyclesSC);
		SX51Ins sIns = {iCode, iOp0, iOp1, iOp2, iIns, iBytes, iCyclesX1, iCyclesSC};
		g_psInsTab[iCode] = sIns;
	}
	if (!(g_psInsOpTab = (SX51InsOp*)malloc((X51_INS_MAX + 1) * sizeof(SX51InsOp)))) goto e1;
	memset(g_psInsOpTab, 0, (X51_INS_MAX + 1) * sizeof(SX51InsOp));
	//Generate table of instruction operand masks and varinats
	for (int iIns = 0; iIns <= X51_INS_MAX; iIns++)
	{
		int iOpMin = 3;
		int iOpMax = 0;
		int ulOp0Msk = 0;
		int ulOp1Msk = 0;
		int ulOp2Msk = 0;
		if (!(g_psInsOpTab[iIns].opvars = (unsigned long*)malloc((X51_OPV_MAX + 1) * sizeof(unsigned long)))) goto e2;
		for (int iCode = 0; iCode < 256; iCode++)
			if (g_psInsTab[iCode].ins == iIns)
			{
				int iOpCnt = ((g_psInsTab[iCode].op0)?1:0) + ((g_psInsTab[iCode].op1)?1:0) + ((g_psInsTab[iCode].op2)?1:0);
				if (iOpCnt < iOpMin) iOpMin = iOpCnt;
				if (iOpCnt > iOpMax) iOpMax = iOpCnt;
				if (g_psInsTab[iCode].op0) ulOp0Msk |= (1 << (g_psInsTab[iCode].op0 - 1));
				if (g_psInsTab[iCode].op1) ulOp1Msk |= (1 << (g_psInsTab[iCode].op1 - 1));
				if (g_psInsTab[iCode].op2) ulOp2Msk |= (1 << (g_psInsTab[iCode].op2 - 1));
				unsigned long ulOpVar = *((unsigned long*)(&g_psInsTab[iCode].code));
				if (g_psInsOpTab[iIns].opvcnt >= X51_OPV_MAX) //check of table consistency - all instruction variants must have same number of operands
				{
					fprintf(stdout, "Error in instruction table\n");
					goto e3;
				}
				g_psInsOpTab[iIns].opvars[g_psInsOpTab[iIns].opvcnt++] = ulOpVar;
			}
		int iOpCnt = (iOpMin == iOpMax)?iOpMin:-1;
		if (iOpCnt == -1) //check of table consistency - all instruction variants must have same number of operands
		{
			fprintf(stdout, "Error in instruction table\n");
			goto e3;
		}
		if (!g_psInsOpTab[iIns].opvcnt)
		{
			free(g_psInsOpTab[iIns].opvars);
			g_psInsOpTab[iIns].opvars = 0;
		}
		else
			if (!(g_psInsOpTab[iIns].opvars = (unsigned long*)realloc(g_psInsOpTab[iIns].opvars, g_psInsOpTab[iIns].opvcnt * sizeof(unsigned long)))) goto e3;
		SX51InsOp sInsOp = {iOpCnt, ulOp0Msk, ulOp1Msk, ulOp2Msk, g_psInsOpTab[iIns].opvcnt, g_psInsOpTab[iIns].opvars};
		g_psInsOpTab[iIns] = sInsOp;
	}
	return 0;
e3:	for (int iIns = 0; iIns <= X51_INS_MAX; iIns++)
		if (g_psInsOpTab[iIns].opvars) free(g_psInsOpTab[iIns].opvars);
e2:	free(g_psInsOpTab);
e1:	free(g_psInsTab);
e0:	return iRet;
}

void X51_Done()
{
	if (g_psInsOpTab)
	{
		for (int iIns = 0; iIns <= X51_INS_MAX; iIns++)
			if (g_psInsOpTab[iIns].opvars) free(g_psInsOpTab[iIns].opvars);
		free(g_psInsOpTab);
	}
	g_psInsOpTab = 0;
	if (g_psInsTab) free(g_psInsTab);
	g_psInsTab = 0;	
}


int X51_BitAddrEnc(unsigned char ucBitAddr)
{
	if (ucBitAddr < 0x80) return (0x20 << 3) + ucBitAddr;
	return (0x80 << 3) + (ucBitAddr & 0x7f);
}

int X51_BitAddrDec(int iIDataX8Addr)
{
	if ((iIDataX8Addr >= (0x20 << 3)) && (iIDataX8Addr <= ((0x20 << 3) + 0x7f))) return iIDataX8Addr & 0x7f;
	else if ((iIDataX8Addr >= (0x80 << 3)) && (iIDataX8Addr <= ((0x80 << 3) + 0x7f))) return (iIDataX8Addr & 0x7f) | 0x80;
	return -1;
}

int X51_DecIns(unsigned char ucCode, int* piIns, int* piOp0, int* piOp1, int* piOp2, int* piBytes, int* piCyclesX1, int* piCyclesSC)
{
#define ret0(ins, cnt, cycx1, cycsc) iIns = ins; iBytes = cnt; iCyclesX1 = cycx1; iCyclesSC = cycsc; break
#define ret1(ins, cnt, cycx1, cycsc, op0) iIns = ins; iBytes = cnt; iCyclesX1 = cycx1; iCyclesSC = cycsc; iOp0 = op0; break
#define ret2(ins, cnt, cycx1, cycsc, op0, op1) iIns = ins; iBytes = cnt; iCyclesX1 = cycx1; iCyclesSC = cycsc; iOp0 = op0; iOp1 = op1; break
#define ret3(ins, cnt, cycx1, cycsc, op0, op1, op2) iIns = ins; iBytes = cnt; iCyclesX1 = cycx1; iCyclesSC = cycsc; iOp0 = op0; iOp1 = op1; iOp2 = op2; break
	int iIns = 0;
	int iOp0 = 0;
	int iOp1 = 0;
	int iOp2 = 0;
	int iBytes = 0;
	int iCyclesX1 = 0;
	int iCyclesSC = 0;
	unsigned char ucLNibble = ucCode & 0x0f;
	unsigned char ucHNibble = ucCode  >> 4;
	switch (ucHNibble) {
	case 0x00:
		switch (ucLNibble) {
		case 0x00: ret0(X51_INS_NOP, 1, 1, 1);
		case 0x01: ret1(X51_INS_AJMP, 2, 2, 3, X51_OP_P0);
		case 0x02: ret1(X51_INS_LJMP, 3, 2, 4, X51_OP_Addr16);
		case 0x03: ret1(X51_INS_RR, 1, 1, 1, X51_OP_A);
		default:
			switch (ucLNibble)
			{
			case 0x04: ret1(X51_INS_INC, 1, 1, 1, X51_OP_A);
			case 0x05: ret1(X51_INS_INC, 2, 2, 2, X51_OP_Dir);
			case 0x06: case 0x07: ret1(X51_INS_INC, 1, 1, 2, X51_OP_iR0 + (ucLNibble - 0x06));
			default: ret1(X51_INS_INC, 1, 1, 1, X51_OP_R0 + (ucLNibble - 0x08));
			} break;
		} break;
	case 0x01:
		switch (ucLNibble) {
		case 0x00: ret2(X51_INS_JBC, 3, 2, 4, X51_OP_Bit, X51_OP_Rel);
		case 0x01: ret1(X51_INS_ACALL, 2, 2, 3, X51_OP_P0);
		case 0x02: ret1(X51_INS_LCALL, 3, 2, 4, X51_OP_Addr16);
		case 0x03: ret1(X51_INS_RRC, 1, 1, 1, X51_OP_A);
		default:
			switch (ucLNibble) {
			case 0x04: ret1(X51_INS_DEC, 1, 1, 1, X51_OP_A);
			case 0x05: ret1(X51_INS_DEC, 2, 2, 2, X51_OP_Dir);
			case 0x06: case 0x07: ret1(X51_INS_DEC, 1, 1, 2, X51_OP_iR0 + (ucLNibble - 0x06));
			default: ret1(X51_INS_DEC, 1, 1, 1, X51_OP_R0 + (ucLNibble - 0x08));
			} break;
		} break;
	case 0x02:
		switch (ucLNibble) {
		case 0x00: ret2(X51_INS_JB, 3, 2, 4, X51_OP_Bit, X51_OP_Rel);
		case 0x01: ret1(X51_INS_AJMP, 2, 2, 3, X51_OP_P1);
		case 0x02: ret0(X51_INS_RET, 1, 2, 4);
		case 0x03: ret1(X51_INS_RL, 1, 1, 1, X51_OP_A);
		default:
			switch (ucLNibble) {
			case 0x04: ret2(X51_INS_ADD, 2, 1, 2, X51_OP_A, X51_OP_Data8);
			case 0x05: ret2(X51_INS_ADD, 2, 1, 2, X51_OP_A, X51_OP_Dir);
			case 0x06: case 0x07: ret2(X51_INS_ADD, 1, 1, 2, X51_OP_A, X51_OP_iR0 + (ucLNibble - 0x06));
			default: ret2(X51_INS_ADD, 1, 1, 1, X51_OP_A, X51_OP_R0 + (ucLNibble - 0x08));
			} break;
		} break;
	case 0x03:
		switch (ucLNibble) {
		case 0x00: ret2(X51_INS_JNB, 3, 2, 4, X51_OP_Bit, X51_OP_Rel);
		case 0x01: ret1(X51_INS_ACALL, 2, 2, 3, X51_OP_P1);
		case 0x02: ret0(X51_INS_RETI, 1, 2, 4);
		case 0x03: ret1(X51_INS_RLC, 1, 1, 1, X51_OP_A);
		default:
			switch (ucLNibble) {
			case 0x04: ret2(X51_INS_ADDC, 2, 1, 2, X51_OP_A, X51_OP_Data8);
			case 0x05: ret2(X51_INS_ADDC, 2, 1, 2, X51_OP_A, X51_OP_Dir);
			case 0x06: case 0x07: ret2(X51_INS_ADDC, 1, 1, 2, X51_OP_A, X51_OP_iR0 + (ucLNibble - 0x06));
			default: ret2(X51_INS_ADDC, 1, 1, 1, X51_OP_A, X51_OP_R0 + (ucLNibble - 0x08));
			} break;
		} break;
	case 0x04:
		switch (ucLNibble) {
		case 0x00: ret1(X51_INS_JC, 2, 2, 3, X51_OP_Rel);
		case 0x01: ret1(X51_INS_AJMP, 2, 2, 3, X51_OP_P2);
		case 0x02: ret2(X51_INS_ORL, 2, 1, 2, X51_OP_Dir, X51_OP_A);
		case 0x03: ret2(X51_INS_ORL, 3, 2, 3, X51_OP_Dir, X51_OP_Data8);
		default:
			switch (ucLNibble) {
			case 0x04: ret2(X51_INS_ORL, 2, 1, 2, X51_OP_A, X51_OP_Data8);
			case 0x05: ret2(X51_INS_ORL, 2, 1, 2, X51_OP_A, X51_OP_Dir);
			case 0x06: case 0x07: ret2(X51_INS_ORL, 1, 1, 2, X51_OP_A, X51_OP_iR0 + (ucLNibble - 0x06));
			default: ret2(X51_INS_ORL, 1, 1, 1, X51_OP_A, X51_OP_R0 + (ucLNibble - 0x08));				
			} break;
		} break;
	case 0x05:
		switch (ucLNibble) {
		case 0x00: ret1(X51_INS_JNC, 2, 2, 3, X51_OP_Rel);
		case 0x01: ret1(X51_INS_ACALL, 2, 2, 3, X51_OP_P2);
		case 0x02: ret2(X51_INS_ANL, 2, 1, 2, X51_OP_Dir, X51_OP_A);
		case 0x03: ret2(X51_INS_ANL, 3, 2, 3, X51_OP_Dir, X51_OP_Data8);
		default:
			switch (ucLNibble) {
			case 0x04: ret2(X51_INS_ANL, 2, 1, 2, X51_OP_A, X51_OP_Data8);
			case 0x05: ret2(X51_INS_ANL, 2, 1, 2, X51_OP_A, X51_OP_Dir);
			case 0x06: case 0x07: ret2(X51_INS_ANL, 1, 1, 2, X51_OP_A, X51_OP_iR0 + (ucLNibble - 0x06));
			default: ret2(X51_INS_ANL, 1, 1, 1, X51_OP_A, X51_OP_R0 + (ucLNibble - 0x08));
			} break;
		} break;
	case 0x06:
		switch (ucLNibble) {
		case 0x00: ret1(X51_INS_JZ, 2, 2, 3, X51_OP_Rel);
		case 0x01: ret1(X51_INS_AJMP, 2, 2, 3, X51_OP_P3);
		case 0x02: ret2(X51_INS_XRL, 2, 1, 2, X51_OP_Dir, X51_OP_A);
		case 0x03: ret2(X51_INS_XRL, 3, 2, 3, X51_OP_Dir, X51_OP_Data8);
		default:
			switch (ucLNibble) {
			case 0x04: ret2(X51_INS_XRL, 2, 1, 2, X51_OP_A, X51_OP_Data8);
			case 0x05: ret2(X51_INS_XRL, 2, 1, 2, X51_OP_A, X51_OP_Dir);
			case 0x06: case 0x07: ret2(X51_INS_XRL, 1, 1, 2, X51_OP_A, X51_OP_iR0 + (ucLNibble - 0x06));
			default: ret2(X51_INS_XRL, 1, 1, 1, X51_OP_A, X51_OP_R0 + (ucLNibble - 0x08));
			} break;
		} break;
	case 0x07:
		switch (ucLNibble) {
		case 0x00: ret1(X51_INS_JNZ, 2, 2, 3, X51_OP_Rel);
		case 0x01: ret1(X51_INS_ACALL, 2, 2, 3, X51_OP_P3);
		case 0x02: ret2(X51_INS_ORL, 2, 2, 2, X51_OP_C, X51_OP_Bit);
		case 0x03: ret1(X51_INS_JMP, 1, 1, 2, X51_OP_iADPTR);
		default:
			switch (ucLNibble) {
			case 0x04: ret2(X51_INS_MOV, 2, 1, 2, X51_OP_A, X51_OP_Data8);
			case 0x05: ret2(X51_INS_MOV, 3, 2, 3, X51_OP_Dir, X51_OP_Data8);
			case 0x06: case 0x07: ret2(X51_INS_MOV, 2, 1, 2, X51_OP_iR0 + (ucLNibble - 0x06), X51_OP_Data8);
			default: ret2(X51_INS_MOV, 2, 1, 2, X51_OP_R0 + (ucLNibble - 0x08), X51_OP_Data8);
			} break;
		} break;
	case 0x08:
		switch (ucLNibble)
		{
		case 0x00: ret1(X51_INS_SJMP, 2, 2, 3, X51_OP_Rel);
		case 0x01: ret1(X51_INS_AJMP, 2, 2, 3, X51_OP_P4);
		case 0x02: ret2(X51_INS_ANL, 2, 2, 2, X51_OP_C, X51_OP_Bit);
		case 0x03: ret2(X51_INS_MOVC, 1, 2, 3, X51_OP_A, X51_OP_iAPC);
		case 0x04: ret1(X51_INS_DIV, 2, 4, 4, X51_OP_AB);
		default:
			switch (ucLNibble) {
			case 0x05: ret2(X51_INS_MOV, 3, 2, 3, X51_OP_Dir, X51_OP_Dir);
			case 0x06: case 0x07: ret2(X51_INS_MOV, 2, 2, 2, X51_OP_Dir, X51_OP_iR0 + (ucLNibble - 0x06));
			default: ret2(X51_INS_MOV, 2, 2, 2, X51_OP_Dir, X51_OP_R0 + (ucLNibble - 0x08));
			} break;
		} break;
	case 0x09:
		switch (ucLNibble) {
		case 0x00: ret2(X51_INS_MOV, 3, 2, 3, X51_OP_DPTR, X51_OP_Data16);
		case 0x01: ret1(X51_INS_ACALL, 2, 2, 3, X51_OP_P4);
		case 0x02: ret2(X51_INS_MOV, 2, 2, 2, X51_OP_Bit, X51_OP_C);
		case 0x03: ret2(X51_INS_MOVC, 1, 2, 3, X51_OP_A, X51_OP_iADPTR);
		default:
			switch (ucLNibble) {
			case 0x04: ret2(X51_INS_SUBB, 2, 1, 2, X51_OP_A, X51_OP_Data8);
			case 0x05: ret2(X51_INS_SUBB, 2, 1, 2, X51_OP_A, X51_OP_Dir);
			case 0x06: case 0x07: ret2(X51_INS_SUBB, 1, 1, 2, X51_OP_A, X51_OP_iR0 + (ucLNibble - 0x06));
			default: ret2(X51_INS_SUBB, 1, 1, 1, X51_OP_A, X51_OP_R0 + (ucLNibble - 0x08));
			} break;
		} break;
	case 0x0A:
		switch (ucLNibble)
		{
		case 0x00: ret2(X51_INS_ORL, 2, 2, 2, X51_OP_C, X51_OP_Bit);
		case 0x01: ret1(X51_INS_AJMP, 2, 2, 3, X51_OP_P5);
		case 0x02: ret2(X51_INS_MOV, 2, 1, 2, X51_OP_C, X51_OP_Bit);
		case 0x03: ret1(X51_INS_INC, 1, 2, 2, X51_OP_DPTR);
		case 0x04: ret1(X51_INS_MUL, 1, 4, 2, X51_OP_AB);
		case 0x05: ret0(X51_INS_0, 1, 1, 1);
		default:
			switch (ucLNibble) {
			case 0x06: case 0x07: ret2(X51_INS_MOV, 2, 2, 2, X51_OP_iR0 + (ucLNibble - 0x06), X51_OP_Dir);
			default: ret2(X51_INS_MOV, 2, 2, 2, X51_OP_R0 + (ucLNibble - 0x08), X51_OP_Dir);
			} break;
		} break;
	case 0x0B:
		switch (ucLNibble)
		{
		case 0x00: ret2(X51_INS_ANL, 2, 2, 2, X51_OP_C, X51_OP_Bit);
		case 0x01: ret1(X51_INS_ACALL, 2, 2, 3, X51_OP_P5);
		case 0x02: ret1(X51_INS_CPL, 2, 1, 2, X51_OP_Bit);
		case 0x03: ret1(X51_INS_CPL, 1, 1, 1, X51_OP_C);
		default:
			switch (ucLNibble) {
			case 0x04: ret3(X51_INS_CJNE, 3, 2, 4, X51_OP_A, X51_OP_Data8, X51_OP_Rel);
			case 0x05: ret3(X51_INS_CJNE, 3, 2, 4, X51_OP_A, X51_OP_Dir, X51_OP_Rel);
			case 0x06: case 0x07: ret3(X51_INS_CJNE, 3, 2, 4, X51_OP_A, X51_OP_iR0 + (ucLNibble - 0x06), X51_OP_Rel);
			default: ret3(X51_INS_CJNE, 3, 2, 4, X51_OP_A, X51_OP_R0 + (ucLNibble - 0x08), X51_OP_Rel);
			} break;
		} break;
	case 0x0C:
		switch (ucLNibble) {
		case 0x00: ret1(X51_INS_PUSH, 2, 2, 2, X51_OP_Dir);
		case 0x01: ret1(X51_INS_AJMP, 2, 2, 3, X51_OP_P6);
		case 0x02: ret1(X51_INS_CLR, 2, 1, 2, X51_OP_Bit);
		case 0x03: ret1(X51_INS_CLR, 1, 1, 1, X51_OP_C);
		case 0x04: ret1(X51_INS_SWAP, 1, 1, 1, X51_OP_A);
		default:
			switch (ucLNibble) {
			case 0x05: ret2(X51_INS_XCH, 2, 1, 2, X51_OP_A, X51_OP_Dir);
			case 0x06: case 0x07: ret2(X51_INS_XCH, 1, 1, 2, X51_OP_A, X51_OP_iR0 + (ucLNibble - 0x06));
			default: ret2(X51_INS_XCH, 1, 1, 1, X51_OP_A, X51_OP_R0 + (ucLNibble - 0x08));
			} break;
		} break;
	case 0x0D:
		switch (ucLNibble) {
		case 0x00: ret1(X51_INS_POP, 2, 2, 2, X51_OP_Dir);
		case 0x01: ret1(X51_INS_ACALL, 2, 2, 3, X51_OP_P6);
		case 0x02: ret1(X51_INS_SETB, 2, 1, 2, X51_OP_Bit);
		case 0x03: ret1(X51_INS_SETB, 1, 1, 1, X51_OP_C);
		case 0x04: ret1(X51_INS_DA, 1, 1, 1, X51_OP_A);
		case 0x05: ret2(X51_INS_DJNZ, 3, 2, 4, X51_OP_Dir, X51_OP_Rel);
		case 0x06: case 0x07: ret2(X51_INS_XCHD, 1, 1, 2, X51_OP_A, X51_OP_iR0 + (ucLNibble - 0x06));
		default: ret2(X51_INS_DJNZ, 2, 2, 3, X51_OP_R0 + (ucLNibble - 0x08), X51_OP_Rel);
		} break;
	case 0x0E:
		switch (ucLNibble) {
		case 0x00: ret2(X51_INS_MOVX, 1, 2, 1, X51_OP_A, X51_OP_iDPTR);
		case 0x01: ret1(X51_INS_AJMP, 2, 2, 3, X51_OP_P7);
		case 0x02: ret2(X51_INS_MOVX, 1, 2, 1, X51_OP_A, X51_OP_iR0);
		case 0x03: ret2(X51_INS_MOVX, 1, 2, 1, X51_OP_A, X51_OP_iR1);
		case 0x04: ret1(X51_INS_CLR, 1, 1, 1, X51_OP_A);
		default:
			switch (ucLNibble) {
			case 0x05: ret2(X51_INS_MOV, 2, 1, 2, X51_OP_A, X51_OP_Dir);
			case 0x06: case 0x07: ret2(X51_INS_MOV, 1, 1, 2, X51_OP_A, X51_OP_iR0 + (ucLNibble - 0x06));
			default: ret2(X51_INS_MOV, 1, 1, 1, X51_OP_A, X51_OP_R0 + (ucLNibble - 0x08));
			} break;
		} break;
	case 0x0F:
		switch (ucLNibble) {
		case 0x00: ret2(X51_INS_MOVX, 1, 2, 1, X51_OP_iDPTR, X51_OP_A);
		case 0x01: ret1(X51_INS_ACALL, 2, 2, 3, X51_OP_P7);
		case 0x02: ret2(X51_INS_MOVX, 1, 2, 1, X51_OP_iR0, X51_OP_A);
		case 0x03: ret2(X51_INS_MOVX, 1, 2, 1, X51_OP_iR1, X51_OP_A);
		case 0x04: ret1(X51_INS_CPL, 1, 1, 1, X51_OP_A);
		default:
			switch (ucLNibble) {
			case 0x05: ret2(X51_INS_MOV, 2, 1, 2, X51_OP_Dir, X51_OP_A);
			case 0x06: case 0x07: ret2(X51_INS_MOV, 1, 1, 1, X51_OP_iR0 + (ucLNibble - 0x06), X51_OP_A);
			default: ret2(X51_INS_MOV, 1, 1, 1, X51_OP_R0 + (ucLNibble - 0x08), X51_OP_A);
			} break;
		} break;
	}
	if (piIns) *piIns = iIns;
	if (piOp0) *piOp0 = iOp0;
	if (piOp1) *piOp1 = iOp1;
	if (piOp2) *piOp2 = iOp2;
	if (piBytes) *piBytes = iBytes;
	if (piCyclesX1) *piCyclesX1 = iCyclesX1;
	if (piCyclesSC) *piCyclesSC = iCyclesSC;
	return iBytes;
#undef ret0
#undef ret1
#undef ret2
#undef ret3
}

int compare_val(const void* p1, const void* p2)
{
	int i1 = ((SStrInt*)p1)->i;
	int i2 = ((SStrInt*)p2)->i;
	return (i1 == i2)?0:((i1 > i2)?1:-1);
}

int compare_str(const void* p1, const void* p2)
{
	char* pc1 = ((SStrInt*)p1)->pc;
	char* pc2 = ((SStrInt*)p2)->pc;
	return strcmp(pc1, pc2);
}

int f_bit32(int v)
{
	for (int i = 0; i < 32; i++)
		if (v & 1) return i;
		else v >>= 1;
	return -1;
}

int l_bit32(int v)
{
	for (int i = 31; i >= 0; i--)
		if (v & (1 << 31)) return i;
		else v <<= 1;
	return -1;
}

int x_bit32(int v)
{
	int i;
	if (v < 0x10000) i = f_bit32(v);
	if (v >= 0x10000) i = l_bit32(v);
	return (v == (1 << i))?i:-1;	
}

int printf_bin(int val, int bits)
{
	char c[33];
	if (bits < 0) return 0;
	if (bits > 32) bits = 32;
	for (int bit = 0; bit < bits; bit++)
	{
		c[bits - bit - 1] = (val & 1)?'1':'0';
		val >>= 1;
	}
	c[bits] = 0;
	printf(c);
	return bits;
}
