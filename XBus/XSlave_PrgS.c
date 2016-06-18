////////////////////////////////////////
// XSlave_PrgS.c

#include <malloc.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "../XPort/XPort.h"
#include "XBus.h"
#include "XSlave.h"


typedef struct SXSlave_PrgS
{
	SXSlave sSlave;
	int iRunning;
	pthread_cond_t sCond;
	pthread_mutex_t sMutex;
	pthread_t sThread;
	char cCLOCK_Time[4];
	char cCALENDAR_Date[4];
	unsigned short usMAIN_AT24C256_Addr;
} SXSlave_PrgS;

char cXSLAVE_ModuleName[] = "PrgS-   ";
char cMAIN_SerialNumber[] = "12345678";
char cMAIN_VersionSupport[] = {0,0,0,0,0,0,0,0};

XSLAVE_TABLE_DECL(ToAll)
XSLAVE_TABLE_DECL(ToMy)


void XSlave_PrgS_10ms(SXSlave_PrgS* pSlave)
{
	if ((pSlave->cCLOCK_Time[3] = (++pSlave->cCLOCK_Time[3] % 100)) == 0)
		if ((pSlave->cCLOCK_Time[2] = (++pSlave->cCLOCK_Time[2] % 60)) == 0)
			if ((pSlave->cCLOCK_Time[1] = (++pSlave->cCLOCK_Time[1] % 60)) == 0)
				if ((pSlave->cCLOCK_Time[0] = (++pSlave->cCLOCK_Time[0] % 24)) == 0)
					pSlave->cCALENDAR_Date[3] = (++pSlave->cCALENDAR_Date[3]);
}

void* XSlave_PrgS_Thread(void* pv)
{
	SXSlave_PrgS* pSlave = (SXSlave_PrgS*)pv;
	pthread_cond_signal(&pSlave->sCond);
	SXBusMsg sMsg;
	while (pSlave->iRunning)
	{
		XSlave_PrgS_10ms(pSlave);
		usleep(10000);
	}
	pthread_cond_signal(&pSlave->sCond);
	return 0;
}

SXSlave_PrgS* XSlave_PrgS_Init(SXPort* pPort, int iFlags)
{
	SXSlave_PrgS* pSlave = (SXSlave_PrgS*)malloc(sizeof(SXSlave_PrgS));
	memset(pSlave, 0, sizeof(SXSlave_PrgS));
	XSlave_Init(&pSlave->sSlave, pPort, iFlags, &XSlaveTable_ToAll, &XSlaveTable_ToMy);
	pSlave->iRunning = 1;
	pthread_cond_init(&pSlave->sCond, 0);
	pthread_create(&pSlave->sThread, 0, &XSlave_PrgS_Thread, pSlave);
	pthread_cond_wait(&pSlave->sCond, 0);
	return pSlave;
}

void XSlave_PrgS_Done(SXSlave_PrgS* pSlave)
{
	pSlave->iRunning = 0;
	pthread_cond_wait(&pSlave->sCond, 0);
	pthread_join(pSlave->sThread, 0);
	pthread_detach(pSlave->sThread);
	pthread_cond_destroy(&pSlave->sCond);
	XSlave_Done(&pSlave->sSlave);
	free(pSlave);
}


void SYSTEM_Reset(void* pSlave, unsigned char ucSize, SXBusMsg* psMsg)
{ printf("SYSTEM_Reset\n"); }
void SYSTEM_WarmReset(void* pSlave, unsigned char ucSize, SXBusMsg* psMsg)
{ printf("SYSTEM_WarmReset\n"); }
void SYSTEM_ColdReset(void* pSlave, unsigned char ucSize, SXBusMsg* psMsg)
{ printf("SYSTEM_ColdReset\n"); }
void MAIN_SelfTest(void* pSlave, unsigned char ucSize, SXBusMsg* psMsg)
{ printf("MAIN_SelfTest\n"); }
void MAIN_RWP1(void* pSlave, unsigned char ucSize, SXBusMsg* psMsg)
{ printf("MAIN_RWP1\n"); }
void MAIN_RWP3(void* pSlave, unsigned char ucSize, SXBusMsg* psMsg)
{ printf("MAIN_RWP3\n"); }

