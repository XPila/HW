////////////////////////////////////////////////////////////////////////////////
// X51modD.h
// X51 module manipulation definition header file


#ifndef _X51MODD_H
#define _X51MODD_H


//module manipulation errors
#define X51MOD_ERR -100 //module error base
#define X51MOD_ERR_MNTLNG X51MOD_ERR - 1 //module name too long
#define X51MOD_ERR_SNTLNG X51MOD_ERR - 2 //symbol name too long

//
#define X51_MAX_STR_MOD 32
#define X51_MAX_STR_SYM 32
#define X51_MAX_STR_SEG 32

//default chunk values for realloc
#define X51MOD_DEF_SYMCHUNK  32 //symbol chunk (number of symbols)
#define X51MOD_DEF_STRCHUNK 128 //string chunk (number of characters)
#define X51MOD_DEF_SEGCHUNK   8 //segment chunk (number of segments)

//value types
#define X51MOD_VTYP_C  1 //code
#define X51MOD_VTYP_D  2 //data
#define X51MOD_VTYP_B  3 //bit
#define X51MOD_VTYP_N  4 //number

//symbol types
#define X51MOD_STYP_SEG  1 //segment
#define X51MOD_STYP_PUB  2 //public
#define X51MOD_STYP_SYM  3 //symbol
#define X51MOD_STYP_EXT  4 //extern
#define X51MOD_STYP_LIN  5 //line#

//combined types
#define X51MOD_TYP_SEGC  ((X51MOD_STYP_SEG << 4) | X51MOD_VTYP_C)
#define X51MOD_TYP_SEGD  ((X51MOD_STYP_SEG << 4) | X51MOD_VTYP_D)
#define X51MOD_TYP_SEGB  ((X51MOD_STYP_SEG << 4) | X51MOD_VTYP_B)
#define X51MOD_TYP_PUBC  ((X51MOD_STYP_PUB << 4) | X51MOD_VTYP_C)
#define X51MOD_TYP_PUBD  ((X51MOD_STYP_PUB << 4) | X51MOD_VTYP_D)
#define X51MOD_TYP_PUBB  ((X51MOD_STYP_PUB << 4) | X51MOD_VTYP_B)
#define X51MOD_TYP_SYMC  ((X51MOD_STYP_SYM << 4) | X51MOD_VTYP_C)
#define X51MOD_TYP_SYMD  ((X51MOD_STYP_SYM << 4) | X51MOD_VTYP_D)
#define X51MOD_TYP_SYMB  ((X51MOD_STYP_SYM << 4) | X51MOD_VTYP_B)
#define X51MOD_TYP_SYMN  ((X51MOD_STYP_SYM << 4) | X51MOD_VTYP_N)
#define X51MOD_TYP_EXTC  ((X51MOD_STYP_EXT << 4) | X51MOD_VTYP_C)
#define X51MOD_TYP_EXTD  ((X51MOD_STYP_EXT << 4) | X51MOD_VTYP_D)
#define X51MOD_TYP_EXTB  ((X51MOD_STYP_EXT << 4) | X51MOD_VTYP_B)
#define X51MOD_TYP_EXTN  ((X51MOD_STYP_EXT << 4) | X51MOD_VTYP_N)
#define X51MOD_TYP_LINC  ((X51MOD_STYP_LIN << 4) | X51MOD_VTYP_C)


#endif //_X51MODD_H