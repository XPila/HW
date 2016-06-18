////////////////////////////////////////////////////////////////////////////////
// X51dasD.h
// X51 disassembler definition header file

#ifndef _X51DASD_H
#define _X51DASD_H


//disassembler flags
#define X51_DAS_FLG_INADDR       1 // include address (5 chars at begining - format "0123 ")
#define X51_DAS_FLG_INCODE       2 // include code (7 chars at begining after address - format "001122 ")
#define X51_DAS_FLG_INRELA       4 // include relative jumps target address (7 chars at end - format "($0123)")
#define X51_DAS_FLG_INLABS       8 // include labels (line containing label and ":")
#define X51_DAS_FLG_DISASM       15 // all flags


#endif //_X51DASD_H