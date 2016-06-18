////////////////////////////////////////
// XNode.c

#include "XNode.h"
#include "../XPort/XPort.h"
#include "XBus.h"
#include "XIDs.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

void swap2(void* p);
void swap4(void* p);

int XNode_Cmd(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned short usMsgID, void* pData, unsigned char ucSize)
{
	if (ucSize > XBUS_MAX_DATA) ucSize = XBUS_MAX_DATA;
	SXBusMsg sMsg = {{ucNodeID, usMsgID, (ucSize>0)?(ucSize-1):0, (ucSize>0)?1:0, 0, 0, 0}, {0,0,0,0,0,0,0,0}};
	if (ucSize) memcpy(sMsg.ucData, pData, ucSize);
	int iTx = XBus_TxMsg(pPort, iFlags, &sMsg);
	return iTx;
}

int XNode_RdO(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned short usMsgID, void* pData, unsigned char ucSize, unsigned char* pucSize)
{
	if (ucSize > XBUS_MAX_DATA) ucSize = XBUS_MAX_DATA;
	SXBusMsg sMsg = {{ucNodeID, usMsgID, (ucSize>0)?(ucSize-1):0, (ucSize>0)?1:0, 0, 1, 0}, {0,0,0,0,0,0,0,0}};
	int iTx = XBus_TxMsg(pPort, iFlags, &sMsg);
	if (iTx < 0) return iTx;
	int iRx = XBus_RxMsg(pPort, iFlags, &sMsg);
	if (iRx < 0) return iRx;
	unsigned char ucRxSize = sMsg.sHead.ucDataFlag?(sMsg.sHead.ucDataSize + 1):0;
	if (ucSize > ucRxSize) ucSize = ucRxSize;
	if (ucSize) memcpy(pData, sMsg.ucData, ucSize);
	if (pucSize) *pucSize = ucSize;
	return XBUS_RES_OK;
}

int XNode_WrO(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned short usMsgID, void* pData, unsigned char ucSize)
{
	if (ucSize > XBUS_MAX_DATA) ucSize = XBUS_MAX_DATA;
	SXBusMsg sMsg = {{ucNodeID, usMsgID, (ucSize>0)?(ucSize-1):0, (ucSize>0)?1:0, 0, 0, 1}, {0,0,0,0,0,0,0,0}};
	if (ucSize) memcpy(sMsg.ucData, pData, ucSize);
	int iTx = XBus_TxMsg(pPort, iFlags, &sMsg);
	return iTx;
}

int XNode_Cmd_Reset(SXPort* pPort, int iFlags, unsigned char ucNodeID)
{ return XNode_Cmd(pPort, iFlags, ucNodeID, XID_MSG_Reset, 0, 0); }

int XNode_Cmd_WarmReset(SXPort* pPort, int iFlags, unsigned char ucNodeID)
{ return XNode_Cmd(pPort, iFlags, ucNodeID, XID_MSG_WarmReset, 0, 0); }

int XNode_Cmd_ColdReset(SXPort* pPort, int iFlags, unsigned char ucNodeID)
{ return XNode_Cmd(pPort, iFlags, ucNodeID, XID_MSG_ColdReset, 0, 0); }

int XNode_Cmd_SelfTest(SXPort* pPort, int iFlags, unsigned char ucNodeID)
{ return XNode_Cmd(pPort, iFlags, ucNodeID, XID_MSG_SelfTest, 0, 0); }

int XNode_RdO_P0(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char* pucP0)
{ return XNode_RdO(pPort, iFlags, ucNodeID, XID_MSG_P0, pucP0, 1, 0); }

int XNode_WrO_P0(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char ucP0)
{ return XNode_WrO(pPort, iFlags, ucNodeID, XID_MSG_P0, &ucP0, 1); }

int XNode_RdO_P1(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char* pucP1)
{ return XNode_RdO(pPort, iFlags, ucNodeID, XID_MSG_P1, pucP1, 1, 0); }

int XNode_WrO_P1(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char ucP1)
{ return XNode_WrO(pPort, iFlags, ucNodeID, XID_MSG_P1, &ucP1, 1); }

int XNode_RdO_P2(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char* pucP2)
{ return XNode_RdO(pPort, iFlags, ucNodeID, XID_MSG_P2, pucP2, 1, 0); }

int XNode_WrO_P2(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char ucP2)
{ return XNode_WrO(pPort, iFlags, ucNodeID, XID_MSG_P2, &ucP2, 1); }

int XNode_RdO_P3(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char* pucP3)
{ return XNode_RdO(pPort, iFlags, ucNodeID, XID_MSG_P3, pucP3, 1, 0); }

