////////////////////////////////////////////////////////////////////////////////
// X51das.c
// X51 disassembler source file


#include "X51das.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern SX51Ins* g_psInsTab;

//Table of instruction names
char* g_pcInsStrTab[X51_INS_MAX + 1] = {"???","ACALL","ADD","ADDC","AJMP","ANL","ANLN","CJNE","CLR","CPL","DA","DEC","DIV","DJNZ","INC","JB","JBC","JC","JMP","JNB","JNC","JNZ","JZ","LCALL","LJMP","MOV","MOVC","MOVX","MUL","NOP","ORL","ORLN","POP","PUSH","RET","RETI","RL","RLC","RR","RRC","SETB","SJMP","SUBB","SWAP","XCH","XCHD","XRL","?A5"};

//Table of operand strings
char* g_pcOpStrTab[X51_OP_MAX + 1] = {"","C","A","AB","DPTR","R0","R1","R2","R3","R4","R5","R6","R7","@A","@DPTR","@A+DPTR","@A+PC","@R0","@R1","Bit","Dir","Rel","P0","P1","P2","P3","P4","P5","P6","P7","#Data8","Addr16","#Data16"};

int X51Das_Op(int iFlags, unsigned char* pucCode, unsigned short usAddr, char* pcAsm, int* piAsm, char cOp, FX51Value2Name* pValue2Name, void* pParam)
{
	int n = 0; //result of sprintf (number of characters printed)
	int iCnt = 0; //count of bytes processed (0, 1 or 2)
	int iVal = 0; //operand value
	char* pcName = 0; //symbol name
	unsigned char ucVal = 0; //operand value 8bit
	unsigned short usVal = 0; //operand value 16bit
	if ((cOp > X51_OP_0) && (cOp < X51_OP_1))
		n = sprintf(pcAsm, "%s", X51Das_OpStr(cOp));
	else if ((cOp >= X51_OP_1) && (cOp < X51_OP_2))
	{
		ucVal = pucCode[usAddr];
		if ((cOp >= X51_OP_P0) && (cOp <= X51_OP_P7))
		{
			usVal = ucVal | ((cOp - X51_OP_P0) << 8);
			if (pValue2Name) pcName = (*pValue2Name)(pParam, 'C', usVal);
			if (pcName) n = sprintf(pcAsm, "%s ($%04x)", pcName, usVal);
			else  n = sprintf(pcAsm, "$%04x", usVal);
		}
		else
			switch (cOp)
			{
			case X51_OP_Bit:
				if (pValue2Name) pcName = (*pValue2Name)(pParam, 'B', ucVal);
				if (pcName) n = sprintf(pcAsm, "%s", pcName);
				else
				{
					if (pValue2Name) pcName = (*pValue2Name)(pParam, 'D', (ucVal < 0x80)?((ucVal >> 3) + 0x20):(ucVal & 0xf8));
					if (pcName) n = sprintf(pcAsm, "%s.%d", pcName, ucVal & 7);
					else n = sprintf(pcAsm, "$%02x", ucVal);
				}
				break;
			case X51_OP_Dir:
				if (pValue2Name) pcName = (*pValue2Name)(pParam, 'D', ucVal);
				if (pcName) n = sprintf(pcAsm, "%s", pcName);
				else n = sprintf(pcAsm, "$%02x", ucVal);
				break;
			case X51_OP_Data8:
				if (pValue2Name) pcName = (*pValue2Name)(pParam, 'N', ucVal);
				if (pcName) n = sprintf(pcAsm, "%s", pcName);
				else n = sprintf(pcAsm, "#$%02x", ucVal);
				break;
			case X51_OP_Rel:
				usVal = usAddr + (signed char)ucVal + 1;
				if (pValue2Name) pcName = (*pValue2Name)(pParam, 'C', usVal);
				if (pcName)
				{
					if (iFlags & X51_DAS_FLG_INRELA)
						n = sprintf(pcAsm, "%s ($%04x)", pcName, usVal);
					else
						n = sprintf(pcAsm, "%s", pcName);
				}
				else
				{					
					if (iFlags & X51_DAS_FLG_INRELA)
						n = sprintf(pcAsm, "$%c%x ($%04x)", (ucVal & 0x80)?'-':'+', (ucVal & 0x80)?0x80 - (ucVal & 0x7f):ucVal, usVal);
					else
						n = sprintf(pcAsm, "$%c%x", (ucVal & 0x80)?'-':'+', (ucVal & 0x80)?0x80 - (ucVal & 0x7f):ucVal, usVal);
				}
				break;
			}
		iCnt = 1;
	}
	else if ((cOp >= X51_OP_2) && (cOp <= X51_OP_MAX))
	{
		unsigned short usVal = pucCode[usAddr + 1] | (pucCode[usAddr] << 8);
		if (cOp == X51_OP_Data16)
			n = sprintf(pcAsm, "#$%04x", usVal);
		else if (cOp == X51_OP_Addr16)
		{
			if (pValue2Name) pcName = (*pValue2Name)(pParam, 'C', usVal);
			if (pcName) n = sprintf(pcAsm, "%s ($%04x)", pcName, usVal);
			else n = sprintf(pcAsm, "$%04x", usVal);
		}
		iCnt = 2;
	}
e0:
	if (piAsm) *piAsm = n;
	return iCnt;
}

