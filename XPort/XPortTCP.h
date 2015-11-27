////////////////////////////////////////
// XPortTCP.h

#ifndef _XPORTTCP_H
#define _XPORTTCP_H

typedef struct SXPort SXPort;
typedef struct SXPortTCP SXPortTCP;

SXPortTCP* XPortTCP_Init(void* pLocAddr, void* pRemAddr, int iTimeout);
void XPortTCP_Done(SXPortTCP* pPort);

#endif //_XPORTTCP_H
