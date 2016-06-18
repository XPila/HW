////////////////////////////////////////
// XSlave.c

#include <malloc.h>
#include <stdio.h>
#include <pthread.h>

#include "../XPort/XPort.h"
#include "XBus.h"
#include "XSlave.h"


void* XSlave_Thread(void* pv);
void XSlave_Cycle(SXSlave* pSlave);

void* XSlave_LogFile = 0;

int XSlave_Init(SXSlave* pSlave, SXPort* pPort, int iFlags, void* pMsgToAll, void* pMsgToMy)
{
	memset(pSlave, 0, sizeof(SXSlave));
	pSlave->pPort = pPort;
	pSlave->iFlags = iFlags;
	pSlave->iRunning = 1;
	pSlave->pMsgToAll = pMsgToAll;
	pSlave->pMsgToMy = pMsgToMy;
	pthread_cond_init(&pSlave->sCond, 0);
	pthread_mutex_init(&pSlave->sMutex, 0);
	pthread_create(&pSlave->sThread, 0, &XSlave_Thread, pSlave);
	pthread_cond_wait(&pSlave->sCond, 0);
	return 0;
}

void XSlave_Done(SXSlave* pSlave)
{
	pSlave->iRunning = 0;
	pthread_cond_wait(&pSlave->sCond, 0);
	pthread_join(pSlave->sThread, 0);
	pthread_detach(pSlave->sThread);
	pthread_cond_destroy(&pSlave->sCond);
	pthread_mutex_destroy(&pSlave->sMutex);
}

void XSlave_Lock(SXSlave* pSlave)
{ pthread_mutex_lock(&pSlave->sMutex); }

void XSlave_Unlock(SXSlave* pSlave)
{ pthread_mutex_unlock(&pSlave->sMutex); }

void* XSlave_Thread(void* pv)
{
	SXSlave* pSlave = (SXSlave*)pv;
	pthread_cond_signal(&pSlave->sCond);
	SXBusMsg sMsg;
	while (pSlave->iRunning)
		XSlave_Cycle(pSlave);
	pthread_cond_signal(&pSlave->sCond);
	return 0;
}

void XSlave_Cycle(SXSlave* pSlave)
{
	SXBusMsg sMsg;
	int iRx = XBus_RxMsg(pSlave->pPort, pSlave->iFlags, &sMsg);
	if (iRx == XBUS_RES_OK)
	{
		if (XSlave_LogFile)
		{
			SXBusMsgHead* ph = &sMsg.sHead;
			int iDataSize = ph->ucDataFlag?ph->ucDataSize + 1:0;
			unsigned char* pcReq = "???";
			if (!ph->ucRdFlag && !ph->ucWrFlag) pcReq = "Cmd";
			else if (ph->ucRdFlag && !ph->ucWrFlag) pcReq = "RdO";
			else if (!ph->ucRdFlag && ph->ucWrFlag) pcReq = "WrO";
			unsigned char* pd = sMsg.ucData;
			char cHex[2 * XBUS_MAX_DATA + 1];
			sprintf(cHex, "%02x%02x%02x%02x%02x%02x%02x%02x", pd[0], pd[1], pd[2], pd[3], pd[4], pd[5], pd[6], pd[7]);
			cHex[ph->ucRdFlag?0:2*iDataSize] = 0;
			fprintf((FILE*)XSlave_LogFile, "XSLV%02x Rx %s 0x%04x %d %s\n", pSlave->iFlags & XBUS_FLG_IDMASK, pcReq, ph->usMsgID, iDataSize, cHex);
		}
		int iResponse = 0;
		pthread_mutex_lock(&pSlave->sMutex);
		if (sMsg.sHead.ucNodeID == 0)
			iResponse = (*(pSlave->pMsgToAll))(pSlave, &sMsg);
		else if (sMsg.sHead.ucNodeID == (pSlave->iFlags & 0xff))
			iResponse = (*(pSlave->pMsgToMy))(pSlave, &sMsg);
		pthread_mutex_unlock(&pSlave->sMutex);
		if (iResponse)
		{
			sMsg.sHead.ucRdFlag = 0;
			int iTx = XBus_TxMsg(pSlave->pPort, pSlave->iFlags, &sMsg);
		}
	}
	else
		if (XSlave_LogFile)
			fprintf((FILE*)XSlave_LogFile, "XSLV%02x Rx Err %s\n", pSlave->iFlags & XBUS_FLG_IDMASK, XBus_ResStr(iRx));
}