void XSLAVE_ModuleName(void* pSlave, unsigned char ucSize, SXBusMsg* psMsg)
{
	if (psMsg->sHead.ucRdFlag) memcpy(psMsg->ucData, cXSLAVE_ModuleName, ucSize);
	printf("XSLAVE_ModuleName %d %d %s\n", ucSize, psMsg->sHead.ucDataSize, cXSLAVE_ModuleName);
}
void MAIN_SerialNumber(void* pSlave, unsigned char ucSize, SXBusMsg* psMsg)
{
	if (psMsg->sHead.ucRdFlag) memcpy(psMsg->ucData, cMAIN_SerialNumber, ucSize);
	printf("MAIN_SerialNumber\n");
}
void MAIN_VersionSupport(void* pSlave, unsigned char ucSize, SXBusMsg* psMsg)
{
	if (psMsg->sHead.ucRdFlag) memcpy(psMsg->ucData, cMAIN_VersionSupport, ucSize);
	printf("MAIN_VersionSupport\n");
}
void SPI_RWConfig(void* pSlave, unsigned char ucSize, SXBusMsg* psMsg)
{ printf("SPI_RWConfig\n"); }
void SPI_RWDataStop(void* pSlave, unsigned char ucSize, SXBusMsg* psMsg)
{ printf("SPI_RWDataStop\n"); }
void SPI_RWDataNoStop(void* pSlave, unsigned char ucSize, SXBusMsg* psMsg)
{ printf("SPI_RWDataNoStop\n"); }

void CLOCK_Time(SXSlave_PrgS* pSlave, unsigned char ucSize, SXBusMsg* psMsg)
{
	if (psMsg->sHead.ucRdFlag) memcpy(psMsg->ucData, pSlave->cCLOCK_Time, ucSize);
	if (psMsg->sHead.ucWrFlag) memcpy(pSlave->cCLOCK_Time, psMsg->ucData, ucSize);
	printf("CLOCK_Time\n");
}
void CALENDAR_Date(SXSlave_PrgS* pSlave, unsigned char ucSize, SXBusMsg* psMsg)
{
	if (psMsg->sHead.ucRdFlag) memcpy(psMsg->ucData, pSlave->cCALENDAR_Date, ucSize);
	if (psMsg->sHead.ucWrFlag) memcpy(pSlave->cCALENDAR_Date, psMsg->ucData, ucSize);
	printf("CALENDAR_Date\n");
}
void MAIN_AT24C256_Addr(void* pSlave, unsigned char ucSize, SXBusMsg* psMsg)
{
	printf("MAIN_AT24C256_Addr\n");
}

void MAIN_AT24C256_RWData(void* pSlave, unsigned char ucSize, SXBusMsg* psMsg)
{ printf("MAIN_AT24C256_RWData\n"); }
void MAIN_ReadAD0(void* pSlave, unsigned char ucSize, SXBusMsg* psMsg)
{ printf("MAIN_ReadAD0\n"); }
void MAIN_ReadAD1(void* pSlave, unsigned char ucSize, SXBusMsg* psMsg)
{ printf("MAIN_ReadAD1\n"); }


XSLAVE_TABLE_BEGIN(ToAll)
XSLAVE_TABLE_END(ToAll)

XSLAVE_TABLE_BEGIN(ToMy)
	XSLAVE_ENTRY_F		(0x0000, 0, SYSTEM_Reset)
	XSLAVE_ENTRY_F		(0x0001, 0, SYSTEM_WarmReset)
	XSLAVE_ENTRY_F		(0x0002, 0, SYSTEM_ColdReset)
	XSLAVE_ENTRY_F		(0x0003, 0, MAIN_SelfTest)
	XSLAVE_ENTRY_FRW	(0x0005, 1, MAIN_RWP1)
	XSLAVE_ENTRY_FRW	(0x0007, 1, MAIN_RWP3)
	XSLAVE_ENTRY_CR		(0x0008, 8, XSLAVE_ModuleName)
	XSLAVE_ENTRY_CR		(0x0009, 8, MAIN_SerialNumber)
	XSLAVE_ENTRY_CR		(0x000A, 8, MAIN_VersionSupport)
	XSLAVE_ENTRY_FRW	(0x000B, 1, SPI_RWConfig)
	XSLAVE_ENTRY_FRW	(0x000C, 8, SPI_RWDataStop)
	XSLAVE_ENTRY_FRW	(0x000D, 8, SPI_RWDataNoStop)
	XSLAVE_ENTRY_IRW	(0x0011, 4, CLOCK_Time)
	XSLAVE_ENTRY_IRW	(0x0021, 4, CALENDAR_Date)
	XSLAVE_ENTRY_IRW	(0x0101, 2, MAIN_AT24C256_Addr)
	XSLAVE_ENTRY_FRW	(0x0102, 8, MAIN_AT24C256_RWData)
	XSLAVE_ENTRY_FR		(0x5000, 2, MAIN_ReadAD0)
	XSLAVE_ENTRY_FR		(0x5001, 2, MAIN_ReadAD1)
//;	XSLAVE_ENTRY_IRW	(0xA001, 4, MAIN_CntDelay)
//;	XSLAVE_ENTRY_FR		(0xA002, 4, MAIN_CntMeasure)
XSLAVE_TABLE_END(ToMy)