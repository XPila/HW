////////////////////////////////////////////////////////////////////////////////
// X51das.h
// X51 disassembler header file


#ifndef _X51DAS_H
#define _X51DAS_H

#include "X51cmn.h"
#include "X51dasD.h"
#include <limits.h>


//disassembly operator
int X51Das_Op(int iFlags, unsigned char* pucCode, unsigned short usAddr, char* pcAsm, int* piAsm, char cOp, FX51Value2Name* pValue2Name, void* pParam);
//disassembly instruction
int X51Das_Ins(int iFlags, unsigned char* pucCode, unsigned short usAddr, char* pcAsm, int* piAsm, FX51Value2Name* pValue2Name, void* pParam);
//disassembly code
int X51Das_ToFile(void* pFile, unsigned char* pucCode, int iAddrStart, int iAddrEnd, int iMaxInsCnt, FX51Value2Name* pValue2Name, void* pParam);

//returns instruction string (e.g. "JMP")
char* X51Das_InsStr(int iIns);
//return operand string (e.g. "@A+DPTR")
char* X51Das_OpStr(int iOp);

#endif //_X51DAS_H
