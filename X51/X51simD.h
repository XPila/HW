////////////////////////////////////////////////////////////////////////////////
// X51SimDef.h
// simulator definition header file

#ifndef _X51SIMDEF_H
#define _X51SIMDEF_H

//chip manufacturer
#define X51_CMNF_Atmel	0x01

//chip family
#define X51_CFAM_AT89C5x   	0x01
#define X51_CFAM_AT89Cx051 	0x02
#define X51_CFAM_AT89S5x   	0x03
#define X51_CFAM_AT89Sx051 	0x04
#define X51_CFAM_AT89LPx052	0x05

//chip type
#define X51_CTYP_AT89C51   	(X51_CMNF_Atmel << 16) | (ex51ChippFamily_AT89C5x << 8) | 0x01,
#define X51_CTYP_AT89C52   	(X51_CMNF_Atmel << 16) | (ex51ChippFamily_AT89C5x << 8) | 0x02,
#define X51_CTYP_AT89C53   	(X51_CMNF_Atmel << 16) | (ex51ChippFamily_AT89C5x << 8) | 0x03,
#define X51_CTYP_AT89S51   	(X51_CMNF_Atmel << 16) | (ex51ChippFamily_AT89S5x << 8) | 0x01,
#define X51_CTYP_AT89S52   	(X51_CMNF_Atmel << 16) | (ex51ChippFamily_AT89S5x << 8) | 0x02,
#define X51_CTYP_AT89S53   	(X51_CMNF_Atmel << 16) | (ex51ChippFamily_AT89S5x << 8) | 0x03,
#define X51_CTYP_AT89C1051 	(X51_CMNF_Atmel << 16) | (ex51ChippFamily_AT89Cx051 << 8) | 0x01,
#define X51_CTYP_AT89C2051 	(X51_CMNF_Atmel << 16) | (ex51ChippFamily_AT89Cx051 << 8) | 0x02,
#define X51_CTYP_AT89C4051 	(X51_CMNF_Atmel << 16) | (ex51ChippFamily_AT89Cx051 << 8) | 0x03,
#define X51_CTYP_AT89S1051 	(X51_CMNF_Atmel << 16) | (ex51ChippFamily_AT89Sx051 << 8) | 0x01,
#define X51_CTYP_AT89S2051 	(X51_CMNF_Atmel << 16) | (ex51ChippFamily_AT89Sx051 << 8) | 0x02,
#define X51_CTYP_AT89S4051 	(X51_CMNF_Atmel << 16) | (ex51ChippFamily_AT89Sx051 << 8) | 0x03,
#define X51_CTYP_AT89LP1052	(X51_CMNF_Atmel << 16) | (ex51ChippFamily_AT89LPx052 << 8) | 0x01,
#define X51_CTYP_AT89LP2052	(X51_CMNF_Atmel << 16) | (ex51ChippFamily_AT89LPx052 << 8) | 0x02,
#define X51_CTYP_AT89LP4052	(X51_CMNF_Atmel << 16) | (ex51ChippFamily_AT89LPx052 << 8) | 0x03,


#endif //_X51SIMDEF_H
