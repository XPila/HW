////////////////////////////////////////
// XIDs.h

#ifndef _XIDS_H
#define _XIDS_H

//Node IDs
#define XID_NODE_Master			0x00
#define XID_NODE_Default		0xff

//Message IDs
#define XID_MSG_Reset			0x0000
#define XID_MSG_WarmReset		0x0001
#define XID_MSG_ColdReset		0x0002
#define XID_MSG_SelfTest		0x0003
#define XID_MSG_P0				0x0004
#define XID_MSG_P1				0x0005
#define XID_MSG_P2				0x0006
#define XID_MSG_P3				0x0007
#define XID_MSG_ModuleName		0x0008
#define XID_MSG_SerialNumber	0x0009
#define XID_MSG_VersionSupport	0x000a
#define XID_MSG_SPIConfig		0x000b
#define XID_MSG_SPIDataStop		0x000c
#define XID_MSG_SPIDataNoStop	0x000d
#define XID_MSG_I2CDataStop		0x000e
#define XID_MSG_I2CDataNoStop	0x000f
#define XID_MSG_EnableProcess	0x0010
#define XID_MSG_Time			0x0011
#define XID_MSG_Date			0x0021
#define XID_MSG_EEPROM_Addr		0x0101
#define XID_MSG_EEPROM_RWData	0x0102
#define XID_MSG_CMOS_Addr		0x0111
#define XID_MSG_CMOS_RWData		0x0112


#endif //_XIDS_H
