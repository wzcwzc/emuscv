#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#define for if(0);else for
typedef unsigned char uint8;
typedef unsigned short uint16;

#define asc(c) ((0x20 <= c && c < 0x7f) ? c : '.')

uint8 src[0x10003];
int pc;

uint8 getb() { return src[pc++]; }
uint16 getw() { uint16 l = getb(); return l | (getb() << 8); }
uint8 getwa() { return getb(); }

// 80-ff
// 800-fff

void dasm(char out[])
{
	char op[64];
	uint8 b, b0 = src[pc], b1 = src[pc + 1], b2 = src[pc + 2], b3 = src[pc + 3];
	uint16 prv = pc, wa;
	
	switch(b = getb())
	{
	case 0x00: sprintf(op, "NOP"); break;
	case 0x01: sprintf(op, "HLT"); break;
	case 0x02: sprintf(op, "INX SP"); break;
	case 0x03: sprintf(op, "DCX SP"); break;
	case 0x04: sprintf(op, "LXI SP,#%04X", getw()); break;
	case 0x05: wa = getwa(); sprintf(op, "ANIW V.#%02X,#%02X", wa, getb()); break;
//	case 0x06:
	case 0x07: b = getb(); sprintf(op, "ANI A,#%02X\t; ANI1 A,#%02X", b, b); break;
	case 0x08: sprintf(op, "RET"); break;
	case 0x09: sprintf(op, "SIO"); break;
	case 0x0a: sprintf(op, "MOV A,B"); break;
	case 0x0b: sprintf(op, "MOV A,C"); break;
	case 0x0c: sprintf(op, "MOV A,D"); break;
	case 0x0d: sprintf(op, "MOV A,E"); break;
	case 0x0e: sprintf(op, "MOV A,H"); break;
	case 0x0f: sprintf(op, "MOV A,L"); break;
	
	case 0x10: sprintf(op, "EX"); break;
	case 0x11: sprintf(op, "EXX"); break;
	case 0x12: sprintf(op, "INX B"); break;
	case 0x13: sprintf(op, "DCX B"); break;
	case 0x14: sprintf(op, "LXI B,#%04X", getw()); break;
	case 0x15: wa = getwa(); sprintf(op, "ORIW V.#%02X,#%02X", wa, getb()); break;
	case 0x16: b = getb(); sprintf(op, "XRI A,#%02X\t; XRI1 A,#%02X", b, b); break;
	case 0x17: b = getb(); sprintf(op, "ORI A,#%02X\t; ORI1 A,#%02X", b, b); break;
	case 0x18: sprintf(op, "RETS"); break;
	case 0x19: sprintf(op, "STM"); break;
	case 0x1a: sprintf(op, "MOV B,A"); break;
	case 0x1b: sprintf(op, "MOV C,A"); break;
	case 0x1c: sprintf(op, "MOV D,A"); break;
	case 0x1d: sprintf(op, "MOV E,A"); break;
	case 0x1e: sprintf(op, "MOV H,A"); break;
	case 0x1f: sprintf(op, "MOV L,A"); break;
	
	case 0x20: sprintf(op, "INRW V.#%02X", getwa()); break;
	case 0x21: sprintf(op, "TABLE"); break;
	case 0x22: sprintf(op, "INX D"); break;
	case 0x23: sprintf(op, "DCX D"); break;
	case 0x24: sprintf(op, "LXI D,#%04X", getw()); break;
	case 0x25: wa = getwa(); sprintf(op, "GTIW V.#%02X,#%02X", wa, getb()); break;
	case 0x26: b = getb(); sprintf(op, "ADINC A,#%02X\t; ADINC1 A,#%02X", b, b); break;
	case 0x27: b = getb(); sprintf(op, "GTI A,#%02X\t; GTI1 A,#%02X", b, b); break;
	case 0x28: sprintf(op, "LDAW V.#%02X", getwa()); break;
	case 0x29: sprintf(op, "LDAX B"); break;
	case 0x2a: sprintf(op, "LDAX D"); break;
	case 0x2b: sprintf(op, "LDAX H"); break;
	case 0x2c: sprintf(op, "LDAX D+"); break;
	case 0x2d: sprintf(op, "LDAX H+"); break;
	case 0x2e: sprintf(op, "LDAX D-"); break;
	case 0x2f: sprintf(op, "LDAX H-"); break;
	
	case 0x30: sprintf(op, "DCRW V.#%02X", getwa()); break;
	case 0x31: sprintf(op, "BLOCK"); break;
	case 0x32: sprintf(op, "INX H"); break;
	case 0x33: sprintf(op, "DCX H"); break;
	case 0x34: sprintf(op, "LXI H,#%04X", getw()); break;
	case 0x35: wa = getwa(); sprintf(op, "LTIW V.#%02X,#%02X", wa, getb()); break;
	case 0x36: b = getb(); sprintf(op, "SUINB A,#%02X\t; SUINB1 A,#%02X", b, b); break;
	case 0x37: b = getb(); sprintf(op, "LTI A,#%02X\t; LTI1 A,#%02X", b, b); break;
	case 0x38: sprintf(op, "STAW V.#%02X", getwa()); break;
	case 0x39: sprintf(op, "STAX B"); break;
	case 0x3a: sprintf(op, "STAX D"); break;
	case 0x3b: sprintf(op, "STAX H"); break;
	case 0x3c: sprintf(op, "STAX D+"); break;
	case 0x3d: sprintf(op, "STAX H+"); break;
	case 0x3e: sprintf(op, "STAX D-"); break;
	case 0x3f: sprintf(op, "STAX H-"); break;
	
//	case 0x40:
	case 0x41: sprintf(op, "INR A"); break;
	case 0x42: sprintf(op, "INR B"); break;
	case 0x43: sprintf(op, "INR C"); break;
	case 0x44: sprintf(op, "CALL #%4X", getw()); break;
	case 0x45: wa = getwa(); sprintf(op, "ONIW V.#%02X,#%02X", wa, getb()); break;
	case 0x46: b = getb(); sprintf(op, "ADI A,#%02X\t; ADI1 A,#%02X", b, b); break;
	case 0x47: b = getb(); sprintf(op, "ONI A,#%02X\t; ONI1 A,#%02X", b, b); break;
	case 0x48:
		switch(b = getb())
		{
		case 0x00: sprintf(op, "SKIT F0"); break;
		case 0x01: sprintf(op, "SKIT FT"); break;
		case 0x02: sprintf(op, "SKIT F1"); break;
		case 0x03: sprintf(op, "SKIT F2"); break;
		case 0x04: sprintf(op, "SKIT FS"); break;
		case 0x0a: sprintf(op, "SKC"); break;
		case 0x0c: sprintf(op, "SKZ"); break;
		case 0x0e: sprintf(op, "PUSH v"); break;
		case 0x0f: sprintf(op, "POP v"); break;
		case 0x10: sprintf(op, "SKNIT F0"); break;
		case 0x11: sprintf(op, "SKNIT FT"); break;
		case 0x12: sprintf(op, "SKNIT F1"); break;
		case 0x13: sprintf(op, "SKNIT F2"); break;
		case 0x14: sprintf(op, "SKNIT FS"); break;
		case 0x1a: sprintf(op, "SKNC"); break;
		case 0x1c: sprintf(op, "SKNZ"); break;
		case 0x1e: sprintf(op, "PUSH B"); break;
		case 0x1f: sprintf(op, "POP B"); break;
		case 0x20: sprintf(op, "EI"); break;
		case 0x24: sprintf(op, "DI"); break;
		case 0x2a: sprintf(op, "CLC"); break;
		case 0x2b: sprintf(op, "STC"); break;
		case 0x2c: sprintf(op, "PEN"); break;
		case 0x2d: sprintf(op, "PEX"); break;
		case 0x2e: sprintf(op, "PUSH D"); break;
		case 0x2f: sprintf(op, "POP D"); break;
		case 0x30: sprintf(op, "RAL"); break;
		case 0x31: sprintf(op, "RAR"); break;
		case 0x32: sprintf(op, "RCL"); break;
		case 0x33: sprintf(op, "RCR"); break;
		case 0x34: sprintf(op, "SHAL"); break;
		case 0x35: sprintf(op, "SHAR"); break;
		case 0x36: sprintf(op, "SHCL"); break;
		case 0x37: sprintf(op, "SHCR"); break;
		case 0x38: sprintf(op, "RLD"); break;
		case 0x39: sprintf(op, "RRD"); break;
		case 0x3c: sprintf(op, "PER"); break;
		case 0x3e: sprintf(op, "PUSH H"); break;
		case 0x3f: sprintf(op, "POP H"); break;
		default: sprintf(op, "DB 48h,#%02X", b);
		}
		break;
	case 0x49: sprintf(op, "MVIX B,#%02X", getb()); break;
	case 0x4a: sprintf(op, "MVIX D,#%02X", getb()); break;
	case 0x4b: sprintf(op, "MVIX H,#%02X", getb()); break;
	case 0x4c:
		switch(b = getb())
		{
		case 0xc0: sprintf(op, "MOV A,PA"); break;
		case 0xc1: sprintf(op, "MOV A,PB"); break;
		case 0xc2: sprintf(op, "MOV A,PC"); break;
		case 0xc3: sprintf(op, "MOV A,MK"); break;
		case 0xc4: sprintf(op, "MOV A,MB"); break;	// ����`?
		case 0xc5: sprintf(op, "MOV A,MC"); break;	// ����`?
		case 0xc6: sprintf(op, "MOV A,TM0"); break;	// ����`?
		case 0xc7: sprintf(op, "MOV A,TM1"); break;	// ����`?
		case 0xc8: sprintf(op, "MOV A,S"); break;
		default:
			if(b < 0xc0) {
				sprintf(op, "IN #%02X", b); break;
			}
			sprintf(op, "DB 4ch,#%02X", b);
		}
		break;
	case 0x4d:
		switch(b = getb())
		{
		case 0xc0: sprintf(op, "MOV PA,A"); break;
		case 0xc1: sprintf(op, "MOV PB,A"); break;
		case 0xc2: sprintf(op, "MOV PC,A"); break;
		case 0xc3: sprintf(op, "MOV MK,A"); break;
		case 0xc4: sprintf(op, "MOV MB,A"); break;
		case 0xc5: sprintf(op, "MOV MC,A"); break;
		case 0xc6: sprintf(op, "MOV TM0,A"); break;
		case 0xc7: sprintf(op, "MOV TM1,A"); break;
		case 0xc8: sprintf(op, "MOV S,A"); break;
		default:
			if(b < 0xc0) {
				sprintf(op, "OUT #%02X", b); break;
			}
			sprintf(op, "DB 4dh,#%02X", b);
		}
		break;
	case 0x4e: b = getb(); sprintf(op, "JRE #%04X", pc + b); break;
	case 0x4f: b = getb(); sprintf(op, "JRE #%04X", (pc + b - 256) & 0xffff); break;
	
//	case 0x50:
	case 0x51: sprintf(op, "DCR A"); break;
	case 0x52: sprintf(op, "DCR B"); break;
	case 0x53: sprintf(op, "DCR C"); break;
	case 0x54: sprintf(op, "JMP #%04X", getw()); break;
	case 0x55: wa = getwa(); sprintf(op, "OFFIW V.#%02X,#%02X", wa, getb()); break;
	case 0x56: b = getb(); sprintf(op, "ACI A,#%02X\t; ACI1 A,#%02X", b, b); break;
	case 0x57: b = getb(); sprintf(op, "OFFI A,#%02X\t; OFFI1 A,#%02X", b, b); break;
	case 0x58: sprintf(op, "BIT 0,V.#%02X", getwa()); break;
	case 0x59: sprintf(op, "BIT 1,V.#%02X", getwa()); break;
	case 0x5a: sprintf(op, "BIT 2,V.#%02X", getwa()); break;
	case 0x5b: sprintf(op, "BIT 3,V.#%02X", getwa()); break;
	case 0x5c: sprintf(op, "BIT 4,V.#%02X", getwa()); break;
	case 0x5d: sprintf(op, "BIT 5,V.#%02X", getwa()); break;
	case 0x5e: sprintf(op, "BIT 6,V.#%02X", getwa()); break;
	case 0x5f: sprintf(op, "BIT 7,V.#%02X", getwa()); break;
	
	case 0x60:
		switch(b = getb())
		{
		case 0x08: sprintf(op, "ANA V,A"); break;
		case 0x09: sprintf(op, "ANA2 A,A"); break;
		case 0x0a: sprintf(op, "ANA B,A"); break;
		case 0x0b: sprintf(op, "ANA C,A"); break;
		case 0x0c: sprintf(op, "ANA D,A"); break;
		case 0x0d: sprintf(op, "ANA E,A"); break;
		case 0x0e: sprintf(op, "ANA H,A"); break;
		case 0x0f: sprintf(op, "ANA L,A"); break;
		case 0x10: sprintf(op, "XRA V,A"); break;
		case 0x11: sprintf(op, "XRA2 A,A"); break;
		case 0x12: sprintf(op, "XRA B,A"); break;
		case 0x13: sprintf(op, "XRA C,A"); break;
		case 0x14: sprintf(op, "XRA D,A"); break;
		case 0x15: sprintf(op, "XRA E,A"); break;
		case 0x16: sprintf(op, "XRA H,A"); break;
		case 0x17: sprintf(op, "XRA L,A"); break;
		case 0x18: sprintf(op, "ORA V,A"); break;
		case 0x19: sprintf(op, "ORA2 A,A"); break;
		case 0x1a: sprintf(op, "ORA B,A"); break;
		case 0x1b: sprintf(op, "ORA C,A"); break;
		case 0x1c: sprintf(op, "ORA D,A"); break;
		case 0x1d: sprintf(op, "ORA E,A"); break;
		case 0x1e: sprintf(op, "ORA H,A"); break;
		case 0x1f: sprintf(op, "ORA L,A"); break;
		case 0x20: sprintf(op, "ADDNC V,A"); break;
		case 0x21: sprintf(op, "ADDNC2 A,A"); break;
		case 0x22: sprintf(op, "ADDNC B,A"); break;
		case 0x23: sprintf(op, "ADDNC C,A"); break;
		case 0x24: sprintf(op, "ADDNC D,A"); break;
		case 0x25: sprintf(op, "ADDNC E,A"); break;
		case 0x26: sprintf(op, "ADDNC H,A"); break;
		case 0x27: sprintf(op, "ADDNC L,A"); break;
		case 0x28: sprintf(op, "GTA V,A"); break;
		case 0x29: sprintf(op, "GTA2 A,A"); break;
		case 0x2a: sprintf(op, "GTA B,A"); break;
		case 0x2b: sprintf(op, "GTA C,A"); break;
		case 0x2c: sprintf(op, "GTA D,A"); break;
		case 0x2d: sprintf(op, "GTA E,A"); break;
		case 0x2e: sprintf(op, "GTA H,A"); break;
		case 0x2f: sprintf(op, "GTA L,A"); break;
		case 0x30: sprintf(op, "SUBNB V,A"); break;
		case 0x31: sprintf(op, "SUBNB2 A,A"); break;
		case 0x32: sprintf(op, "SUBNB B,A"); break;
		case 0x33: sprintf(op, "SUBNB C,A"); break;
		case 0x34: sprintf(op, "SUBNB D,A"); break;
		case 0x35: sprintf(op, "SUBNB E,A"); break;
		case 0x36: sprintf(op, "SUBNB H,A"); break;
		case 0x37: sprintf(op, "SUBNB L,A"); break;
		case 0x38: sprintf(op, "LTA V,A"); break;
		case 0x39: sprintf(op, "LTA2 A,A\t"); break;
		case 0x3a: sprintf(op, "LTA B,A"); break;
		case 0x3b: sprintf(op, "LTA C,A"); break;
		case 0x3c: sprintf(op, "LTA D,A"); break;
		case 0x3d: sprintf(op, "LTA E,A"); break;
		case 0x3e: sprintf(op, "LTA H,A"); break;
		case 0x3f: sprintf(op, "LTA L,A"); break;
		case 0x40: sprintf(op, "ADD V,A"); break;
		case 0x41: sprintf(op, "ADD2 A,A"); break;
		case 0x42: sprintf(op, "ADD B,A"); break;
		case 0x43: sprintf(op, "ADD C,A"); break;
		case 0x44: sprintf(op, "ADD D,A"); break;
		case 0x45: sprintf(op, "ADD E,A"); break;
		case 0x46: sprintf(op, "ADD H,A"); break;
		case 0x47: sprintf(op, "ADD L,A"); break;
		case 0x50: sprintf(op, "ADC V,A"); break;
		case 0x51: sprintf(op, "ADC2 A,A"); break;
		case 0x52: sprintf(op, "ADC B,A"); break;
		case 0x53: sprintf(op, "ADC C,A"); break;
		case 0x54: sprintf(op, "ADC D,A"); break;
		case 0x55: sprintf(op, "ADC E,A"); break;
		case 0x56: sprintf(op, "ADC H,A"); break;
		case 0x57: sprintf(op, "ADC L,A"); break;
		case 0x60: sprintf(op, "SUB V,A"); break;
		case 0x61: sprintf(op, "SUB2 A,A"); break;
		case 0x62: sprintf(op, "SUB B,A"); break;
		case 0x63: sprintf(op, "SUB C,A"); break;
		case 0x64: sprintf(op, "SUB D,A"); break;
		case 0x65: sprintf(op, "SUB E,A"); break;
		case 0x66: sprintf(op, "SUB H,A"); break;
		case 0x67: sprintf(op, "SUB L,A"); break;
		case 0x68: sprintf(op, "NEA V,A"); break;
		case 0x69: sprintf(op, "NEA2 A,A"); break;
		case 0x6a: sprintf(op, "NEA B,A"); break;
		case 0x6b: sprintf(op, "NEA C,A"); break;
		case 0x6c: sprintf(op, "NEA D,A"); break;
		case 0x6d: sprintf(op, "NEA E,A"); break;
		case 0x6e: sprintf(op, "NEA H,A"); break;
		case 0x6f: sprintf(op, "NEA L,A"); break;
		case 0x70: sprintf(op, "SBB V,A"); break;
		case 0x71: sprintf(op, "SBB2 A,A"); break;
		case 0x72: sprintf(op, "SBB B,A"); break;
		case 0x73: sprintf(op, "SBB C,A"); break;
		case 0x74: sprintf(op, "SBB D,A"); break;
		case 0x75: sprintf(op, "SBB E,A"); break;
		case 0x76: sprintf(op, "SBB H,A"); break;
		case 0x77: sprintf(op, "SBB L,A"); break;
		case 0x78: sprintf(op, "EQA V,A"); break;
		case 0x79: sprintf(op, "EQA2 A,A"); break;
		case 0x7a: sprintf(op, "EQA B,A"); break;
		case 0x7b: sprintf(op, "EQA C,A"); break;
		case 0x7c: sprintf(op, "EQA D,A"); break;
		case 0x7d: sprintf(op, "EQA E,A"); break;
		case 0x7e: sprintf(op, "EQA H,A"); break;
		case 0x7f: sprintf(op, "EQA L,A"); break;
		case 0x88: sprintf(op, "ANA A,V"); break;
		case 0x89: sprintf(op, "ANA A,A\t; ANA1 A,A"); break;
		case 0x8a: sprintf(op, "ANA A,B"); break;
		case 0x8b: sprintf(op, "ANA A,C"); break;
		case 0x8c: sprintf(op, "ANA A,D"); break;
		case 0x8d: sprintf(op, "ANA A,E"); break;
		case 0x8e: sprintf(op, "ANA A,H"); break;
		case 0x8f: sprintf(op, "ANA A,L"); break;
		case 0x90: sprintf(op, "XRA A,V"); break;
		case 0x91: sprintf(op, "XRA A,A\t; XRA1 A,A"); break;
		case 0x92: sprintf(op, "XRA A,B"); break;
		case 0x93: sprintf(op, "XRA A,C"); break;
		case 0x94: sprintf(op, "XRA A,D"); break;
		case 0x95: sprintf(op, "XRA A,E"); break;
		case 0x96: sprintf(op, "XRA A,H"); break;
		case 0x97: sprintf(op, "XRA A,L"); break;
		case 0x98: sprintf(op, "ORA A,V"); break;
		case 0x99: sprintf(op, "ORA A,A\t; ORA1 A,A"); break;
		case 0x9a: sprintf(op, "ORA A,B"); break;
		case 0x9b: sprintf(op, "ORA A,C"); break;
		case 0x9c: sprintf(op, "ORA A,D"); break;
		case 0x9d: sprintf(op, "ORA A,E"); break;
		case 0x9e: sprintf(op, "ORA A,H"); break;
		case 0x9f: sprintf(op, "ORA A,L"); break;
		case 0xa0: sprintf(op, "ADDNC A,V"); break;
		case 0xa1: sprintf(op, "ADDNC A,A\t; ADDNC1 A,A"); break;
		case 0xa2: sprintf(op, "ADDNC A,B"); break;
		case 0xa3: sprintf(op, "ADDNC A,C"); break;
		case 0xa4: sprintf(op, "ADDNC A,D"); break;
		case 0xa5: sprintf(op, "ADDNC A,E"); break;
		case 0xa6: sprintf(op, "ADDNC A,H"); break;
		case 0xa7: sprintf(op, "ADDNC A,L"); break;
		case 0xa8: sprintf(op, "GTA A,V"); break;
		case 0xa9: sprintf(op, "GTA A,A\t; GTA1 A,A"); break;
		case 0xaa: sprintf(op, "GTA A,B"); break;
		case 0xab: sprintf(op, "GTA A,C"); break;
		case 0xac: sprintf(op, "GTA A,D"); break;
		case 0xad: sprintf(op, "GTA A,E"); break;
		case 0xae: sprintf(op, "GTA A,H"); break;
		case 0xaf: sprintf(op, "GTA A,L"); break;
		case 0xb0: sprintf(op, "SUBNB A,V"); break;
		case 0xb1: sprintf(op, "SUBNB A,A\t; SUBNB1 A,A"); break;
		case 0xb2: sprintf(op, "SUBNB A,B"); break;
		case 0xb3: sprintf(op, "SUBNB A,C"); break;
		case 0xb4: sprintf(op, "SUBNB A,D"); break;
		case 0xb5: sprintf(op, "SUBNB A,E"); break;
		case 0xb6: sprintf(op, "SUBNB A,H"); break;
		case 0xb7: sprintf(op, "SUBNB A,L"); break;
		case 0xb8: sprintf(op, "LTA A,V"); break;
		case 0xb9: sprintf(op, "LTA A,A\t; LTA1 A,A"); break;
		case 0xba: sprintf(op, "LTA A,B"); break;
		case 0xbb: sprintf(op, "LTA A,C"); break;
		case 0xbc: sprintf(op, "LTA A,D"); break;
		case 0xbd: sprintf(op, "LTA A,E"); break;
		case 0xbe: sprintf(op, "LTA A,H"); break;
		case 0xbf: sprintf(op, "LTA A,L"); break;
		case 0xc0: sprintf(op, "ADD A,V"); break;
		case 0xc1: sprintf(op, "ADD A,A\t; ADD1 A,A"); break;
		case 0xc2: sprintf(op, "ADD A,B"); break;
		case 0xc3: sprintf(op, "ADD A,C"); break;
		case 0xc4: sprintf(op, "ADD A,D"); break;
		case 0xc5: sprintf(op, "ADD A,E"); break;
		case 0xc6: sprintf(op, "ADD A,H"); break;
		case 0xc7: sprintf(op, "ADD A,L"); break;
		case 0xc8: sprintf(op, "ONA A,V"); break;
		case 0xc9: sprintf(op, "ONA A,A"); break;
		case 0xca: sprintf(op, "ONA A,B"); break;
		case 0xcb: sprintf(op, "ONA A,C"); break;
		case 0xcc: sprintf(op, "ONA A,D"); break;
		case 0xcd: sprintf(op, "ONA A,E"); break;
		case 0xce: sprintf(op, "ONA A,H"); break;
		case 0xcf: sprintf(op, "ONA A,L"); break;
		case 0xd0: sprintf(op, "ADC A,V"); break;
		case 0xd1: sprintf(op, "ADC A,A\t; ADC1 A,A"); break;
		case 0xd2: sprintf(op, "ADC A,B"); break;
		case 0xd3: sprintf(op, "ADC A,C"); break;
		case 0xd4: sprintf(op, "ADC A,D"); break;
		case 0xd5: sprintf(op, "ADC A,E"); break;
		case 0xd6: sprintf(op, "ADC A,H"); break;
		case 0xd7: sprintf(op, "ADC A,L"); break;
		case 0xd8: sprintf(op, "OFFA A,V"); break;
		case 0xd9: sprintf(op, "OFFA A,A"); break;
		case 0xda: sprintf(op, "OFFA A,B"); break;
		case 0xdb: sprintf(op, "OFFA A,C"); break;
		case 0xdc: sprintf(op, "OFFA A,D"); break;
		case 0xdd: sprintf(op, "OFFA A,E"); break;
		case 0xde: sprintf(op, "OFFA A,H"); break;
		case 0xdf: sprintf(op, "OFFA A,L"); break;
		case 0xe0: sprintf(op, "SUB A,V"); break;
		case 0xe1: sprintf(op, "SUB A,A\t; SUB1 A,A"); break;
		case 0xe2: sprintf(op, "SUB A,B"); break;
		case 0xe3: sprintf(op, "SUB A,C"); break;
		case 0xe4: sprintf(op, "SUB A,D"); break;
		case 0xe5: sprintf(op, "SUB A,E"); break;
		case 0xe6: sprintf(op, "SUB A,H"); break;
		case 0xe7: sprintf(op, "SUB A,L"); break;
		case 0xe8: sprintf(op, "NEA A,V"); break;
		case 0xe9: sprintf(op, "NEA A,A\t; NEA1 A,A"); break;
		case 0xea: sprintf(op, "NEA A,B"); break;
		case 0xeb: sprintf(op, "NEA A,C"); break;
		case 0xec: sprintf(op, "NEA A,D"); break;
		case 0xed: sprintf(op, "NEA A,E"); break;
		case 0xee: sprintf(op, "NEA A,H"); break;
		case 0xef: sprintf(op, "NEA A,L"); break;
		case 0xF0: sprintf(op, "SBB A,V"); break;
		case 0xF1: sprintf(op, "SBB A,A\t; SBB1 A,A"); break;
		case 0xF2: sprintf(op, "SBB A,B"); break;
		case 0xf3: sprintf(op, "SBB A,C"); break;
		case 0xf4: sprintf(op, "SBB A,D"); break;
		case 0xf5: sprintf(op, "SBB A,E"); break;
		case 0xf6: sprintf(op, "SBB A,H"); break;
		case 0xf7: sprintf(op, "SBB A,L"); break;
		case 0xf8: sprintf(op, "EQA A,V"); break;
		case 0xf9: sprintf(op, "EQA A,A\t; EQA1 A,A"); break;
		case 0xfa: sprintf(op, "EQA A,B"); break;
		case 0xfb: sprintf(op, "EQA A,C"); break;
		case 0xfc: sprintf(op, "EQA A,D"); break;
		case 0xfd: sprintf(op, "EQA A,E"); break;
		case 0xfe: sprintf(op, "EQA A,H"); break;
		case 0xff: sprintf(op, "EQA A,L"); break;
		default: sprintf(op, "DB 60h,#%02X", b);
		}
		break;
	case 0x61: sprintf(op, "DAA"); break;
	case 0x62: sprintf(op, "RETI"); break;
	case 0x63: sprintf(op, "CALB"); break;
	case 0x64:
		switch(b = getb())
		{
		case 0x08: sprintf(op, "ANI V,#%02X", getb()); break;
		case 0x09: b = getb(); sprintf(op, "ANI2 A,#%02X\t; Slower than ANI1 A,#%02X", b, b); break;
		case 0x0a: sprintf(op, "ANI B,#%02X", getb()); break;
		case 0x0b: sprintf(op, "ANI C,#%02X", getb()); break;
		case 0x0c: sprintf(op, "ANI D,#%02X", getb()); break;
		case 0x0d: sprintf(op, "ANI E,#%02X", getb()); break;
		case 0x0e: sprintf(op, "ANI H,#%02X", getb()); break;
		case 0x0f: sprintf(op, "ANI L,#%02X", getb()); break;
		case 0x10: sprintf(op, "XRI V,#%02X", getb()); break;
		case 0x11: b = getb(); sprintf(op, "XRI2 A,#%02X\t; Slower than XRI1 A,#%02X", b, b); break;
		case 0x12: sprintf(op, "XRI B,#%02X", getb()); break;
		case 0x13: sprintf(op, "XRI C,#%02X", getb()); break;
		case 0x14: sprintf(op, "XRI D,#%02X", getb()); break;
		case 0x15: sprintf(op, "XRI E,#%02X", getb()); break;
		case 0x16: sprintf(op, "XRI H,#%02X", getb()); break;
		case 0x17: sprintf(op, "XRI L,#%02X", getb()); break;
		case 0x18: sprintf(op, "ORI V,#%02X", getb()); break;
		case 0x19: b = getb(); sprintf(op, "ORI2 A,#%02X\t; Slower than ORI1 A,#%02X", b, b); break;
		case 0x1a: sprintf(op, "ORI B,#%02X", getb()); break;
		case 0x1b: sprintf(op, "ORI C,#%02X", getb()); break;
		case 0x1c: sprintf(op, "ORI D,#%02X", getb()); break;
		case 0x1d: sprintf(op, "ORI E,#%02X", getb()); break;
		case 0x1e: sprintf(op, "ORI H,#%02X", getb()); break;
		case 0x1f: sprintf(op, "ORI L,#%02X", getb()); break;
		case 0x20: sprintf(op, "ADINC V,#%02X", getb()); break;
		case 0x21: b = getb(); sprintf(op, "ADINC2 A,#%02X\t; Slower than ADINC1 A,#%02X", b, b); break;
		case 0x22: sprintf(op, "ADINC B,#%02X", getb()); break;
		case 0x23: sprintf(op, "ADINC C,#%02X", getb()); break;
		case 0x24: sprintf(op, "ADINC D,#%02X", getb()); break;
		case 0x25: sprintf(op, "ADINC E,#%02X", getb()); break;
		case 0x26: sprintf(op, "ADINC H,#%02X", getb()); break;
		case 0x27: sprintf(op, "ADINC L,#%02X", getb()); break;
		case 0x28: sprintf(op, "GTI V,#%02X", getb()); break;
		case 0x29: b = getb(); sprintf(op, "GTI2 A,#%02X\t; Slower than GTI1 A,#%02X", b, b); break;
		case 0x2a: sprintf(op, "GTI B,#%02X", getb()); break;
		case 0x2b: sprintf(op, "GTI C,#%02X", getb()); break;
		case 0x2c: sprintf(op, "GTI D,#%02X", getb()); break;
		case 0x2d: sprintf(op, "GTI E,#%02X", getb()); break;
		case 0x2e: sprintf(op, "GTI H,#%02X", getb()); break;
		case 0x2f: sprintf(op, "GTI L,#%02X", getb()); break;
		case 0x30: sprintf(op, "SUINB V,#%02X", getb()); break;
		case 0x31: b = getb(); sprintf(op, "SUINB2 A,#%02X\t; Slower than SUINB1 A,#%02X", b, b); break;
		case 0x32: sprintf(op, "SUINB B,#%02X", getb()); break;
		case 0x33: sprintf(op, "SUINB C,#%02X", getb()); break;
		case 0x34: sprintf(op, "SUINB D,#%02X", getb()); break;
		case 0x35: sprintf(op, "SUINB E,#%02X", getb()); break;
		case 0x36: sprintf(op, "SUINB H,#%02X", getb()); break;
		case 0x37: sprintf(op, "SUINB L,#%02X", getb()); break;
		case 0x38: sprintf(op, "LTI V,#%02X", getb()); break;
		case 0x39: b = getb(); sprintf(op, "LTI2 A,#%02X\t; Slower than LTI1 A,#%02X", b, b); break;
		case 0x3a: sprintf(op, "LTI B,#%02X", getb()); break;
		case 0x3b: sprintf(op, "LTI C,#%02X", getb()); break;
		case 0x3c: sprintf(op, "LTI D,#%02X", getb()); break;
		case 0x3d: sprintf(op, "LTI E,#%02X", getb()); break;
		case 0x3e: sprintf(op, "LTI H,#%02X", getb()); break;
		case 0x3f: sprintf(op, "LTI L,#%02X", getb()); break;
		case 0x40: sprintf(op, "ADI V,#%02X", getb()); break;
		case 0x41: b = getb(); sprintf(op, "ADI2 A,#%02X\t; Slower than ADI1 A,#%02X", b, b); break;
		case 0x42: sprintf(op, "ADI B,#%02X", getb()); break;
		case 0x43: sprintf(op, "ADI C,#%02X", getb()); break;
		case 0x44: sprintf(op, "ADI D,#%02X", getb()); break;
		case 0x45: sprintf(op, "ADI E,#%02X", getb()); break;
		case 0x46: sprintf(op, "ADI H,#%02X", getb()); break;
		case 0x47: sprintf(op, "ADI L,#%02X", getb()); break;
		case 0x48: sprintf(op, "ONI V,#%02X", getb()); break;
		case 0x49: b = getb(); sprintf(op, "ONI2 A,#%02X\t; Slower than ONI1 A,#%02X", b, b); break;
		case 0x4a: sprintf(op, "ONI B,#%02X", getb()); break;
		case 0x4b: sprintf(op, "ONI C,#%02X", getb()); break;
		case 0x4c: sprintf(op, "ONI D,#%02X", getb()); break;
		case 0x4d: sprintf(op, "ONI E,#%02X", getb()); break;
		case 0x4e: sprintf(op, "ONI H,#%02X", getb()); break;
		case 0x4f: sprintf(op, "ONI L,#%02X", getb()); break;
		case 0x50: sprintf(op, "ACI V,#%02X", getb()); break;
		case 0x51: b = getb(); sprintf(op, "ACI2 A,#%02X\t; Slower than ACI1 A,#%02X", b, b); break;
		case 0x52: sprintf(op, "ACI B,#%02X", getb()); break;
		case 0x53: sprintf(op, "ACI C,#%02X", getb()); break;
		case 0x54: sprintf(op, "ACI D,#%02X", getb()); break;
		case 0x55: sprintf(op, "ACI E,#%02X", getb()); break;
		case 0x56: sprintf(op, "ACI H,#%02X", getb()); break;
		case 0x57: sprintf(op, "ACI L,#%02X", getb()); break;
		case 0x58: sprintf(op, "OFFI V,#%02X", getb()); break;
		case 0x59: b = getb(); sprintf(op, "OFFI2 A,#%02X\t; Slower than OFFI1 A,#%02X", b, b); break;
		case 0x5a: sprintf(op, "OFFI B,#%02X", getb()); break;
		case 0x5b: sprintf(op, "OFFI C,#%02X", getb()); break;
		case 0x5c: sprintf(op, "OFFI D,#%02X", getb()); break;
		case 0x5d: sprintf(op, "OFFI E,#%02X", getb()); break;
		case 0x5e: sprintf(op, "OFFI H,#%02X", getb()); break;
		case 0x5f: sprintf(op, "OFFI L,#%02X", getb()); break;
		case 0x60: sprintf(op, "SUI V,#%02X", getb()); break;
		case 0x61: b = getb(); sprintf(op, "SUI2 A,#%02X\t; Slower than SUI1 A,#%02X", b, b); break;
		case 0x62: sprintf(op, "SUI B,#%02X", getb()); break;
		case 0x63: sprintf(op, "SUI C,#%02X", getb()); break;
		case 0x64: sprintf(op, "SUI D,#%02X", getb()); break;
		case 0x65: sprintf(op, "SUI E,#%02X", getb()); break;
		case 0x66: sprintf(op, "SUI H,#%02X", getb()); break;
		case 0x67: sprintf(op, "SUI L,#%02X", getb()); break;
		case 0x68: sprintf(op, "NEI V,#%02X", getb()); break;
		case 0x69: b = getb(); sprintf(op, "NEI2 A,#%02X\t; Slower than NEI1 A,#%02X", b, b); break;
		case 0x6a: sprintf(op, "NEI B,#%02X", getb()); break;
		case 0x6b: sprintf(op, "NEI C,#%02X", getb()); break;
		case 0x6c: sprintf(op, "NEI D,#%02X", getb()); break;
		case 0x6d: sprintf(op, "NEI E,#%02X", getb()); break;
		case 0x6e: sprintf(op, "NEI H,#%02X", getb()); break;
		case 0x6f: sprintf(op, "NEI L,#%02X", getb()); break;
		case 0x70: sprintf(op, "SBI V,#%02X", getb()); break;
		case 0x71: b = getb(); sprintf(op, "SBI2 A,#%02X\t; Slower than SBI1 A,#%02X", b, b); break;
		case 0x72: sprintf(op, "SBI B,#%02X", getb()); break;
		case 0x73: sprintf(op, "SBI C,#%02X", getb()); break;
		case 0x74: sprintf(op, "SBI D,#%02X", getb()); break;
		case 0x75: sprintf(op, "SBI E,#%02X", getb()); break;
		case 0x76: sprintf(op, "SBI H,#%02X", getb()); break;
		case 0x77: sprintf(op, "SBI L,#%02X", getb()); break;
		case 0x78: sprintf(op, "EQI V,#%02X", getb()); break;
		case 0x79: b = getb(); sprintf(op, "EQI2 A,#%02X\t; Slower than EQI1 A,#%02X", b, b); break;
		case 0x7a: sprintf(op, "EQI B,#%02X", getb()); break;
		case 0x7b: sprintf(op, "EQI C,#%02X", getb()); break;
		case 0x7c: sprintf(op, "EQI D,#%02X", getb()); break;
		case 0x7d: sprintf(op, "EQI E,#%02X", getb()); break;
		case 0x7e: sprintf(op, "EQI H,#%02X", getb()); break;
		case 0x7f: sprintf(op, "EQI L,#%02X", getb()); break;
		case 0x88: sprintf(op, "ANI PA,#%02X", getb()); break;
		case 0x89: sprintf(op, "ANI PB,#%02X", getb()); break;
		case 0x8a: sprintf(op, "ANI PC,#%02X", getb()); break;
		case 0x8b: sprintf(op, "ANI MK,#%02X", getb()); break;
		case 0x90: sprintf(op, "XRI PA,#%02X", getb()); break;
		case 0x91: sprintf(op, "XRI PB,#%02X", getb()); break;
		case 0x92: sprintf(op, "XRI PC,#%02X", getb()); break;
		case 0x93: sprintf(op, "XRI MK,#%02X", getb()); break;
		case 0x98: sprintf(op, "ORI PA,#%02X", getb()); break;
		case 0x99: sprintf(op, "ORI PB,#%02X", getb()); break;
		case 0x9a: sprintf(op, "ORI PC,#%02X", getb()); break;
		case 0x9b: sprintf(op, "ORI MK,#%02X", getb()); break;
		case 0xa0: sprintf(op, "ADINC PA,#%02X", getb()); break;
		case 0xa1: sprintf(op, "ADINC PB,#%02X", getb()); break;
		case 0xa2: sprintf(op, "ADINC PC,#%02X", getb()); break;
		case 0xa3: sprintf(op, "ADINC MK,#%02X", getb()); break;
		case 0xa8: sprintf(op, "GTI PA,#%02X", getb()); break;
		case 0xa9: sprintf(op, "GTI PB,#%02X", getb()); break;
		case 0xaa: sprintf(op, "GTI PC,#%02X", getb()); break;
		case 0xab: sprintf(op, "GTI MK,#%02X", getb()); break;
		case 0xb0: sprintf(op, "SUINB PA,#%02X", getb()); break;
		case 0xb1: sprintf(op, "SUINB PB,#%02X", getb()); break;
		case 0xb2: sprintf(op, "SUINB PC,#%02X", getb()); break;
		case 0xb3: sprintf(op, "SUINB MK,#%02X", getb()); break;
		case 0xb8: sprintf(op, "LTI PA,#%02X", getb()); break;
		case 0xb9: sprintf(op, "LTI PB,#%02X", getb()); break;
		case 0xba: sprintf(op, "LTI PC,#%02X", getb()); break;
		case 0xbb: sprintf(op, "LTI MK,#%02X", getb()); break;
		case 0xc0: sprintf(op, "ADI PA,#%02X", getb()); break;
		case 0xc1: sprintf(op, "ADI PB,#%02X", getb()); break;
		case 0xc2: sprintf(op, "ADI PC,#%02X", getb()); break;
		case 0xc3: sprintf(op, "ADI MK,#%02X", getb()); break;
		case 0xc8: sprintf(op, "ONI PA,#%02X", getb()); break;
		case 0xc9: sprintf(op, "ONI PB,#%02X", getb()); break;
		case 0xca: sprintf(op, "ONI PC,#%02X", getb()); break;
		case 0xcb: sprintf(op, "ONI MK,#%02X", getb()); break;
		case 0xd0: sprintf(op, "ACI PA,#%02X", getb()); break;
		case 0xd1: sprintf(op, "ACI PB,#%02X", getb()); break;
		case 0xd2: sprintf(op, "ACI PC,#%02X", getb()); break;
		case 0xd3: sprintf(op, "ACI MK,#%02X", getb()); break;
		case 0xd8: sprintf(op, "OFFI PA,#%02X", getb()); break;
		case 0xd9: sprintf(op, "OFFI PB,#%02X", getb()); break;
		case 0xda: sprintf(op, "OFFI PC,#%02X", getb()); break;
		case 0xdb: sprintf(op, "OFFI MK,#%02X", getb()); break;
		case 0xe0: sprintf(op, "SUI PA,#%02X", getb()); break;
		case 0xe1: sprintf(op, "SUI PB,#%02X", getb()); break;
		case 0xe2: sprintf(op, "SUI PC,#%02X", getb()); break;
		case 0xe3: sprintf(op, "SUI MK,#%02X", getb()); break;
		case 0xe8: sprintf(op, "NEI PA,#%02X", getb()); break;
		case 0xe9: sprintf(op, "NEI PB,#%02X", getb()); break;
		case 0xea: sprintf(op, "NEI PC,#%02X", getb()); break;
		case 0xeb: sprintf(op, "NEI MK,#%02X", getb()); break;
		case 0xF0: sprintf(op, "SBI PA,#%02X", getb()); break;
		case 0xF1: sprintf(op, "SBI PB,#%02X", getb()); break;
		case 0xF2: sprintf(op, "SBI PC,#%02X", getb()); break;
		case 0xf3: sprintf(op, "SBI MK,#%02X", getb()); break;
		case 0xf8: sprintf(op, "EQI PA,#%02X", getb()); break;
		case 0xf9: sprintf(op, "EQI PB,#%02X", getb()); break;
		case 0xfa: sprintf(op, "EQI PC,#%02X", getb()); break;
		case 0xfb: sprintf(op, "EQI MK,#%02X", getb()); break;
		default: sprintf(op, "DB 64h,#%02X", b);
		}
		break;
	case 0x65: wa = getwa(); sprintf(op, "NEIW V.#%02X,#%02X", wa, getb()); break;
	case 0x66: b = getb(); sprintf(op, "SUI A,#%02X\t; SUI1 A,#%02X", b, b); break;
	case 0x67: b = getb(); sprintf(op, "NEI A,#%02X\t; NEI1 A,#%02X", b, b); break;
	case 0x68: sprintf(op, "MVI V,#%02X", getb()); break;
	case 0x69: sprintf(op, "MVI A,#%02X", getb()); break;
	case 0x6a: sprintf(op, "MVI B,#%02X", getb()); break;
	case 0x6b: sprintf(op, "MVI C,#%02X", getb()); break;
	case 0x6c: sprintf(op, "MVI D,#%02X", getb()); break;
	case 0x6d: sprintf(op, "MVI E,#%02X", getb()); break;
	case 0x6e: sprintf(op, "MVI H,#%02X", getb()); break;
	case 0x6f: sprintf(op, "MVI L,#%02X", getb()); break;
	
	case 0x70:
		switch(b = getb())
		{
		case 0x0e: sprintf(op, "SSPD #%04X", getw()); break;
		case 0x0f: sprintf(op, "LSPD #%04X", getw()); break;
		case 0x1e: sprintf(op, "SBCD #%04X", getw()); break;
		case 0x1f: sprintf(op, "LBCD #%04X", getw()); break;
		case 0x2e: sprintf(op, "SDED #%04X", getw()); break;
		case 0x2f: sprintf(op, "LDED #%04X", getw()); break;
		case 0x3e: sprintf(op, "SHLD #%04X", getw()); break;
		case 0x3f: sprintf(op, "LHLD #%04X", getw()); break;
		case 0x68: sprintf(op, "MOV V,#%04X", getw()); break;
		case 0x69: sprintf(op, "MOV A,#%04X", getw()); break;
		case 0x6a: sprintf(op, "MOV B,#%04X", getw()); break;
		case 0x6b: sprintf(op, "MOV C,#%04X", getw()); break;
		case 0x6c: sprintf(op, "MOV D,#%04X", getw()); break;
		case 0x6d: sprintf(op, "MOV E,#%04X", getw()); break;
		case 0x6e: sprintf(op, "MOV H,#%04X", getw()); break;
		case 0x6f: sprintf(op, "MOV L,#%04X", getw()); break;
		case 0x78: sprintf(op, "MOV #%04X,V", getw()); break;
		case 0x79: sprintf(op, "MOV #%04X,A", getw()); break;
		case 0x7a: sprintf(op, "MOV #%04X,B", getw()); break;
		case 0x7b: sprintf(op, "MOV #%04X,C", getw()); break;
		case 0x7c: sprintf(op, "MOV #%04X,D", getw()); break;
		case 0x7d: sprintf(op, "MOV #%04X,E", getw()); break;
		case 0x7e: sprintf(op, "MOV #%04X,H", getw()); break;
		case 0x7f: sprintf(op, "MOV #%04X,L", getw()); break;
		case 0x89: sprintf(op, "ANAX B"); break;
		case 0x8a: sprintf(op, "ANAX D"); break;
		case 0x8b: sprintf(op, "ANAX H"); break;
		case 0x8c: sprintf(op, "ANAX D+"); break;
		case 0x8d: sprintf(op, "ANAX H+"); break;
		case 0x8e: sprintf(op, "ANAX D-"); break;
		case 0x8f: sprintf(op, "ANAX H-"); break;
		case 0x91: sprintf(op, "XRAX B"); break;
		case 0x92: sprintf(op, "XRAX D"); break;
		case 0x93: sprintf(op, "XRAX H"); break;
		case 0x94: sprintf(op, "XRAX D+"); break;
		case 0x95: sprintf(op, "XRAX H+"); break;
		case 0x96: sprintf(op, "XRAX D-"); break;
		case 0x97: sprintf(op, "XRAX H-"); break;
		case 0x99: sprintf(op, "ORAX B"); break;
		case 0x9a: sprintf(op, "ORAX D"); break;
		case 0x9b: sprintf(op, "ORAX H"); break;
		case 0x9c: sprintf(op, "ORAX D+"); break;
		case 0x9d: sprintf(op, "ORAX H+"); break;
		case 0x9e: sprintf(op, "ORAX D-"); break;
		case 0x9f: sprintf(op, "ORAX H-"); break;
		case 0xa1: sprintf(op, "ADDNCX B"); break;
		case 0xa2: sprintf(op, "ADDNCX D"); break;
		case 0xa3: sprintf(op, "ADDNCX H"); break;
		case 0xa4: sprintf(op, "ADDNCX D+"); break;
		case 0xa5: sprintf(op, "ADDNCX H+"); break;
		case 0xa6: sprintf(op, "ADDNCX D-"); break;
		case 0xa7: sprintf(op, "ADDNCX H-"); break;
		case 0xa9: sprintf(op, "GTAX B"); break;
		case 0xaa: sprintf(op, "GTAX D"); break;
		case 0xab: sprintf(op, "GTAX H"); break;
		case 0xac: sprintf(op, "GTAX D+"); break;
		case 0xad: sprintf(op, "GTAX H+"); break;
		case 0xae: sprintf(op, "GTAX D-"); break;
		case 0xaf: sprintf(op, "GTAX H-"); break;
		case 0xb1: sprintf(op, "SUBNBX B"); break;
		case 0xb2: sprintf(op, "SUBNBX D"); break;
		case 0xb3: sprintf(op, "SUBNBX H"); break;
		case 0xb4: sprintf(op, "SUBNBX D+"); break;
		case 0xb5: sprintf(op, "SUBNBX H+"); break;
		case 0xb6: sprintf(op, "SUBNBX D-"); break;
		case 0xb7: sprintf(op, "SUBNBX H-"); break;
		case 0xb9: sprintf(op, "LTAX B"); break;
		case 0xba: sprintf(op, "LTAX D"); break;
		case 0xbb: sprintf(op, "LTAX H"); break;
		case 0xbc: sprintf(op, "LTAX D+"); break;
		case 0xbd: sprintf(op, "LTAX H+"); break;
		case 0xbe: sprintf(op, "LTAX D-"); break;
		case 0xbf: sprintf(op, "LTAX H-"); break;
		case 0xc1: sprintf(op, "ADDX B"); break;
		case 0xc2: sprintf(op, "ADDX D"); break;
		case 0xc3: sprintf(op, "ADDX H"); break;
		case 0xc4: sprintf(op, "ADDX D+"); break;
		case 0xc5: sprintf(op, "ADDX H+"); break;
		case 0xc6: sprintf(op, "ADDX D-"); break;
		case 0xc7: sprintf(op, "ADDX H-"); break;
		case 0xc9: sprintf(op, "ONAX B"); break;
		case 0xca: sprintf(op, "ONAX D"); break;
		case 0xcb: sprintf(op, "ONAX H"); break;
		case 0xcc: sprintf(op, "ONAX D+"); break;
		case 0xcd: sprintf(op, "ONAX H+"); break;
		case 0xce: sprintf(op, "ONAX D-"); break;
		case 0xcf: sprintf(op, "ONAX H-"); break;
		case 0xd1: sprintf(op, "ADCX B"); break;
		case 0xd2: sprintf(op, "ADCX D"); break;
		case 0xd3: sprintf(op, "ADCX H"); break;
		case 0xd4: sprintf(op, "ADCX D+"); break;
		case 0xd5: sprintf(op, "ADCX H+"); break;
		case 0xd6: sprintf(op, "ADCX D-"); break;
		case 0xd7: sprintf(op, "ADCX H-"); break;
		case 0xd9: sprintf(op, "OFFAX B"); break;
		case 0xda: sprintf(op, "OFFAX D"); break;
		case 0xdb: sprintf(op, "OFFAX H"); break;
		case 0xdc: sprintf(op, "OFFAX D+"); break;
		case 0xdd: sprintf(op, "OFFAX H+"); break;
		case 0xde: sprintf(op, "OFFAX D-"); break;
		case 0xdf: sprintf(op, "OFFAX H-"); break;
		case 0xe1: sprintf(op, "SUBX B"); break;
		case 0xe2: sprintf(op, "SUBX D"); break;
		case 0xe3: sprintf(op, "SUBX H"); break;
		case 0xe4: sprintf(op, "SUBX D+"); break;
		case 0xe5: sprintf(op, "SUBX H+"); break;
		case 0xe6: sprintf(op, "SUBX D-"); break;
		case 0xe7: sprintf(op, "SUBX H-"); break;
		case 0xe9: sprintf(op, "NEAX B"); break;
		case 0xea: sprintf(op, "NEAX D"); break;
		case 0xeb: sprintf(op, "NEAX H"); break;
		case 0xec: sprintf(op, "NEAX D+"); break;
		case 0xed: sprintf(op, "NEAX H+"); break;
		case 0xee: sprintf(op, "NEAX D-"); break;
		case 0xef: sprintf(op, "NEAX H-"); break;
		case 0xF1: sprintf(op, "SBBX B"); break;
		case 0xF2: sprintf(op, "SBBX D"); break;
		case 0xf3: sprintf(op, "SBBX H"); break;
		case 0xf4: sprintf(op, "SBBX D+"); break;
		case 0xf5: sprintf(op, "SBBX H+"); break;
		case 0xf6: sprintf(op, "SBBX D-"); break;
		case 0xf7: sprintf(op, "SBBX H-"); break;
		case 0xf9: sprintf(op, "EQAX B"); break;
		case 0xfa: sprintf(op, "EQAX D"); break;
		case 0xfb: sprintf(op, "EQAX H"); break;
		case 0xfc: sprintf(op, "EQAX D+"); break;
		case 0xfd: sprintf(op, "EQAX H+"); break;
		case 0xfe: sprintf(op, "EQAX D-"); break;
		case 0xff: sprintf(op, "EQAX H-"); break;
		default: sprintf(op, "DB 70h,#%02X", b);
		}
		break;
	case 0x71: wa = getwa(); sprintf(op, "MVIW V.#%02X,#%02X", wa, getb()); break;
	case 0x72: sprintf(op, "SOFTI"); break;
	case 0x73: sprintf(op, "JB"); break;
	case 0x74:
		switch(b = getb())
		{
		case 0x88: sprintf(op, "ANAW V.#%02X", getwa()); break;
		case 0x90: sprintf(op, "XRAW V.#%02X", getwa()); break;
		case 0x98: sprintf(op, "ORAW V.#%02X", getwa()); break;
		case 0xa0: sprintf(op, "ADDNCW V.#%02X", getwa()); break;
		case 0xa8: sprintf(op, "GTAW V.#%02X", getwa()); break;
		case 0xb0: sprintf(op, "SUBNBW V.#%02X", getwa()); break;
		case 0xb8: sprintf(op, "LTAW V.#%02X", getwa()); break;
		case 0xc0: sprintf(op, "ADDW V.#%02X", getwa()); break;
		case 0xc8: sprintf(op, "ONAW V.#%02X", getwa()); break;
		case 0xd0: sprintf(op, "ADCW V.#%02X", getwa()); break;
		case 0xd8: sprintf(op, "OFFAW V.#%02X", getwa()); break;
		case 0xe0: sprintf(op, "SUBW V.#%02X", getwa()); break;
		case 0xe8: sprintf(op, "NEAW V.#%02X", getwa()); break;
		case 0xF0: sprintf(op, "SBBW V.#%02X", getwa()); break;
		case 0xf8: sprintf(op, "EQAW V.#%02X", getwa()); break;
		default: sprintf(op, "DB 74h,#%02X", b);
		}
		break;
	case 0x75: wa = getwa(); sprintf(op, "EQIW V.#%02X,#%02X", wa, getb()); break;
	case 0x76: b= getb(); sprintf(op, "SBI A,#%02X\t; SBI1 A,#%02X", b, b); break;
	case 0x77: b= getb(); sprintf(op, "EQI A,#%02X\t; EQI1 A,#%02X", b, b); break;
	case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7e: case 0x7f:
		sprintf(op, "CALF #%03X", 0x800 | ((b & 7) << 8) | getb()); break;
	
	case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87:
	case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8e: case 0x8f:
	case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
	case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9e: case 0x9f:
	case 0xa0: case 0xa1: case 0xa2: case 0xa3: case 0xa4: case 0xa5: case 0xa6: case 0xa7:
	case 0xa8: case 0xa9: case 0xaa: case 0xab: case 0xac: case 0xad: case 0xae: case 0xaf:
	case 0xb0: case 0xb1: case 0xb2: case 0xb3: case 0xb4: case 0xb5: case 0xb6: case 0xb7:
	case 0xb8: case 0xb9: case 0xba: case 0xbb: case 0xbc: case 0xbd: case 0xbe: case 0xbf:
		sprintf(op, "CALT #%02X", 0x80 | ((b & 0x3f) << 1)); break;
		
	case 0xc0: case 0xc1: case 0xc2: case 0xc3: case 0xc4: case 0xc5: case 0xc6: case 0xc7:
	case 0xc8: case 0xc9: case 0xca: case 0xcb: case 0xcc: case 0xcd: case 0xce: case 0xcf:
	case 0xd0: case 0xd1: case 0xd2: case 0xd3: case 0xd4: case 0xd5: case 0xd6: case 0xd7:
	case 0xd8: case 0xd9: case 0xda: case 0xdb: case 0xdc: case 0xdd: case 0xde: case 0xdf:
		sprintf(op, "JR #%04X", pc + (b & 0x1f)); break;
	
	case 0xe0: case 0xe1: case 0xe2: case 0xe3: case 0xe4: case 0xe5: case 0xe6: case 0xe7:
	case 0xe8: case 0xe9: case 0xea: case 0xeb: case 0xec: case 0xed: case 0xee: case 0xef:
	case 0xF0: case 0xF1: case 0xF2: case 0xf3: case 0xf4: case 0xf5: case 0xf6: case 0xf7:
	case 0xf8: case 0xf9: case 0xfa: case 0xfb: case 0xfc: case 0xfd: case 0xfe: case 0xff:
		sprintf(op, "JR #%04X", pc + ((b & 0x1f) - 0x20)); break;
	
	default: sprintf(op, "DB #%02X", b); break;
	}
	
	if(pc - prv == 4)
		sprintf(out, "%04X\t%02X,%02X,%02X,%02X\t%c%c%c%c\t%s", prv, b0,b1,b2,b3, asc(b0),asc(b1),asc(b2),asc(b3), op);
	else if(pc - prv == 3)
		sprintf(out, "%04X\t%02X,%02X,%2X   \t%c%c%c\t%s", prv, b0,b1,b2, asc(b0),asc(b1),asc(b2), op);
	else if(pc - prv == 2)
		sprintf(out, "%04X\t%02X,%2X      \t%c%c\t%s", prv, b0,b1, asc(b0),asc(b1), op);
	else
		sprintf(out, "%04X\t%2X         \t%c\t%s", prv, b0, asc(b0), op);
}

