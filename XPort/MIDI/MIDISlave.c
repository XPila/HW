////////////////////////////////////////
// MIDISlave.c

#include <malloc.h>
#include <stdio.h>
#include <pthread.h>

#include "../XPort.h"
#include "MIDISlave.h"


void* MIDISlave_Thread(void* pv);
void MIDISlave_Cycle(SMIDISlave* pSlave);

void* MIDISlave_LogFile = 0;

int MIDISlave_Init(SMIDISlave* pSlave, SXPort* pPort, int iFlags, void* pMsgCB)
{
	memset(pSlave, 0, sizeof(SMIDISlave));
	pSlave->pPort = pPort;
	pSlave->iFlags = iFlags;
	pSlave->iRunning = 1;
	pSlave->pMsgCB = pMsgCB;
	pthread_cond_init(&pSlave->sCond, 0);
	pthread_mutex_init(&pSlave->sMutex, 0);
	pthread_create(&pSlave->sThread, 0, &MIDISlave_Thread, pSlave);
	pthread_cond_wait(&pSlave->sCond, 0);
	return 0;
}

void MIDISlave_Done(SMIDISlave* pSlave)
{
	pSlave->iRunning = 0;
	pthread_cond_wait(&pSlave->sCond, 0);
	pthread_join(pSlave->sThread, 0);
	pthread_detach(pSlave->sThread);
	pthread_cond_destroy(&pSlave->sCond);
	pthread_mutex_destroy(&pSlave->sMutex);
}

void MIDISlave_Lock(SMIDISlave* pSlave)
{ pthread_mutex_lock(&pSlave->sMutex); }

void MIDISlave_Unlock(SMIDISlave* pSlave)
{ pthread_mutex_unlock(&pSlave->sMutex); }

void* MIDISlave_Thread(void* pv)
{
	//printf("MIDISlave_Thread - begin\n");
	SMIDISlave* pSlave = (SMIDISlave*)pv;
	pthread_cond_signal(&pSlave->sCond);
	while (pSlave->iRunning)
		MIDISlave_Cycle(pSlave);
	pthread_cond_signal(&pSlave->sCond);
	//printf("MIDISlave_Thread - end\n");
	return 0;
}

int MIDISlave_Rx(SMIDISlave* pSlave, char* pcAddr, char* pcData)
{
	//printf("MIDISlave_Rx - begin\n");
	char cAddr = 0;
	char cData = 0;
	if ((XPort_Rx(pSlave->pPort, &cAddr, 1) == 1) && (cAddr & 0x80))
	{
		//printf("MIDISlave_Rx cAddr %02x\n", cAddr);
		if ((XPort_Rx(pSlave->pPort, &cData, 1) == 1) && !(cData & 0x80))
		{
			if (pcAddr) *pcAddr = cAddr & 0x7f;
			if (pcData) *pcData = cData;
			//printf("MIDISlave_Rx - end 0\n");
			return 0;
		}
	}
	//printf("MIDISlave_Rx - end -1\n");
	return -1;
}

void MIDISlave_Cycle(SMIDISlave* pSlave)
{
	char cAddr = 0;
	char cData = 0;
	int iRx = MIDISlave_Rx(pSlave, &cAddr, &cData);
	if (iRx == 0)
	{
		if (MIDISlave_LogFile)
		{
			//log the message
		}
		pthread_mutex_lock(&pSlave->sMutex);
		(*(pSlave->pMsgCB))(pSlave, cAddr, cData);
		pthread_mutex_unlock(&pSlave->sMutex);
	}
	else
		if (MIDISlave_LogFile)
		{
			//log error
		}
}