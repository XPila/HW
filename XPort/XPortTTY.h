////////////////////////////////////////
// XPortTTY.h

#ifndef _XPORTTTY_H
#define _XPORTTTY_H

typedef struct SXPort SXPort;
typedef struct SXPortTTY SXPortTTY;

void* XPortTTY_LogFile;

SXPortTTY* XPortTTY_Init(char* pcDev, int iBaud, int iBits, int iParity, int iStop, int iTimeout);
void XPortTTY_Done(SXPortTTY* pPort);
int XPortTTY_GetBaud(SXPortTTY* pPort);
int XPortTTY_SetBaud(SXPortTTY* pPort, int iBaud);
int XPortTTY_GetModem(SXPortTTY* pPort);
int XPortTTY_SetModem(SXPortTTY* pPort, int iModem);
int XPortTTY_ClrSetModem(SXPortTTY* pPort, int iModem, int iMask);

#endif //_XPORTTTY_H