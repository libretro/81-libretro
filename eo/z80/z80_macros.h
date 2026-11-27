/* z80_macros.h: Some commonly used z80 things as macros
   Copyright (c) 1999-2001 Philip Kendall

   $Id: z80_macros.h,v 1.19 2003/02/10 15:04:12 pak21 Exp $

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

   Author contact information:

   E-mail: pak@ast.cam.ac.uk
   Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

*/

#ifndef FUSE_Z80_MACROS_H
#define FUSE_Z80_MACROS_H

/* Macros used for accessing the registers */
#define A   z80.af.b.h
#define F   z80.af.b.l
#define AF  z80.af.w

#define B   z80.bc.b.h
#define C   z80.bc.b.l
#define BC  z80.bc.w

#define D   z80.de.b.h
#define E   z80.de.b.l
#define DE  z80.de.w

#define H   z80.hl.b.h
#define L   z80.hl.b.l
#define HL  z80.hl.w

#define A_  z80.af_.b.h
#define F_  z80.af_.b.l
#define AF_ z80.af_.w

#define B_  z80.bc_.b.h
#define C_  z80.bc_.b.l
#define BC_ z80.bc_.w

#define D_  z80.de_.b.h
#define E_  z80.de_.b.l
#define DE_ z80.de_.w

#define H_  z80.hl_.b.h
#define L_  z80.hl_.b.l
#define HL_ z80.hl_.w

#define IXH z80.ix.b.h
#define IXL z80.ix.b.l
#define IX  z80.ix.w

#define IYH z80.iy.b.h
#define IYL z80.iy.b.l
#define IY  z80.iy.w

#define SPH z80.sp.b.h
#define SPL z80.sp.b.l
#define SP  z80.sp.w

#define PCH z80.pc.b.h
#define PCL z80.pc.b.l
#define PC  z80.pc.w

#define I  z80.i
#define R  z80.r
#define R7 z80.r7
#define IR ( ( z80.i ) << 8 | ( z80.r7 & 0x80 ) | ( z80.r & 0x7f ) )

#define IFF1 z80.iff1
#define IFF2 z80.iff2
#define IM   z80.im

/* The flags */

#define FLAG_C	0x01
#define FLAG_N	0x02
#define FLAG_P	0x04
#define FLAG_V	FLAG_P
#define FLAG_3	0x08
#define FLAG_H	0x10
#define FLAG_5	0x20
#define FLAG_Z	0x40
#define FLAG_S	0x80

/* Get the appropriate contended memory delay. Use this macro later
   to avoid a function call if memory contention is disabled */
#define contend(address,time) tstates += machine.contendmem(address,tstates,time);
#define contend_io(port,time) tstates += machine.contendio(port,tstates,time);

/* Some commonly used instructions */
#define AND(value)\
{\
  A &= (value);\
  F = (BYTE)(FLAG_H | sz53p_table[A]);\
}

#define ADC(value)\
{\
  WORD adctemp = (WORD)(A + (value) + ( F & FLAG_C ));\
  BYTE lookup = (BYTE)(( (A & 0x88) >> 3 ) | ( ( (value) & 0x88 ) >> 2 ) |\
    ( (adctemp & 0x88) >> 1 ));\
  A=(BYTE)(adctemp);\
  F = (BYTE)(( adctemp & 0x100 ? FLAG_C : 0 ) |\
    halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] |\
    sz53_table[A]);\
}

#define ADC16(value)\
{\
  DWORD add16temp = (DWORD)(HL + (value) + ( F & FLAG_C ));\
  BYTE lookup = (BYTE)(( ( HL & 0x8800 ) >> 11 ) |\
    ( ( (value) & 0x8800 ) >> 10 ) |\
    ( ( add16temp & 0x8800 ) >> 9 ));\
  HL = (WORD)add16temp;\
  F = (BYTE)(( add16temp & 0x10000 ? FLAG_C : 0 )|\
    overflow_add_table[lookup >> 4] |\
    ( H & ( FLAG_3 | FLAG_5 | FLAG_S ) ) |\
    halfcarry_add_table[lookup&0x07]|\
    ( HL ? 0 : FLAG_Z ));\
  AddToMCycle(4); \
  AddToMCycle(3);\
}

#define ADD(value)\
{\
  WORD addtemp = (WORD)(A + (value));\
  BYTE lookup = (BYTE)(( (A & 0x88) >> 3 ) | ( ( (value) & 0x88 ) >> 2 ) |\
    ( (addtemp & 0x88) >> 1 ));\
  A=(BYTE)addtemp;\
  F = (BYTE)(( addtemp & 0x100 ? FLAG_C : 0 ) |\
    halfcarry_add_table[lookup & 0x07] | overflow_add_table[lookup >> 4] |\
    sz53_table[A]);\
}

