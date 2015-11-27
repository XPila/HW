////////////////////////////////////////
// XPortTCP.c

#include "XPortTCP.h"
#include "XPort.h"
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>

typedef struct SXPortTCP
{
	SXPort sPort;
 	int iSocket;
	struct sockaddr_in sLocAddr;
	struct sockaddr_in sRemAddr;
} SXPortTCP;

int XPortTCP_Rx(SXPortTCP* pPort, void* pData, int iSize);
int XPortTCP_Tx(SXPortTCP* pPort, void* pData, int iSize);

SXPortTCP* XPortTCP_Init(void* pLocAddr, void* pRemAddr, int iTimeout)
{
	if (!pLocAddr && !pRemAddr) return 0;
	SXPortTCP sPort = {{(FXPortRxTx*)&XPortTCP_Rx, (FXPortRxTx*)&XPortTCP_Tx}, 0, {0, 0, 0}, {0, 0, 0}};
	sPort.iSocket = socket(AF_INET, SOCK_STREAM, 0);
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
	{
		sPort.sRemAddr = *(struct sockaddr_in*)pRemAddr;
		if (connect(sPort.iSocket, (struct sockaddr*)&sPort.sRemAddr, sizeof(struct sockaddr_in))) goto error;
	}
	else
	{
		if (listen(sPort.iSocket, 1)) goto error;
		socklen_t nAddrLen = sizeof(struct sockaddr_in);
		int iSocket = 0;
		if ((iSocket = accept(sPort.iSocket, (struct sockaddr*)&sPort.sRemAddr, &nAddrLen)) < 0) goto error;
		shutdown(sPort.iSocket, SHUT_RDWR);
		close(sPort.iSocket);
		sPort.iSocket = iSocket;
	}
	SXPortTCP* pPort = (SXPortTCP*)malloc(sizeof(SXPortTCP));
	*pPort = sPort;
	return pPort;
error:
	shutdown(sPort.iSocket, SHUT_RDWR);
	close(sPort.iSocket);
	return 0;
}
	
void XPortTCP_Done(SXPortTCP* pPort)
{
	if (!pPort) return;
	if (pPort->iSocket)
	{
		shutdown(pPort->iSocket, SHUT_RDWR);
		close(pPort->iSocket);
	}
	free(pPort);
}

int XPortTCP_Rx(SXPortTCP* pPort, void* pData, int iSize)
{
	return recv(pPort->iSocket, pData, iSize, 0);
}

int XPortTCP_Tx(SXPortTCP* pPort, void* pData, int iSize)
{
	return send(pPort->iSocket, pData, iSize, 0);
}
