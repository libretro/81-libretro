/* EightyOne  - A Windows ZX80/81/clone emulator.
 * Copyright (C) 2003-2006 Michael D Wynne
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * DbgDissassem.cpp
 */

BYTE GetMem(int Addr)
{
        if (zx81.machine==MACHINESPEC48
                || zx81.machine==MACHINEACE
                || Addr<zx81.m1not)
                return(readbyte(Addr));
        else
                return(readbyte(Addr&32767));
}

// #define GetMem(Addr) (memory[(Addr)&32767])

AnsiString TDbg::StrRep(AnsiString Text, int Pos, int Len, AnsiString NewText)
{
        AnsiString t="";

        if (Pos>1) t=Text.SubString(1,Pos-1);
        t+=NewText;
        Pos+=Len;
        if (Len <= Text.Length()) t += Text.SubString(Pos, 1+Text.Length()-Len);
        return(t);
}



AnsiString TDbg::Disassemble(int *Ad)
{
        int Addr = *Ad;
        int Opcode;
        int p;
        int Offset;

        AnsiString StrAddr;
        AnsiString StrCode;
        AnsiString StrText;

        Opcode = GetMem(Addr);
        StrAddr = "$"+Hex16(Addr);
        StrCode += Hex8(Opcode);
        Addr++;

        if (Addr>=zx81.m1not && !(Opcode&64) && !(zx81.machine==MACHINEACE
                                                || zx81.machine==MACHINESPEC48))
                Opcode=-1;


        switch(Opcode)
        {
        case -1:   StrText="*NOP*"; break;
        case 0x00: StrText="NOP"; break;
        case 0x01: StrText="LD BC,nnnn"; break;
        case 0x02: StrText="LD (BC),A"; break;
        case 0x03: StrText="INC BC"; break;
        case 0x04: StrText="INC B"; break;
        case 0x05: StrText="DEC B"; break;
        case 0x06: StrText="LD B,nn"; break;
        case 0x07: StrText="RLCA"; break;
        case 0x08: StrText="EX AF,AF'"; break;
        case 0x09: StrText="ADD HL,BC"; break;
        case 0x0a: StrText="LD A,(BC)"; break;
        case 0x0b: StrText="DEC BC"; break;
        case 0x0c: StrText="INC C"; break;
        case 0x0d: StrText="DEC C"; break;
        case 0x0e: StrText="LD C,nn"; break;
        case 0x0f: StrText="RRCA"; break;
        case 0x10: StrText="DJNZ fz"; break;
        case 0x11: StrText="LD DE,nnnn"; break;
        case 0x12: StrText="LD (DE),A"; break;
        case 0x13: StrText="INC DE"; break;
        case 0x14: StrText="INC D"; break;
        case 0x15: StrText="DEC D"; break;
        case 0x16: StrText="LD D,nn"; break;
        case 0x17: StrText="RLA"; break;
        case 0x18: StrText="JR fz"; break;
        case 0x19: StrText="ADD HL,DE"; break;
        case 0x1a: StrText="LD A,(DE)"; break;
        case 0x1b: StrText="DEC DE"; break;
        case 0x1c: StrText="INC E"; break;
        case 0x1d: StrText="DEC E"; break;
        case 0x1e: StrText="LD E,nn"; break;
        case 0x1f: StrText="RRA"; break;
        case 0x20: StrText="JR NZ,fz"; break;
        case 0x21: StrText="LD HL,nnnn"; break;
        case 0x22: StrText="LD (nnnn),HL"; break;
        case 0x23: StrText="INC HL"; break;
        case 0x24: StrText="INC H"; break;
        case 0x25: StrText="DEC H"; break;
        case 0x26: StrText="LD H,nn"; break;
        case 0x27: StrText="DAA"; break;
        case 0x28: StrText="JR Z,fz"; break;
        case 0x29: StrText="ADD HL,HL"; break;
        case 0x2a: StrText="LD HL,(nnnn)"; break;
        case 0x2b: StrText="DEC HL"; break;
        case 0x2c: StrText="INC L"; break;
        case 0x2d: StrText="DEC L"; break;
        case 0x2e: StrText="LD L,nn"; break;
        case 0x2f: StrText="CPL"; break;
        case 0x30: StrText="JR NC,fz"; break;
        case 0x31: StrText="LD SP,nnnn"; break;
        case 0x32: StrText="LD (nnnn),A"; break;
        case 0x33: StrText="INC SP"; break;
        case 0x34: StrText="INC (HL)"; break;
        case 0x35: StrText="DEC (HL)"; break;
        case 0x36: StrText="LD (HL),nn"; break;
        case 0x37: StrText="SCF"; break;
        case 0x38: StrText="JR C,fz"; break;
        case 0x39: StrText="ADD HL,SP"; break;
        case 0x3a: StrText="LD A,(nnnn)"; break;
        case 0x3b: StrText="DEC SP"; break;
        case 0x3c: StrText="INC A"; break;
        case 0x3d: StrText="DEC A"; break;
        case 0x3e: StrText="LD A,nn"; break;
        case 0x3f: StrText="CCF"; break;
        case 0x40: StrText="LD B,B"; break;
        case 0x41: StrText="LD B,C"; break;
        case 0x42: StrText="LD B,D"; break;
        case 0x43: StrText="LD B,E"; break;
        case 0x44: StrText="LD B,H"; break;
        case 0x45: StrText="LD B,L"; break;
        case 0x46: StrText="LD B,(HL)"; break;
        case 0x47: StrText="LD B,A"; break;
        case 0x48: StrText="LD C,B"; break;
        case 0x49: StrText="LD C,C"; break;
        case 0x4a: StrText="LD C,D"; break;
        case 0x4b: StrText="LD C,E"; break;
        case 0x4c: StrText="LD C,H"; break;
        case 0x4d: StrText="LD C,L"; break;
        case 0x4e: StrText="LD C,(HL)"; break;
        case 0x4f: StrText="LD C,A"; break;
        case 0x50: StrText="LD D,B"; break;
        case 0x51: StrText="LD D,C"; break;
        case 0x52: StrText="LD D,D"; break;
        case 0x53: StrText="LD D,E"; break;
        case 0x54: StrText="LD D,H"; break;
        case 0x55: StrText="LD D,L"; break;
        case 0x56: StrText="LD D,(HL)"; break;
        case 0x57: StrText="LD D,A"; break;
        case 0x58: StrText="LD E,B"; break;
        case 0x59: StrText="LD E,C"; break;
        case 0x5a: StrText="LD E,D"; break;
        case 0x5b: StrText="LD E,E"; break;
        case 0x5c: StrText="LD E,H"; break;
        case 0x5d: StrText="LD E,L"; break;
        case 0x5e: StrText="LD E,(HL)"; break;
        case 0x5f: StrText="LD E,A"; break;
        case 0x60: StrText="LD H,B"; break;
        case 0x61: StrText="LD H,C"; break;
        case 0x62: StrText="LD H,D"; break;
        case 0x63: StrText="LD H,E"; break;
        case 0x64: StrText="LD H,H"; break;
        case 0x65: StrText="LD H,L"; break;
        case 0x66: StrText="LD H,(HL)"; break;
        case 0x67: StrText="LD H,A"; break;
        case 0x68: StrText="LD L,B"; break;
        case 0x69: StrText="LD L,C"; break;
        case 0x6a: StrText="LD L,D"; break;
        case 0x6b: StrText="LD L,E"; break;
        case 0x6c: StrText="LD L,H"; break;
        case 0x6d: StrText="LD L,L"; break;
        case 0x6e: StrText="LD L,(HL)"; break;
        case 0x6f: StrText="LD L,A"; break;
        case 0x70: StrText="LD (HL),B"; break;
        case 0x71: StrText="LD (HL),C"; break;
        case 0x72: StrText="LD (HL),D"; break;
        case 0x73: StrText="LD (HL),E"; break;
        case 0x74: StrText="LD (HL),H"; break;
        case 0x75: StrText="LD (HL),L"; break;
        case 0x76: StrText="HALT"; break;
        case 0x77: StrText="LD (HL),A"; break;
        case 0x78: StrText="LD A,B"; break;
        case 0x79: StrText="LD A,C"; break;
        case 0x7a: StrText="LD A,D"; break;
        case 0x7b: StrText="LD A,E"; break;
        case 0x7c: StrText="LD A,H"; break;
        case 0x7d: StrText="LD A,L"; break;
        case 0x7e: StrText="LD A,(HL)"; break;
        case 0x7f: StrText="LD A,A"; break;
        case 0x80: StrText="ADD A,B"; break;
        case 0x81: StrText="ADD A,C"; break;
        case 0x82: StrText="ADD A,D"; break;
        case 0x83: StrText="ADD A,E"; break;
        case 0x84: StrText="ADD A,H"; break;
        case 0x85: StrText="ADD A,L"; break;
        case 0x86: StrText="ADD A,(HL)"; break;
        case 0x87: StrText="ADD A,A"; break;
        case 0x88: StrText="ADC A,B"; break;
        case 0x89: StrText="ADC A,C"; break;
        case 0x8a: StrText="ADC A,D"; break;
        case 0x8b: StrText="ADC A,E"; break;
        case 0x8c: StrText="ADC A,H"; break;
        case 0x8d: StrText="ADC A,L"; break;
        case 0x8e: StrText="ADC A,(HL)"; break;
        case 0x8f: StrText="ADC A,A"; break;
        case 0x90: StrText="SUB A,B"; break;
        case 0x91: StrText="SUB A,C"; break;
        case 0x92: StrText="SUB A,D"; break;
        case 0x93: StrText="SUB A,E"; break;
        case 0x94: StrText="SUB A,H"; break;
        case 0x95: StrText="SUB A,L"; break;
        case 0x96: StrText="SUB A,(HL)"; break;
        case 0x97: StrText="SUB A,A"; break;
        case 0x98: StrText="SBC A,B"; break;
        case 0x99: StrText="SBC A,C"; break;
        case 0x9a: StrText="SBC A,D"; break;
        case 0x9b: StrText="SBC A,E"; break;
        case 0x9c: StrText="SBC A,H"; break;
        case 0x9d: StrText="SBC A,L"; break;
        case 0x9e: StrText="SBC A,(HL)"; break;
        case 0x9f: StrText="SBC A,A"; break;
        case 0xa0: StrText="AND A,B"; break;
        case 0xa1: StrText="AND A,C"; break;
        case 0xa2: StrText="AND A,D"; break;
        case 0xa3: StrText="AND A,E"; break;
        case 0xa4: StrText="AND A,H"; break;
        case 0xa5: StrText="AND A,L"; break;
        case 0xa6: StrText="AND A,(HL)"; break;
        case 0xa7: StrText="AND A,A"; break;
        case 0xa8: StrText="XOR A,B"; break;
        case 0xa9: StrText="XOR A,C"; break;
        case 0xaa: StrText="XOR A,D"; break;
        case 0xab: StrText="XOR A,E"; break;
        case 0xac: StrText="XOR A,H"; break;
        case 0xad: StrText="XOR A,L"; break;
        case 0xae: StrText="XOR A,(HL)"; break;
        case 0xaf: StrText="XOR A,A"; break;
        case 0xb0: StrText="OR A,B"; break;
        case 0xb1: StrText="OR A,C"; break;
        case 0xb2: StrText="OR A,D"; break;
        case 0xb3: StrText="OR A,E"; break;
        case 0xb4: StrText="OR A,H"; break;
        case 0xb5: StrText="OR A,L"; break;
        case 0xb6: StrText="OR A,(HL)"; break;
        case 0xb7: StrText="OR A,A"; break;
        case 0xb8: StrText="CP B"; break;
        case 0xb9: StrText="CP C"; break;
        case 0xba: StrText="CP D"; break;
        case 0xbb: StrText="CP E"; break;
        case 0xbc: StrText="CP H"; break;
        case 0xbd: StrText="CP L"; break;
        case 0xbe: StrText="CP (HL)"; break;
        case 0xbf: StrText="CP A"; break;
        case 0xc0: StrText="RET NZ"; break;
        case 0xc1: StrText="POP BC"; break;
        case 0xc2: StrText="JP NZ,nnnn"; break;
        case 0xc3: StrText="JP nnnn"; break;
        case 0xc4: StrText="CALL NZ,nnnn"; break;
        case 0xc5: StrText="PUSH BC"; break;
        case 0xc6: StrText="ADD A,nn"; break;
        case 0xc7: StrText="RST 00"; break;
        case 0xc8: StrText="RET Z"; break;
        case 0xc9: StrText="RET"; break;
        case 0xca: StrText="JP Z,nnnn"; break;
        case 0xcb:
                Opcode = GetMem(Addr);
                if (Addr>=zx81.m1not && !(Opcode&64) && !(zx81.machine==MACHINEACE
                                                || zx81.machine==MACHINESPEC48))
                        Opcode=-1;
                StrCode += Hex8(Opcode);
                Addr++;

                switch(Opcode)
                {
                case 0x00:	StrText="RLC B"; break;
                case 0x01:	StrText="RLC C"; break;
                case 0x02:	StrText="RLC D"; break;
                case 0x03:	StrText="RLC E"; break;
                case 0x04:	StrText="RLC H"; break;
                case 0x05:	StrText="RLC L"; break;
                case 0x06:	StrText="RLC (HL)"; break;
                case 0x07:	StrText="RLC A"; break;
                case 0x08:	StrText="RRC B"; break;
                case 0x09:	StrText="RRC C"; break;
                case 0x0a:	StrText="RRC D"; break;
                case 0x0b:	StrText="RRC E"; break;
                case 0x0c:	StrText="RRC H"; break;
                case 0x0d:	StrText="RRC L"; break;
                case 0x0e:	StrText="RRC (HL)"; break;
                case 0x0f:	StrText="RRC A"; break;
                case 0x10:	StrText="RL B"; break;
                case 0x11:	StrText="RL C"; break;
                case 0x12:	StrText="RL D"; break;
                case 0x13:	StrText="RL E"; break;
                case 0x14:	StrText="RL H"; break;
                case 0x15:	StrText="RL L"; break;
                case 0x16:	StrText="RL (HL)"; break;
                case 0x17:	StrText="RL A"; break;
                case 0x18:	StrText="RR B"; break;
                case 0x19:	StrText="RR C"; break;
                case 0x1a:	StrText="RR D"; break;
                case 0x1b:	StrText="RR E"; break;
                case 0x1c:	StrText="RR H"; break;
                case 0x1d:	StrText="RR L"; break;
                case 0x1e:	StrText="RR (HL)"; break;
                case 0x1f:	StrText="RR A"; break;
                case 0x20:	StrText="SLA B"; break;
                case 0x21:	StrText="SLA C"; break;
                case 0x22:	StrText="SLA D"; break;
                case 0x23:	StrText="SLA E"; break;
                case 0x24:	StrText="SLA H"; break;
                case 0x25:	StrText="SLA L"; break;
                case 0x26:	StrText="SLA (HL)"; break;
                case 0x27:	StrText="SLA A"; break;
                case 0x28:	StrText="SRA B"; break;
                case 0x29:	StrText="SRA C"; break;
                case 0x2a:	StrText="SRA D"; break;
                case 0x2b:	StrText="SRA E"; break;
                case 0x2c:	StrText="SRA H"; break;
                case 0x2d:	StrText="SRA L"; break;
                case 0x2e:	StrText="SRA (HL)"; break;
                case 0x2f:	StrText="SRA A"; break;
                case 0x30:	StrText="SLL B"; break;
                case 0x31:	StrText="SLL C"; break;
                case 0x32:	StrText="SLL D"; break;
                case 0x33:	StrText="SLL E"; break;
                case 0x34:	StrText="SLL H"; break;
                case 0x35:	StrText="SLL L"; break;
                case 0x36:	StrText="SLL (HL)"; break;
                case 0x37:	StrText="SLL A"; break;
                case 0x38:	StrText="SRL B"; break;
                case 0x39:	StrText="SRL C"; break;
                case 0x3a:	StrText="SRL D"; break;
                case 0x3b:	StrText="SRL E"; break;
                case 0x3c:	StrText="SRL H"; break;
                case 0x3d:	StrText="SRL L"; break;
                case 0x3e:	StrText="SRL (HL)"; break;
                case 0x3f:	StrText="SRL A"; break;
                case 0x40:	StrText="BIT 0,B"; break;
                case 0x41:	StrText="BIT 0,C"; break;
                case 0x42:	StrText="BIT 0,D"; break;
                case 0x43:	StrText="BIT 0,E"; break;
                case 0x44:	StrText="BIT 0,H"; break;
                case 0x45:	StrText="BIT 0,L"; break;
                case 0x46:	StrText="BIT 0,(HL)"; break;
                case 0x47:	StrText="BIT 0,A"; break;
                case 0x48:	StrText="BIT 1,B"; break;
                case 0x49:	StrText="BIT 1,C"; break;
                case 0x4a:	StrText="BIT 1,D"; break;
                case 0x4b:	StrText="BIT 1,E"; break;
                case 0x4c:	StrText="BIT 1,H"; break;
                case 0x4d:	StrText="BIT 1,L"; break;
                case 0x4e:	StrText="BIT 1,(HL)"; break;
                case 0x4f:	StrText="BIT 1,A"; break;
                case 0x50:	StrText="BIT 2,B"; break;
                case 0x51:	StrText="BIT 2,C"; break;
                case 0x52:	StrText="BIT 2,D"; break;
                case 0x53:	StrText="BIT 2,E"; break;
                case 0x54:	StrText="BIT 2,H"; break;
                case 0x55:	StrText="BIT 2,L"; break;
                case 0x56:	StrText="BIT 2,(HL)"; break;
                case 0x57:	StrText="BIT 2,A"; break;
                case 0x58:	StrText="BIT 3,B"; break;
                case 0x59:	StrText="BIT 3,C"; break;
                case 0x5a:	StrText="BIT 3,D"; break;
                case 0x5b:	StrText="BIT 3,E"; break;
                case 0x5c:	StrText="BIT 3,H"; break;
                case 0x5d:	StrText="BIT 3,L"; break;
                case 0x5e:	StrText="BIT 3,(HL)"; break;
                case 0x5f:	StrText="BIT 3,A"; break;
                case 0x60:	StrText="BIT 4,B"; break;
                case 0x61:	StrText="BIT 4,C"; break;
                case 0x62:	StrText="BIT 4,D"; break;
                case 0x63:	StrText="BIT 4,E"; break;
                case 0x64:	StrText="BIT 4,H"; break;
                case 0x65:	StrText="BIT 4,L"; break;
                case 0x66:	StrText="BIT 4,(HL)"; break;
                case 0x67:	StrText="BIT 4,A"; break;
                case 0x68:	StrText="BIT 5,B"; break;
                case 0x69:	StrText="BIT 5,C"; break;
                case 0x6a:	StrText="BIT 5,D"; break;
                case 0x6b:	StrText="BIT 5,E"; break;
                case 0x6c:	StrText="BIT 5,H"; break;
                case 0x6d:	StrText="BIT 5,L"; break;
                case 0x6e:	StrText="BIT 5,(HL)"; break;
                case 0x6f:	StrText="BIT 5,A"; break;
                case 0x70:	StrText="BIT 6,B"; break;
                case 0x71:	StrText="BIT 6,C"; break;
                case 0x72:	StrText="BIT 6,D"; break;
                case 0x73:	StrText="BIT 6,E"; break;
                case 0x74:	StrText="BIT 6,H"; break;
                case 0x75:	StrText="BIT 6,L"; break;
                case 0x76:	StrText="BIT 6,(HL)"; break;
                case 0x77:	StrText="BIT 6,A"; break;
                case 0x78:	StrText="BIT 7,B"; break;
                case 0x79:	StrText="BIT 7,C"; break;
                case 0x7a:	StrText="BIT 7,D"; break;
                case 0x7b:	StrText="BIT 7,E"; break;
                case 0x7c:	StrText="BIT 7,H"; break;
                case 0x7d:	StrText="BIT 7,L"; break;
                case 0x7e:	StrText="BIT 7,(HL)"; break;
                case 0x7f:	StrText="BIT 7,A"; break;
                case 0x80:	StrText="RES 0,B"; break;
                case 0x81:	StrText="RES 0,C"; break;
                case 0x82:	StrText="RES 0,D"; break;
                case 0x83:	StrText="RES 0,E"; break;
                case 0x84:	StrText="RES 0,H"; break;
                case 0x85:	StrText="RES 0,L"; break;
                case 0x86:	StrText="RES 0,(HL)"; break;
                case 0x87:	StrText="RES 0,A"; break;
                case 0x88:	StrText="RES 1,B"; break;
                case 0x89:	StrText="RES 1,C"; break;
                case 0x8a:	StrText="RES 1,D"; break;
                case 0x8b:	StrText="RES 1,E"; break;
                case 0x8c:	StrText="RES 1,H"; break;
                case 0x8d:	StrText="RES 1,L"; break;
                case 0x8e:	StrText="RES 1,(HL)"; break;
                case 0x8f:	StrText="RES 1,A"; break;
                case 0x90:	StrText="RES 2,B"; break;
                case 0x91:	StrText="RES 2,C"; break;
                case 0x92:	StrText="RES 2,D"; break;
                case 0x93:	StrText="RES 2,E"; break;
                case 0x94:	StrText="RES 2,H"; break;
                case 0x95:	StrText="RES 2,L"; break;
                case 0x96:	StrText="RES 2,(HL)"; break;
                case 0x97:	StrText="RES 2,A"; break;
                case 0x98:	StrText="RES 3,B"; break;
                case 0x99:	StrText="RES 3,C"; break;
                case 0x9a:	StrText="RES 3,D"; break;
                case 0x9b:	StrText="RES 3,E"; break;
                case 0x9c:	StrText="RES 3,H"; break;
                case 0x9d:	StrText="RES 3,L"; break;
                case 0x9e:	StrText="RES 3,(HL)"; break;
                case 0x9f:	StrText="RES 3,A"; break;
                case 0xa0:	StrText="RES 4,B"; break;
                case 0xa1:	StrText="RES 4,C"; break;
                case 0xa2:	StrText="RES 4,D"; break;
                case 0xa3:	StrText="RES 4,E"; break;
                case 0xa4:	StrText="RES 4,H"; break;
                case 0xa5:	StrText="RES 4,L"; break;
                case 0xa6:	StrText="RES 4,(HL)"; break;
                case 0xa7:	StrText="RES 4,A"; break;
                case 0xa8:	StrText="RES 5,B"; break;
                case 0xa9:	StrText="RES 5,C"; break;
                case 0xaa:	StrText="RES 5,D"; break;
                case 0xab:	StrText="RES 5,E"; break;
                case 0xac:	StrText="RES 5,H"; break;
                case 0xad:	StrText="RES 5,L"; break;
                case 0xae:	StrText="RES 5,(HL)"; break;
                case 0xaf:	StrText="RES 5,A"; break;
                case 0xb0:	StrText="RES 6,B"; break;
                case 0xb1:	StrText="RES 6,C"; break;
                case 0xb2:	StrText="RES 6,D"; break;
                case 0xb3:	StrText="RES 6,E"; break;
                case 0xb4:	StrText="RES 6,H"; break;
                case 0xb5:	StrText="RES 6,L"; break;
                case 0xb6:	StrText="RES 6,(HL)"; break;
                case 0xb7:	StrText="RES 6,A"; break;
                case 0xb8:	StrText="RES 7,B"; break;
                case 0xb9:	StrText="RES 7,C"; break;
                case 0xba:	StrText="RES 7,D"; break;
                case 0xbb:	StrText="RES 7,E"; break;
                case 0xbc:	StrText="RES 7,H"; break;
                case 0xbd:	StrText="RES 7,L"; break;
                case 0xbe:	StrText="RES 7,(HL)"; break;
                case 0xbf:	StrText="RES 7,A"; break;
                case 0xc0:	StrText="SET 0,B"; break;
                case 0xc1:	StrText="SET 0,C"; break;
                case 0xc2:	StrText="SET 0,D"; break;
                case 0xc3:	StrText="SET 0,E"; break;
                case 0xc4:	StrText="SET 0,H"; break;
                case 0xc5:	StrText="SET 0,L"; break;
                case 0xc6:	StrText="SET 0,(HL)"; break;
                case 0xc7:	StrText="SET 0,A"; break;
                case 0xc8:	StrText="SET 1,B"; break;
                case 0xc9:	StrText="SET 1,C"; break;
                case 0xca:	StrText="SET 1,D"; break;
                case 0xcb:	StrText="SET 1,E"; break;
                case 0xcc:	StrText="SET 1,H"; break;
                case 0xcd:	StrText="SET 1,L"; break;
                case 0xce:	StrText="SET 1,(HL)"; break;
                case 0xcf:	StrText="SET 1,A"; break;
                case 0xd0:	StrText="SET 2,B"; break;
                case 0xd1:	StrText="SET 2,C"; break;
                case 0xd2:	StrText="SET 2,D"; break;
                case 0xd3:	StrText="SET 2,E"; break;
                case 0xd4:	StrText="SET 2,H"; break;
                case 0xd5:	StrText="SET 2,L"; break;
                case 0xd6:	StrText="SET 2,(HL)"; break;
                case 0xd7:	StrText="SET 2,A"; break;
                case 0xd8:	StrText="SET 3,B"; break;
                case 0xd9:	StrText="SET 3,C"; break;
                case 0xda:	StrText="SET 3,D"; break;
                case 0xdb:	StrText="SET 3,E"; break;
                case 0xdc:	StrText="SET 3,H"; break;
                case 0xdd:	StrText="SET 3,L"; break;
                case 0xde:	StrText="SET 3,(HL)"; break;
                case 0xdf:	StrText="SET 3,A"; break;
                case 0xe0:	StrText="SET 4,B"; break;
                case 0xe1:	StrText="SET 4,C"; break;
                case 0xe2:	StrText="SET 4,D"; break;
                case 0xe3:	StrText="SET 4,E"; break;
                case 0xe4:	StrText="SET 4,H"; break;
                case 0xe5:	StrText="SET 4,L"; break;
                case 0xe6:	StrText="SET 4,(HL)"; break;
                case 0xe7:	StrText="SET 4,A"; break;
                case 0xe8:	StrText="SET 5,B"; break;
                case 0xe9:	StrText="SET 5,C"; break;
                case 0xea:	StrText="SET 5,D"; break;
                case 0xeb:	StrText="SET 5,E"; break;
                case 0xec:	StrText="SET 5,H"; break;
                case 0xed:	StrText="SET 5,L"; break;
                case 0xee:	StrText="SET 5,(HL)"; break;
                case 0xef:	StrText="SET 5,A"; break;
                case 0xf0:	StrText="SET 6,B"; break;
                case 0xf1:	StrText="SET 6,C"; break;
                case 0xf2:	StrText="SET 6,D"; break;
                case 0xf3:	StrText="SET 6,E"; break;
                case 0xf4:	StrText="SET 6,H"; break;
                case 0xf5:	StrText="SET 6,L"; break;
                case 0xf6:	StrText="SET 6,(HL)"; break;
                case 0xf7:	StrText="SET 6,A"; break;
                case 0xf8:	StrText="SET 7,B"; break;
                case 0xf9:	StrText="SET 7,C"; break;
                case 0xfa:	StrText="SET 7,D"; break;
                case 0xfb:	StrText="SET 7,E"; break;
                case 0xfc:	StrText="SET 7,H"; break;
                case 0xfd:	StrText="SET 7,L"; break;
                case 0xfe:	StrText="SET 7,(HL)"; break;
                case 0xff:	StrText="SET 7,A"; break;
                }
                break;
        case 0xcc: StrText="CALL Z,nnnn"; break;
        case 0xcd: StrText="CALL nnnn"; break;
        case 0xce: StrText="ADC A,nn"; break;
        case 0xcf: StrText="RST 8"; break;
        case 0xd0: StrText="RET NC"; break;
        case 0xd1: StrText="POP DE"; break;
        case 0xd2: StrText="JP NC,nnnn"; break;
        case 0xd3: StrText="OUT (nn),A"; break;
        case 0xd4: StrText="CALL NC,nnnn"; break;
        case 0xd5: StrText="PUSH DE"; break;
        case 0xd6: StrText="SUB nn"; break;
        case 0xd7: StrText="RST 10"; break;
        case 0xd8: StrText="RET C"; break;
        case 0xd9: StrText="EXX"; break;
        case 0xda: StrText="JP C,nnnn"; break;
        case 0xdb: StrText="IN A,(nn)"; break;
        case 0xdc: StrText="CALL C,nnnn"; break;
        case 0xdd:
                Opcode = GetMem(Addr);
                if (Addr>=zx81.m1not && !(Opcode&64) && !(zx81.machine==MACHINEACE
                                                || zx81.machine==MACHINESPEC48))
                        Opcode=-1;
                StrCode += Hex8(Opcode);
                Addr++;

                switch(Opcode)
                {
                case 0x09: StrText="ADD IX,BC"; break;
                case 0x19: StrText="ADD IX,DE"; break;
                case 0x21: StrText="LD IX,nnnn"; break;
                case 0x22: StrText="LD (nnnn),IX"; break;
                case 0x23: StrText="INC IX"; break;
                case 0x24: StrText="INC IXH"; break;
                case 0x25: StrText="DEC IXH"; break;
                case 0x26: StrText="LD IXH,nn"; break;
                case 0x29: StrText="ADD IX,IX"; break;
                case 0x2a: StrText="LD IX,(nnnn)"; break;
                case 0x2b: StrText="DEC IX"; break;
                case 0x2c: StrText="INC IXL"; break;
                case 0x2d: StrText="DEC IXL"; break;
                case 0x2e: StrText="LD IXL,nn"; break;
                case 0x34: StrText="INC (IX+dd)"; break;
                case 0x35: StrText="DEC (IX+dd)"; break;
                case 0x36: StrText="LD (IX+dd),nn"; break;
                case 0x39: StrText="ADD IX,SP"; break;
                case 0x44: StrText="LD B,IXH"; break;
                case 0x45: StrText="LD B,IXL"; break;
                case 0x46: StrText="LD B,(IX+dd)"; break;
                case 0x4c: StrText="LD C,IXH"; break;
                case 0x4d: StrText="LD C,IXL"; break;
                case 0x4e: StrText="LD C,(IX+dd)"; break;
                case 0x54: StrText="LD D,IXH"; break;
                case 0x55: StrText="LD D,IXL"; break;
                case 0x56: StrText="LD D,(IX+dd)"; break;
                case 0x5c: StrText="LD E,IXH"; break;
                case 0x5d: StrText="LD E,IXL"; break;
                case 0x5e: StrText="LD E,(IX+dd)"; break;
                case 0x60: StrText="LD IXH,B"; break;
                case 0x61: StrText="LD IXH,C"; break;
                case 0x62: StrText="LD IXH,D"; break;
                case 0x63: StrText="LD IXH,E"; break;
                case 0x64: StrText="LD IXH,IXH"; break;
                case 0x65: StrText="LD IXH,IXL"; break;
                case 0x66: StrText="LD H,(IX+dd)"; break;
                case 0x67: StrText="LD IXH,A"; break;
                case 0x68: StrText="LD IXL,B"; break;
                case 0x69: StrText="LD IXL,C"; break;
                case 0x6a: StrText="LD IXL,D"; break;
                case 0x6b: StrText="LD IXL,E"; break;
                case 0x6c: StrText="LD IXL,IXH"; break;
                case 0x6d: StrText="LD IXL,IXL"; break;
                case 0x6e: StrText="LD L,(IX+dd)"; break;
                case 0x6f: StrText="LD IXL,A"; break;
                case 0x70: StrText="LD (IX+dd),B"; break;
                case 0x71: StrText="LD (IX+dd),C"; break;
                case 0x72: StrText="LD (IX+dd),D"; break;
                case 0x73: StrText="LD (IX+dd),E"; break;
                case 0x74: StrText="LD (IX+dd),H"; break;
                case 0x75: StrText="LD (IX+dd),L"; break;
                case 0x77: StrText="LD (IX+dd),A"; break;
                case 0x7c: StrText="LD A,IXH"; break;
                case 0x7d: StrText="LD A,IXL"; break;
                case 0x7e: StrText="LD A,(IX+dd)"; break;
                case 0x84: StrText="ADD A,IXH"; break;
                case 0x85: StrText="ADD A,IXL"; break;
                case 0x86: StrText="ADD A,(IX+dd)"; break;
                case 0x8c: StrText="ADC A,IXH"; break;
                case 0x8d: StrText="ADC A,IXL"; break;
                case 0x8e: StrText="ADC A,(IX+dd)"; break;
                case 0x94: StrText="SUB A,IXH"; break;
                case 0x95: StrText="SUB A,IXL"; break;
                case 0x96: StrText="SUB A,(IX+dd)"; break;
                case 0x9c: StrText="SBC A,IXH"; break;
                case 0x9d: StrText="SBC A,IXL"; break;
                case 0x9e: StrText="SBC A,(IX+dd)"; break;
                case 0xa4: StrText="AND A,IXH"; break;
                case 0xa5: StrText="AND A,IXL"; break;
                case 0xa6: StrText="AND A,(IX+dd)"; break;
                case 0xac: StrText="XOR A,IXH"; break;
                case 0xad: StrText="XOR A,IXL"; break;
                case 0xae: StrText="XOR A,(IX+dd)"; break;
                case 0xb4: StrText="OR A,IXH"; break;
                case 0xb5: StrText="OR A,IXL"; break;
                case 0xb6: StrText="OR A,(IX+dd)"; break;
                case 0xbc: StrText="CP A,IXH"; break;
                case 0xbd: StrText="CP A,IXL"; break;
                case 0xbe: StrText="CP A,(IX+dd)"; break;
                case 0xcb:
                        Offset = GetMem(Addr++);
                        StrCode += Hex8(Offset);
                        Opcode = GetMem(Addr);
                        if (Addr>=zx81.m1not && !(Opcode&64) && !(zx81.machine==MACHINEACE
                                                || zx81.machine==MACHINESPEC48))
                                Opcode=-1;
                        StrCode += Hex8(Opcode);
                        Addr++;

                        switch(Opcode)
                        {
                        case 0x00: StrText="LD B,RLC (IX+DD)"; break;
                        case 0x01: StrText="LD C,RLC (IX+DD)"; break;
                        case 0x02: StrText="LD D,RLC (IX+DD)"; break;
                        case 0x03: StrText="LD E,RLC (IX+DD)"; break;
                        case 0x04: StrText="LD H,RLC (IX+DD)"; break;
                        case 0x05: StrText="LD L,RLC (IX+DD)"; break;
                        case 0x06: StrText="RLC (IX+DD)"; break;
                        case 0x07: StrText="LD A,RLC (IX+DD)"; break;
                        case 0x08: StrText="LD B,RRC (IX+DD)"; break;
                        case 0x09: StrText="LD C,RRC (IX+DD)"; break;
                        case 0x0a: StrText="LD D,RRC (IX+DD)"; break;
                        case 0x0b: StrText="LD E,RRC (IX+DD)"; break;
                        case 0x0c: StrText="LD H,RRC (IX+DD)"; break;
                        case 0x0d: StrText="LD L,RRC (IX+DD)"; break;
                        case 0x0e: StrText="RRC (IX+DD)"; break;
                        case 0x0f: StrText="LD A,RRC (IX+DD)"; break;
                        case 0x10: StrText="LD B,RL (IX+DD)"; break;
                        case 0x11: StrText="LD C,RL (IX+DD)"; break;
                        case 0x12: StrText="LD D,RL (IX+DD)"; break;
                        case 0x13: StrText="LD E,RL (IX+DD)"; break;
                        case 0x14: StrText="LD H,RL (IX+DD)"; break;
                        case 0x15: StrText="LD L,RL (IX+DD)"; break;
                        case 0x16: StrText="RL (IX+DD)"; break;
                        case 0x17: StrText="LD A,RL (IX+DD)"; break;
                        case 0x18: StrText="LD B,RR (IX+DD)"; break;
                        case 0x19: StrText="LD C,RR (IX+DD)"; break;
                        case 0x1a: StrText="LD D,RR (IX+DD)"; break;
                        case 0x1b: StrText="LD E,RR (IX+DD)"; break;
                        case 0x1c: StrText="LD H,RR (IX+DD)"; break;
                        case 0x1d: StrText="LD L,RR (IX+DD)"; break;
                        case 0x1e: StrText="RR (IX+DD)"; break;
                        case 0x1f: StrText="LD A,RR (IX+DD)"; break;
                        case 0x20: StrText="LD B,SLA (IX+DD)"; break;
                        case 0x21: StrText="LD C,SLA (IX+DD)"; break;
                        case 0x22: StrText="LD D,SLA (IX+DD)"; break;
                        case 0x23: StrText="LD E,SLA (IX+DD)"; break;
                        case 0x24: StrText="LD H,SLA (IX+DD)"; break;
                        case 0x25: StrText="LD L,SLA (IX+DD)"; break;
                        case 0x26: StrText="SLA (IX+DD)"; break;
                        case 0x27: StrText="LD A,SLA (IX+DD)"; break;
                        case 0x28: StrText="LD B,SRA (IX+DD)"; break;
                        case 0x29: StrText="LD C,SRA (IX+DD)"; break;
                        case 0x2a: StrText="LD D,SRA (IX+DD)"; break;
                        case 0x2b: StrText="LD E,SRA (IX+DD)"; break;
                        case 0x2c: StrText="LD H,SRA (IX+DD)"; break;
                        case 0x2d: StrText="LD L,SRA (IX+DD)"; break;
                        case 0x2e: StrText="SRA (IX+DD)"; break;
                        case 0x2f: StrText="LD A,SRA (IX+DD)"; break;
                        case 0x30: StrText="LD B,SLL (IX+DD)"; break;
                        case 0x31: StrText="LD C,SLL (IX+DD)"; break;
                        case 0x32: StrText="LD D,SLL (IX+DD)"; break;
                        case 0x33: StrText="LD E,SLL (IX+DD)"; break;
                        case 0x34: StrText="LD H,SLL (IX+DD)"; break;
                        case 0x35: StrText="LD L,SLL (IX+DD)"; break;
                        case 0x36: StrText="SLL (IX+DD)"; break;
                        case 0x37: StrText="LD A,SLL (IX+DD)"; break;
                        case 0x38: StrText="LD B,SRL (IX+DD)"; break;
                        case 0x39: StrText="LD C,SRL (IX+DD)"; break;
                        case 0x3a: StrText="LD D,SRL (IX+DD)"; break;
                        case 0x3b: StrText="LD E,SRL (IX+DD)"; break;
                        case 0x3c: StrText="LD H,SRL (IX+DD)"; break;
                        case 0x3d: StrText="LD L,SRL (IX+DD)"; break;
                        case 0x3e: StrText="SRL (IX+DD)"; break;
                        case 0x3f: StrText="LD A,SRL (IX+DD)"; break;
                        case 0x40:
                        case 0x41:
                        case 0x42:
                        case 0x43:
                        case 0x44:
                        case 0x45:
                        case 0x46:
                        case 0x47: StrText="BIT 0,(IX+DD)"; break;
                        case 0x48:
                        case 0x49:
                        case 0x4a:
                        case 0x4b:
                        case 0x4c:
                        case 0x4d:
                        case 0x4e:
                        case 0x4f: StrText="BIT 1,(IX+DD)"; break;
                        case 0x50:
                        case 0x51:
                        case 0x52:
                        case 0x53:
                        case 0x54:
                        case 0x55:
                        case 0x56:
                        case 0x57: StrText="BIT 2,(IX+DD)"; break;
                        case 0x58:
                        case 0x59:
                        case 0x5a:
                        case 0x5b:
                        case 0x5c:
                        case 0x5d:
                        case 0x5e:
                        case 0x5f: StrText="BIT 3,(IX+DD)"; break;
                        case 0x60:
                        case 0x61:
                        case 0x62:
                        case 0x63:
                        case 0x64:
                        case 0x65:
                        case 0x66:
                        case 0x67: StrText="BIT 4,(IX+DD)"; break;
                        case 0x68:
                        case 0x69:
                        case 0x6a:
                        case 0x6b:
                        case 0x6c:
                        case 0x6d:
                        case 0x6e:
                        case 0x6f: StrText="BIT 5,(IX+DD)"; break;
                        case 0x70:
                        case 0x71:
                        case 0x72:
                        case 0x73:
                        case 0x74:
                        case 0x75:
                        case 0x76:
                        case 0x77: StrText="BIT 6,(IX+DD)"; break;
                        case 0x78:
                        case 0x79:
                        case 0x7a:
                        case 0x7b:
                        case 0x7c:
                        case 0x7d:
                        case 0x7e:
                        case 0x7f: StrText="BIT 7,(IX+DD)"; break;
                        case 0x80: StrText="LD B,RES 0,(IX+DD)"; break;
                        case 0x81: StrText="LD C,RES 0,(IX+DD)"; break;
                        case 0x82: StrText="LD D,RES 0,(IX+DD)"; break;
                        case 0x83: StrText="LD E,RES 0,(IX+DD)"; break;
                        case 0x84: StrText="LD H,RES 0,(IX+DD)"; break;
                        case 0x85: StrText="LD L,RES 0,(IX+DD)"; break;
                        case 0x86: StrText="RES 0,(IX+DD)"; break;
                        case 0x87: StrText="LD A,RES 0,(IX+DD)"; break;
                        case 0x88: StrText="LD B,RES 1,(IX+DD)"; break;
                        case 0x89: StrText="LD C,RES 1,(IX+DD)"; break;
                        case 0x8a: StrText="LD D,RES 1,(IX+DD)"; break;
                        case 0x8b: StrText="LD E,RES 1,(IX+DD)"; break;
                        case 0x8c: StrText="LD H,RES 1,(IX+DD)"; break;
                        case 0x8d: StrText="LD L,RES 1,(IX+DD)"; break;
                        case 0x8e: StrText="RES 1,(IX+DD)"; break;
                        case 0x8f: StrText="LD A,RES 1,(IX+DD)"; break;
                        case 0x90: StrText="LD B,RES 2,(IX+DD)"; break;
                        case 0x91: StrText="LD C,RES 2,(IX+DD)"; break;
                        case 0x92: StrText="LD D,RES 2,(IX+DD)"; break;
                        case 0x93: StrText="LD E,RES 2,(IX+DD)"; break;
                        case 0x94: StrText="LD H,RES 2,(IX+DD)"; break;
                        case 0x95: StrText="LD L,RES 2,(IX+DD)"; break;
                        case 0x96: StrText="RES 2,(IX+DD)"; break;
                        case 0x97: StrText="LD A,RES 2,(IX+DD)"; break;
                        case 0x98: StrText="LD B,RES 3,(IX+DD)"; break;
                        case 0x99: StrText="LD C,RES 3,(IX+DD)"; break;
                        case 0x9a: StrText="LD D,RES 3,(IX+DD)"; break;
                        case 0x9b: StrText="LD E,RES 3,(IX+DD)"; break;
                        case 0x9c: StrText="LD H,RES 3,(IX+DD)"; break;
                        case 0x9d: StrText="LD L,RES 3,(IX+DD)"; break;
                        case 0x9e: StrText="RES 3,(IX+DD)"; break;
                        case 0x9f: StrText="LD A,RES 3,(IX+DD)"; break;
                        case 0xa0: StrText="LD B,RES 4,(IX+DD)"; break;
                        case 0xa1: StrText="LD C,RES 4,(IX+DD)"; break;
                        case 0xa2: StrText="LD D,RES 4,(IX+DD)"; break;
                        case 0xa3: StrText="LD E,RES 4,(IX+DD)"; break;
                        case 0xa4: StrText="LD H,RES 4,(IX+DD)"; break;
                        case 0xa5: StrText="LD L,RES 4,(IX+DD)"; break;
                        case 0xa6: StrText="RES 4,(IX+DD)"; break;
                        case 0xa7: StrText="LD A,RES 4,(IX+DD)"; break;
                        case 0xa8: StrText="LD B,RES 5,(IX+DD)"; break;
                        case 0xa9: StrText="LD C,RES 5,(IX+DD)"; break;
                        case 0xaa: StrText="LD D,RES 5,(IX+DD)"; break;
                        case 0xab: StrText="LD E,RES 5,(IX+DD)"; break;
                        case 0xac: StrText="LD H,RES 5,(IX+DD)"; break;
                        case 0xad: StrText="LD L,RES 5,(IX+DD)"; break;
                        case 0xae: StrText="RES 5,(IX+DD)"; break;
                        case 0xaf: StrText="LD A,RES 5,(IX+DD)"; break;
                        case 0xb0: StrText="LD B,RES 6,(IX+DD)"; break;
                        case 0xb1: StrText="LD C,RES 6,(IX+DD)"; break;
                        case 0xb2: StrText="LD D,RES 6,(IX+DD)"; break;
                        case 0xb3: StrText="LD E,RES 6,(IX+DD)"; break;
                        case 0xb4: StrText="LD H,RES 6,(IX+DD)"; break;
                        case 0xb5: StrText="LD L,RES 6,(IX+DD)"; break;
                        case 0xb6: StrText="RES 6,(IX+DD)"; break;
                        case 0xb7: StrText="LD A,RES 6,(IX+DD)"; break;
                        case 0xb8: StrText="LD B,RES 7,(IX+DD)"; break;
                        case 0xb9: StrText="LD C,RES 7,(IX+DD)"; break;
                        case 0xba: StrText="LD D,RES 7,(IX+DD)"; break;
                        case 0xbb: StrText="LD E,RES 7,(IX+DD)"; break;
                        case 0xbc: StrText="LD H,RES 7,(IX+DD)"; break;
                        case 0xbd: StrText="LD L,RES 7,(IX+DD)"; break;
                        case 0xbe: StrText="RES 7,(IX+DD)"; break;
                        case 0xbf: StrText="LD A,RES 7,(IX+DD)"; break;
                        case 0xc0: StrText="LD B,SET 0,(IX+DD)"; break;
                        case 0xc1: StrText="LD C,SET 0,(IX+DD)"; break;
                        case 0xc2: StrText="LD D,SET 0,(IX+DD)"; break;
                        case 0xc3: StrText="LD E,SET 0,(IX+DD)"; break;
                        case 0xc4: StrText="LD H,SET 0,(IX+DD)"; break;
                        case 0xc5: StrText="LD L,SET 0,(IX+DD)"; break;
                        case 0xc6: StrText="SET 0,(IX+DD)"; break;
                        case 0xc7: StrText="LD A,SET 0,(IX+DD)"; break;
                        case 0xc8: StrText="LD B,SET 1,(IX+DD)"; break;
                        case 0xc9: StrText="LD C,SET 1,(IX+DD)"; break;
                        case 0xca: StrText="LD D,SET 1,(IX+DD)"; break;
                        case 0xcb: StrText="LD E,SET 1,(IX+DD)"; break;
                        case 0xcc: StrText="LD H,SET 1,(IX+DD)"; break;
                        case 0xcd: StrText="LD L,SET 1,(IX+DD)"; break;
                        case 0xce: StrText="SET 1,(IX+DD)"; break;
                        case 0xcf: StrText="LD A,SET 1,(IX+DD)"; break;
                        case 0xd0: StrText="LD B,SET 2,(IX+DD)"; break;
                        case 0xd1: StrText="LD C,SET 2,(IX+DD)"; break;
                        case 0xd2: StrText="LD D,SET 2,(IX+DD)"; break;
                        case 0xd3: StrText="LD E,SET 2,(IX+DD)"; break;
                        case 0xd4: StrText="LD H,SET 2,(IX+DD)"; break;
                        case 0xd5: StrText="LD L,SET 2,(IX+DD)"; break;
                        case 0xd6: StrText="SET 2,(IX+DD)"; break;
                        case 0xd7: StrText="LD A,SET 2,(IX+DD)"; break;
                        case 0xd8: StrText="LD B,SET 3,(IX+DD)"; break;
                        case 0xd9: StrText="LD C,SET 3,(IX+DD)"; break;
                        case 0xda: StrText="LD D,SET 3,(IX+DD)"; break;
                        case 0xdb: StrText="LD E,SET 3,(IX+DD)"; break;
                        case 0xdc: StrText="LD H,SET 3,(IX+DD)"; break;
                        case 0xdd: StrText="LD L,SET 3,(IX+DD)"; break;
                        case 0xde: StrText="SET 3,(IX+DD)"; break;
                        case 0xdf: StrText="LD A,SET 3,(IX+DD)"; break;
                        case 0xe0: StrText="LD B,SET 4,(IX+DD)"; break;
                        case 0xe1: StrText="LD C,SET 4,(IX+DD)"; break;
                        case 0xe2: StrText="LD D,SET 4,(IX+DD)"; break;
                        case 0xe3: StrText="LD E,SET 4,(IX+DD)"; break;
                        case 0xe4: StrText="LD H,SET 4,(IX+DD)"; break;
                        case 0xe5: StrText="LD L,SET 4,(IX+DD)"; break;
                        case 0xe6: StrText="SET 4,(IX+DD)"; break;
                        case 0xe7: StrText="LD A,SET 4,(IX+DD)"; break;
                        case 0xe8: StrText="LD B,SET 5,(IX+DD)"; break;
                        case 0xe9: StrText="LD C,SET 5,(IX+DD)"; break;
                        case 0xea: StrText="LD D,SET 5,(IX+DD)"; break;
                        case 0xeb: StrText="LD E,SET 5,(IX+DD)"; break;
                        case 0xec: StrText="LD H,SET 5,(IX+DD)"; break;
                        case 0xed: StrText="LD L,SET 5,(IX+DD)"; break;
                        case 0xee: StrText="SET 5,(IX+DD)"; break;
                        case 0xef: StrText="LD A,SET 5,(IX+DD)"; break;
                        case 0xf0: StrText="LD B,SET 6,(IX+DD)"; break;
                        case 0xf1: StrText="LD C,SET 6,(IX+DD)"; break;
                        case 0xf2: StrText="LD D,SET 6,(IX+DD)"; break;
                        case 0xf3: StrText="LD E,SET 6,(IX+DD)"; break;
                        case 0xf4: StrText="LD H,SET 6,(IX+DD)"; break;
                        case 0xf5: StrText="LD L,SET 6,(IX+DD)"; break;
                        case 0xf6: StrText="SET 6,(IX+DD)"; break;
                        case 0xf7: StrText="LD A,SET 6,(IX+DD)"; break;
                        case 0xf8: StrText="LD B,SET 7,(IX+DD)"; break;
                        case 0xf9: StrText="LD C,SET 7,(IX+DD)"; break;
                        case 0xfa: StrText="LD D,SET 7,(IX+DD)"; break;
                        case 0xfb: StrText="LD E,SET 7,(IX+DD)"; break;
                        case 0xfc: StrText="LD H,SET 7,(IX+DD)"; break;
                        case 0xfd: StrText="LD L,SET 7,(IX+DD)"; break;
                        case 0xfe: StrText="SET 7,(IX+DD)"; break;
                        case 0xff: StrText="LD A,SET 7,(IX+DD)"; break;
                        default:   break;
                        }
                        break;
                case 0xe1: StrText="POP IX"; break;
                case 0xe3: StrText="EX (SP),IX"; break;
                case 0xe5: StrText="PUSH IX"; break;
                case 0xe9: StrText="JP IX"; break;
                case 0xf9: StrText="LD SP,IX"; break;
                }
                break;
        case 0xde: StrText="SBC A,nn"; break;
        case 0xdf: StrText="RST 18"; break;
        case 0xe0: StrText="RET PO"; break;
        case 0xe1: StrText="POP HL"; break;
        case 0xe2: StrText="JP PO,nnnn"; break;
        case 0xe3: StrText="EX (SP),HL"; break;
        case 0xe4: StrText="CALL PO,nnnn"; break;
        case 0xe5: StrText="PUSH HL"; break;
        case 0xe6: StrText="AND nn"; break;
        case 0xe7: StrText="RST 20"; break;
        case 0xe8: StrText="RET PE"; break;
        case 0xe9: StrText="JP HL"; break;
        case 0xea: StrText="JP PE,nnnn"; break;
        case 0xeb: StrText="EX DE,HL"; break;
        case 0xec: StrText="CALL PE,nnnn"; break;
        case 0xed:
                Opcode = GetMem(Addr);
                if (Addr>=zx81.m1not && !(Opcode&64) && !(zx81.machine==MACHINEACE
                                                || zx81.machine==MACHINESPEC48))
                        Opcode=-1;
                StrCode += Hex8(Opcode);
                Addr++;

                switch(Opcode)
                {
                case 0x40:	StrText="IN B,(C)"; break;
                case 0x41:	StrText="OUT (C),B"; break;
                case 0x42:	StrText="SBC HL,BC"; break;
                case 0x43:	StrText="LD (nnnn),BC"; break;
                case 0x44:
                case 0x4c:
                case 0x54:
                case 0x5c:
                case 0x64:
                case 0x6c:
                case 0x74:
                case 0x7c:	StrText="NEG"; break;
                case 0x45:
                case 0x4d:
                case 0x55:
                case 0x5d:
                case 0x65:
                case 0x6d:
                case 0x75:
                case 0x7d:      StrText="RETN"; break;
                case 0x46:
                case 0x4e:
                case 0x66:
                case 0x6e:	StrText="IM 0"; break;
                case 0x47:	StrText="LD I,A"; break;
                case 0x48:	StrText="IN C,(C)"; break;
                case 0x49:	StrText="OUT (C),C"; break;
                case 0x4a:	StrText="ADC HL,BC"; break;
                case 0x4b:	StrText="LD BC,(nnnn)"; break;
                case 0x4f:	StrText="LD R,A"; break;
                case 0x50:	StrText="IN D,(C)"; break;
                case 0x51:	StrText="OUT (C),D"; break;
                case 0x52:	StrText="SBC HL,DE"; break;
                case 0x53:	StrText="LD (nnnn),DE"; break;
                case 0x56:
                case 0x76:	StrText="IM 1"; break;
                case 0x57:	StrText="LD A,I"; break;
                case 0x58:	StrText="IN E,(C)"; break;
                case 0x59:	StrText="OUT (C),E"; break;
                case 0x5a:	StrText="ADC HL,DE"; break;
                case 0x5b:	StrText="LD DE,(nnnn)"; break;
                case 0x5e:
                case 0x7e:	StrText="IM 2"; break;
                case 0x5f:	StrText="LD A,R"; break;
                case 0x60:	StrText="IN H,(C)"; break;
                case 0x61:	StrText="OUT (C),H"; break;
                case 0x62:	StrText="SBC HL,HL"; break;
                case 0x63:	StrText="LD (nnnn),HL"; break;
                case 0x67:	StrText="RRD"; break;
                case 0x68:	StrText="IN L,(C)"; break;
                case 0x69:	StrText="OUT (C),L"; break;
                case 0x6a:	StrText="ADC HL,HL"; break;
                case 0x6b:	StrText="LD HL,(nnnn)"; break;
                case 0x6f:	StrText="RLD"; break;
                case 0x70:	StrText="IN F,(C)"; break;
                case 0x71:	StrText="OUT (C),0"; break;
                case 0x72:	StrText="SBC HL,SP"; break;
                case 0x73:	StrText="LD (nnnn),SP"; break;
                case 0x78:	StrText="IN A,(C)"; break;
                case 0x79:	StrText="OUT (C),A"; break;
                case 0x7a:	StrText="ADC HL,SP"; break;
                case 0x7b:	StrText="LD SP,(nnnn)"; break;
                case 0xa0:	StrText="LDI"; break;
                case 0xa1:	StrText="CPI"; break;
                case 0xa2:	StrText="INI"; break;
                case 0xa3:	StrText="OUTI"; break;
                case 0xa8:	StrText="LDD"; break;
                case 0xa9:	StrText="CPD"; break;
                case 0xaa:	StrText="IND"; break;
                case 0xab:	StrText="OUTD"; break;
                case 0xb0:	StrText="LDIR"; break;
                case 0xb1:	StrText="CPIR"; break;
                case 0xb2:	StrText="INIR"; break;
                case 0xb3:	StrText="OTIR"; break;
                case 0xb8:	StrText="LDDR"; break;
                case 0xb9:	StrText="CPDR"; break;
                case 0xba:	StrText="INDR"; break;
                case 0xbb:	StrText="OTDR"; break;
                default:        break;
                }
                break;
        case 0xee: StrText="XOR A,nn"; break;
        case 0xef: StrText="RST 28"; break;
        case 0xf0: StrText="RET P"; break;
        case 0xf1: StrText="POP AF"; break;
        case 0xf2: StrText="JP P,nnnn"; break;
        case 0xf3: StrText="DI"; break;
        case 0xf4: StrText="CALL P,nnnn"; break;
        case 0xf5: StrText="PUSH AF"; break;
        case 0xf6: StrText="OR nn"; break;
        case 0xf7: StrText="RST 30"; break;
        case 0xf8: StrText="RET M"; break;
        case 0xf9: StrText="LD SP,HL"; break;
        case 0xfa: StrText="JP M,nnnn"; break;
        case 0xfb: StrText="EI"; break;
        case 0xfc: StrText="CALL M,nnnn"; break;
        case 0xfd:
                Opcode = GetMem(Addr);
                if (Addr>=zx81.m1not && !(Opcode&64) && !(zx81.machine==MACHINEACE
                                                || zx81.machine==MACHINESPEC48))
                        Opcode=-1;
                StrCode += Hex8(Opcode);
                Addr++;

                switch(Opcode)
                {
                case 0x09: StrText="ADD IY,BC"; break;
                case 0x19: StrText="ADD IY,DE"; break;
                case 0x21: StrText="LD IY,nnnn"; break;
                case 0x22: StrText="LD (nnnn),IY"; break;
                case 0x23: StrText="INC IY"; break;
                case 0x24: StrText="INC IYH"; break;
                case 0x25: StrText="DEC IYH"; break;
                case 0x26: StrText="LD IYH,nn"; break;
                case 0x29: StrText="ADD IY,IY"; break;
                case 0x2a: StrText="LD IY,(nnnn)"; break;
                case 0x2b: StrText="DEC IY"; break;
                case 0x2c: StrText="INC IYL"; break;
                case 0x2d: StrText="DEC IYL"; break;
                case 0x2e: StrText="LD IYL,nn"; break;
                case 0x34: StrText="INC (IY+dd)"; break;
                case 0x35: StrText="DEC (IY+dd)"; break;
                case 0x36: StrText="LD (IY+dd),nn"; break;
                case 0x39: StrText="ADD IY,SP"; break;
                case 0x44: StrText="LD B,IYH"; break;
                case 0x45: StrText="LD B,IYL"; break;
                case 0x46: StrText="LD B,(IY+dd)"; break;
                case 0x4c: StrText="LD C,IYH"; break;
                case 0x4d: StrText="LD C,IYL"; break;
                case 0x4e: StrText="LD C,(IY+dd)"; break;
                case 0x54: StrText="LD D,IYH"; break;
                case 0x55: StrText="LD D,IYL"; break;
                case 0x56: StrText="LD D,(IY+dd)"; break;
                case 0x5c: StrText="LD E,IYH"; break;
                case 0x5d: StrText="LD E,IYL"; break;
                case 0x5e: StrText="LD E,(IY+dd)"; break;
                case 0x60: StrText="LD IYH,B"; break;
                case 0x61: StrText="LD IYH,C"; break;
                case 0x62: StrText="LD IYH,D"; break;
                case 0x63: StrText="LD IYH,E"; break;
                case 0x64: StrText="LD IYH,IYH"; break;
                case 0x65: StrText="LD IYH,IYL"; break;
                case 0x66: StrText="LD H,(IY+dd)"; break;
                case 0x67: StrText="LD IYH,A"; break;
                case 0x68: StrText="LD IYL,B"; break;
                case 0x69: StrText="LD IYL,C"; break;
                case 0x6a: StrText="LD IYL,D"; break;
                case 0x6b: StrText="LD IYL,E"; break;
                case 0x6c: StrText="LD IYL,IYH"; break;
                case 0x6d: StrText="LD IYL,IYL"; break;
                case 0x6e: StrText="LD L,(IY+dd)"; break;
                case 0x6f: StrText="LD IYL,A"; break;
                case 0x70: StrText="LD (IY+dd),B"; break;
                case 0x71: StrText="LD (IY+dd),C"; break;
                case 0x72: StrText="LD (IY+dd),D"; break;
                case 0x73: StrText="LD (IY+dd),E"; break;
                case 0x74: StrText="LD (IY+dd),H"; break;
                case 0x75: StrText="LD (IY+dd),L"; break;
                case 0x77: StrText="LD (IY+dd),A"; break;
                case 0x7c: StrText="LD A,IYH"; break;
                case 0x7d: StrText="LD A,IYL"; break;
                case 0x7e: StrText="LD A,(IY+dd)"; break;
                case 0x84: StrText="ADD A,IYH"; break;
                case 0x85: StrText="ADD A,IYL"; break;
                case 0x86: StrText="ADD A,(IY+dd)"; break;
                case 0x8c: StrText="ADC A,IYH"; break;
                case 0x8d: StrText="ADC A,IYL"; break;
                case 0x8e: StrText="ADC A,(IY+dd)"; break;
                case 0x94: StrText="SUB A,IYH"; break;
                case 0x95: StrText="SUB A,IYL"; break;
                case 0x96: StrText="SUB A,(IY+dd)"; break;
                case 0x9c: StrText="SBC A,IYH"; break;
                case 0x9d: StrText="SBC A,IYL"; break;
                case 0x9e: StrText="SBC A,(IY+dd)"; break;
                case 0xa4: StrText="AND A,IYH"; break;
                case 0xa5: StrText="AND A,IYL"; break;
                case 0xa6: StrText="AND A,(IY+dd)"; break;
                case 0xac: StrText="XOR A,IYH"; break;
                case 0xad: StrText="XOR A,IYL"; break;
                case 0xae: StrText="XOR A,(IY+dd)"; break;
                case 0xb4: StrText="OR A,IYH"; break;
                case 0xb5: StrText="OR A,IYL"; break;
                case 0xb6: StrText="OR A,(IY+dd)"; break;
                case 0xbc: StrText="CP A,IYH"; break;
                case 0xbd: StrText="CP A,IYL"; break;
                case 0xbe: StrText="CP A,(IY+dd)"; break;
                case 0xcb:
                        Offset = GetMem(Addr++);
                        StrCode += Hex8(Offset);
                        Opcode = GetMem(Addr);
                        if (Addr>=zx81.m1not && !(Opcode&64) && !(zx81.machine==MACHINEACE
                                                        || zx81.machine==MACHINESPEC48))
                                Opcode=-1;
                        StrCode += Hex8(Opcode);
                        Addr++;

                        switch(Opcode)
                        {
                        case 0x00: StrText="LD B,RLC (IY+DD)"; break;
                        case 0x01: StrText="LD C,RLC (IY+DD)"; break;
                        case 0x02: StrText="LD D,RLC (IY+DD)"; break;
                        case 0x03: StrText="LD E,RLC (IY+DD)"; break;
                        case 0x04: StrText="LD H,RLC (IY+DD)"; break;
                        case 0x05: StrText="LD L,RLC (IY+DD)"; break;
                        case 0x06: StrText="RLC (IY+DD)"; break;
                        case 0x07: StrText="LD A,RLC (IY+DD)"; break;
                        case 0x08: StrText="LD B,RRC (IY+DD)"; break;
                        case 0x09: StrText="LD C,RRC (IY+DD)"; break;
                        case 0x0a: StrText="LD D,RRC (IY+DD)"; break;
                        case 0x0b: StrText="LD E,RRC (IY+DD)"; break;
                        case 0x0c: StrText="LD H,RRC (IY+DD)"; break;
                        case 0x0d: StrText="LD L,RRC (IY+DD)"; break;
                        case 0x0e: StrText="RRC (IY+DD)"; break;
                        case 0x0f: StrText="LD A,RRC (IY+DD)"; break;
                        case 0x10: StrText="LD B,RL (IY+DD)"; break;
                        case 0x11: StrText="LD C,RL (IY+DD)"; break;
                        case 0x12: StrText="LD D,RL (IY+DD)"; break;
                        case 0x13: StrText="LD E,RL (IY+DD)"; break;
                        case 0x14: StrText="LD H,RL (IY+DD)"; break;
                        case 0x15: StrText="LD L,RL (IY+DD)"; break;
                        case 0x16: StrText="RL (IY+DD)"; break;
                        case 0x17: StrText="LD A,RL (IY+DD)"; break;
                        case 0x18: StrText="LD B,RR (IY+DD)"; break;
                        case 0x19: StrText="LD C,RR (IY+DD)"; break;
                        case 0x1a: StrText="LD D,RR (IY+DD)"; break;
                        case 0x1b: StrText="LD E,RR (IY+DD)"; break;
                        case 0x1c: StrText="LD H,RR (IY+DD)"; break;
                        case 0x1d: StrText="LD L,RR (IY+DD)"; break;
                        case 0x1e: StrText="RR (IY+DD)"; break;
                        case 0x1f: StrText="LD A,RR (IY+DD)"; break;
                        case 0x20: StrText="LD B,SLA (IY+DD)"; break;
                        case 0x21: StrText="LD C,SLA (IY+DD)"; break;
                        case 0x22: StrText="LD D,SLA (IY+DD)"; break;
                        case 0x23: StrText="LD E,SLA (IY+DD)"; break;
                        case 0x24: StrText="LD H,SLA (IY+DD)"; break;
                        case 0x25: StrText="LD L,SLA (IY+DD)"; break;
                        case 0x26: StrText="SLA (IY+DD)"; break;
                        case 0x27: StrText="LD A,SLA (IY+DD)"; break;
                        case 0x28: StrText="LD B,SRA (IY+DD)"; break;
                        case 0x29: StrText="LD C,SRA (IY+DD)"; break;
                        case 0x2a: StrText="LD D,SRA (IY+DD)"; break;
                        case 0x2b: StrText="LD E,SRA (IY+DD)"; break;
                        case 0x2c: StrText="LD H,SRA (IY+DD)"; break;
                        case 0x2d: StrText="LD L,SRA (IY+DD)"; break;
                        case 0x2e: StrText="SRA (IY+DD)"; break;
                        case 0x2f: StrText="LD A,SRA (IY+DD)"; break;
                        case 0x30: StrText="LD B,SLL (IY+DD)"; break;
                        case 0x31: StrText="LD C,SLL (IY+DD)"; break;
                        case 0x32: StrText="LD D,SLL (IY+DD)"; break;
                        case 0x33: StrText="LD E,SLL (IY+DD)"; break;
                        case 0x34: StrText="LD H,SLL (IY+DD)"; break;
                        case 0x35: StrText="LD L,SLL (IY+DD)"; break;
                        case 0x36: StrText="SLL (IY+DD)"; break;
                        case 0x37: StrText="LD A,SLL (IY+DD)"; break;
                        case 0x38: StrText="LD B,SRL (IY+DD)"; break;
                        case 0x39: StrText="LD C,SRL (IY+DD)"; break;
                        case 0x3a: StrText="LD D,SRL (IY+DD)"; break;
                        case 0x3b: StrText="LD E,SRL (IY+DD)"; break;
                        case 0x3c: StrText="LD H,SRL (IY+DD)"; break;
                        case 0x3d: StrText="LD L,SRL (IY+DD)"; break;
                        case 0x3e: StrText="SRL (IY+DD)"; break;
                        case 0x3f: StrText="LD A,SRL (IY+DD)"; break;
                        case 0x40:
                        case 0x41:
                        case 0x42:
                        case 0x43:
                        case 0x44:
                        case 0x45:
                        case 0x46:
                        case 0x47: StrText="BIT 0,(IY+DD)"; break;
                        case 0x48:
                        case 0x49:
                        case 0x4a:
                        case 0x4b:
                        case 0x4c:
                        case 0x4d:
                        case 0x4e:
                        case 0x4f: StrText="BIT 1,(IY+DD)"; break;
                        case 0x50:
                        case 0x51:
                        case 0x52:
                        case 0x53:
                        case 0x54:
                        case 0x55:
                        case 0x56:
                        case 0x57: StrText="BIT 2,(IY+DD)"; break;
                        case 0x58:
                        case 0x59:
                        case 0x5a:
                        case 0x5b:
                        case 0x5c:
                        case 0x5d:
                        case 0x5e:
                        case 0x5f: StrText="BIT 3,(IY+DD)"; break;
                        case 0x60:
                        case 0x61:
                        case 0x62:
                        case 0x63:
                        case 0x64:
                        case 0x65:
                        case 0x66:
                        case 0x67: StrText="BIT 4,(IY+DD)"; break;
                        case 0x68:
                        case 0x69:
                        case 0x6a:
                        case 0x6b:
                        case 0x6c:
                        case 0x6d:
                        case 0x6e:
                        case 0x6f: StrText="BIT 5,(IY+DD)"; break;
                        case 0x70:
                        case 0x71:
                        case 0x72:
                        case 0x73:
                        case 0x74:
                        case 0x75:
                        case 0x76:
                        case 0x77: StrText="BIT 6,(IY+DD)"; break;
                        case 0x78:
                        case 0x79:
                        case 0x7a:
                        case 0x7b:
                        case 0x7c:
                        case 0x7d:
                        case 0x7e:
                        case 0x7f: StrText="BIT 7,(IY+DD)"; break;
                        case 0x80: StrText="LD B,RES 0,(IY+DD)"; break;
                        case 0x81: StrText="LD C,RES 0,(IY+DD)"; break;
                        case 0x82: StrText="LD D,RES 0,(IY+DD)"; break;
                        case 0x83: StrText="LD E,RES 0,(IY+DD)"; break;
                        case 0x84: StrText="LD H,RES 0,(IY+DD)"; break;
                        case 0x85: StrText="LD L,RES 0,(IY+DD)"; break;
                        case 0x86: StrText="RES 0,(IY+DD)"; break;
                        case 0x87: StrText="LD A,RES 0,(IY+DD)"; break;
                        case 0x88: StrText="LD B,RES 1,(IY+DD)"; break;
                        case 0x89: StrText="LD C,RES 1,(IY+DD)"; break;
                        case 0x8a: StrText="LD D,RES 1,(IY+DD)"; break;
                        case 0x8b: StrText="LD E,RES 1,(IY+DD)"; break;
                        case 0x8c: StrText="LD H,RES 1,(IY+DD)"; break;
                        case 0x8d: StrText="LD L,RES 1,(IY+DD)"; break;
                        case 0x8e: StrText="RES 1,(IY+DD)"; break;
                        case 0x8f: StrText="LD A,RES 1,(IY+DD)"; break;
                        case 0x90: StrText="LD B,RES 2,(IY+DD)"; break;
                        case 0x91: StrText="LD C,RES 2,(IY+DD)"; break;
                        case 0x92: StrText="LD D,RES 2,(IY+DD)"; break;
                        case 0x93: StrText="LD E,RES 2,(IY+DD)"; break;
                        case 0x94: StrText="LD H,RES 2,(IY+DD)"; break;
                        case 0x95: StrText="LD L,RES 2,(IY+DD)"; break;
                        case 0x96: StrText="RES 2,(IY+DD)"; break;
                        case 0x97: StrText="LD A,RES 2,(IY+DD)"; break;
                        case 0x98: StrText="LD B,RES 3,(IY+DD)"; break;
                        case 0x99: StrText="LD C,RES 3,(IY+DD)"; break;
                        case 0x9a: StrText="LD D,RES 3,(IY+DD)"; break;
                        case 0x9b: StrText="LD E,RES 3,(IY+DD)"; break;
                        case 0x9c: StrText="LD H,RES 3,(IY+DD)"; break;
                        case 0x9d: StrText="LD L,RES 3,(IY+DD)"; break;
                        case 0x9e: StrText="RES 3,(IY+DD)"; break;
                        case 0x9f: StrText="LD A,RES 3,(IY+DD)"; break;
                        case 0xa0: StrText="LD B,RES 4,(IY+DD)"; break;
                        case 0xa1: StrText="LD C,RES 4,(IY+DD)"; break;
                        case 0xa2: StrText="LD D,RES 4,(IY+DD)"; break;
                        case 0xa3: StrText="LD E,RES 4,(IY+DD)"; break;
                        case 0xa4: StrText="LD H,RES 4,(IY+DD)"; break;
                        case 0xa5: StrText="LD L,RES 4,(IY+DD)"; break;
                        case 0xa6: StrText="RES 4,(IY+DD)"; break;
                        case 0xa7: StrText="LD A,RES 4,(IY+DD)"; break;
                        case 0xa8: StrText="LD B,RES 5,(IY+DD)"; break;
                        case 0xa9: StrText="LD C,RES 5,(IY+DD)"; break;
                        case 0xaa: StrText="LD D,RES 5,(IY+DD)"; break;
                        case 0xab: StrText="LD E,RES 5,(IY+DD)"; break;
                        case 0xac: StrText="LD H,RES 5,(IY+DD)"; break;
                        case 0xad: StrText="LD L,RES 5,(IY+DD)"; break;
                        case 0xae: StrText="RES 5,(IY+DD)"; break;
                        case 0xaf: StrText="LD A,RES 5,(IY+DD)"; break;
                        case 0xb0: StrText="LD B,RES 6,(IY+DD)"; break;
                        case 0xb1: StrText="LD C,RES 6,(IY+DD)"; break;
                        case 0xb2: StrText="LD D,RES 6,(IY+DD)"; break;
                        case 0xb3: StrText="LD E,RES 6,(IY+DD)"; break;
                        case 0xb4: StrText="LD H,RES 6,(IY+DD)"; break;
                        case 0xb5: StrText="LD L,RES 6,(IY+DD)"; break;
                        case 0xb6: StrText="RES 6,(IY+DD)"; break;
                        case 0xb7: StrText="LD A,RES 6,(IY+DD)"; break;
                        case 0xb8: StrText="LD B,RES 7,(IY+DD)"; break;
                        case 0xb9: StrText="LD C,RES 7,(IY+DD)"; break;
                        case 0xba: StrText="LD D,RES 7,(IY+DD)"; break;
                        case 0xbb: StrText="LD E,RES 7,(IY+DD)"; break;
                        case 0xbc: StrText="LD H,RES 7,(IY+DD)"; break;
                        case 0xbd: StrText="LD L,RES 7,(IY+DD)"; break;
                        case 0xbe: StrText="RES 7,(IY+DD)"; break;
                        case 0xbf: StrText="LD A,RES 7,(IY+DD)"; break;
                        case 0xc0: StrText="LD B,SET 0,(IY+DD)"; break;
                        case 0xc1: StrText="LD C,SET 0,(IY+DD)"; break;
                        case 0xc2: StrText="LD D,SET 0,(IY+DD)"; break;
                        case 0xc3: StrText="LD E,SET 0,(IY+DD)"; break;
                        case 0xc4: StrText="LD H,SET 0,(IY+DD)"; break;
                        case 0xc5: StrText="LD L,SET 0,(IY+DD)"; break;
                        case 0xc6: StrText="SET 0,(IY+DD)"; break;
                        case 0xc7: StrText="LD A,SET 0,(IY+DD)"; break;
                        case 0xc8: StrText="LD B,SET 1,(IY+DD)"; break;
                        case 0xc9: StrText="LD C,SET 1,(IY+DD)"; break;
                        case 0xca: StrText="LD D,SET 1,(IY+DD)"; break;
                        case 0xcb: StrText="LD E,SET 1,(IY+DD)"; break;
                        case 0xcc: StrText="LD H,SET 1,(IY+DD)"; break;
                        case 0xcd: StrText="LD L,SET 1,(IY+DD)"; break;
                        case 0xce: StrText="SET 1,(IY+DD)"; break;
                        case 0xcf: StrText="LD A,SET 1,(IY+DD)"; break;
                        case 0xd0: StrText="LD B,SET 2,(IY+DD)"; break;
                        case 0xd1: StrText="LD C,SET 2,(IY+DD)"; break;
                        case 0xd2: StrText="LD D,SET 2,(IY+DD)"; break;
                        case 0xd3: StrText="LD E,SET 2,(IY+DD)"; break;
                        case 0xd4: StrText="LD H,SET 2,(IY+DD)"; break;
                        case 0xd5: StrText="LD L,SET 2,(IY+DD)"; break;
                        case 0xd6: StrText="SET 2,(IY+DD)"; break;
                        case 0xd7: StrText="LD A,SET 2,(IY+DD)"; break;
                        case 0xd8: StrText="LD B,SET 3,(IY+DD)"; break;
                        case 0xd9: StrText="LD C,SET 3,(IY+DD)"; break;
                        case 0xda: StrText="LD D,SET 3,(IY+DD)"; break;
                        case 0xdb: StrText="LD E,SET 3,(IY+DD)"; break;
                        case 0xdc: StrText="LD H,SET 3,(IY+DD)"; break;
                        case 0xdd: StrText="LD L,SET 3,(IY+DD)"; break;
                        case 0xde: StrText="SET 3,(IY+DD)"; break;
                        case 0xdf: StrText="LD A,SET 3,(IY+DD)"; break;
                        case 0xe0: StrText="LD B,SET 4,(IY+DD)"; break;
                        case 0xe1: StrText="LD C,SET 4,(IY+DD)"; break;
                        case 0xe2: StrText="LD D,SET 4,(IY+DD)"; break;
                        case 0xe3: StrText="LD E,SET 4,(IY+DD)"; break;
                        case 0xe4: StrText="LD H,SET 4,(IY+DD)"; break;
                        case 0xe5: StrText="LD L,SET 4,(IY+DD)"; break;
                        case 0xe6: StrText="SET 4,(IY+DD)"; break;
                        case 0xe7: StrText="LD A,SET 4,(IY+DD)"; break;
                        case 0xe8: StrText="LD B,SET 5,(IY+DD)"; break;
                        case 0xe9: StrText="LD C,SET 5,(IY+DD)"; break;
                        case 0xea: StrText="LD D,SET 5,(IY+DD)"; break;
                        case 0xeb: StrText="LD E,SET 5,(IY+DD)"; break;
                        case 0xec: StrText="LD H,SET 5,(IY+DD)"; break;
                        case 0xed: StrText="LD L,SET 5,(IY+DD)"; break;
                        case 0xee: StrText="SET 5,(IY+DD)"; break;
                        case 0xef: StrText="LD A,SET 5,(IY+DD)"; break;
                        case 0xf0: StrText="LD B,SET 6,(IY+DD)"; break;
                        case 0xf1: StrText="LD C,SET 6,(IY+DD)"; break;
                        case 0xf2: StrText="LD D,SET 6,(IY+DD)"; break;
                        case 0xf3: StrText="LD E,SET 6,(IY+DD)"; break;
                        case 0xf4: StrText="LD H,SET 6,(IY+DD)"; break;
                        case 0xf5: StrText="LD L,SET 6,(IY+DD)"; break;
                        case 0xf6: StrText="SET 6,(IY+DD)"; break;
                        case 0xf7: StrText="LD A,SET 6,(IY+DD)"; break;
                        case 0xf8: StrText="LD B,SET 7,(IY+DD)"; break;
                        case 0xf9: StrText="LD C,SET 7,(IY+DD)"; break;
                        case 0xfa: StrText="LD D,SET 7,(IY+DD)"; break;
                        case 0xfb: StrText="LD E,SET 7,(IY+DD)"; break;
                        case 0xfc: StrText="LD H,SET 7,(IY+DD)"; break;
                        case 0xfd: StrText="LD L,SET 7,(IY+DD)"; break;
                        case 0xfe: StrText="SET 7,(IY+DD)"; break;
                        case 0xff: StrText="LD A,SET 7,(IY+DD)"; break;
                        default:   break;
                        }
                        break;
                case 0xe1: StrText="POP IY"; break;
                case 0xe3: StrText="EX (SP),IY"; break;
                case 0xe5: StrText="PUSH IY"; break;
                case 0xe9: StrText="JP IY"; break;
                case 0xf9: StrText="LD SP,IY"; break;
                }
                break;
        case 0xfe: StrText="CP nn"; break;
        case 0xff: StrText="RST 38"; break;
        }

        p=StrText.AnsiPos("nnnn");
        if (p)
        {
                StrText=StrRep(StrText,p,4,"$"+Hex16(GetMem(Addr) + 256*GetMem(Addr+1)));
                StrCode += Hex16(GetMem(Addr) + 256*GetMem(Addr+1));
                Addr+=2;
        }

        p=StrText.AnsiPos("nn");
        if (p)
        {
                StrText=StrRep(StrText,p,2,"$"+Hex8(GetMem(Addr)));
                StrCode += Hex8(GetMem(Addr++));
        }

        p=StrText.AnsiPos("fz");
        if (p)
        {
                StrText=StrRep(StrText,p,2,"$"+Hex16(Addr + 1 +((signed char)GetMem(Addr))));
                StrCode += Hex8(GetMem(Addr++));
        }

        p=StrText.AnsiPos("+dd");
        if (p)
        {
                int i;
                AnsiString chr="+";

                i=GetMem(Addr);
                if (i>=128) StrText=StrRep(StrText,p,3,"-$"+Hex8(255-i));
                else StrText=StrRep(StrText,p,3,"+$" + Hex8(i));
                StrCode += Hex8(GetMem(Addr++));
        }

        p=StrText.AnsiPos("+DD");
        if (p)
        {
                int i;
                AnsiString chr="+";

                i=Offset;
                if (i>=128) StrText=StrRep(StrText,p,3,"-$"+Hex8(255-i));
                else StrText=StrRep(StrText,p,3,"+$" + Hex8(i));
        }

        while (StrCode.Length() < 8) StrCode += "  ";

        StrAddr += " " + StrCode + " " + StrText;

        *Ad = Addr;
        return(StrAddr);
}
