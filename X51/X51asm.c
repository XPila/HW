////////////////////////////////////////////////////////////////////////////////
// X51asm.c
// X51 assembler source file


#include "X51cmn.h"
#include "X51asm.h"
#include "X51das.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


extern SX51Ins* g_psInsTab; //declared in X51.c

extern SX51InsOp* g_psInsOpTab; //declared in X51.c

//Line scan string
char g_cLScan[X51_MAX_STR_SCAN + 1] = "";

//Token scan string
char g_cTScan[X51_MAX_STR_SCAN + 1] = "";


SX51Asm* X51Asm_Init(char* pcPath)
{
	//generate format string for scanning line
	//it will contain "%x[^\r\n]%n%1*[\r]%1*[\n]", x is max line length + 1 (for overflow checking)
	//it means:
	//  "%x[^\r\n]" -store all chars stop on CR or LF
	//  "%n"        -store number of chars scanned
	sprintf(g_cLScan, "%%%d[^\r\n]%%n", X51_MAX_STR_LINE + 1);
	sprintf(g_cTScan, "%%%d[^ \t]%%n", X51_MAX_STR_TOKEN + 1);
	
	SX51Asm* pAsm = (SX51Asm*)malloc(sizeof(SX51Asm));
	if (!pAsm) return 0;
	memset(pAsm, 0, sizeof(SX51Asm));
	strcpy(pAsm->filename[0], pcPath);
	return pAsm;
}

void X51Asm_Done(SX51Asm* pAsm)
{
	if (!pAsm) return;
	free(pAsm);
}

int X51Asm_Make(SX51Asm* pAsm, void* pOut)
{
	int iRet = 0;
	pAsm->pass = 1;
	pAsm->depth = 0;
	iRet = X51Asm_File(pAsm, pAsm->filename[0]);
	if (iRet < 0) return iRet;
//	pAsm->pass = 2;
//	pAsm->depth = 0;
//	iRet = X51Asm_File(pAsm, pAsm->filename[0]);
	return iRet;
}

int X51Asm_GetErr(SX51Asm* pAsm, char** ppcFileName, char** ppcLine, char** ppcToken, int* piLine, int* piPos)
{
	if (!pAsm) return - 1;
	if (ppcFileName) *ppcFileName = pAsm->filename[pAsm->depth];
	if (ppcLine) *ppcLine = pAsm->line;
	if (ppcToken) *ppcToken = pAsm->token;
	if (piLine) *piLine = pAsm->linenum + 1;
	if (piPos) *piPos = pAsm->linepos + 1;
	return 0;
}

int X51_Asm_Str2Tok(SX51Asm* pAsm, char* pc)
{
	switch (pc[0])
	{
	case '\'': return X51_ASM_TKT_STRCN;
	case '$':
		switch (pc[1]) {
		case 'I': return (strcmp(pc + 2, "NCLUDE") == 0)?((X51_ASM_TKT_DIREC << 8) || X51_ASM_DIR_INCLUDE):0;
		default: return (sscanf(pc + 1, "%x") == 1)?(X51_ASM_TKT_NUMCN << 8):0;
		} break;
	}
	return 0;
}

int X51_sscanf_par(char* str, char* fmt, void* pv, int* pn)
{
	char* pc = str;
	int c = 0, n = 0;
	//skip whitespace
	n = 0; sscanf(pc, "%*[ \t]%n", &n); pc += n;
	//match '('
	n = 0; sscanf(pc, "(%n", &n); pc += n;
	if (n != 1) goto e0;
	//skip whitespace
	n = 0; sscanf(pc, "%*[ \t]%n", &n); pc += n;
	//parse variable
	n = 0; c = sscanf(pc, fmt, pv, &n); pc += n;
	if (c < 1) goto e0;
	//skip whitespace
	n = 0; sscanf(pc, "%*[ \t]%n", &n); pc += n;
	//match ')'
	n = 0; sscanf(pc, ")%n", &n); pc += n;
	if (n != 1) c = 0;
e0:
	if (pn) *pn = pc - str + 1;
	return c;
}

