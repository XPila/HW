////////////////////////////////////////
// XPort.h

#ifndef _XPORT_H
#define _XPORT_H

#define XPort_Rx(pPort, pData, iSize) ((pPort && pPort->pRx)?(*(pPort->pRx))(pPort, pData, iSize):-1)
#define XPort_Tx(pPort, pData, iSize) ((pPort && pPort->pTx)?(*(pPort->pTx))(pPort, pData, iSize):-1)

typedef struct SXPort SXPort;

typedef int (FXPortRxTx)(SXPort* pPort, void* pData, int iSize);

typedef struct SXPort
{
	FXPortRxTx* pRx;
	FXPortRxTx* pTx;
} SXPort;
	
#endif //_XPORT_H
