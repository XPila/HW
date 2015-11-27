////////////////////////////////////////
// XBuf.h

#ifndef _XBUF_H
#define _XBUF_H

#define XBuf_Clr(pBuf) XBuf_Set(pBuf, 0, 0)

typedef struct SXBuf
{
	int iLen;
	int iPos;
	int iCnt;
	char* p;
} SXBuf;

SXBuf* XBuf_Init(SXBuf* pBuf, int iLen);
void XBuf_Done(SXBuf* pBuf, int iFree);
int XBuf_Get(SXBuf* pBuf, void* pData, int iSize);
int XBuf_Put(SXBuf* pBuf, void* pData, int iSize);
int XBuf_Set(SXBuf* pBuf, int iPos, int iCnt);

#endif //_XBUF_H
