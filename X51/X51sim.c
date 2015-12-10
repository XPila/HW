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
	unsigned short PC; //PC - program counter
	unsigned char Code[X51_MAX_CODE + 1]; //Code memory
	unsigned char IData[X51_MAX_IDATA + 1]; //IData memory
	unsigned char XData[X51_MAX_XDATA + 1]; //XData memory
	unsigned char SFR[X51_MAX_SFR - X51_MIN_SFR + 1]; //SFR memory
	FX51SimRd* rdSFR; //SFR read function
	FX51SimWr* wrSFR; //SFR write function
	unsigned long long tick; //Xtal tick (from reset)
	char cycle; //instruction cycle (0-4)
	char stop; //stop flag
	SX51Ins* pi; //instruction structure pointer (updated in cycle0)
} SX51Sim;

//initializes simulator
int X51Sim_Init(SX51Sim* pSim, char* pcChip);
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

void X51Sim_Print(SX51Sim* pSim, void* pFile);

unsigned char X51Sim_Rd(SX51Sim* ps, char t, unsigned short a);
void X51Sim_Wr(SX51Sim* ps, char t, unsigned short a, unsigned char v);


#endif //_X51SIM_H
