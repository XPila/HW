////////////////////////////////////////////////////////////////////////////////
// X51cmn.h
// X51 common header file


#ifndef _X51CMN_H
#define _X51CMN_H

#include "X51cmnD.h" //common definition header


typedef char* (FX51Value2Name)(void* pParam, char cType, int iValue);
typedef int (FX51Name2Value)(void* pParam, char cType, char* pcName);
typedef unsigned char (FX51Rd)(void* pParam, char cType, int iAddr);
typedef void (FX51Wr)(void* pParam, char cType, int iAddr, unsigned char ucByte);

//Structure for instruction definition
typedef struct SX51Ins
{
	char code; //instruction code
	char op0; //operand 0 type
	char op1; //operand 1 type
	char op2; //operand 2 type
	char ins; //instruction index
	char len; //length in bytes
	char cyc_x1; //number of cycles in X1 mode (normal)
	char cyc_sc; //number of cycles in SC mode (single cycle)
} SX51Ins;

//Structure for operand masks and variants
typedef struct SX51InsOp
{
	char opcnt; //number of operands
	unsigned long opm0; //operand 0 mask
	unsigned long opm1; //operand 1 mask
	unsigned long opm2; //operand 2 mask
	char opvcnt; //variant count
	unsigned long* opvars; //variants
} SX51InsOp;


//--------------------------------------
//general functions

//initialization
int X51_Init();
//shutdown
void X51_Done();



//--------------------------------------
//helper functions

//encode bit addres (0-255) to idata addres multiplied by 8 (bit 0-2 is bit index)
int X51_BitAddrEnc(unsigned char ucBitAddr);
//decode bit addres (0-255) from idata addres multiplied by 8 (bit 0-2 is bit index)
int X51_BitAddrDec(int iIDataX8Addr);


//decode instruction - this function is used just for generating the instruction table ()
int X51_DecIns(unsigned char ucCode, int* piIns, int* piOp0, int* piOp1, int* piOp2, int* piBytes, int* piCyclesX1, int* piCyclesSC);

int printf_bin(int val, int bits);


#endif //_X51CMN_H
