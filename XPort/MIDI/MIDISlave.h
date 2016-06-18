////////////////////////////////////////
// MIDISlave.h

#ifndef _MIDISLAVE_H
#define _MIDISLAVE_H

#include <pthread.h>

typedef int (FMIDISlaveCB)(void* pSlave, char cAddr, char cData);

typedef struct SMIDISlave
{
	SXPort* pPort;
	int iFlags;
	int iRunning;
	pthread_cond_t sCond;
	pthread_mutex_t sMutex;
	pthread_t sThread;
	FMIDISlaveCB* pMsgCB;
} SMIDISlave;

void* MIDISlave_LogFile;

int MIDISlave_Init(SMIDISlave* pSlave, SXPort* pPort, int iFlags, void* pMsgCB);
void MIDISlave_Done(SMIDISlave* pSlave);
void MIDISlave_Lock(SMIDISlave* pSlave);
void MIDISlave_Unlock(SMIDISlave* pSlave);


#endif //_MIDISLAVE_H