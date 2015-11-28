////////////////////////////////////////
// XSlave.h

#ifndef _XSLAVE_H
#define _XSLAVE_H


#define XSLAVE_TABLE_DECL(name) \
	int XSlaveTable_##name(void* pSlave, SXBusMsg* psMsg);
	
#define XSLAVE_TABLE_BEGIN(name) \
	int XSlaveTable_##name(void* pSlave, SXBusMsg* psMsg) { if (0) {}

#define XSLAVE_TABLE_END(name) return 0; }

#define XSLAVE_ENTRY_F(msgid, size, func) \
	else if ((!psMsg->sHead.ucRdFlag && !psMsg->sHead.ucWrFlag) && \
	(psMsg->sHead.usMsgID == msgid)) \
	{ func(pSlave, size, psMsg); return 0; }

#define XSLAVE_ENTRY_FR(msgid, size, func) \
	else if ((psMsg->sHead.ucRdFlag && !psMsg->sHead.ucWrFlag) && \
	(psMsg->sHead.usMsgID == msgid)) \
	{ func(pSlave, size, psMsg); return 1; }

#define XSLAVE_ENTRY_FRW(msgid, size, func) \
	else if (((psMsg->sHead.ucRdFlag && !psMsg->sHead.ucWrFlag) || (!psMsg->sHead.ucRdFlag && psMsg->sHead.ucWrFlag)) && \
	(psMsg->sHead.usMsgID == msgid)) \
	{ func(pSlave, size, psMsg); return psMsg->sHead.ucRdFlag; }

#define XSLAVE_ENTRY_CR(msgid, size, ptr) \
	XSLAVE_ENTRY_FR(msgid, size, ptr)

#define XSLAVE_ENTRY_IR(msgid, size, ptr) \
	XSLAVE_ENTRY_FR(msgid, size, ptr)

#define XSLAVE_ENTRY_IRW(msgid, size, ptr) \
	XSLAVE_ENTRY_FRW(msgid, size, ptr)

#define XSLAVE_ENTRY_CR_(msgid, size, ptr) \
	else if ((psMsg->sHead.ucRdFlag && !psMsg->sHead.ucWrFlag) && \
	(psMsg->sHead.usMsgID == msgid)) \
	{ memcpy(psMsg->ucData, ptr, size); psMsg->sHead.ucRdFlag = 0; return 1; }

#define XSLAVE_ENTRY_IR_(msgid, size, ptr) \
	else if ((psMsg->sHead.ucRdFlag && !psMsg->sHead.ucWrFlag) && \
	(psMsg->sHead.usMsgID == msgid)) \
	{ memcpy(psMsg->ucData, ptr, size); psMsg->sHead.ucRdFlag = 0; return 1; }

typedef int (FXSlaveCB)(void* pSlave, SXBusMsg* psMsg);

typedef struct SXSlave
{
	SXPort* pPort;
	int iFlags;
	int iRunning;
	pthread_cond_t sCond;
	pthread_mutex_t sMutex;
	pthread_t sThread;
	FXSlaveCB* pMsgToAll;
	FXSlaveCB* pMsgToMy;
} SXSlave;

void* XSlave_LogFile;

int XSlave_Init(SXSlave* pSlave, SXPort* pPort, int iFlags, void* pMsgToAll, void* pMsgToMy);
void XSlave_Done(SXSlave* pSlave);
void XSlave_Lock(SXSlave* pSlave);
void XSlave_Unlock(SXSlave* pSlave);


#endif //_XSLAVE_H