int X51_Asm_Dir_INCLUDE(SX51Asm* pAsm, char* pcToken)
{
	int c = 0, n = 0;
	char* pc = pcToken;
	char cFileName[PATH_MAX + 1] = {0};
	//parse filename in parenthesis
	c = X51_sscanf_par(pcToken, "%[^)]%n", cFileName, &n); pc += n;
	if (!c) return X51_ASM_ERR_SYNTAX; //not parsed
	//skip whitespace
	n = 0; sscanf(pc, "%*[ \t]%n", &n); pc += n;
	if (*pc) return X51_ASM_ERR_INVCHR; //not end of line
	//cut whitespace at end of filename
	int len = strlen(cFileName); pc = cFileName + len;
	while ((pc > cFileName) && ((pc[-1] == ' ') || (pc[-1] == '\t'))) pc--;
	*pc = 0;//terminate with nul char
	printf("INCLUDE: '%s'\n", cFileName);
	return 0;
}

//pass 1
int X51_Asm_Token_P1(SX51Asm* pAsm, char* pcToken, int iLen)
{
/*	int iToken = X51_Asm_Str2Tok(pAsm, pcToken);
	if (iToken == 0) return 
	pAsm->token0 = X51_Asm_Str2Tok(pAsm, pcToken);
	switch (pAsm->token0)
	{
	case X51_ASM_KWD_INCLUDE: return 1;
	}*/
	return 0;
}

//pass 1, token n
int X51_Asm_P1_TokenN(SX51Asm* pAsm, char* pcToken, int iLen)
{
/*	switch (pAsm->token0)
	{
	case X51_ASM_KWD_INCLUDE: return X51_Asm_Dir_INCLUDE(pAsm, pcToken);
	}*/
}

int X51Asm_Token(SX51Asm* pAsm, char* pcToken, int iLen)
{
	strcpy(pAsm->token, pcToken); //copy current token to asm context
	printf("%2d %2d \"%s\" %d\n", pAsm->linenum, pAsm->tokennum, pcToken, iLen);
/*	if (pAsm->pass == 1)
	{
		switch (pAsm->tokennum)
		{
		case 0: return X51_Asm_P1_Token0(pAsm, pcToken, iLen);
		default: return X51_Asm_P1_TokenN(pAsm, pcToken, iLen);
		}			
	}*/
	return 0;
}

int X51Asm_Line(SX51Asm* pAsm, char* pcLine, int iLen)
{
	int iRet = 0; //return value
	int c = 0, n = 0; //variables for scanf
	char cToken[X51_MAX_STR_TOKEN + 2] = {0}; //buffer for token
	char* pcEnd = pcLine + iLen; //calculate line end pointer
	char* pcComment = 0; //comment pointer
	char* pcStrConst = 0; //constant begin pointer
	char* pcStrConstEnd = 0; //constant end pointer
	char* pc = pcLine; //current pointer
	strcpy(pAsm->line, pcLine); //copy current line to asm context
	pAsm->tokennum = 0; //reset token counter
	//following code finds comments and string constant
	//this is necesary to exclude comments and do not break string constants containing ';' char
	while (pcComment = strchr(pc, ';')) //loop while comment char found
	{
		while (pcStrConst = strchr(pc, '\'')) //loop while string constant char found
		{
			if (pcStrConst < pcComment) //comment char is maybe inside string constant
			{
				pcStrConstEnd = strchr(pcStrConst + 1, '\''); //find next string constant char
				//loop while string constant char found and next char is also string constant char
				//because of the rule: two chars "''" inside string constant means store one char ' and continue
				while (pcStrConstEnd && (pcStrConstEnd[1] == '\''))
					pcStrConstEnd = strchr(pcStrConstEnd + 2, '\''); //find next string constant char
				if (pcStrConstEnd) //string constant end found
				{
/*					char cStrConst[64];
					int cStrConstLen = pcStrConstEnd - pcStrConst + 1;
					strncpy(cStrConst, pcStrConst, pcStrConstEnd - pcStrConst + 1);
					cStrConst[cStrConstLen] = 0;
					printf("STR: '%s'\n", cStrConst);*/
					
					pcComment = 0; //set comment pointer to null because statments bellow
					pc = pcStrConstEnd + 1; //set current pointer to next char after string constant
					break; //end loop
				}
				else //string constant end not found
					return X51_ASM_ERR_INVSTR; //Error "Invalid string constant"
			}
			else //string constant char is inside comment
				break; //end loop
		}
		if (pcComment) //comment char found (and is not inside comments)
		{
			//exclude comment from line
			*pcComment = 0; //terminate the line with nul char at begining of comment
			pcEnd = pcComment; //set line end pointer
			iLen = pcEnd - pcLine; //calculate line length
			break; //end loop
		}
	}
	pc = pcLine; //set pointer at begining of line
	while (pc < pcEnd) //loop while not end of line
	{
		//skip whitespace
		n = 0; c = sscanf(pc, "%*[ \t]%n", &n); pc += n;
		//parse token
		n = 0; c = sscanf(pc, g_cTScan, cToken, &n);
//		printf("TOKEN:'%s'\n", cToken);
		if (c > 0) //token scanned
		{
			pcStrConst = strchr(cToken, '\''); // string constant char found inside token
			if (pcStrConst) //token contain string constant character
			{
				if (pcStrConst == cToken) //token is string constant (or begin of string constant because it can contain spaces)
				{
					pcStrConst = pc; //set string constant pointer to begin of token in pcLine
					pcStrConstEnd = strchr(pcStrConst + 1, '\''); //find next string constant char
					//loop while string constant char found and next char is also string constant char
					while (pcStrConstEnd && (pcStrConstEnd[1] == '\''))
						pcStrConstEnd = strchr(pcStrConstEnd + 2, '\''); //find next string constant char
					if (pcStrConstEnd) //string constant end found
					{
						n = pcStrConstEnd - pcStrConst + 1; //calculate length of string constant
						strncpy(cToken, pcStrConst, n); //copy string constant to token buffer
						cToken[n] = 0; //terminate token with nul char
					}
					else //string constant end not found
					{
						pAsm->linepos = pc - pcLine; //set line position counter to begin of string constant
						return X51_ASM_ERR_INVSTR; //Error "Invalid string constant"
					}
				}
				else //string constant character is not separated by whitespace
				{
					pAsm->linepos = (pc - pcLine) + (pcStrConst - cToken); //set line position counter to begin of string constant
					return X51_ASM_ERR_EXWHSP; //Error "Expected whitespace"
				}
			}
			if (n > 0)
			{
				pAsm->linepos = pc - pcLine; //set line position counter to begin token
				//assembly token
				iRet = X51Asm_Token(pAsm, cToken, n);
				if (iRet < 0) return iRet; //if X51_Asm_Token fail, return its result
				pAsm->tokennum++; //increment token counter
				pc += n; //set pointer to next char after token
				if (iRet > 0) //>0 means that next token will by all remainig chars
					break; //end loop
			}
		}
		else //token not scanned
			break; //end loop
	}
	pAsm->linepos = pc - pcLine; //set line position counter to remaining chars (or end of line)
	//at end is called X51_Asm_Token with pointer to remaining chars and its count
	return X51Asm_Token(pAsm, pc, pcEnd - pc);
}

