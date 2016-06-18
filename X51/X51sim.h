////////////////////////////////////////////////////////////////////////////////
// X51sim.h
// X51 simulator header file


#ifndef _X51SIM_H
#define _X51SIM_H

#include "X51cmn.h" //common header
#include "X51simD.h" //simulator definition header


typedef unsigned char (FX51SimRd)(void* pParam, char cType, unsigned short usAddr);
typedef void (FX51SimWr)(void* pParam, char cType, unsigned short usAddr, unsigned char ucVal);

//Structure for simulator
typedef struct SX51Sim
{
	char core; //core type (0=X1, 1=SC)
	unsigned long freq; //Xtal frequency in Hz
	unsigned short PC; //PC - program counter
	unsigned char* Code; //Code memory pointer
	unsigned char* IData; //IData memory pointer
	unsigned char* XData; //XData memory pointer
	unsigned char* SFR; //SFR space pointer
	unsigned short code_msk; //Code address mask (e.g. 0x3ff for 1kb, 0x7ff for 2kb, etc)
	unsigned char idata_msk; //IData address mask (0x7f for 128byte or 0xff for 256 byte)
	unsigned short xdata_msk; //XData address mask
	unsigned char sfr_msk; //SFR address mask (always 0x7f)
	int code_sz; //Code memory size (in bytes)
	int idata_sz; //IData memory size (128 or 256)
	int xdata_sz; //XData memory size (in bytes)
	unsigned char sfr_sz; //SFR space size (always 128)
	FX51SimRd* rdSFR; //SFR read function
	FX51SimWr* wrSFR; //SFR write function
	unsigned long long tick; //Xtal tick (from reset)
	char cycle; //instruction cycle (0-3)
	char stop; //stop flag
	SX51Ins* pi; //instruction structure pointer (updated in cycle0)
	unsigned long code_q; //code queue - contain last four values readed from code memory
} SX51Sim;

//initializes simulator
SX51Sim* X51Sim_Init(int iChip, int iFreq);
//shutdowns simulator
void X51Sim_Done(SX51Sim* pSim);
//resets simulator (hardware reset)
void X51Sim_Reset(SX51Sim* pSim);
//executes one or more Xtal ticks (iCnt parameter), returns executed tick count (can be stopped durring execution)
int X51Sim_Tick(SX51Sim* pSim, int iCnt);
//executes one or more cycles (iCnt parameter), returns executed cycle count (can be stopped durring execution)
int X51Sim_Cycle(SX51Sim* pSim, int iCnt);
//executes one or more instructions (iCnt parameter), returns executed step count (can be stopped durring execution)
int X51Sim_Step(SX51Sim* pSim, int iCnt);
//run realtime (or time multiplied) execution, loops while not stopped, this function blocks, the iChunk is number of cycles at one time, returns executed cycle count
int X51Sim_Run(SX51Sim* pSim, float fTimeMultiply, int iChunk);
//same as X51Sim_Run, but nonblocking (runs in other thread), returns -1 in case of thread create error
int X51Sim_RunAsync(SX51Sim* pSim, float fTimeMultiply, int iChunk);
//stops the execution (set the flag "stop" to 1)
void X51Sim_Stop(SX51Sim* pSim);

void X51Sim_Print(SX51Sim* pSim, void* pFile, FX51Value2Name* pValue2Name, void* pParam);
int X51Sim_LoadHex(SX51Sim* pSim, char* pcFileName);

unsigned char X51Sim_Rd(SX51Sim* ps, char t, unsigned short a);
void X51Sim_Wr(SX51Sim* ps, char t, unsigned short a, unsigned char v);

void X51Sim_Int(SX51Sim* ps, char cIntV);

void X51Sim_Rx(SX51Sim* ps, char cRx);
void X51Sim_Tx(SX51Sim* ps);

#endif //_X51SIM_H