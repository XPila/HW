////////////////////////////////////////
// XHub.c

#include "XHub.h"
#include "XPort.h"
#include "XBuf.h"
#include <malloc.h>
#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>


typedef struct SXHubPort
{
	SXPort sPort;
	pthread_mutex_t sLock;
	SXHub* pHub;
	int iTimeout;
	SXBuf sBuf;
	SXPort* pLnkPort;
} SXHubPort;

typedef struct SXHub
{
	pthread_mutex_t sLnkLock;
	pthread_mutex_t sBufLock;
	pthread_cond_t sBufCond;
	int iPorts;
	SXHubPort sPort[];
} SXHub;

SXHubPort* XHubPort_Init(SXHubPort* pPort, SXHub* pHub, int iTimeout, int iBufLen, SXPort* pLnkPort);
void XHubPort_Done(SXHubPort* pPort);
int XHubPort_Rx(SXHubPort* pPort, void* pData, int iSize);
int XHubPort_Tx(SXHubPort* pPort, void* pData, int iSize);

char* hexstr(char* p, int n);
void* XHub_LogFile = 0;

SXHub* XHub_Init(int iPorts, int iTimeout, int iBufLen)
{
	SXHub* pHub = (SXHub*)malloc(sizeof(SXHub) + iPorts * sizeof(SXHubPort));
	if (!pHub) return 0;
	if (pthread_mutex_init(&pHub->sLnkLock, 0)) goto e0;
	if (pthread_mutex_init(&pHub->sBufLock, 0)) goto e1;
	if (pthread_cond_init(&pHub->sBufCond, 0)) goto e2;
	pHub->iPorts = iPorts;
	memset(pHub->sPort, iPorts * sizeof(SXHubPort), 0);
	int i; for (i = 0; i < pHub->iPorts; i++)
		if (!XHubPort_Init(pHub->sPort + i, pHub, iTimeout, iBufLen, 0)) goto e3;
	return pHub;
e3:	for (i = 0; i < pHub->iPorts; i++)
		XHubPort_Done(pHub->sPort + i);
	pthread_cond_destroy(&pHub->sBufCond);
e2:	pthread_mutex_destroy(&pHub->sBufLock);
e1:	pthread_mutex_destroy(&pHub->sLnkLock);
e0:	free(pHub);
	return 0;
}

void XHub_Done(SXHub* pHub)
{
	if (!pHub) return;
	int i; for (i = 0; i < pHub->iPorts; i++)
		XHubPort_Done(pHub->sPort + i);
	pthread_cond_destroy(&pHub->sBufCond);
	pthread_mutex_destroy(&pHub->sBufLock);
	pthread_mutex_destroy(&pHub->sLnkLock);
	free(pHub);
}

SXPort* XHub_Link(SXHub* pHub, int iPort, SXPort* pLnkPort)
{
	if (!pHub) return 0;
	if ((iPort < 0) || (iPort >= pHub->iPorts)) return 0;
	SXHubPort* pPort = pHub->sPort + iPort;
	pthread_mutex_lock(&pHub->sLnkLock);
	pthread_mutex_lock(&pPort->sLock);
	pPort->pLnkPort = pLnkPort;
	pthread_mutex_unlock(&pPort->sLock);
	pthread_mutex_unlock(&pHub->sLnkLock);
//	printf("port %d\r\n", &pPort->sPort);
	return &pPort->sPort;
}

void XHub_Stop(SXHub* pHub)
{
	if (!pHub) return;//0;
/*	if ((iPort < 0) || (iPort >= pHub->iPorts)) return 0;
	SXHubPort* pPort = pHub->sPort + iPort;
	pthread_mutex_lock(&pHub->sLnkLock);
	pthread_mutex_lock(&pPort->sLock);
	pPort->pLnkPort = pLnkPort;
	pthread_mutex_unlock(&pPort->sLock);
	pthread_mutex_unlock(&pHub->sLnkLock);
//	printf("port %d\r\n", &pPort->sPort);
	return &pPort->sPort;*/
}

int XHub_Put(SXHub* pHub, SXHubPort* pSrcPort, void* pData, int iSize)
{
	if (!pHub) return -1;
	pthread_mutex_lock(&pHub->sBufLock);
	SXHubPort* pPort;
	int i; for (i = 0; i < pHub->iPorts; i++)
		if ((pPort = pHub->sPort + i) != pSrcPort)
			XBuf_Put(&pPort->sBuf, pData, iSize);
	pthread_mutex_unlock(&pHub->sBufLock);
	pthread_cond_signal(&pHub->sBufCond);
	pthread_mutex_lock(&pHub->sLnkLock);
	for (i = 0; i < pHub->iPorts; i++)
		if (((pPort = pHub->sPort + i) != pSrcPort) && (pPort->pLnkPort))
		{
			pthread_mutex_lock(&pPort->sLock);
			int iTx = XPort_Tx(pPort->pLnkPort, pData, iSize);
			pthread_mutex_unlock(&pPort->sLock);
		}
	pthread_mutex_unlock(&pHub->sLnkLock);
	return iSize;
}

