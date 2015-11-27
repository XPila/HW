////////////////////////////////////////
// XBuf.c

#include "XBuf.h"
#include <malloc.h>
#include <string.h>

SXBuf* XBuf_Init(SXBuf* pBuf, int iLen)
{
	SXBuf sBuf = {iLen, 0, 0, 0};
	if (!(sBuf.p = (char*)malloc(iLen))) return 0;
	if (!pBuf) pBuf = (SXBuf*)malloc(sizeof(SXBuf));
	if (pBuf) *pBuf = sBuf; else free(sBuf.p);
	return pBuf;
}

void XBuf_Done(SXBuf* pBuf, int iFree)
{
	if (!pBuf) return;
	if (pBuf->p) free(pBuf->p);
	if (iFree) free(pBuf);
}

int XBuf_Get(SXBuf* pBuf, void* pData, int iSize)
{
	if (!(pBuf && pBuf->p)) return -1;
	if (pBuf->iCnt <= 0) return 0;
	if (iSize > pBuf->iCnt) iSize = pBuf->iCnt;
	if (pData) memcpy(pData, pBuf->p + pBuf->iPos, iSize);
	pBuf->iCnt -= iSize;
	pBuf->iPos += iSize;
	if (pBuf->iCnt == 0) pBuf->iPos = 0;
	return iSize;
}

int XBuf_Put(SXBuf* pBuf, void* pData, int iSize)
{
	if (!(pBuf && pBuf->p)) return -1;
	if ((pBuf->iLen - pBuf->iCnt) <= 0) return 0;
	if (iSize > (pBuf->iLen - pBuf->iCnt)) iSize = (pBuf->iLen - pBuf->iCnt);
	if (iSize > (pBuf->iLen - (pBuf->iPos + pBuf->iCnt)))
	{
		if (pBuf->iCnt > 0) memcpy(pBuf->p, pBuf->p + pBuf->iPos, pBuf->iCnt);
		pBuf->iPos = 0;
	}
	if (pData) memcpy(pBuf->p + pBuf->iPos + pBuf->iCnt, pData, iSize);
	pBuf->iCnt += iSize;
	return iSize;
}
	
int XBuf_Set(SXBuf* pBuf, int iPos, int iCnt)
{
	if (!(pBuf && pBuf->p)) return -1;
	int iOldCnt = pBuf->iCnt;
	pBuf->iPos = iPos;
	pBuf->iCnt = iCnt;
	return iOldCnt;
}
