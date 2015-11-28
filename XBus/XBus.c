////////////////////////////////////////
// XBus.c

#include "XBus.h"
#include "../XPort/XPort.h"
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


#define XBUS_CHR_START		0x5a
#define XBUS_CHR_ACK		0xa5

#define XBUS_LEN_HEAD		4
#define XBUS_MAX_MSG		1 + XBUS_LEN_HEAD + 1 + XBUS_MAX_DATA + 1 + 1;
#define XBUS_LEN_RXBUF		4 * XBUS_MAX_MSG

#define XBUS_MAX_MSGSTR 40


#pragma pack(push)
#pragma pack(1)
typedef struct SXBusMsgBuf
{
	unsigned char ucStart;
	SXBusMsgHead sHead;
	unsigned char ucChsHead;
	unsigned char ucData[XBUS_MAX_DATA + 1];
	unsigned char ucAck;
} SXBusMsgBuf;
#pragma pack(pop)

char* hexstr(char* p, int n);//implemented in XPort.c
void swap2(void* p);
void swap4(void* p);

void* XBus_LogFile = 0;
	
unsigned char XBus_CheckSum(unsigned char* pucData, int iSize, unsigned char ucChs)
{
	while (iSize--) ucChs ^= *(pucData++);
	return ucChs;
}

int XBus_RxMsg(SXPort* pPort, int iFlags, SXBusMsg* pMsg)
{
	int iRes = XBUS_RES_PTR;
	if (!(pPort && pMsg)) goto end;
	SXBusMsgBuf sBuf;
	int iRx, iTx, iDataSize;
	iRx = XPort_Rx(pPort, &sBuf, 1);
	iRes = XBUS_RES_RX_START;
	if (iRx != 1) goto end;
	iRes = XBUS_RES_START;
	if (sBuf.ucStart != XBUS_CHR_START) goto end;
	iRx = XPort_Rx(pPort, &sBuf.sHead, XBUS_LEN_HEAD + 1);
	iRes = XBUS_RES_RX_HEAD;
	if (iRx != (XBUS_LEN_HEAD + 1)) goto end;
	iRes = XBUS_RES_CHS_HEAD;
	if (sBuf.ucChsHead != XBus_CheckSum((unsigned char*)&sBuf.sHead, XBUS_LEN_HEAD, 0)) goto end;
	swap2(&sBuf.sHead.usMsgID);
	iDataSize = sBuf.sHead.ucRdFlag?0:(sBuf.sHead.ucDataFlag?sBuf.sHead.ucDataSize + 1:0);
	if (iDataSize)
	{
		iRx = XPort_Rx(pPort, sBuf.ucData, iDataSize + 1);
		iRes = XBUS_RES_RX_DATA;
		if (iRx != (iDataSize + 1)) goto end;
		iRes = XBUS_RES_CHS_DATA;
		if (sBuf.ucData[iDataSize] != XBus_CheckSum(sBuf.ucData, iDataSize, 0)) goto end;
	}
	if ((iFlags & XBUS_FLG_ACK) && (((iFlags & XBUS_FLG_IDMASK) == 0) || ((iFlags & XBUS_FLG_IDMASK) == sBuf.sHead.ucNodeID)))
	{
		sBuf.ucAck = XBUS_CHR_ACK;
		iTx = XPort_Tx(pPort, &sBuf.ucAck, 1);
		iRes = XBUS_RES_TX_ACK;
		if (iTx != 1) goto end;
		if (iFlags & XBUS_FLG_ECHO)
		{
			iRx = XPort_Rx(pPort, &sBuf.ucAck, 1);
			iRes = XBUS_RES_RX_ECHO;
			if (iRx != 1) goto end;
			iRes = XBUS_RES_ECHO;
			if (sBuf.ucAck != XBUS_CHR_ACK) goto end;
		}
	}
	pMsg->sHead = sBuf.sHead;
	if (iDataSize) memcpy(pMsg->ucData, sBuf.ucData, iDataSize);
	iRes = XBUS_RES_OK;
end:
	if (iFlags & XBUS_FLG_RXDL) usleep(100);
	if (!XBus_LogFile) return iRes;
	if (iRes == XBUS_RES_OK)
	{
		char cMsg[XBUS_MAX_MSGSTR];
		XBus_sprintf_Msg(cMsg, pMsg);
		fprintf((FILE*)XBus_LogFile, "%02x RxMsg %s: %s\n", iFlags & XBUS_FLG_IDMASK, XBus_ResStr(iRes), cMsg);
	}
	else
		fprintf((FILE*)XBus_LogFile, "%02x RxMsg %s\r\n", iFlags & XBUS_FLG_IDMASK, XBus_ResStr(iRes));
	return iRes;
}
	