int X51Asm_File(SX51Asm* pAsm, char* pcFileName)
{
	int iRet = 0; //return value
	int c = 0, n = 0; //variables for scanf
	char cLine[X51_MAX_STR_LINE + 2] = {0}; //line buffer
	FILE* pFile = fopen(pcFileName, "r"); //open the file
	if (!pFile) return X51_ASM_ERR_FILEIO; //Error "File I/O error"
	strcpy(pAsm->filename[pAsm->depth], pcFileName); //copy current filename to asm context
	pAsm->linenum = 0; //reset line counter
	while (!feof(pFile)) //loop until end of file
	{
		//parse line with scan string g_cLScan (generated in X51_Init)
		if ((c = fscanf(pFile, g_cLScan, cLine, &n)) < 0)
		{
			//not eof means other io error, eof is ok
			if (!feof(pFile)) iRet = X51_ASM_ERR_FILEIO; //Error "File I/O error"
			break;
		}
		//check line length
		if (n > X51_MAX_STR_LINE)
		{
			iRet = X51_ASM_ERR_LNTLNG; //Error "Line too long"
			break;
		}
		//assembly line
		if ((c > 0) && ((iRet = X51Asm_Line(pAsm, cLine, n)) < 0))
			break;
		pAsm->linenum++; //increment line number counter
		//skip one CR and one LF char (or CRLF sequence)
		if ((c = fscanf(pFile, "%1*[\r]")) < 0) //skip CR
		{
			//not eof means other io error, eof is ok
			if (!feof(pFile)) iRet = X51_ASM_ERR_FILEIO; //Error "File I/O error"
			break;
		}
		if ((c = fscanf(pFile, "%1*[\n]")) < 0) //skip LF
		{
			//not eof means other io error, eof is ok
			if (!feof(pFile)) iRet = X51_ASM_ERR_FILEIO; //Error "File I/O error"
			break;
		}
	}
	fclose(pFile); //close the file
	return iRet;
}