int XNode_WrO_P3(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char ucP3)
{ return XNode_WrO(pPort, iFlags, ucNodeID, XID_MSG_P3, &ucP3, 1); }

int XNode_RdO_ModuleName(SXPort* pPort, int iFlags, unsigned char ucNodeID, char* pcModuleName)
{ int iRet = XNode_RdO(pPort, iFlags, ucNodeID, XID_MSG_ModuleName, pcModuleName, 8, 0); pcModuleName[8] = 0; return iRet; }

int XNode_RdO_SerialNumber(SXPort* pPort, int iFlags, unsigned char ucNodeID, char* pcSerialNumber)
{ int iRet = XNode_RdO(pPort, iFlags, ucNodeID, XID_MSG_SerialNumber, pcSerialNumber, 8, 0); pcSerialNumber[8] = 0; return iRet; }

int XNode_RdO_VersionSupport(SXPort* pPort, int iFlags, unsigned char ucNodeID, SXVersionSupport* psVersionSupport)
{ int iRet = XNode_RdO(pPort, iFlags, ucNodeID, XID_MSG_VersionSupport, psVersionSupport, 8, 0); swap2(&psVersionSupport->usBuild); return iRet; }

int XNode_RdO_SPIConfig(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char* pucSPIConfig)
{ return XNode_RdO(pPort, iFlags, ucNodeID, XID_MSG_SPIConfig, pucSPIConfig, 1, 0); }

int XNode_WrO_SPIConfig(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char ucSPIConfig)
{ return XNode_WrO(pPort, iFlags, ucNodeID, XID_MSG_SPIConfig, &ucSPIConfig, 1); }

int XNode_RdO_SPIDataStop(SXPort* pPort, int iFlags, unsigned char ucNodeID, void* pvSPIData, unsigned char ucSize)
{ int iRet = XNode_RdO(pPort, iFlags, ucNodeID, XID_MSG_SPIDataStop, pvSPIData, ucSize, 0); return iRet; }

int XNode_WrO_SPIDataStop(SXPort* pPort, int iFlags, unsigned char ucNodeID, void* pvSPIData, unsigned char ucSize)
{ return XNode_WrO(pPort, iFlags, ucNodeID, XID_MSG_SPIDataStop, pvSPIData, ucSize); }

int XNode_RdO_SPIDataNoStop(SXPort* pPort, int iFlags, unsigned char ucNodeID, void* pvSPIData, unsigned char ucSize)
{ int iRet = XNode_RdO(pPort, iFlags, ucNodeID, XID_MSG_SPIDataNoStop, pvSPIData, ucSize, 0); return iRet; }

int XNode_WrO_SPIDataNoStop(SXPort* pPort, int iFlags, unsigned char ucNodeID, void* pvSPIData, unsigned char ucSize)
{ return XNode_WrO(pPort, iFlags, ucNodeID, XID_MSG_SPIDataNoStop, pvSPIData, ucSize); }

/*
int XNode_RdO_I2CDataStop(SXPort* pPort, int iFlags, unsigned char ucNodeID, void* pvSPIData, unsigned char ucSize)
int XNode_WrO_I2CDataStop(SXPort* pPort, int iFlags, unsigned char ucNodeID, void* pvSPIData, unsigned char ucSize)
int XNode_RdO_I2CDataNoStop(SXPort* pPort, int iFlags, unsigned char ucNodeID, void* pvSPIData, unsigned char ucSize)
int XNode_WrO_I2CDataNoStop(SXPort* pPort, int iFlags, unsigned char ucNodeID, void* pvSPIData, unsigned char ucSize)*/

int XNode_RdO_Time(SXPort* pPort, int iFlags, unsigned char ucNodeID, SXTime* psTime)
{ int iRet = XNode_RdO(pPort, iFlags, ucNodeID, XID_MSG_Time, psTime, 4, 0); return iRet; }

int XNode_WrO_Time(SXPort* pPort, int iFlags, unsigned char ucNodeID, SXTime* psTime)
{ return XNode_WrO(pPort, iFlags, ucNodeID, XID_MSG_Time, psTime, 4); }

int XNode_RdO_Date(SXPort* pPort, int iFlags, unsigned char ucNodeID, SXDate* psDate)
{ int iRet = XNode_RdO(pPort, iFlags, ucNodeID, XID_MSG_Date, psDate, 4, 0); return iRet; }

int XNode_WrO_Date(SXPort* pPort, int iFlags, unsigned char ucNodeID, SXDate* psDate)
{ return XNode_WrO(pPort, iFlags, ucNodeID, XID_MSG_Date, psDate, 4); }

	