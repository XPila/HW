////////////////////////////////////////
// XNode.h

#ifndef _XNODE_H
#define _XNODE_H

#include "XTypes.h"

typedef struct SXPort SXPort;

int XNode_Cmd(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned short usMsgID, void* pData, unsigned char ucSize);
int XNode_RdO(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned short usMsgID, void* pData, unsigned char ucSize, unsigned char* pucSize);
int XNode_WrO(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned short usMsgID, void* pData, unsigned char ucSize);

int XNode_Cmd_Reset(SXPort* pPort, int iFlags, unsigned char ucNodeID);
int XNode_Cmd_WarmReset(SXPort* pPort, int iFlags, unsigned char ucNodeID);
int XNode_Cmd_ColdReset(SXPort* pPort, int iFlags, unsigned char ucNodeID);
int XNode_Cmd_SelfTest(SXPort* pPort, int iFlags, unsigned char ucNodeID);
int XNode_RdO_P0(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char* pucP0);
int XNode_WrO_P0(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char ucP0);
int XNode_RdO_P1(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char* pucP1);
int XNode_WrO_P1(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char ucP1);
int XNode_RdO_P2(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char* pucP2);
int XNode_WrO_P2(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char ucP2);
int XNode_RdO_P3(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char* pucP3);
int XNode_WrO_P3(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char ucP3);
int XNode_RdO_ModuleName(SXPort* pPort, int iFlags, unsigned char ucNodeID, char* pcModuleName);
int XNode_RdO_SerialNumber(SXPort* pPort, int iFlags, unsigned char ucNodeID, char* pcSerialNumber);
int XNode_RdO_VersionSupport(SXPort* pPort, int iFlags, unsigned char ucNodeID, SXVersionSupport* psVersionSupport);
int XNode_RdO_SPIConfig(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char* pucSPIConfig);
int XNode_WrO_SPIConfig(SXPort* pPort, int iFlags, unsigned char ucNodeID, unsigned char ucSPIConfig);
int XNode_RdO_SPIDataStop(SXPort* pPort, int iFlags, unsigned char ucNodeID, void* pvSPIData, unsigned char ucSize);
int XNode_WrO_SPIDataStop(SXPort* pPort, int iFlags, unsigned char ucNodeID, void* pvSPIData, unsigned char ucSize);
int XNode_RdO_SPIDataNoStop(SXPort* pPort, int iFlags, unsigned char ucNodeID, void* pvSPIData, unsigned char ucSize);
int XNode_WrO_SPIDataNoStop(SXPort* pPort, int iFlags, unsigned char ucNodeID, void* pvSPIData, unsigned char ucSize);
int XNode_RdO_I2CDataStop(SXPort* pPort, int iFlags, unsigned char ucNodeID, void* pvSPIData, unsigned char ucSize);
int XNode_WrO_I2CDataStop(SXPort* pPort, int iFlags, unsigned char ucNodeID, void* pvSPIData, unsigned char ucSize);
int XNode_RdO_I2CDataNoStop(SXPort* pPort, int iFlags, unsigned char ucNodeID, void* pvSPIData, unsigned char ucSize);
int XNode_WrO_I2CDataNoStop(SXPort* pPort, int iFlags, unsigned char ucNodeID, void* pvSPIData, unsigned char ucSize);
int XNode_RdO_Time(SXPort* pPort, int iFlags, unsigned char ucNodeID, SXTime* psTime);
int XNode_WrO_Time(SXPort* pPort, int iFlags, unsigned char ucNodeID, SXTime* psTime);
int XNode_RdO_Date(SXPort* pPort, int iFlags, unsigned char ucNodeID, SXDate* psDate);
int XNode_WrO_Date(SXPort* pPort, int iFlags, unsigned char ucNodeID, SXDate* psDate);


#endif //_XNODE_H