SXHubPort* XHubPort_Init(SXHubPort* pPort, SXHub* pHub, int iTimeout, int iBufLen, SXPort* pLnkPort)
{
	if (!pHub) return 0;
	SXHubPort sPort = {{(FXPortRxTx*)&XHubPort_Rx, (FXPortRxTx*)&XHubPort_Tx}, {}, pHub, iTimeout, {0, 0, 0, 0}, pLnkPort};
	if (pthread_mutex_init(&sPort.sLock, 0)) return 0;
	if (iBufLen > 0) if (!XBuf_Init(&sPort.sBuf, iBufLen)) goto e0;
	*pPort = sPort;
	return pPort;
e0:	pthread_mutex_destroy(&sPort.sLock);
	return 0;
}

void XHubPort_Done(SXHubPort* pPort)
{
	if (!pPort) return;
	if (!pPort->pHub) return;
	XBuf_Done(&pPort->sBuf, 0);
	pthread_mutex_destroy(&pPort->sLock);
}

int XHubPort_Rx(SXHubPort* pPort, void* pData, int iSize)
{
	if (!pPort) return -1;
	if (pPort->sBuf.p)
	{
		struct timespec sTime;
		clock_gettime(0, &sTime);
		sTime.tv_sec += pPort->iTimeout / 1000;
		sTime.tv_nsec += (pPort->iTimeout * 1000) % 1000000;
		if (sTime.tv_nsec > 1000000)
		{
			sTime.tv_sec++;
			sTime.tv_nsec -= 1000000;
		}
		pthread_mutex_lock(&pPort->pHub->sBufLock);
		int iBuf = XBuf_Get(&pPort->sBuf, pData, iSize);
		pthread_mutex_unlock(&pPort->pHub->sBufLock);
		if (iBuf == iSize) return iSize;
		if (iBuf == 0)
		{
			pthread_mutex_lock(&pPort->sLock);
			int iRx = (pPort->pLnkPort)?XPort_Rx(pPort->pLnkPort, pData, iSize):0;
			pthread_mutex_unlock(&pPort->sLock);
			int iHub = (iRx > 0)?XHub_Put(pPort->pHub, pPort, pData, iRx):0;
			if (iRx != 0) return iRx;
		}
		do
		{
			pthread_mutex_lock(&pPort->pHub->sBufLock);
			iBuf += XBuf_Get(&pPort->sBuf, (char*)pData + iBuf, iSize - iBuf);
			pthread_mutex_unlock(&pPort->pHub->sBufLock);
			if (iBuf == iSize) return iBuf;
		} while (!pthread_cond_timedwait(&pPort->pHub->sBufCond, 0, &sTime));
		return iBuf;
	}
	pthread_mutex_lock(&pPort->sLock);
	int iRx = (pPort->pLnkPort)?XPort_Rx(pPort->pLnkPort, pData, iSize):0;
	pthread_mutex_unlock(&pPort->sLock);
	int iHub = (iRx > 0)?XHub_Put(pPort->pHub, pPort, pData, iRx):0;
	if (XHub_LogFile)
	{
		char* pcHex = hexstr(pData, iRx);
		if (iHub == iRx == iSize)
			fprintf((FILE*)XHub_LogFile, "HUB%d Rx %d %s\n", pPort - pPort->pHub->sPort, iSize, pcHex);
		else
			fprintf((FILE*)XHub_LogFile, "HUB%d Rx Err %d %s (tx=%d hub=%d)\n", pPort - pPort->pHub->sPort, iSize, pcHex, iRx, iHub);
		free(pcHex);
	}
	return iRx;
}

int XHubPort_Tx(SXHubPort* pPort, void* pData, int iSize)
{
	if (!pPort) return -1;
	pthread_mutex_lock(&pPort->sLock);
	int iTx = (pPort->pLnkPort)?XPort_Tx(pPort->pLnkPort, pData, iSize):iSize;
	pthread_mutex_unlock(&pPort->sLock);
	int iHub = (iTx > 0)?XHub_Put(pPort->pHub, pPort, pData, iTx):0;
	if (XHub_LogFile)
	{
		char* pcHex = hexstr(pData, iSize);
		if ((iHub == iTx) == iSize)
			fprintf((FILE*)XHub_LogFile, "HUB%d Tx %d %s\n", pPort - pPort->pHub->sPort, iSize, pcHex);
		else
			fprintf((FILE*)XHub_LogFile, "HUB%d Tx Err %d %s (tx=%d hub=%d)\n", pPort - pPort->pHub->sPort, iSize, pcHex, iTx, iHub);
		free(pcHex);
	}
	return iTx;
}

short hex(char c)
{
	static char* tab = "0123456789abcdef";
	return tab[c>>4] | (tab[c&15] << 8);
}

char* hexstr(char* p, int n)
{
	short* str = (short*)malloc(2*n+1);
	int i; for (i = 0; i < n; i++)
		str[i] = hex(p[i]);
	*(char*)(str+n) = 0;
	return (char*)str;
}