int htoi(char src[])
{
	int len = strlen(src), val = 0;
	for(int i = 0; i < len; i++) {
		if('0' <= src[i] && src[i] <= '9')
			val = (val << 4) | (src[i] - '0');
		else if('A' <= src[i] && src[i] <= 'F')
			val = (val << 4) | (src[i] - 'A' + 0xa);
		else if('a' <= src[i] && src[i] <= 'f')
			val = (val << 4) | (src[i] - 'a' + 0xa);
	}
	return val;
}

int main(int argc, char *argv[])
{
	int org = 0x8000, start = 0x8001, last = 0xffff;
	
	if(argc < 2) {
		printf("usage: dasm7801(.exe) (srcfile) [o????] [s????] [l????]\n");
		printf("\to : top ADDress of srcfile (0000-ffff, default=8000)\n");
		printf("\ts : start ADDress to be disassembled (0000-ffff, default=8001)\n");
		printf("\tl : last ADDress to be disassembled (0000-ffff, default=ffff)\n"); 
		return 0;
	}
	for(int i = 2; i < argc; i++) {
		// get option
		if(argv[i][0] == 'O' || argv[i][0] == 'o')
			org = htoi(&argv[i][1]) & 0xffff;
		else if(argv[i][0] == 'S' || argv[i][0] == 's')
			start = htoi(&argv[i][1]) & 0xffff;
		else if(argv[i][0] == 'L' || argv[i][0] == 'l')
			last = htoi(&argv[i][1]) & 0xffff;
	}
	
	memset(src, 0, sizeof(src));
	FILE* fp = fopen(argv[1], "rb");
	fread(&src[org], 0x10000 - org, 1, fp);
	fclose(fp);
	
	for(pc = start; pc <= last;) {
		char out[64];
		dasm(out);
		printf("%s\n", out);
	}
	
	return 0;
}
