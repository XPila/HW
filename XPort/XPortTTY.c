////////////////////////////////////////
// XPortTTY.c

#include "XPortTTY.h"
#include "XPort.h"
#include <malloc.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <stdint.h>
#include <termios.h>
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>


//#include <unistd.h>
//#include <sys/time.h>
//#include <termios.h>

typedef struct SXPortTTY
{
	SXPort sPort;
	int iFile;
} SXPortTTY;

char* hexstr(char* p, int n);//implemented in XPort.c
speed_t encode_baud(int baud);
int decode_baud(speed_t cbaud);
tcflag_t encode_size(int size);
int decode_size(tcflag_t csize);
int setup_ts(struct termios* pts, int baud, int size, int parity, int stop, int timeout);


void* XPortTTY_LogFile = 0;

int XPortTTY_Rx(SXPortTTY* pPort, void* pData, int iSize);
int XPortTTY_Tx(SXPortTTY* pPort, void* pData, int iSize);

SXPortTTY* XPortTTY_Init(char* pcDev, int iBaud, int iSize, int iParity, int iStop, int iTimeout)
{
	if (!pcDev) return 0;
	SXPortTTY sPort = {{(FXPortRxTx*)&XPortTTY_Rx, (FXPortRxTx*)&XPortTTY_Tx}, 0};
	sPort.iFile = open(pcDev, O_RDWR | O_NOCTTY);
	if (sPort.iFile == -1) return 0;
	struct termios ts;
	if (tcgetattr(sPort.iFile, &ts) != 0) goto e0;
	if (setup_ts(&ts, iBaud, iSize, iParity, iStop, iTimeout) != 0) goto e0;
	if (tcsetattr(sPort.iFile, TCSANOW, &ts) != 0) goto e0;
	SXPortTTY* pPort = (SXPortTTY*)malloc(sizeof(SXPortTTY));
	*pPort = sPort;
	return pPort;
e0:
	close(sPort.iFile);
	return 0;
}
	
void XPortTTY_Done(SXPortTTY* pPort)
{
	if (!pPort) return;
	if (pPort->iFile) close(pPort->iFile);
	free(pPort);
}

int XPortTTY_GetBaud(SXPortTTY* pPort)
{
	if (!pPort) return -1;
	struct termios ts;
	if (tcgetattr(pPort->iFile, &ts) != 0) return -1;
	return decode_baud(cfgetospeed(&ts));
}

int XPortTTY_SetBaud(SXPortTTY* pPort, int iBaud)
{
	if (!pPort) return -1;
	struct termios ts;
	if (tcgetattr(pPort->iFile, &ts) != 0) return -1;
	int baud = decode_baud(cfgetispeed(&ts));
	int cbaud = encode_baud(iBaud);
	if (cbaud == 0) return 
	cfsetispeed(&ts, cbaud);
	cfsetospeed(&ts, cbaud);
	if (tcsetattr(pPort->iFile, TCSANOW, &ts) != 0) return -1;
	return baud;
}

int XPortTTY_GetModem(SXPortTTY* pPort)
{
	if (!pPort) return -1;
	int iModem = 0;
	if (ioctl(pPort->iFile, TIOCMGET, &iModem) < 0) return -1;
	return iModem;
}

int XPortTTY_SetModem(SXPortTTY* pPort, int iModem)
{
	if (!pPort) return -1;
	if (ioctl(pPort->iFile, TIOCMSET, &iModem) < 0) return -1;
	return 0;
}

int XPortTTY_ClrSetModem(SXPortTTY* pPort, int iModem, int iMask)
{
	if (!pPort) return -1;
	int iClr = ~iModem & iMask;
	int iSet = iModem & iMask;
	if (iClr) if (ioctl(pPort->iFile, TIOCMBIC, &iClr) < 0) return -1;
	if (iSet) if (ioctl(pPort->iFile, TIOCMBIS, &iSet) < 0) return -1;
	return 0;
}

int XPortTTY_Rx(SXPortTTY* pPort, void* pData, int iSize)
{
	if (!pPort) return -1;
	int iRx = read(pPort->iFile, pData, iSize);
	int iCnt = iRx;
	if ((iCnt > 0) && (iCnt < iSize))
		while (iCnt < iSize)
			if ((iRx = read(pPort->iFile, pData + iCnt, iSize - iCnt)) <= 0) break;
			else iCnt += iRx;
	if (XPortTTY_LogFile)
	{
		char* pcHex = hexstr(pData, iSize);
		pcHex[2*iCnt] = 0;
		fprintf((FILE*)XPortTTY_LogFile, "TTY Rx %d %s (cnt=%d rx=%d)\n", iSize, pcHex, iCnt, iRx);
//		if (iHub == iRx == iSize)
//			fprintf((FILE*)XHub_LogFile, "HUB%d Rx %d %s\n", pPort - pPort->pHub->sPort, iSize, pcHex);
//		else
//			fprintf((FILE*)XHub_LogFile, "HUB%d Rx Err %d %s (tx=%d hub=%d)\n", pPort - pPort->pHub->sPort, iSize, pcHex, iRx, iHub);
		free(pcHex);
	}
	return iCnt;
}

