#include <malloc.h>
#include <stdio.h>

#include "../XPort.h"
#include "../XHub.h"
#include "MIDISlave.h"

//MIDI slave message callback
int MIDISlaveCB(void* pSlave, char cAddr, char cData);

//MIDI transmit message function
void MIDI_Tx(SXPort* pPort, char cAddr, char cData);

int main(int argc, char* argv[])
{
	SXHub* pHub = 0;//hub pointer
	SXPort* pPortHub0 = 0;//port0 pointer
	SXPort* pPortHub1 = 0;//port1 pointer
	SMIDISlave sSlave = {0};//slave structure
	//XHub_LogFile = stdout;//XHub log to stdout
	pHub = XHub_Init(2, 500, 64);//2 ports, 500ms timeout, 64bytes buffers
	pPortHub0 = XHub_Link(pHub, 0, 0);//link port0
	pPortHub1 = XHub_Link(pHub, 1, 0);//link port1
	
	MIDISlave_Init(&sSlave, pPortHub1, 0, &MIDISlaveCB);

	MIDI_Tx(pPortHub0, 0x00, 0x00);//transmit 0x00 to address 0x00
	MIDI_Tx(pPortHub0, 0x01, 0x01);//transmit 0x01 to address 0x01
	MIDI_Tx(pPortHub0, 0x02, 0x02);//transmit 0x02 to address 0x02

	sleep(1);//wait 1s

	MIDISlave_Done(&sSlave);

	XHub_Done(pHub);
	pHub = 0;
	pPortHub0 = 0;
	pPortHub1 = 0;
	
	return 0;
}

int MIDISlaveCB(void* pSlave, char cAddr, char cData)
{
	if (cAddr == 0x00)
	{
		printf("addr=0x00\n");//print the message
		// do something...
	}
	printf("addr=0x%02x data=0x%02x\n", cAddr, cData);//print the message
	return 0;
}

void MIDI_Tx(SXPort* pPort, char cAddr, char cData)
{
	char cTx[2] = {cAddr | 0x80, cData & 0x7f};
	XPort_Tx(pPort, cTx, 2);
}