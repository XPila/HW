////////////////////////////////////////
// XPortUDP.h

#ifndef _XPORTUDP_H
#define _XPORTUDP_H

typedef struct SXPort SXPort;
typedef struct SXPortUDP SXPortUDP;

SXPortUDP* XPortUDP_Init(void* pLocAddr, void* pRemAddr, int iTimeout, int iBufLen);
void XPortUDP_Done(SXPortUDP* pPort);

#endif //_XPORTUDP_H
