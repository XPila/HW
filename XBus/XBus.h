////////////////////////////////////////
// XBus.h

#ifndef _XBUS_H
#define _XBUS_H


#define XBUS_RES_OK			0
#define XBUS_RES_ERR		-1
#define XBUS_RES_PTR		-2
#define XBUS_RES_RX_START	-3
#define XBUS_RES_RX_HEAD	-4
#define XBUS_RES_RX_DATA	-5
#define XBUS_RES_RX_ECHO	-6
#define XBUS_RES_RX_ACK		-7
#define XBUS_RES_TX_MSG		-8
#define XBUS_RES_TX_ACK		-9
#define XBUS_RES_START		-10
#define XBUS_RES_ACK		-11
#define XBUS_RES_ECHO		-12
#define XBUS_RES_CHS_HEAD	-13
#define XBUS_RES_CHS_DATA	-14

#define XBUS_FLG_IDMASK		0xff
#define XBUS_FLG_ACK		0x100
#define XBUS_FLG_ECHO		0x200
#define XBUS_FLG_TXDL		0x400
#define XBUS_FLG_RXDL		0x800

#define XBUS_MAX_DATA		8

typedef struct SXPort SXPort;

#pragma pack(push)
#pragma pack(1)
typedef struct SXBusMsgHead
{
	unsigned char ucNodeID;
	unsigned short usMsgID;
	unsigned char ucDataSize:3;
 	unsigned char ucDataFlag:1;
	unsigned char ucReserved:2;
	unsigned char ucRdFlag:1;
	unsigned char ucWrFlag:1;
} SXBusMsgHead;

typedef struct SXBusMsg
{
	SXBusMsgHead sHead;
	unsigned char ucData[XBUS_MAX_DATA];
} SXBusMsg;
#pragma pack(pop)

void* XBus_LogFile;

int XBus_RxMsg(SXPort* pPort, int iFlags, SXBusMsg* pMsg);
int XBus_TxMsg(SXPort* pPort, int iFlags, SXBusMsg* pMsg); 

char* XBus_ResStr(int iRes);
int XBus_sprintf_Msg(char* pcMsg, SXBusMsg* pMsg);


#endif //_XBUS_H