int XBus_TxMsg(SXPort* pPort, int iFlags, SXBusMsg* pMsg)
{
	int iRes = XBUS_RES_PTR;
	if (!(pPort && pMsg)) goto end;
	SXBusMsgBuf sBuf;
	int iRx, iTx, iDataSize;
	sBuf.ucStart = XBUS_CHR_START;
	sBuf.sHead = pMsg->sHead;
	swap2(&sBuf.sHead.usMsgID);
	sBuf.ucChsHead = XBus_CheckSum((unsigned char*)&sBuf.sHead, XBUS_LEN_HEAD, 0);
	iDataSize = pMsg->sHead.ucRdFlag?0:(pMsg->sHead.ucDataFlag?pMsg->sHead.ucDataSize + 1:0);
	if (iDataSize) memcpy(sBuf.ucData, pMsg->ucData, iDataSize);
	sBuf.ucData[iDataSize] = XBus_CheckSum(sBuf.ucData, iDataSize, 0);
	iTx = XPort_Tx(pPort, &sBuf,  1 + XBUS_LEN_HEAD + 1 + iDataSize + (iDataSize?1:0));
	iRes = XBUS_RES_TX_MSG;
	if (iTx !=  (1 + XBUS_LEN_HEAD + 1 + iDataSize + (iDataSize?1:0))) goto end;
	if (iFlags & XBUS_FLG_ECHO)
	{
		SXBusMsgBuf sBufEcho;
		iRx = XPort_Rx(pPort, &sBufEcho, iTx);
		iRes = XBUS_RES_RX_ECHO;
		if (iRx != iTx) goto end;
		iRes = XBUS_RES_ECHO;
		if (memcmp(&sBuf, &sBufEcho, iRx)) goto end;
	}
	if (iFlags & XBUS_FLG_ACK)
	{
		iRx = XPort_Rx(pPort, &sBuf.ucAck, 1);
		iRes = XBUS_RES_RX_ACK;
		if (iRx != 1) goto end;
		iRes = XBUS_RES_ACK;
		if (sBuf.ucAck != XBUS_CHR_ACK) goto end;
	}
	iRes = XBUS_RES_OK;
end:
	if (iFlags & XBUS_FLG_TXDL) usleep(100);
	if (!XBus_LogFile) return iRes;
	if (iRes == XBUS_RES_OK)
	{
		char cMsg[XBUS_MAX_MSGSTR];
		XBus_sprintf_Msg(cMsg, pMsg);
		fprintf((FILE*)XBus_LogFile, "%02x TxMsg %s: %s\n", iFlags & XBUS_FLG_IDMASK, XBus_ResStr(iRes), cMsg);
	}
	else
		fprintf((FILE*)XBus_LogFile, "%02x TxMsg %s\r\n", iFlags & XBUS_FLG_IDMASK, XBus_ResStr(iRes));
	return iRes;
}

char* XBus_ResStr(int iRes)
{
	switch (iRes)
	{
	case XBUS_RES_OK: return "OK";
	case XBUS_RES_ERR: return "ERR";
	case XBUS_RES_PTR: return "PTR";
	case XBUS_RES_RX_START: return "RX_START";
	case XBUS_RES_RX_HEAD: return "RX_HEAD";
	case XBUS_RES_RX_DATA: return "RX_DATA";
	case XBUS_RES_RX_ECHO: return "RX_ECHO";
	case XBUS_RES_RX_ACK: return "RX_ACK";
	case XBUS_RES_TX_MSG: return "TX_MSG";
	case XBUS_RES_TX_ACK: return "TX_ACK";
	case XBUS_RES_START: return "START";
	case XBUS_RES_ACK: return "ACK";
	case XBUS_RES_ECHO: return "ECHO";
	case XBUS_RES_CHS_HEAD: return "CHS_HEAD";
	case XBUS_RES_CHS_DATA: return "CHS_DATA";
	}
	return "???";
}

int XBus_sprintf_Msg(char* pcMsg, SXBusMsg* pMsg)
{
	if (!pcMsg) return 0;
	if (!pMsg) return sprintf(pcMsg, "null");
	SXBusMsgHead* ph = &pMsg->sHead;
	int iDataSize = ph->ucDataFlag?ph->ucDataSize + 1:0;
	char* pcHex = hexstr(pMsg->ucData, ph->ucRdFlag?0:iDataSize);
	char* pcFmt = "%02x %04x %d %c%c %s";
	int iRet = sprintf(pcMsg, pcFmt, ph->ucNodeID, ph->usMsgID, iDataSize, ph->ucRdFlag?'R':'-', ph->ucWrFlag?'W':'-', pcHex);
	free(pcHex);
	return iRet;
}

void swap2(void* p)
{
	short s = *((short*)p);
	((char*)p)[0] = ((char*)&s)[1];
	((char*)p)[1] = ((char*)&s)[0];
}

void swap4(void* p)
{
	long l = *((long*)p);
	((char*)p)[0] = ((char*)&l)[3];
	((char*)p)[1] = ((char*)&l)[2];
	((char*)p)[2] = ((char*)&l)[1];
	((char*)p)[3] = ((char*)&l)[0];
}
