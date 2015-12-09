////////////////////////////////////////////////////////////////////////////////
// X51sim.h
// X51 simulator header file


#ifndef _X51SIM_H
#define _X51SIM_H

#include "X51cmn.h" //common header


typedef unsigned char (FX51SimRd)(void* pParam, char cType, unsigned short usAddr);
typedef void (FX51SimWr)(void* pParam, char cType, unsigned short usAddr, unsigned char ucVal);

//Structure for simulator
typedef struct SX51Sim
{
	char core; //core type (0=X1, 1=SC)
	unsigned long freq; //Xtal frequency in Hz
	unsigned char* Code; //Code memory pointer
	unsigned char* IData; //IData memory pointer
	unsigned char* XData; //XData memory pointer
	unsigned char* SFR; //SFR memory pointer
	int Code_len; //Code memory size (max 65536)
	int IData_len; //IData memory size (128 or 256 bytes)
	int XData_len; //XData memory size (max 65536)
	int SFR_len; //SRF memory size (always 128)
	unsigned char Code_empty; //empty Code memory value
	unsigned char IData_empty; //empty IData memory value
	unsigned char XData_empty; //empty XData memory value
	unsigned char SFR_empty; //empty SFR memory value
	FX51SimRd* rdSFR; //SFR read function
	FX51SimWr* wrSFR; //SFR write function
	unsigned short PC; //PC - program counter
	unsigned long long tick; //Xtal tick (from reset)
	char cycle; //instruction cycle (0-4)
	char stop; //stop flag
	SX51Ins* pi; //instruction structure pointer (updated in cycle0)
} SX51Sim;

//initialize simulator
SX51Sim* X51Sim_Init(char* pcChip);
//shutdown simulator
void X51Sim_Done(SX51Sim* pSim);
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


#endif //_X51SIM_H
