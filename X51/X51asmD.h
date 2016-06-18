////////////////////////////////////////////////////////////////////////////////
// X51asmD.h
// X51 assembler definition header file

#ifndef _X51ASMD_H
#define _X51ASMD_H


//max other..
#define X51_MAX_INCDEPTH     16 //assembler max include depth (0-based)
#define X51_MAX_TOKEN        4 //assembler max token number (0-based)
#define X51_MAX_IDENT        256 //assembler max identificator number (0-based)

//max string lengths
#define X51_MAX_STR_OP       64 //operand string
#define X51_MAX_STR_INS      5 //instruction name
#define X51_MAX_STR_LINE     256 //line (assembler)
#define X51_MAX_STR_TOKEN    128 //token (assembler)
#define X51_MAX_STR_SCAN     64 //scan string (assembler)


//assembler segment types
#define X51_ASM_ASEG_CODE  'C'	//Code - absolute (CSEG AT)
#define X51_ASM_ASEG_DATA  'D'	//Data - absolute (DSEG AT)
#define X51_ASM_ASEG_BIT   'B'	//Bit - absolute (BSEG AT)
#define X51_ASM_RSEG_CODE  'c'	//Code - relocatable (SEGMENT CODE)
#define X51_ASM_RSEG_DATA  'd'	//Data - relocatable (SEGMENT DATA)
#define X51_ASM_RSEG_BIT   'b'	//Bit - relocatable (SEGMENT BIT)

//assembler token types
#define X51_ASM_TKT_DIREC     1 //directive
#define X51_ASM_TKT_CTLST     2 //control statement
#define X51_ASM_TKT_INSTR     3 //instruction
#define X51_ASM_TKT_NUMCN     4 //numeric constant
#define X51_ASM_TKT_STRCN     5 //string constant
#define X51_ASM_TKT_IDENT     6 //identificator
#define X51_ASM_TKT_LABEL     7 //label

//assembler symbol types
#define X51_SYM_CODE  'C'	//Code
#define X51_SYM_DATA  'D'	//Data
#define X51_SYM_BIT   'B'	//Bit
#define X51_SYM_NUM   'N'	//Number

//assembler directives
#define X51_ASM_DIR_INCLUDE  1 //"$INCLUDE"
#define X51_ASM_DIR_SET      2 //"$SET"

//assembler control statements
#define X51_ASM_STA_NAME     1 //"NAME"
#define X51_ASM_STA_ORG      2 //"ORG"

/*#define X51_ASM_KWD_END      2 //"END"
#define X51_ASM_KWD_SEGMENT  3 //"SEGMENT"
#define X51_ASM_KWD_CODE     4 //"CODE"
#define X51_ASM_KWD_DATA     5 //"DATA"
#define X51_ASM_KWD_CSEG     6 //"CSEG"
#define X51_ASM_KWD_DSEG     7 //"DSEG"
#define X51_ASM_KWD_NAME     8 //"NAME"
#define X51_ASM_KWD_EXTRN    9 //"EXTRN"
#define X51_ASM_KWD_BIT     10 //"BIT"
#define X51_ASM_KWD_IF      11 //"IF"
#define X51_ASM_KWD_ENDIF   12 //"ENDIF"
#define X51_ASM_KWD_ELSEIF  13 //"ELSEIF"
#define X51_ASM_KWD_EQU     14 //"EQU"
#define X51_ASM_KWD_USING   15 //"USING"
#define X51_ASM_KWD_DS      16 //"DS"
#define X51_ASM_KWD_DB      17 //"DB"
#define X51_ASM_KWD_DW      18 //"DW"
#define X51_ASM_KWD_DBIT    19 //"DBIT"*/

//asembler error codes
#define X51_ASM_ERR_SUCCES        0 // Success
#define X51_ASM_ERR_SYNTAX       -1 // Syntax error
#define X51_ASM_ERR_INVCHR       -2 // Invalid character
#define X51_ASM_ERR_UNKINS       -3 // Unknown instruction
#define X51_ASM_ERR_UEXPOP       -4 // Unexpected operand - !!!unused?
#define X51_ASM_ERR_EXP0OP       -5 // Expected 0 operands
#define X51_ASM_ERR_EXP1OP       -6 // Expected 1 operands
#define X51_ASM_ERR_EXP2OP       -7 // Expected 2 operands
#define X51_ASM_ERR_EXP3OP       -8 // Expected 3 operands
#define X51_ASM_ERR_INVOP0       -9 // Invalid operand 0
#define X51_ASM_ERR_INVOP1      -10 // Invalid operand 1
#define X51_ASM_ERR_INVOP2      -11 // Invalid operand 2
#define X51_ASM_ERR_SYMBNF      -12 // Symbol not found
#define X51_ASM_ERR_LNTLNG      -13 // Line too long
#define X51_ASM_ERR_FILEIO      -14 // File input/output error
#define X51_ASM_ERR_INVSTR      -15 // Invalid string constant
#define X51_ASM_ERR_EXWHSP      -16 // Expected white space
#define X51_ASM_ERR_EXEOLN      -17 // Expected end of line
#define X51_ASM_ERR_NULSEG      -18 // Null segment


#endif //_X51ASMD_H