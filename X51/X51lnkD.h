////////////////////////////////////////////////////////////////////////////////
// X51lnkD.h
// X51 linker definition header file

#ifndef _X51LNKD_H
#define _X51LNKD_H


#define X51LNK_ERR_LNTLNG -2 //line too long
#define X51LNK_ERR_MNTLNG -3 //module name too long
#define X51LNK_ERR_SNTLNG -4 //symbol name too long


#define X51LNK_DEF_MODCHUNK 8


#define X51_MAX_STR_M51LINE 120
#define X51_MAX_STR_M51VAL  7
#define X51_MAX_STR_M51TYPE 16
#define X51_MAX_STR_M51NAME 32


#define X51LNK_FLGM51_EXCSFR 1 //exclude SFR symbols (DATA or BIT, address > 0x80)
#define X51LNK_FLGM51_EXCLIN 2 //exclude LINE# symbols
#define X51LNK_FLGM51_EXCNUM 3 //exclude NUMBER symbols


#endif //_X51LNKD_H