////////////////////////////////////////
// XPortUDP.c

#include "XPortUDP.h"
#include "XPort.h"
#include "XBuf.h"
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>

typedef struct SXPortUDP
{
	SXPort sPort;
 	int iSocket;
	struct sockaddr_in sLocAddr;
	struct sockaddr_in sRemAddr;
	SXBuf sBuf;
} SXPortUDP;

int XPortUDP_Rx(SXPortUDP* pPort, void* pData, int iSize);
int XPortUDP_Tx(SXPortUDP* pPort, void* pData, int iSize);

SXPortUDP* XPortUDP_Init(void* pLocAddr, void* pRemAddr, int iTimeout, int iBufLen)
{
	if (!pLocAddr && !pRemAddr) return 0;
	SXPortUDP sPort = {{(FXPortRxTx*)&XPortUDP_Rx, (FXPortRxTx*)&XPortUDP_Tx}, 0, {0, 0, 0}, {0, 0, 0}, {0, 0, 0, 0}};
	sPort.iSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (sPort.iSocket < 0) return 0;
	if (iTimeout > 0)
	{
		struct timeval sTimeout = {iTimeout / 1000, (iTimeout - 1000 * (iTimeout / 1000)) * 1000};
		if (setsockopt(sPort.iSocket, SOL_SOCKET, SO_RCVTIMEO, &sTimeout, sizeof(struct timeval))) goto error;
	}
	if (pLocAddr)
	{
		sPort.sLocAddr = *(struct sockaddr_in*)pLocAddr;
		int iVal = 1;
		if (setsockopt(sPort.iSocket, SOL_SOCKET, SO_REUSEADDR, &iVal, sizeof(int))) goto error;
		if (bind(sPort.iSocket, (struct sockaddr*)&sPort.sLocAddr, sizeof(struct sockaddr_in))) goto error;
	}
	if (pRemAddr)
		sPort.sRemAddr = *(struct sockaddr_in*)pRemAddr;
	if (iBufLen > 0) if (!XBuf_Init(&sPort.sBuf, iBufLen)) goto error;
	SXPortUDP* pPort = (SXPortUDP*)malloc(sizeof(SXPortUDP));
	*pPort = sPort;
	return pPort;
error:
	shutdown(sPort.iSocket, SHUT_RDWR);
	close(sPort.iSocket);
	return 0;
}
	
void XPortUDP_Done(SXPortUDP* pPort)
{
	if (!pPort) return;
	if (pPort->iSocket)
	{
		shutdown(pPort->iSocket, SHUT_RDWR);
		close(pPort->iSocket);
	}
	XBuf_Done(&pPort->sBuf, 0);
	free(pPort);
}

int XPortUDP_Rx(SXPortUDP* pPort, void* pData, int iSize)
{
	socklen_t nAddrLen = sizeof(struct sockaddr_in);
	if (pPort->sBuf.p)
	{
		int iBuf = XBuf_Get(&pPort->sBuf, pData, iSize);
		if (iBuf == iSize) return iSize;
		int iRx = recvfrom(pPort->iSocket, pPort->sBuf.p, pPort->sBuf.iLen, 0, (struct sockaddr*)&pPort->sRemAddr, &nAddrLen);
		if (iRx < 0) return iRx;
		XBuf_Set(&pPort->sBuf, 0, iRx);
		iBuf += XBuf_Get(&pPort->sBuf, (char*)pData + iBuf, iSize - iBuf);
		return iBuf;
	}
	return recvfrom(pPort->iSocket, pData, iSize, 0, (struct sockaddr*)&pPort->sRemAddr, &nAddrLen);
}

int XPortUDP_Tx(SXPortUDP* pPort, void* pData, int iSize)
{
	return sendto(pPort->iSocket, pData, iSize, 0, (struct sockaddr*)&pPort->sRemAddr, sizeof(struct sockaddr_in));
}