int X51Asm_Op(int iFlags, int* piOpVal, unsigned short usAddr, char* pcAsm, int* piAsm, int* piOpMsk, FX51Name2Value* pName2Value, void* pParam)
{
	int c = 0; //sscanf result (number of parameters parsed)
	int n = 0; //scanf result of "%n" (number of characters procesed)
	char cOp[X51_MAX_STR_OP + 2] = {0}; //bufer for operand
	int iOpMsk = 0; //operand mask
	int iOpVal = 0; //Operand value
	if (piAsm) *piAsm = n;
	//parse operand
	n = 0; c = sscanf(pcAsm, "%63[#$@0-9a-zA-Z_+-]%n", cOp, &n);
	printf("1 i=%d c=%d n=%d pc='%s'\n", 0, c, n, pcAsm + 0);
	if (c != 1) return X51_ASM_ERR_INVCHR;
	printf("Op = '%s'\n", cOp);
	iOpMsk = X51Asm_Str2OpMskAndVal(cOp, &iOpVal);
	if (iOpMsk == 0)
	{
		n = 0; c = sscanf(pcAsm + ((pcAsm[0] == '#')?1:0), "%1[a-zA-Z]%62[0-9a-zA-Z_]%n", cOp, cOp + 1, &n);
		if (c > 0)
		{
			if (c == 1) n = 1;
			if (pcAsm[0] == '#') n++;
			printf("SYMBOL: %s\n", cOp);
			if (pcAsm[0] == '#') iOpMsk |= (X51_OPM_Data8 | X51_OPM_Data16);
			else iOpMsk |= (X51_OPM_Bit | X51_OPM_Dir | X51_OPM_Addr16 | X51_OPM_Rel | X51_OPM_P_ALL);
		}
	}
	if (iOpMsk == 0) return X51_ASM_ERR_INVOP0;
	printf("OpMsk = 0x%08x\n", iOpMsk);
	printf("OpVal = %d\n", iOpVal);
	if (piOpVal) *piOpVal = iOpVal;
	if (piOpMsk) *piOpMsk = iOpMsk;
	if (piAsm) *piAsm = n;
	return 0;
}

