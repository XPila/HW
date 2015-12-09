////////////////////////////////////////////////////////////////////////////////
// X51cmnD.h
// X51 common definition header file

#ifndef _X51CMND_H
#define _X51CMND_H

//core type
#define X51_CORE_X1   	0x00
#define X51_CORE_SC   	0x01

//min/max memory address
#define X51_MIN_CODE 		0x0000
#define X51_MAX_CODE 		0xffff
#define X51_MIN_IDATA		0x00
#define X51_MAX_IDATA		0xff
#define X51_MIN_XDATA		0x0000
#define X51_MAX_XDATA		0xffff
#define X51_MIN_SFR  		0x80
#define X51_MAX_SFR 	 	0xff

//max operand variant index
#define	X51_OPV_MAX	63

//operands
#define X51_OP_0      0x00 // 0 byte
#define X51_OP_C      0x01
#define X51_OP_A      0x02
#define X51_OP_AB     0x03
#define X51_OP_DPTR   0x04
#define X51_OP_R0     0x05
#define X51_OP_R1     0x06
#define X51_OP_R2     0x07
#define X51_OP_R3     0x08
#define X51_OP_R4     0x09
#define X51_OP_R5     0x0a
#define X51_OP_R6     0x0b
#define X51_OP_R7     0x0c
#define X51_OP_iA     0x0d
#define X51_OP_iDPTR  0x0e
#define X51_OP_iADPTR 0x0f
#define X51_OP_iAPC   0x10
#define X51_OP_iR0    0x11
#define X51_OP_iR1    0x12
#define X51_OP_1      0x13 // 1 byte
#define X51_OP_Bit    0x13
#define X51_OP_Dir    0x14
#define X51_OP_Rel    0x15
#define X51_OP_P0     0x16
#define X51_OP_P1     0x17
#define X51_OP_P2     0x18
#define X51_OP_P3     0x19
#define X51_OP_P4     0x1a
#define X51_OP_P5     0x1b
#define X51_OP_P6     0x1c
#define X51_OP_P7     0x1d
#define X51_OP_Data8  0x1e
#define X51_OP_2      0x1f // 2 byte 
#define X51_OP_Addr16 0x1f
#define X51_OP_Data16 0x20
#define X51_OP_MAX	0x20

//operand variants
#define X51_OPV_0         0
#define X51_OPV_C         (X51_OP_C << 8)
#define X51_OPV_A         (X51_OP_A << 8)
#define X51_OPV_A_R0      ((X51_OP_A | (X51_OP_R0 << 8)) << 8)
#define X51_OPV_A_R1      ((X51_OP_A | (X51_OP_R1 << 8)) << 8)
#define X51_OPV_A_R2      ((X51_OP_A | (X51_OP_R2 << 8)) << 8)
#define X51_OPV_A_R3      ((X51_OP_A | (X51_OP_R3 << 8)) << 8)
#define X51_OPV_A_R4      ((X51_OP_A | (X51_OP_R4 << 8)) << 8)
#define X51_OPV_A_R5      ((X51_OP_A | (X51_OP_R5 << 8)) << 8)
#define X51_OPV_A_R6      ((X51_OP_A | (X51_OP_R6 << 8)) << 8)
#define X51_OPV_A_R7      ((X51_OP_A | (X51_OP_R7 << 8)) << 8)
#define X51_OPV_A_Dir     ((X51_OP_A | (X51_OP_Dir << 8)) << 8)
#define X51_OPV_A_Data8   ((X51_OP_A | (X51_OP_Data8 << 8)) << 8)

//operand masks
#define X51_OPM_0      ((1 << (X51_OP_1 - 1)) - 1)
#define X51_OPM_C      (1 << (X51_OP_C - 1))
#define X51_OPM_A      (1 << (X51_OP_A - 1))
#define X51_OPM_AB     (1 << (X51_OP_AB - 1))
#define X51_OPM_DPTR   (1 << (X51_OP_DPTR - 1))
#define X51_OPM_R0     (1 << (X51_OP_R0 - 1))
#define X51_OPM_R1     (1 << (X51_OP_R1 - 1))
#define X51_OPM_R2     (1 << (X51_OP_R2 - 1))
#define X51_OPM_R3     (1 << (X51_OP_R3 - 1))
#define X51_OPM_R4     (1 << (X51_OP_R4 - 1))
#define X51_OPM_R5     (1 << (X51_OP_R5 - 1))
#define X51_OPM_R6     (1 << (X51_OP_R6 - 1))
#define X51_OPM_R7     (1 << (X51_OP_R7 - 1))
#define X51_OPM_iA     (1 << (X51_OP_iA - 1))
#define X51_OPM_iDPTR  (1 << (X51_OP_iDPTR - 1))
#define X51_OPM_iADPTR (1 << (X51_OP_iADPTR - 1))
#define X51_OPM_iAPC   (1 << (X51_OP_iAPC - 1))
#define X51_OPM_iR0    (1 << (X51_OP_iR0 - 1))
#define X51_OPM_iR1    (1 << (X51_OP_iR1 - 1))
#define X51_OPM_1      (((1 << (X51_OP_2 - 1)) - 1) & ~X51_OPM_0) 
#define X51_OPM_Bit    (1 << (X51_OP_Bit - 1))
#define X51_OPM_Dir    (1 << (X51_OP_Dir - 1))
#define X51_OPM_Rel    (1 << (X51_OP_Rel - 1))
#define X51_OPM_P0     (1 << (X51_OP_P0 - 1))
#define X51_OPM_P1     (1 << (X51_OP_P1 - 1))
#define X51_OPM_P2     (1 << (X51_OP_P2 - 1))
#define X51_OPM_P3     (1 << (X51_OP_P3 - 1))
#define X51_OPM_P4     (1 << (X51_OP_P4 - 1))
#define X51_OPM_P5     (1 << (X51_OP_P5 - 1))
#define X51_OPM_P6     (1 << (X51_OP_P6 - 1))
#define X51_OPM_P7     (1 << (X51_OP_P7 - 1))
#define X51_OPM_Data8  (1 << (X51_OP_Data8 - 1))
#define X51_OPM_2      (X51_OPM_Addr16 | X51_OPM_Data16)
#define X51_OPM_Addr16 (1 << (X51_OP_Addr16 - 1))
#define X51_OPM_Data16 (1 << (X51_OP_Data16 - 1))
#define X51_OPM_ALL	(X51_OPM_0 | X51_OPM_1 | X51_OPM_2)
#define X51_OPM_P_ALL  (((1 << (X51_OP_P7)) - 1) ^ (((1 << (X51_OP_P0 - 1)) - 1)))

