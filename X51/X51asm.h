////////////////////////////////////////////////////////////////////////////////
// X51asm.h
// X51 assembler header file


#ifndef _X51ASM_H
#define _X51ASM_H

#include "X51cmn.h" //common header
#include "X51asmD.h" //assembler definition header
#include <limits.h> //limits header (PATH_MAX..)


//Structure for assembler context
typedef struct SX51Asm
{
	int pass; //current pass (1 or 2)
	int depth; //current include depth
	char filename[X51_MAX_INCDEPTH + 1][PATH_MAX + 1]; //filename array
	char line[X51_MAX_STR_LINE + 2]; //current line
	int linenum; //current line number (0-based)
	int linepos; //current position in line (0-based)
	char token[X51_MAX_STR_TOKEN + 2]; //current token
	int tokennum; //current token number (0-based)
	int tokentyp[X51_MAX_TOKEN + 1];//token type array in current line
	int token0;
} SX51Asm;


//--------------------------------------
//assembler functions



SX51Asm* X51Asm_Init(char* pcFileName);
void X51Asm_Done(SX51Asm* pAsm);
int X51Asm_Make(SX51Asm* pAsm, void* pFileOut);
int X51Asm_GetErr(SX51Asm* pAsm, char** ppcFileName, char** ppcLine, char** ppcToken, int* piLine, int* piPos);

int X51_Asm_Str2Tok(SX51Asm* pAsm, char* pc);

//evaluate expression
int X51Asm_Eval(SX51Asm* pAsm, char* pcExpr, int* iRes);
//assembly token
int X51Asm_Token(SX51Asm* pAsm, char* pcToken, int iLen);
//assembly line
int X51Asm_Line(SX51Asm* pAsm, char* pcLine, int iLen);
//assembly file
int X51Asm_File(SX51Asm* pAsm, char* pcFileName);

//assembly operand
int X51Asm_Op(int iFlags, int* piVal, unsigned short usAddr, char* pcAsm, int* piAsm, int* piOpMsk, FX51Name2Value* pName2Value, void* pParam);
//assembly instruction
int X51Asm_Ins(int iFlags, unsigned char* pucCode, unsigned short usAddr, char* pcAsm, int* piAsm, FX51Name2Value* pName2Value, void* pParam);

//parses operand string and returns operand mask (set of possible operand types) and fills piVal with the operand value
int X51Asm_Str2OpMskAndVal(char* pc, int* piVal);
//parses instruction name and returns instruction type (e.g. X51_INS_JMP) or 0 if not match
int X51Asm_Str2Ins(char* pc);

//returns token type string (e.g. "INSTR")
char* X51Asm_TkTStr(int iTkT);
//returns directive string (e.g. "INCLUDE")
char* X51Asm_DirStr(int iDir);
//returns statement string (e.g. "ORG")
char* X51Asm_StaStr(int iSta);
//returns error string (e.g. "INVCHR")
char* X51Asm_ErrStr(int iRes);
//returns error message (e.g. "Invalid character")
char* X51Asm_ErrMsg(int iRes);


#endif //_X51ASM_H