#define ADD16(value1,value2)\
{\
  DWORD add16temp = (value1) + (value2);\
  BYTE lookup = (BYTE)(( ( (value1) & 0x0800 ) >> 11 ) |\
    ( ( (value2) & 0x0800 ) >> 10 ) |\
    ( ( add16temp & 0x0800 ) >> 9 ));\
  tstates += 7;\
  AddToMCycle(4);\
  AddToMCycle(3);\
  (value1) = (WORD)add16temp;\
  F = (BYTE)(( F & ( FLAG_V | FLAG_Z | FLAG_S ) ) |\
    ( add16temp & 0x10000 ? FLAG_C : 0 )|\
    ( ( add16temp >> 8 ) & ( FLAG_3 | FLAG_5 ) ) |\
    halfcarry_add_table[lookup]);\
}

#define BIT(bit,value)\
{\
  F = (BYTE)(( F & FLAG_C ) | ( (value) & ( FLAG_3 | FLAG_5 ) ) |\
    ( ( (value) & ( 0x01 << bit ) ) ? FLAG_H : ( FLAG_P | FLAG_H | FLAG_Z ) ));\
}

#define BIT7(value)\
{\
  F = (BYTE)(( F & FLAG_C ) | ( (value) & ( FLAG_3 | FLAG_5 ) ) |\
    ( ( (value) & 0x80 ) ? ( FLAG_H | FLAG_S ) :\
      ( FLAG_P | FLAG_H | FLAG_Z ) ));\
}

#define CALL()\
{\
  BYTE calltempl, calltemph;\
  calltempl=readoperandbyte(PC++);\
  AddToMCycle(1);\
  contend( PC, 1 );\
  calltemph=readoperandbyte(PC++);\
  PUSH16(PCL,PCH);\
  PCL=calltempl; PCH=calltemph;\
}

#define CP(value)\
{\
  WORD cptemp = (WORD)(A - value);\
  BYTE lookup = (BYTE)(( (A & 0x88) >> 3 ) | ( ( (value) & 0x88 ) >> 2 ) |\
    ( (cptemp & 0x88) >> 1 ));\
  F = (BYTE)(( cptemp & 0x100 ? FLAG_C : ( cptemp ? 0 : FLAG_Z ) ) | FLAG_N |\
    halfcarry_sub_table[lookup & 0x07] |\
    overflow_sub_table[lookup >> 4] |\
    ( value & ( FLAG_3 | FLAG_5 ) ) |\
    ( cptemp & FLAG_S ));\
}

/* Macro for the {DD,FD} CB dd xx rotate/shift instructions */
#define DDFDCB_ROTATESHIFT(time, target, instruction)\
tstates+=(time);\
{\
  (target) = readbyte( tempaddr );\
  instruction( (target) );\
  writebyte( tempaddr, (target) );\
}\
break

#define DEC(value)\
{\
  F = (BYTE)(( F & FLAG_C ) | ( (value)&0x0f ? 0 : FLAG_H ) | FLAG_N);\
  (value)--;\
  F |= (BYTE)(( (value)==0x7f ? FLAG_V : 0 ) | sz53_table[value]);\
}

#define IN(reg,port)\
{\
  InsertMCycle(4);\
  inputOutputMCycle = mCycleIndex;\
  contend_io( port, 3 );\
  (reg)=readport((port),&tstates);\
  F = (BYTE)(( F & FLAG_C) | sz53p_table[(reg)]);\
}

#define INC(value)\
{\
  (value)++;\
  F = (BYTE)(( F & FLAG_C ) | ( (value)==0x80 ? FLAG_V : 0 ) |\
  ( (value)&0x0f ? 0 : FLAG_H ) | sz53_table[(value)]);\
}

#define LD16_NNRR(regl,regh)\
{\
  WORD ldtemp;\
  InsertMCycle(3);\
  contend( PC, 3 );\
  ldtemp=readoperandbyte(PC++);\
  InsertMCycle(3);\
  contend( PC, 3 );\
  ldtemp|=(WORD)(readoperandbyte(PC++) << 8);\
  InsertMCycle(3);\
  contend( ldtemp, 3 );\
  writebyte(ldtemp++,(regl));\
  InsertMCycle(3);\
  contend( ldtemp, 3 );\
  writebyte(ldtemp,(regh));\
}

#define LD16_RRNN(regl,regh)\
{\
  WORD ldtemp;\
  InsertMCycle(3);\
  contend( PC, 3 );\
  ldtemp=readoperandbyte(PC++);\
  InsertMCycle(3);\
  contend( PC, 3 );\
  ldtemp|=(WORD)(readoperandbyte(PC++) << 8);\
  InsertMCycle(3);\
  contend( ldtemp, 3 );\
  (regl)=readbyte(ldtemp++);\
  InsertMCycle(3);\
  contend( ldtemp, 3 );\
  (regh)=readbyte(ldtemp);\
}

#define JP()\
{\
  WORD jptemp=PC;\
  PCL=readoperandbyte(jptemp++);\
  PCH=readoperandbyte(jptemp);\
}

#define JR()\
{\
  AddToMCycle(5);\
  contend( PC, 1 ); contend( PC, 1 ); contend( PC, 1 ); contend( PC, 1 );\
  contend( PC, 1 );\
  PC+=(SBYTE)readoperandbyte(PC);\
}

#define OR(value)\
{\
  A |= (value);\
  F = sz53p_table[A];\
}