int XPortTTY_Tx(SXPortTTY* pPort, void* pData, int iSize)
{
	if (!pPort) return -1;
	int iTx = write(pPort->iFile, pData, iSize);
	fsync(pPort->iFile);
	if (XPortTTY_LogFile)
	{
		char* pcHex = hexstr(pData, iSize);
		if (iTx == iSize)
			fprintf((FILE*)XPortTTY_LogFile, "TTY Tx %d %s\n", iSize, pcHex);
		else
			fprintf((FILE*)XPortTTY_LogFile, "TTY Tx Err %d %s (tx=%d)\n", iSize, pcHex, iTx);
		free(pcHex);
	}
	return iTx;
}

speed_t encode_baud(int baud)
{
	switch (baud)
	{
	case 300: return B300;
	case 600: return B600;
	case 1200: return B1200;
	case 2400: return B2400;
	case 4800: return B4800;
	case 9600: return B9600;
	case 19200: return B19200;
	case 38400: return B38400;
	case 57600: return B57600;
	case 115200: return B115200;
	}
	return 0;
}

int decode_baud(speed_t cbaud)
{
	switch (cbaud)
	{
	case B300: return 300;
	case B600: return 600;
	case B1200: return 1200;
	case B2400: return 2400;
	case B4800: return 4800;
	case B9600: return 9600;
	case B19200: return 19200;
	case B38400: return 38400;
	case B57600: return 57600;
	case B115200: return 115200;
	}
	return 0;
}

tcflag_t encode_size(int size)
{
	switch (size)
	{
	case 5: return CS5;
	case 6: return CS6;
	case 7: return CS7;
	case 8: return CS8;
	}
	return 0;
}


int decode_size(tcflag_t csize)
{
	switch (csize)
	{
	case CS5: return 5;
	case CS6: return 6;
	case CS7: return 7;
	case CS8: return 8;
	}
	return 0;
}


int setup_ts(struct termios* pts, int baud, int size, int parity, int stop, int timeout)
{
	//encode baud
	speed_t cbaud = encode_baud(baud);
	//check baud
	if (cbaud == 0) return errno = EINVAL?-1:0;
	//encode size
	tcflag_t csize = encode_size(size);
	//check size
	if (csize == 0) return errno = EINVAL?-1:0;
	//check parity
	if ((parity < 0) || (parity > 2)) return errno = EINVAL?-1:0;
	//check stop
	if ((stop < 1) || (stop > 2)) return errno = EINVAL?-1:0;
	//check timeout
	if (timeout < 0) return errno = EINVAL?-1:0;
	//cleanup
	bzero(pts, sizeof(struct termios));
	//default settings
	cfmakeraw(pts);
	//reset input modes
	pts->c_iflag = 0;
	//reset output modes
	pts->c_oflag = 0;
	//set input baudrate
	cfsetispeed(pts, cbaud);
	//set output baudrate
	cfsetospeed(pts, cbaud);
	//local mode
	pts->c_cflag |= CLOCAL;
	//enable the receiver
	pts->c_cflag |= CREAD;
	//disable hardware flow control
	pts->c_cflag &= ~CRTSCTS;
	//no parity
	pts->c_cflag &= ~PARENB;
	//stopbit
	if (stop == 1)
		pts->c_cflag &= ~CSTOPB;
	if (stop == 2)
		pts->c_cflag |= CSTOPB;
	//mask datasize
	pts->c_cflag &= ~CSIZE;
	//datasize 5,6,7,8 bit
	pts->c_cflag |= csize;
	//data processed as raw input (noncanonical)
	pts->c_lflag &= ~ICANON;
	//no echo
	pts->c_lflag &= ~ECHO;
	//no signal
	pts->c_lflag &= ~ISIG;
	//minimum number of characters
	pts->c_cc[VMIN] = timeout?0:1;
	//timeout in 1/10s
	pts->c_cc[VTIME] = timeout?(timeout / 100):0;
	return 0;
}
