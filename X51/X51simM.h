////////////////////////////////////////////////////////////////////////////////
// X51SimM.h
// simulator macros header file


#ifndef _X51SIMM_H
#define _X51SIMM_H

//macros for smart access and more readability of the code
#define _CODE(a)    (ps->Code[a])
#define _IDATA(a)   (ps->IData[a])
#define _XDATA(a)   (ps->XData[a])
#define _SFR(a)     (ps->SFR[a - X51_MIN_SFR])
#define _SBIT0(a)   (((SX51Byte*)(&_SFR(a & 0xf8)))->b0)
#define _SBIT1(a)   (((SX51Byte*)(&_SFR(a & 0xf8)))->b1)
#define _SBIT2(a)   (((SX51Byte*)(&_SFR(a & 0xf8)))->b2)
#define _SBIT3(a)   (((SX51Byte*)(&_SFR(a & 0xf8)))->b3)
#define _SBIT4(a)   (((SX51Byte*)(&_SFR(a & 0xf8)))->b4)
#define _SBIT5(a)   (((SX51Byte*)(&_SFR(a & 0xf8)))->b5)
#define _SBIT6(a)   (((SX51Byte*)(&_SFR(a & 0xf8)))->b6)
#define _SBIT7(a)   (((SX51Byte*)(&_SFR(a & 0xf8)))->b7)
#define _SBIT(a, b) _SBIT##b(a)
#define _IBIT0(a)   (((SX51Byte*)(ps->IData + 0x20 + a))->b0)
#define _IBIT1(a)   (((SX51Byte*)(ps->IData + 0x20 + a))->b1)
#define _IBIT2(a)   (((SX51Byte*)(ps->IData + 0x20 + a))->b2)
#define _IBIT3(a)   (((SX51Byte*)(ps->IData + 0x20 + a))->b3)
#define _IBIT4(a)   (((SX51Byte*)(ps->IData + 0x20 + a))->b4)
#define _IBIT5(a)   (((SX51Byte*)(ps->IData + 0x20 + a))->b5)
#define _IBIT6(a)   (((SX51Byte*)(ps->IData + 0x20 + a))->b6)
#define _IBIT7(a)   (((SX51Byte*)(ps->IData + 0x20 + a))->b7)
#define _IBIT(a, b) _IBIT##b(a)


#define _PC    	(ps->PC)
#define _PCL   	(((unsigned char*)&(ps->PC))[0])
#define _PCH   	(((unsigned char*)&(ps->PC))[1])
#define _ACC       _SFR(X51_SFR_ACC)
#define _B         _SFR(X51_SFR_B)
#define _PSW       _SFR(X51_SFR_PSW)
#define _DPL       _SFR(X51_SFR_DPL)
#define _DPH       _SFR(X51_SFR_DPH)
#define _SP        _SFR(X51_SFR_SP)
#define _R0        _IDATA((_PSW & 0x18) + 0)
#define _R1        _IDATA((_PSW & 0x18) + 1)
#define _R2        _IDATA((_PSW & 0x18) + 2)
#define _R3        _IDATA((_PSW & 0x18) + 3)
#define _R4        _IDATA((_PSW & 0x18) + 4)
#define _R5        _IDATA((_PSW & 0x18) + 5)
#define _R6        _IDATA((_PSW & 0x18) + 6)
#define _R7        _IDATA((_PSW & 0x18) + 7)
#define _Rn(n)     _IDATA((_PSW & 0x18) + n)
#define _DPTR      (_SFR(X51_SFR_DPL) | (_SFR(X51_SFR_DPH) << 8))

#define _P         _SBIT(X51_SFR_PSW, 0) //parity flag
#define _OV        _SBIT(X51_SFR_PSW, 2) //overflow flag
#define _RS0       _SBIT(X51_SFR_PSW, 3) //register bank select bit 0
#define _RS1       _SBIT(X51_SFR_PSW, 4) //register bank select bit 1
#define _F0        _SBIT(X51_SFR_PSW, 5) //flag 0
#define _AC        _SBIT(X51_SFR_PSW, 6) //auxiliary carry flag (for BCD operations)
#define _CY        _SBIT(X51_SFR_PSW, 7) //carry flag

#define _ACC_0     _SBIT(X51_SFR_ACC, 0) //accumolator bit 0
#define _ACC_7     _SBIT(X51_SFR_ACC, 7) //accumolator bit 7

#define X51_SIM_DIRECT
#undef X51_SIM_DIRECT

#ifdef X51_SIM_DIRECT

#define rCODE(a)      _CODE(a)
#define rDATA(a)      ((a < X51_MIN_SFR)?_IDATA(a):_SFR(a))
#define rIDATA(a)     _IDATA(a)
#define rXDATA(a)     _XDATA(a)
#define rIBIT(a)      ((_IDATA(0x20 + ((a & 0x78) >> 3)) & (1 << (a & 7)))?1:0)
#define rSBIT(a)      ((_SFR(a & 0xf8) & (1 << (a & 7)))?1:0)
#define rBIT(a)       ((a < X51_MIN_SFR)?rIBIT(a):rSBIT(a))
#define rDPTR         (_DPL | (_DPH << 8))
#define wDPTR(v)      { _DPL = v; _DPH = (v >> 8); }

#define wDATA(a, v)   { if (a < X51_MIN_SFR) _IDATA(a) = v; else _SFR(a) = v; }
#define wIDATA(a, v)  { _IDATA(a) = v; }
#define wXDATA(a, v)  { _XDATA(a) = v; }
#define wIBIT(a, v)   { if (v) _IDATA(0x20 + ((a & 0x78) >> 3)) |= (1 << (a & 7)); else _IDATA(0x20 + ((a & 0x78) >> 3)) &= ~(1 << (a & 7)); }
#define wSBIT(a, v)   { if (v) _SFR(a & 0xf8) |= (1 << (a & 7)); else _SFR(a & 0xf8) &= ~(1 << (a & 7)); }
#define wBIT(a, v)    { if (a < X51_MIN_SFR) wIBIT(a, v) else wSBIT(a, v); }

#else // X51_SIM_DIRECT

#define rCODE(a)      X51Sim_Rd(ps, 'C', a)
#define rDATA(a)      X51Sim_Rd(ps, 'D', a)
#define rIDATA(a)     X51Sim_Rd(ps, 'I', a)
#define rXDATA(a)     X51Sim_Rd(ps, 'X', a)
#define rBIT(a)       X51Sim_Rd(ps, 'B', a)
#define rDPTR         (X51Sim_Rd(ps, 'D', X51_SFR_DPL) | (X51Sim_Rd(ps, 'D', X51_SFR_DPH) << 8))
#define wDPTR(v)      { X51Sim_Wr(ps, 'D', X51_SFR_DPL, v & 0xff); X51Sim_Wr(ps, 'D', X51_SFR_DPH, v >> 8); }

#define wDATA(a, v)   X51Sim_Wr(ps, 'D', a, v)
#define wIDATA(a, v)  X51Sim_Wr(ps, 'I', a, v)
#define wXDATA(a, v)  X51Sim_Wr(ps, 'X', a, v)
#define wBIT(a, v)    X51Sim_Wr(ps, 'B', a, v)

#endif // X51_SIM_DIRECT


#define _PUSH(v) wIDATA(++_SP, v)
#define _POP rIDATA(_SP--)


#endif //_X51SIMM_H