#define OUT(port,reg)\
{\
  InsertMCycle(4);\
  inputOutputMCycle = mCycleIndex;\
  contend_io( port, 3 );\
  writeport(port,reg, &tstates);\
}

#define POP16(regl,regh)\
{\
  InsertMCycle(3);\
  contend( SP, 3 );\
  (regl)=readbyte(SP++);\
  InsertMCycle(3);\
  contend( SP, 3 );\
  (regh)=readbyte(SP++);\
  StackChange -= 2;\
  if (StackChange < 0) StackChange = 0;\
}

#define PUSH16(regl,regh)\
{\
  InsertMCycle(3);\
  SP--; contend( SP, 3 );\
  writebyte(SP,(regh));\
  InsertMCycle(3);\
  SP--; contend( SP, 3 );\
  writebyte(SP,(regl));\
  StackChange += 2;\
}

#define RET()\
{\
  InsertMCycle(3);\
  contend( SP, 3 );\
  (PCL)=readbyte(SP++);\
  InsertMCycle(3);\
  contend( SP, 3 );\
  (PCH)=readbyte(SP++);\
  StackChange -= 2;\
  RetExecuted = 1;\
}

#define RL(value)\
{\
  BYTE rltemp = (value);\
  (value) = (BYTE)(( (value)<<1 ) | ( F & FLAG_C ));\
  F = (BYTE)(( rltemp >> 7 ) | sz53p_table[(value)]);\
}

#define RLC(value)\
{\
  (value) = (BYTE)(( (value)<<1 ) | ( (value)>>7 ));\
  F = (BYTE)(( (value) & FLAG_C ) | sz53p_table[(value)]);\
}

#define RR(value)\
{\
  BYTE rrtemp = (value);\
  (value) = (BYTE)(( (value)>>1 ) | ( F << 7 ));\
  F = (BYTE)(( rrtemp & FLAG_C ) | sz53p_table[(value)]);\
}

#define RRC(value)\
{\
  F = (BYTE)((value) & FLAG_C);\
  (value) = (BYTE)(( (value)>>1 ) | ( (value)<<7 ));\
  F |= (BYTE)(sz53p_table[(value)]);\
}

#define RST(value)\
{\
  PUSH16(PCL,PCH);\
  PC=(value);\
}

#define SBC(value)\
{\
  WORD sbctemp = (WORD)(A - (value) - ( F & FLAG_C ));\
  BYTE lookup = (BYTE)(( (A & 0x88) >> 3 ) | ( ( (value) & 0x88 ) >> 2 ) |\
    ( (sbctemp & 0x88) >> 1 ));\
  A=(BYTE)sbctemp;\
  F = (BYTE)(( sbctemp & 0x100 ? FLAG_C : 0 ) | FLAG_N |\
    halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] |\
    sz53_table[A]);\
}

#define SBC16(value)\
{\
  DWORD sub16temp = HL - (value) - (F & FLAG_C);\
  BYTE lookup = (BYTE)(( ( HL & 0x8800 ) >> 11 ) |\
    ( ( (value) & 0x8800 ) >> 10 ) |\
    ( ( sub16temp & 0x8800 ) >> 9 ));\
  HL = (WORD)sub16temp;\
  F = (BYTE)(( sub16temp & 0x10000 ? FLAG_C : 0 ) |\
    FLAG_N | overflow_sub_table[lookup >> 4] |\
    ( H & ( FLAG_3 | FLAG_5 | FLAG_S ) ) |\
    halfcarry_sub_table[lookup&0x07] |\
    ( HL ? 0 : FLAG_Z));\
  AddToMCycle(4);\
  AddToMCycle(3);\
}

#define SLA(value)\
{\
  F = (BYTE)((value) >> 7);\
  (value) <<= (BYTE)1;\
  F |= sz53p_table[(value)];\
}

#define SLL(value)\
{\
  F = (BYTE)((value) >> 7);\
  (value) = (BYTE)(( (value) << 1 ) | 0x01);\
  F |= sz53p_table[(value)];\
}

#define SRA(value)\
{\
  F = (BYTE)((value) & FLAG_C);\
  (value) = (BYTE)(( (value) & 0x80 ) | ( (value) >> 1 ));\
  F |= sz53p_table[(value)];\
}

#define SRL(value)\
{\
  F = (BYTE)((value) & FLAG_C);\
  (value) >>= 1;\
  F |= sz53p_table[(value)];\
}

#define SUB(value)\
{\
  WORD subtemp = (WORD)(A - (value));\
  BYTE lookup = (BYTE)(( (A & 0x88) >> 3 ) | ( ( (value) & 0x88 ) >> 2 ) |\
    ( (subtemp & 0x88) >> 1 ));\
  A=(BYTE)subtemp;\
  F = (BYTE)(( subtemp & 0x100 ? FLAG_C : 0 ) | FLAG_N |\
    halfcarry_sub_table[lookup & 0x07] | overflow_sub_table[lookup >> 4] |\
    sz53_table[A]);\
}

#define XOR(value)\
{\
  A ^= (value);\
  F = sz53p_table[A];\
}

#endif		/* #ifndef FUSE_Z80_MACROS_H */
