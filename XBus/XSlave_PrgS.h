////////////////////////////////////////
// XSlave_PrgS.h


#ifndef _XSLAVE_PRGS_H
#define _XSLAVE_PRGS_H

typedef struct SXSlave_PrgS SXSlave_PrgS;

SXSlave_PrgS* XSlave_PrgS_Init(SXPort* pPort, int iFlags);
void XSlave_PrgS_Done(SXSlave_PrgS* pSlave);

#endif //_XSLAVE_PRGS_H