int X51Asm_Ins(int iFlags, unsigned char* pucCode, unsigned short usAddr, char* pcAsm, int* piAsm, FX51Name2Value* pName2Value, void* pParam)
{
	int iLen = strlen(pcAsm); //length of string
	int i = 0; //current position in pcAsm
	int c = 0; //sscanf result (number of parameters parsed)
	int n = 0; //scanf result of "%n" (number of characters procesed)
	char cIns[X51_MAX_STR_INS + 2] = {0}; //buffer for instruction name
//	char cOp0[64] = {0}; //bufer for operand 0
//	char cOp1[64] = {0}; //--||-- 1
//	char cOp2[64] = {0}; //--||-- 2
	char cTmp[X51_MAX_STR_LINE + 1] = {0}; //temporary buffer
//	char* pcOp[3] = {cOp0, cOp1, cOp2}; //pointer array
	int iIns = 0; //instruction index
	int iOp = 0; //operand index
	int iOpCnt = 0; //operand count;
	int iOpV = 0; //operand variant index
	int iOpVCnt = 0; //operand variant count
	int iOpMsk[3] = {0, 0, 0}; //Operand masks
	int iOpVal[3] = {0, 0, 0}; //Operand values
	int iOpOK = 0; //Operands matched in any variant
	char cByteCnt = 1; //instruction byte count (default 1)
	unsigned char ucCode = 0; //instruction code
	unsigned long ulOpVar = -1; //operand variant
	
	//match valid characters
	n = 0; c = sscanf(pcAsm, "%*[ \t,#$@0-9a-zA-Z_+-]%n", &n);
	if (n != iLen) return X51_ASM_ERR_INVCHR;
	//parse instruction name
	n = 0; c = sscanf(pcAsm, "%6[A-Z]%n", cIns, &n); i = n;
	printf("0 i=%d c=%d n=%d pc='%s'\n", i, c, n, pcAsm + i);
	if (c == 0) return X51_ASM_ERR_SYNTAX;
	if (n > 5) return X51_ASM_ERR_UNKINS;
	if (pcAsm[i]) //not end of string
	{
		//skip whitespace
		n = 0; c = sscanf(pcAsm + i, "%*[ \t]%n", &n); i += n;
		//must be at least one whitespace char between instruction and operands
		if (n < 1) return X51_ASM_ERR_INVCHR;
		printf("x i=%d c=%d n=%d pc='%s'\n", i, c, n, pcAsm + i);
	}
		
	iIns = X51Asm_Str2Ins(cIns);
	if (iIns < 0) return X51_ASM_ERR_UNKINS;
	iOpCnt = g_psInsOpTab[iIns].opcnt;
	iOpVCnt = g_psInsOpTab[iIns].opvcnt;
//	printf("INS=%d %s\n", iIns, X51Asm_Ins2Str(iIns));
	printf("operands=%d\n", iOpCnt);
	printf("variants=%d\n", iOpVCnt);
	for (iOp = 0; iOp < iOpCnt; iOp++)
	{
		//check end of string
		if (!pcAsm[i]) return X51_ASM_ERR_EXP0OP - iOpCnt;
		//parse operand
		n = 0; c = X51Asm_Op(iFlags, iOpVal + iOp, usAddr + cByteCnt, pcAsm + i, &n, iOpMsk + iOp, pName2Value, pParam); i += n;
		//
		if (c < 0) return (c == X51_ASM_ERR_INVOP0)?c - iOp:c;
		if (iOp < (iOpCnt - 1)) //Not last operand
		{
			//skip whitespace
			n = 0; c = sscanf(pcAsm + i, "%*[ \t]%n", &n); i += n;
			printf("2 i=%d c=%d n=%d pc='%s'\n", i, c, n, pcAsm + i);
			//match ','
			n = 0; c = sscanf(pcAsm + i, ",%n", &n); i += n;
			printf("3 i=%d c=%d n=%d pc='%s'\n", i, c, n, pcAsm + i);
			if (n != 1) return X51_ASM_ERR_EXP0OP - iOpCnt;
			//skip whitespace
			n = 0; c = sscanf(pcAsm + i, "%*[ \t]%n", &n); i += n;
			printf("4 i=%d c=%d n=%d pc='%s'\n", i, c, n, pcAsm + i);
		}
	}
	if (pcAsm[i]) //not end of string
	{
		//skip whitespace
		n = 0; c = sscanf(pcAsm + i, "%*[ \t]%n", &n); i += n;
		//unexpected operand when ',' found
		if ((iOpCnt > 0) && (pcAsm[i] == ',')) return X51_ASM_ERR_EXP0OP - iOpCnt;
		//only whitespace allowed after instruction and operands
		if (n < iLen) return X51_ASM_ERR_INVCHR;
		printf("5 i=%d c=%d n=%d pc='%s'\n", i, c, n, pcAsm + i);
	}
	for (iOpV = 0; iOpV < iOpVCnt; iOpV++)
	{
		ulOpVar = g_psInsOpTab[iIns].opvars[iOpV];
		char* pcOpVar = (char*)&ulOpVar;
		printf("opv: %d  %06x ", iOpV, ulOpVar);
		for (iOp = 0; iOp < iOpCnt; iOp++)
		{
			int iMsk = (1 << (pcOpVar[iOp + 1] - 1));
			printf("msk%d %08x  ", iOp, iMsk);
			if ((iMsk & iOpMsk[iOp]) == 0) break;
			if (iOpOK < (iOp + 1)) iOpOK = iOp + 1;
		}
		printf(" %s (ok=%d)\n", (iOp == iOpCnt)?"OK":"NG", iOpOK);
		if (iOp == iOpCnt)
			break;
	}
	if (iOpV == iOpVCnt) return X51_ASM_ERR_INVOP0 - iOpOK;
	ucCode = g_psInsOpTab[iIns].opvars[iOpV] & 0xff;
	if (iOpCnt > 0) printf("OP0: %s\n", X51Das_OpStr(g_psInsTab[ucCode].op0));
	if (iOpCnt > 1) printf("OP1: %s\n", X51Das_OpStr(g_psInsTab[ucCode].op1));
	if (iOpCnt > 2) printf("OP2: %s\n", X51Das_OpStr(g_psInsTab[ucCode].op2));
		
/*		printf("opv: %d\n", iOpV);
		for (iOpV = 0; iOpV < iOpVCnt; iOpV++)
		{
			unsigned long ulOpVar = g_psInsOpTab[iIns].opvars[iOpV];
			printf("opv: %d  %06x \n", iOpV, ulOpVar);
		}*/
//	if (iOpCnt > 0) printf("OP0: %s\n", X51_Op2Str(g_psInsTab[ucCode].op0));
//	if (iOpCnt > 1) printf("OP1: %s\n", X51_Op2Str(g_psInsTab[ucCode].op1));
//	if (iOpCnt > 2) printf("OP2: %s\n", X51_Op2Str(g_psInsTab[ucCode].op2));
	
	//store code
	pucCode += usAddr;
	*(pucCode++) = ucCode; //instruction code
	printf("CODE: %02x", ucCode);
	SX51Ins* psIns = g_psInsTab + ucCode;
	char* pcOp = &psIns->op0;
	for (iOp = 0; iOp < iOpCnt; iOp++)
	{
		if (pcOp[iOp] >= X51_OP_2)
		{
			*(pucCode++) = iOpVal[iOp] >> 8;
			printf(" %02x", pucCode[-1]);
		}
		if (pcOp[iOp] >= X51_OP_1)
		{
			*(pucCode++) = iOpVal[iOp] & 0xff;
			printf(" %02x", pucCode[-1]);
		}
	}
	printf("\n");
				
	
//	  printf("OP0: %s\n", X51Das_OpStr(g_psInsTab[ucCode].op0));
	return psIns->len;
}

