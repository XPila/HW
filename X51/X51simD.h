////////////////////////////////////////////////////////////////////////////////
// X51SimD.h
// simulator definition header file


#ifndef _X51SIMD_H
#define _X51SIMD_H

#define X51SIM_RES_UNHINS  -1 //unhandled instruction (just for debuging, stable code will never return this value)
#define X51SIM_RES_INS_A5   1 //instruction 0xa5 - can be used as breakpoint
#define X51SIM_RES_STCKOF   2 //stack overflow (_SP > stk_max)
#define X51SIM_RES_STCKUF   3 //stack overflow (_SP < stk_min)
#define X51SIM_RES_AVCODE   4 //Code access violation
#define X51SIM_RES_AVIDAT   5 //IData access violation
#define X51SIM_RES_AVXDAT   6 //XData access violation

//chip manufacturer
#define X51_CMNF_Atmel	0x01

//chip family
#define X51_CFAM_AT89C5x   	0x01
#define X51_CFAM_AT89Cx051 	0x02
#define X51_CFAM_AT89S5x   	0x03
#define X51_CFAM_AT89Sx051 	0x04
#define X51_CFAM_AT89LPx052	0x05

//chip definitions (MNF<<16 | FAM<<8 | TYP)
#define X51_CTYP_AT89C51   	((X51_CMNF_Atmel << 16) | (X51_CFAM_AT89C5x << 8) | 0x01)
#define X51_CTYP_AT89C52   	((X51_CMNF_Atmel << 16) | (X51_CFAM_AT89C5x << 8) | 0x02)
#define X51_CTYP_AT89C53   	((X51_CMNF_Atmel << 16) | (X51_CFAM_AT89C5x << 8) | 0x03)
#define X51_CTYP_AT89S51   	((X51_CMNF_Atmel << 16) | (X51_CFAM_AT89S5x << 8) | 0x01)
#define X51_CTYP_AT89S52   	((X51_CMNF_Atmel << 16) | (X51_CFAM_AT89S5x << 8) | 0x02)
#define X51_CTYP_AT89S53   	((X51_CMNF_Atmel << 16) | (X51_CFAM_AT89S5x << 8) | 0x03)
#define X51_CTYP_AT89C1051 	((X51_CMNF_Atmel << 16) | (X51_CFAM_AT89Cx051 << 8) | 0x01)
#define X51_CTYP_AT89C2051 	((X51_CMNF_Atmel << 16) | (X51_CFAM_AT89Cx051 << 8) | 0x02)
#define X51_CTYP_AT89C4051 	((X51_CMNF_Atmel << 16) | (X51_CFAM_AT89Cx051 << 8) | 0x03)
#define X51_CTYP_AT89S1051 	((X51_CMNF_Atmel << 16) | (X51_CFAM_AT89Sx051 << 8) | 0x01)
#define X51_CTYP_AT89S2051 	((X51_CMNF_Atmel << 16) | (X51_CFAM_AT89Sx051 << 8) | 0x02)
#define X51_CTYP_AT89S4051 	((X51_CMNF_Atmel << 16) | (X51_CFAM_AT89Sx051 << 8) | 0x03)
#define X51_CTYP_AT89LP1052	((X51_CMNF_Atmel << 16) | (X51_CFAM_AT89LPx052 << 8) | 0x01)
#define X51_CTYP_AT89LP2052	((X51_CMNF_Atmel << 16) | (X51_CFAM_AT89LPx052 << 8) | 0x02)
#define X51_CTYP_AT89LP4052	((X51_CMNF_Atmel << 16) | (X51_CFAM_AT89LPx052 << 8) | 0x03)


#endif //_X51SIMDEF_H