//instructions
#define X51_INS_0     0x00
#define X51_INS_ACALL 0x01
#define X51_INS_ADD   0x02
#define X51_INS_ADDC  0x03
#define X51_INS_AJMP  0x04
#define X51_INS_ANL   0x05
#define X51_INS_CJNE  0x06
#define X51_INS_CLR   0x07
#define X51_INS_CPL   0x08
#define X51_INS_DA    0x09
#define X51_INS_DEC   0x0a
#define X51_INS_DIV   0x0b
#define X51_INS_DJNZ  0x0c
#define X51_INS_INC   0x0d
#define X51_INS_JB    0x0e
#define X51_INS_JBC   0x0f
#define X51_INS_JC    0x10
#define X51_INS_JMP   0x11
#define X51_INS_JNB   0x12
#define X51_INS_JNC   0x13
#define X51_INS_JNZ   0x14
#define X51_INS_JZ    0x15
#define X51_INS_LCALL 0x16
#define X51_INS_LJMP  0x17
#define X51_INS_MOV   0x18
#define X51_INS_MOVC  0x19
#define X51_INS_MOVX  0x1a
#define X51_INS_MUL   0x1b
#define X51_INS_NOP   0x1c
#define X51_INS_ORL   0x1d
#define X51_INS_POP   0x1e
#define X51_INS_PUSH  0x1f
#define X51_INS_RET   0x20
#define X51_INS_RETI  0x21
#define X51_INS_RL    0x22
#define X51_INS_RLC   0x23
#define X51_INS_RR    0x24
#define X51_INS_RRC   0x25
#define X51_INS_SETB  0x26
#define X51_INS_SJMP  0x27
#define X51_INS_SUBB  0x28
#define X51_INS_SWAP  0x29
#define X51_INS_XCH   0x2a
#define X51_INS_XCHD  0x2b
#define X51_INS_XRL   0x2c
#define X51_INS_MAX   0x2c

//special function registers
#define X51_SFR_P0     0x80
#define X51_SFR_SP     0x81
#define X51_SFR_DPL    0x82
#define X51_SFR_DPH    0x83
#define X51_SFR_PCON   0x87
#define X51_SFR_TCON   0x88
#define X51_SFR_TMOD   0x89
#define X51_SFR_TL0    0x8a
#define X51_SFR_TL1    0x8b
#define X51_SFR_TH0    0x8c
#define X51_SFR_TH1    0x8d
#define X51_SFR_AUXR   0x8e
#define X51_SFR_CKCON  0x8f
#define X51_SFR_P1     0x90
#define X51_SFR_SCON   0x98
#define X51_SFR_SBUF   0x99
#define X51_SFR_P2     0xa0
#define X51_SFR_IE     0xa8
#define X51_SFR_P3     0xb0
#define X51_SFR_IP     0xb8
#define X51_SFR_T2CON  0xc8
#define X51_SFR_T2MOD  0xc9
#define X51_SFR_RCAP2L 0xca
#define X51_SFR_RCAP2H 0xcb
#define X51_SFR_TL2    0xcc
#define X51_SFR_TH2    0xcd
#define X51_SFR_PSW    0xd0
#define X51_SFR_ACC    0xe1
#define X51_SFR_B      0xf0

#endif //_X51CMND_H

/*
directives
 CASE
 COND
 DATE
 DEBUG
 DEFINE
 ECRM
 EJECT
 ELSE
 ELSEIF
 ENDIF
 ERRORPRINT
 GEN
 GENONLY
 IF
 INCDIR
 INCLUDE
 LIST
 MACRO
 MOD_CONT
 MOD_MX51
 MOD51
 MPL
 NOAMAKE
 NOCASE
 NOCOND
 NODEBUG
 NOERRORPRINT
 NOGEN
 NOLINES
 NOLIST
 NOMACRO
 NOMOD51
 NOMPL
 NOOBJECT
 NOPRINT
 NOREGISTERBANK
 NOSYMBOLS
 NOSYMLIST
 NOXREF
 OBJECT
 PAGELENGTH
 PAGEWIDTH
 PRINT
 REGISTERBANK
 REGUSE
 RESET
 RESTORE
 SAVE
 SET
 SYMBOLS
 SYMLIST
 TITLE
 XREF

control statements
 __ERROR__
 __WARNING__
 ALIGN
 BIT
 BSEG
 CODE
 CSEG
 DATA
 DB
 DBIT
 DD
 DS
 DSB
 DSD
 DSEG
 DSW
 DW
 ELSE
 ELSEIF
 END
 ENDIF
 ENDP
 EQU
 EVEN
 EXTERN
 EXTRN
 IDATA
 IF
 ISEG
 LABEL
 LIT
 NAME
 ORG
 PROC
 PUBLIC
 RSEG
 SBIT
 SEGMENT
 SET
 SFR
 SFR16
 USING
 XDATA
 XSEG 
*/