int X51Asm_Str2OpMskAndVal(char* pc, int* piVal)
{
	int iVal, n = 0;
	switch (pc[0])
	{
	case '@':
		switch (pc[1]) {
		case 'A':
			switch (pc[2]) {
			case '\0': return X51_OPM_iA;
			case '+':
				switch (pc[3]) {
				case 'D': return ((pc[4] == 'P') && (pc[5] == 'T') && (pc[6] == 'R') && (pc[7] == '\0'))?X51_OPM_iADPTR:0;
				case 'P': return ((pc[4] == 'C') && (pc[5] == '\0'))?X51_OPM_iAPC:0;
				} return 0;
			} return 0;
		case 'D': return ((pc[2] == 'P') && (pc[3] == 'T') && (pc[4] == 'R') && (pc[5] == '\0'))?X51_OPM_iDPTR:0;
		case 'R':
			switch (pc[2]) {
			case '0': return (pc[3] == '\0')?X51_OPM_iR0:0;
			case '1': return (pc[3] == '\0')?X51_OPM_iR1:0;
			} return 0;
		} return 0;
	case 'A':
		switch (pc[1]) {
		case '\0': return X51_OPM_A;
		case 'B': return (pc[2] == '\0')?X51_OPM_AB:0;
		} return 0;
	case 'C': return (pc[1] == '\0')?X51_OPM_C:0;
	case 'D': return ((pc[1] == 'P') && (pc[2] == 'T') && (pc[3] == 'R') && (pc[4] == '\0'))?X51_OPM_DPTR:0;
	case 'R':
		switch (pc[1]) {
		case '0': return (pc[2] == '\0')?X51_OPM_R0:0;
		case '1': return (pc[2] == '\0')?X51_OPM_R1:0;
		case '2': return (pc[2] == '\0')?X51_OPM_R2:0;
		case '3': return (pc[2] == '\0')?X51_OPM_R3:0;
		case '4': return (pc[2] == '\0')?X51_OPM_R4:0;
		case '5': return (pc[2] == '\0')?X51_OPM_R5:0;
		case '6': return (pc[2] == '\0')?X51_OPM_R6:0;
		case '7': return (pc[2] == '\0')?X51_OPM_R7:0;
		} return 0;
	case '#':
		if ((sscanf(pc + 1, (pc[1] == '$')?"$%x%n":"%d%n", &iVal, &n) != 1) || (pc[n + 1] != 0)) return 0;
		if ((iVal < 0) || (iVal >= 0x10000)) return 0;
		if (piVal) *piVal = iVal;
		if (iVal < 0x100) return (X51_OPM_Data8 | X51_OPM_Data16);
		return X51_OPM_Data16;
	case '$':
		if ((pc[1] == '+') || (pc[1] == '-'))
		{
			if ((sscanf(pc + 2, "%x%n", &iVal, &n) != 1) || (pc[n + 2] != 0)) return 0;
			if (iVal < 0) return 0;
			if ((pc[1] == '+') && (iVal > 0x7f)) return 0;
			if ((pc[1] == '-') && (iVal > 0x80)) return 0;
			if (piVal) *piVal = ((pc[1] == '-')?(-iVal):iVal);
			return X51_OPM_Rel;
		}
		if ((sscanf(pc + 1, "%x%n", &iVal, &n) != 1) || (pc[n + 1] != 0)) return 0;
		if ((iVal < 0) || (iVal >= 0x10000)) return 0;
		if (piVal) *piVal = iVal;
		if (iVal < 0x100) return (X51_OPM_Dir | X51_OPM_Bit | X51_OPM_Addr16 | X51_OPM_P_ALL);
		return X51_OPM_Addr16 | X51_OPM_P_ALL;
	default:
		if ((sscanf(pc, "%d%n", &iVal, &n) != 1) || (pc[n] != 0)) return 0;
		if ((iVal < 0) || (iVal >= 0x10000)) return 0;
		if (piVal) *piVal = iVal;
		if (iVal < 0x100) return (X51_OPM_Dir | X51_OPM_Bit | X51_OPM_Addr16 | X51_OPM_P_ALL);
		return X51_OPM_Addr16 | X51_OPM_P_ALL;
	}
	return 0;
}

