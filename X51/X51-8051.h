////////////////////////////////////////////////////////////////////////////////
// X51-8051.h
// header file for chip 8051


#ifndef _X51_8051_H
#define _X51_8051_H


int X51_8051_Init();
void X51_8051_Done();
char* X51_8051_SFRValue2Name(void* pParam, char cType, int iVal);


#endif //_X51_8051_H