////////////////////////////////////////////////////////////////////////////////
// X51exp.c
// X51 expression evaluator source file (used in assembler)


#include "X51exp.h"
#include <stdio.h>
#include <string.h>


int X51_Name2Value(void* pParam, char* pcName, int* piVal)
{
	if (strcmp(pcName, "ahoj") == 0)
	{
		*piVal = 100; return 0;
	}
	if (strcmp(pcName, "bhoj") == 0)
	{
		*piVal = 300; return 0;
	}
	return -1;
}

int X51Exp_Eval(char* pcExp, int iLen, int* piRes)
{
	char cName[MAX_STR_IDENT] = {0}; //buffer for identificators
	char cOprTab[] = "*&^/+-|"; //operator table sorted by priority
	int c = 0, n = 0; //variables for scanf etc.
	int iArg[10] = {0}; //arguments
	char cOpr[10] = {0}; //operators
	int iCnt = 0; //count of operators (arguments - 1)
	char* pcEnd = pcExp + iLen; //calculate end pointer
	char* pc = pcExp; //set pointer to first char
	while (pc < pcEnd) //loop while not end of expression
	{
		//skip whitespace
		n = 0; c = sscanf(pc, "%*[ \t]%n", &n); pc += n;
		if (cOpr[iCnt] == '?') //expected operator flag
		{
			if (strchr(cOprTab, *pc)) //match operator
			{
				cOpr[iCnt++] = *pc++; //store current char as operator, increment counter iCnt and pointer pc
				cOpr[iCnt] = 0; //set next operator to 0
				continue; //proces next chars
			}
			return X51MOD_ERR_EXPOPR; //error - expected operator
		}
		//process parenthesis recursively
		if (*pc == '(')
		{
			int depth = 1; //depth, number of "(" - number of ")"
			n = 0; //reset counter
			//find closing ")"
			while (((pc + (++n)) < pcEnd) && depth) //loop while not end and depth != 0 (closing parenthesis found)
				if (pc[n] == ')') depth--; //closing parenthesis - decrement depth
				else if (pc[n] == '(') depth++; //opening parenthesis - increment depth
			if (depth == 0) //closing parenthesis found
			{
				//evaluate inner part of the parenthesis
				int iRet = X51Exp_Eval(pc + 1, n - 2, iArg + iCnt);
				if (iRet != 0) return iRet;//if failed, return its result
				pc += n;//incremet pointer
				cOpr[iCnt] = '?';//set current operator flag - it menas next token must be operator
				continue;//proces next chars
			}
			return X51MOD_ERR_EXPPAR; //error - expected closing parenthesis
		}
		//scan integer decimal number
		n = 0; if ((c = sscanf(pc, "%d%n", iArg + iCnt, &n)) == 1)
		{
			pc += n;
			cOpr[iCnt] = '?';
			continue;
		}
		//scan integer hexadecimal number
		n = 0; if ((c = sscanf(pc, "$%x%n", iArg + iCnt, &n)) == 1)
		{
			pc += n;
			cOpr[iCnt] = '?';
			continue;
		}
		//scan identificator
		n = 0; if ((c = sscanf(pc, "%1[a-zA-Z]%15[_0-9a-zA-Z]%n", cName, cName + 1, &n)) == 2)
		{
			//translate identificator name to value
			if (X51_Name2Value(0, cName, iArg + iCnt) != 0)
				return X51MOD_ERR_UIDENT; //error - unknown identificator
			pc += n;
			cOpr[iCnt] = '?';
			continue;
		}
		return X51MOD_ERR_EXPARG; //error - expected argument
	}
	//process operators by priority
	if (cOpr[iCnt] != '?') return X51MOD_ERR_EXPARG;
	for (int j = 0; j < sizeof(cOprTab) - 1; j++)
		for (int i = 0; i <= iCnt; i++)
			if (cOpr[i] == cOprTab[j])
			{
				switch (cOpr[i])
				{
				case '*': iArg[i + 1] = iArg[i] * iArg[i + 1]; break;
				case '/': iArg[i + 1] = iArg[i] / iArg[i + 1]; break;
				case '+': iArg[i + 1] = iArg[i] + iArg[i + 1]; break;
				case '-': iArg[i + 1] = iArg[i] - iArg[i + 1]; break;
				case '&': iArg[i + 1] = iArg[i] & iArg[i + 1]; break;
				case '|': iArg[i + 1] = iArg[i] | iArg[i + 1]; break;
				case '^': iArg[i + 1] = iArg[i] ^ iArg[i + 1]; break;
				}
				//shift followed argument table entries (discard argument at current position and replace it with next)
				memcpy(iArg + i, iArg + i + 1, (iCnt - i + 1) * sizeof(int));
				//shift followed operator table entries (discard operator at current position and replace it with next)
				memcpy(cOpr + i, cOpr + i + 1, (iCnt - i + 1) * sizeof(char));
				iCnt--; //decrement count of operators
				i--; //decrement i, we need repeat processing of this operator in next loop because of table shifting
			}
	if (piRes) *piRes = iArg[0]; //the result value
	return 0; //OK
}