int X51Asm_Str2Ins(char* pc)
{
	switch (pc[0]) {
	case 'A':
		switch (pc[1]) {
		case 'C': return ((pc[2] == 'A') && (pc[3] == 'L') && (pc[4] == 'L') && (pc[5] == '\0'))?X51_INS_ACALL:-1;
		case 'D':
			if (pc[2] == 'D')
				switch (pc[3]) {
				case '\0': return X51_INS_ADD;
				case 'C': return (pc[4] == 0)?X51_INS_ADDC:-1;
				} return -1;
		case 'J': return ((pc[2] == 'M') && (pc[3] == 'P') && (pc[4] == '\0'))?X51_INS_AJMP:-1;
		case 'N': return ((pc[2] == 'L') && (pc[3] == '\0'))?X51_INS_ANL:-1;
		} return -1;
	case 'C':
		switch (pc[1]) {
		case 'J': return ((pc[2] == 'N') && (pc[3] == 'E') && (pc[4] == '\0'))?X51_INS_CJNE:-1;
		case 'L': return ((pc[2] == 'R') && (pc[3] == '\0'))?X51_INS_CLR:-1;
		case 'P': return ((pc[2] == 'L') && (pc[3] == '\0'))?X51_INS_CPL:-1;
		} return -1;
	case 'D':
		switch (pc[1]) {
		case 'A': return X51_INS_DA;
		case 'E': return ((pc[2] == 'C') && (pc[3] == '\0'))?X51_INS_DEC:-1;
		case 'I': return ((pc[2] == 'V') && (pc[3] == '\0'))?X51_INS_DIV:-1;
		case 'J': return ((pc[2] == 'N') && (pc[3] == 'Z') && (pc[4] == '\0'))?X51_INS_DJNZ:-1;
		} return -1;
	case 'I': return ((pc[1] == 'N') && (pc[2] == 'C') && (pc[3] == '\0'))?X51_INS_INC:-1;
	case 'J':
		switch (pc[1]) {
		case 'B':
			switch (pc[2]) {
			case '\0': return X51_INS_JB;
			case 'C': return (pc[3] == 0)?X51_INS_JBC:-1;
			} return -1;
		case 'C': return (pc[2] == '\0')?X51_INS_JC:-1;
		case 'M': return ((pc[2] == 'P') && (pc[3] == '\0'))?X51_INS_JMP:-1;
		case 'N':
			switch (pc[2]) {
			case 'B': return (pc[3] == 0)?X51_INS_JNB:-1;
			case 'C': return (pc[3] == 0)?X51_INS_JNC:-1;
			case 'Z': return (pc[3] == 0)?X51_INS_JNZ:-1;
			} return -1;
		case 'Z': return (pc[2] == '\0')?X51_INS_JZ:-1;
		} return -1;
	case 'L':
		switch (pc[1]) {
		case 'C': return ((pc[2] == 'A') && (pc[3] == 'L') && (pc[4] == 'L') && (pc[5] == '\0'))?X51_INS_LCALL:-1;
		case 'J':  return ((pc[2] == 'M') && (pc[3] == 'P') && (pc[4] == '\0'))?X51_INS_LJMP:-1;
		} return -1;
	case 'M':
		switch (pc[1]) {
		case 'O':
			if (pc[2] == 'V')
				switch (pc[3]) {
				case '\0': return X51_INS_MOV;
				case 'C': return (pc[4] == 0)?X51_INS_MOVC:-1;
				case 'X': return (pc[4] == 0)?X51_INS_MOVX:-1;
				} return -1;
		case 'U':  return ((pc[2] == 'L') && (pc[3] == '\0'))?X51_INS_MUL:-1;
		} return -1;
	case 'N': return ((pc[1] == 'O') && (pc[2] == 'P') && (pc[3] == '\0'))?X51_INS_NOP:-1;
	case 'O': return ((pc[1] == 'R') && (pc[2] == 'L') && (pc[3] == '\0'))?X51_INS_ORL:-1;
	case 'P':
		switch (pc[1]) {
		case 'O': return ((pc[2] == 'P') && (pc[3] == '\0'))?X51_INS_POP:-1;
		case 'U': return ((pc[2] == 'S') && (pc[3] == 'H') && (pc[4] == '\0'))?X51_INS_PUSH:-1;
		} return -1;
	case 'R':
		switch (pc[1]) {
		case 'E':
			if (pc[2] == 'T')
				switch (pc[3]) {
				case '\0': return X51_INS_RET;
				case 'I': return (pc[4] == 0)?X51_INS_RETI:-1;
				} return -1;
		case 'L':
			switch (pc[2]) {
			case '\0': return X51_INS_RL;
			case 'C': return (pc[3] == 0)?X51_INS_RLC:-1;
			} return -1;
		case 'R':
			switch (pc[2]) {
			case '\0': return X51_INS_RR;
			case 'C': return (pc[3] == 0)?X51_INS_RRC:-1;
			} return -1;
		} return -1;
	case 'S':
		switch (pc[1]) {
		case 'E': return ((pc[2] == 'T') && (pc[3] == 'B') && (pc[4] == '\0'))?X51_INS_SETB:-1;
		case 'J':  return ((pc[2] == 'M') && (pc[3] == 'P') && (pc[4] == '\0'))?X51_INS_SJMP:-1;
		case 'U': return ((pc[2] == 'B') && (pc[3] == 'B') && (pc[4] == '\0'))?X51_INS_SUBB:-1;
		case 'W':  return ((pc[2] == 'A') && (pc[3] == 'P') && (pc[4] == '\0'))?X51_INS_SWAP:-1;
		} return -1;
	case 'X':
		switch (pc[1]) {
		case 'C':
			if (pc[2] == 'H')
				switch (pc[3]) {
				case '\0': return X51_INS_XCH;
				case 'D': return (pc[4] == 0)?X51_INS_XCHD:-1;
				} return -1;
		case 'R':  return ((pc[2] == 'L') && (pc[3] == '\0'))?X51_INS_XRL:-1;
		} return -1;
	}
	return -1;
}

