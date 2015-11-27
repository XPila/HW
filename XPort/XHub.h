////////////////////////////////////////
// XHub.h

#ifndef _XHUB_H
#define _XHUB_H

typedef struct SXPort SXPort;
typedef struct SXHub SXHub;

void* XHub_LogFile;

SXHub* XHub_Init(int iPorts, int iTimeout, int iBufLen);
void XHub_Done(SXHub* pHub);
SXPort* XHub_Link(SXHub* pHub, int iPort, SXPort* pLnkPort);
void XHub_Stop(SXHub* pHub);

#endif //_XHUB_H
