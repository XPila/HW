////////////////////////////////////////////////////////////////////////////////
// X51-AT89LPx052.h
// header file for chip family AT89LPx052


#ifndef _X51_AT89LPx052_H
#define _X51_AT89LPx052_H


int X51_AT89LPx052_Init();
void X51_AT89LPx052_Done();
char* X51_AT89LPx052_SFRValue2Name(void* pParam, char cType, int iVal);
int X51_AT89LPx052_SFRName2Value(void* pParam, char cType, char* pcName);

void* X51_AT89LPx052_SimInit(char cChipType, int iFreq);

#endif //_X51_AT89LPx052_H