char* X51Asm_TkTStr(int iTkT)
{
	switch (iTkT)
	{
	case X51_ASM_TKT_DIREC: return "DIREC";
	case X51_ASM_TKT_CTLST: return "CTLST";
	case X51_ASM_TKT_INSTR: return "INSTR";
	case X51_ASM_TKT_NUMCN: return "NUMCN";
	case X51_ASM_TKT_STRCN: return "STRCN";
	case X51_ASM_TKT_IDENT: return "IDENT";
	case X51_ASM_TKT_LABEL: return "LABEL";
	}
	return "?????";
}

char* X51Asm_DirStr(int iDir)
{
	switch (iDir)
	{
	case X51_ASM_DIR_INCLUDE: return "$INCLUDE";
	case X51_ASM_DIR_SET:     return "$SET";
	}
	return "??";
}

char* X51Asm_StaStr(int iSta)
{
	switch (iSta)
	{
	case X51_ASM_STA_NAME:    return "NAME";
	case X51_ASM_STA_ORG:     return "ORG";
	}
	return "?";
}

char* X51Asm_ErrStr(int iRes)
{
	switch (iRes)
	{
	case X51_ASM_ERR_SUCCES: return "SUCCES";
	case X51_ASM_ERR_SYNTAX: return "SYNTAX";
	case X51_ASM_ERR_INVCHR: return "INVCHR";
	case X51_ASM_ERR_UNKINS: return "UNKINS";
	case X51_ASM_ERR_EXP0OP: return "EXP0OP";
	case X51_ASM_ERR_EXP1OP: return "EXP1OP";
	case X51_ASM_ERR_EXP2OP: return "EXP2OP";
	case X51_ASM_ERR_EXP3OP: return "EXP3OP";
	case X51_ASM_ERR_INVOP0: return "INVOP0";
	case X51_ASM_ERR_INVOP1: return "INVOP1";
	case X51_ASM_ERR_INVOP2: return "INVOP2";
	case X51_ASM_ERR_SYMBNF: return "SYMBNF";
	case X51_ASM_ERR_LNTLNG: return "LNTLNG";
	case X51_ASM_ERR_FILEIO: return "FILEIO";
	case X51_ASM_ERR_INVSTR: return "INVSTR";
	case X51_ASM_ERR_EXWHSP: return "EXWHSP";
	case X51_ASM_ERR_EXEOLN: return "EXEOLN";
	}
	return "???";
}

char* X51Asm_ErrMsg(int iRes)
{
	return "";
}
