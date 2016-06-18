////////////////////////////////////////////////////////////////////////////////
// X51exp.h
// X51 expression evaluator header file (used in assembler)


#ifndef _X51EXP_H
#define _X51EXP_H


#define X51MOD_ERR_EXPOPR -1
#define X51MOD_ERR_EXPARG -2
#define X51MOD_ERR_EXPPAR -3
#define X51MOD_ERR_UIDENT -4
#define MAX_STR_IDENT 16

int X51Exp_Eval(char* pcExp, int iLen, int* piRes);

#endif //_X51EXP_H