int X51Das_Ins(int iFlags, unsigned char* pucCode, unsigned short usAddr, char* pcAsm, int* piAsm, FX51Value2Name* pValue2Name, void* pParam)
{
	int n = 0;
	SX51Ins* psIns = g_psInsTab + pucCode[usAddr];
	int iCnt = 1;
	if (iFlags & X51_DAS_FLG_INADDR) n = sprintf(pcAsm, "%04x ", usAddr);
	int iAsm = n;
	if (iFlags & X51_DAS_FLG_INCODE)
		switch (psIns->len)
		{
		case 1: n = sprintf(pcAsm + iAsm, "%02x     ", pucCode[usAddr+0]); break;
		case 2: n = sprintf(pcAsm + iAsm, "%02x%02x   ", pucCode[usAddr+0], pucCode[usAddr+1]); break;
		case 3: n = sprintf(pcAsm + iAsm, "%02x%02x%02x ", pucCode[usAddr+0], pucCode[usAddr+1], pucCode[usAddr+2]); break;
		}
	iAsm += n;
	n = sprintf(pcAsm + iAsm, "%s", X51Das_InsStr(psIns->ins));
	if (psIns->op0) while (n <= 5) pcAsm[iAsm + (n++)] = ' ';
	iAsm += n;
	if (psIns->op0)
	{
		iCnt += X51Das_Op(iFlags, pucCode, usAddr + iCnt, pcAsm + iAsm, &n, psIns->op0, pValue2Name, pParam);
		iAsm += n;
	}
	if (psIns->op1)
	{
		iAsm += sprintf(pcAsm + iAsm, ", ");
		iCnt += X51Das_Op(iFlags, pucCode, usAddr + iCnt, pcAsm + iAsm, &n, psIns->op1, pValue2Name, pParam);
		iAsm += n;
	}
	if (psIns->op2)
	{
		iAsm += sprintf(pcAsm + iAsm, ", ");
		iCnt += X51Das_Op(iFlags, pucCode, usAddr + iCnt, pcAsm + iAsm, &n, psIns->op2, pValue2Name, pParam);
		iAsm += n;
	}
	if (piAsm) *piAsm = iAsm;
	return iCnt;
}

int X51Das_ToFile(void* pFile, int iFlags, unsigned char* pucCode, int iAddrStart, int iAddrEnd, int iMaxInsCnt, FX51Value2Name* pValue2Name, void* pParam)
{
	char cAsm[256] = {0};
	int iAddr = iAddrStart;
	int iInsCnt = 0;
	int iSize = 0;
	while ((iAddr < iAddrEnd) && (iMaxInsCnt--))
	{
		int iAsm = 0;
		if (iFlags & X51_DAS_FLG_INLABS)
		{
			char* pcName = 0;
			if (pValue2Name) pcName = (*pValue2Name)(pParam, 'C', iAddr);
			if (pcName)
			{
				iAsm += sprintf(cAsm + iAsm, "%s:", pcName);
				fprintf((FILE*)pFile, "%s\n", cAsm);
			}
		}
		int iCnt = X51Das_Ins(iFlags, pucCode, iAddr, cAsm, &iAsm, pValue2Name, pParam);
		fprintf((FILE*)pFile, "%s\n", cAsm);
		iAddr += iCnt;
	}
	return iSize;
}

char* X51Das_InsStr(int iIns)
{
	if ((iIns >= 0) && (iIns <= X51_INS_MAX)) return g_pcInsStrTab[iIns];
	return 0;
}

char* X51Das_OpStr(int iOp)
{
	if ((iOp >= 0) && (iOp <= X51_OP_MAX)) return g_pcOpStrTab[iOp];
	return 0;
}