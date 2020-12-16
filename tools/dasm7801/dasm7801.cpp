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
	case 0x00: sprintf(op, "nop"); break;
	case 0x01: sprintf(op, "hlt"); break;
	case 0x02: sprintf(op, "inx sp"); break;
	case 0x03: sprintf(op, "dcx sp"); break;
	case 0x04: sprintf(op, "lxi sp,%4xh", getw()); break;
	case 0x05: wa = getwa(); sprintf(op, "aniw v.%xh,%2xh", wa, getb()); break;
//	case 0x06:
	case 0x07: sprintf(op, "ani a,%2xh", getb()); break;
	case 0x08: sprintf(op, "ret"); break;
	case 0x09: sprintf(op, "sio"); break;
	case 0x0a: sprintf(op, "mov a,b"); break;
	case 0x0b: sprintf(op, "mov a,c"); break;
	case 0x0c: sprintf(op, "mov a,d"); break;
	case 0x0d: sprintf(op, "mov a,e"); break;
	case 0x0e: sprintf(op, "mov a,h"); break;
	case 0x0f: sprintf(op, "mov a,l"); break;
	
	case 0x10: sprintf(op, "ex"); break;
	case 0x11: sprintf(op, "exx"); break;
	case 0x12: sprintf(op, "inx b"); break;
	case 0x13: sprintf(op, "dcx b"); break;
	case 0x14: sprintf(op, "lxi b,%4xh", getw()); break;
	case 0x15: wa = getwa(); sprintf(op, "oriw v.%xh,%2xh", wa, getb()); break;
	case 0x16: sprintf(op, "xri a,%2xh", getb()); break;
	case 0x17: sprintf(op, "ori a,%2xh", getb()); break;
	case 0x18: sprintf(op, "rets"); break;
	case 0x19: sprintf(op, "stm"); break;
	case 0x1a: sprintf(op, "mov b,a"); break;
	case 0x1b: sprintf(op, "mov c,a"); break;
	case 0x1c: sprintf(op, "mov d,a"); break;
	case 0x1d: sprintf(op, "mov e,a"); break;
	case 0x1e: sprintf(op, "mov h,a"); break;
	case 0x1f: sprintf(op, "mov l,a"); break;
	
	case 0x20: sprintf(op, "inrw v.%xh", getwa()); break;
	case 0x21: sprintf(op, "table"); break;
	case 0x22: sprintf(op, "inx d"); break;
	case 0x23: sprintf(op, "dcx d"); break;
	case 0x24: sprintf(op, "lxi d,%4xh", getw()); break;
	case 0x25: wa = getwa(); sprintf(op, "gtiw v.%xh,%2xh", wa, getb()); break;
	case 0x26: sprintf(op, "adinc a,%2xh", getb()); break;
	case 0x27: sprintf(op, "gti a,%2xh", getb()); break;
	case 0x28: sprintf(op, "ldaw v.%xh", getwa()); break;
	case 0x29: sprintf(op, "ldax b"); break;
	case 0x2a: sprintf(op, "ldax d"); break;
	case 0x2b: sprintf(op, "ldax h"); break;
	case 0x2c: sprintf(op, "ldax d+"); break;
	case 0x2d: sprintf(op, "ldax h+"); break;
	case 0x2e: sprintf(op, "ldax d-"); break;
	case 0x2f: sprintf(op, "ldax h-"); break;
	
	case 0x30: sprintf(op, "dcrw v.%xh", getwa()); break;
	case 0x31: sprintf(op, "block"); break;
	case 0x32: sprintf(op, "inx h"); break;
	case 0x33: sprintf(op, "dcx h"); break;
	case 0x34: sprintf(op, "lxi h,%4xh", getw()); break;
	case 0x35: wa = getwa(); sprintf(op, "ltiw v.%xh,%2xh", wa, getb()); break;
	case 0x36: sprintf(op, "suinb a,%2xh", getb()); break;
	case 0x37: sprintf(op, "lti a,%2xh", getb()); break;
	case 0x38: sprintf(op, "staw v.%xh", getwa()); break;
	case 0x39: sprintf(op, "stax b"); break;
	case 0x3a: sprintf(op, "stax d"); break;
	case 0x3b: sprintf(op, "stax h"); break;
	case 0x3c: sprintf(op, "stax d+"); break;
	case 0x3d: sprintf(op, "stax h+"); break;
	case 0x3e: sprintf(op, "stax d-"); break;
	case 0x3f: sprintf(op, "stax h-"); break;
	
//	case 0x40:
	case 0x41: sprintf(op, "inr a"); break;
	case 0x42: sprintf(op, "inr b"); break;
	case 0x43: sprintf(op, "inr c"); break;
	case 0x44: sprintf(op, "call %4xh", getw()); break;
	case 0x45: wa = getwa(); sprintf(op, "oniw v.%xh,%2xh", wa, getb()); break;
	case 0x46: sprintf(op, "adi a,%2xh", getb()); break;
	case 0x47: sprintf(op, "oni a,%2xh", getb()); break;
	case 0x48:
		switch(b = getb())
		{
		case 0x00: sprintf(op, "skit intf0"); break;
		case 0x01: sprintf(op, "skit intft"); break;
		case 0x02: sprintf(op, "skit intf1"); break;
		case 0x03: sprintf(op, "skit intf2"); break;
		case 0x04: sprintf(op, "skit intfs"); break;
		case 0x0a: sprintf(op, "sk cy"); break;
		case 0x0c: sprintf(op, "sk z"); break;
		case 0x0e: sprintf(op, "push v"); break;
		case 0x0f: sprintf(op, "pop v"); break;
		case 0x10: sprintf(op, "sknit f0"); break;
		case 0x11: sprintf(op, "sknit ft"); break;
		case 0x12: sprintf(op, "sknit f1"); break;
		case 0x13: sprintf(op, "sknit f2"); break;
		case 0x14: sprintf(op, "sknit fs"); break;
		case 0x1a: sprintf(op, "skn cy"); break;
		case 0x1c: sprintf(op, "skn z"); break;
		case 0x1e: sprintf(op, "push b"); break;
		case 0x1f: sprintf(op, "pop b"); break;
		case 0x20: sprintf(op, "ei"); break;
		case 0x24: sprintf(op, "di"); break;
		case 0x2a: sprintf(op, "clc"); break;
		case 0x2b: sprintf(op, "stc"); break;
		case 0x2c: sprintf(op, "pen"); break;
		case 0x2d: sprintf(op, "pex"); break;
		case 0x2e: sprintf(op, "push d"); break;
		case 0x2f: sprintf(op, "pop d"); break;
		case 0x30: sprintf(op, "rll a"); break;
		case 0x31: sprintf(op, "rlr a"); break;
		case 0x32: sprintf(op, "rll c"); break;
		case 0x33: sprintf(op, "rlr c"); break;
		case 0x34: sprintf(op, "sll a"); break;
		case 0x35: sprintf(op, "slr a"); break;
		case 0x36: sprintf(op, "sll c"); break;
		case 0x37: sprintf(op, "sll c"); break;
		case 0x38: sprintf(op, "rld"); break;
		case 0x39: sprintf(op, "rrd"); break;
		case 0x3c: sprintf(op, "per"); break;
		case 0x3e: sprintf(op, "push h"); break;
		case 0x3f: sprintf(op, "pop h"); break;
		default: sprintf(op, "db 48h,%2xh", b);
		}
		break;
	case 0x49: sprintf(op, "mvix b,%2xh", getb()); break;
	case 0x4a: sprintf(op, "mvix d,%2xh", getb()); break;
	case 0x4b: sprintf(op, "mvix h,%2xh", getb()); break;
	case 0x4c:
		switch(b = getb())
		{
		case 0xc0: sprintf(op, "mov a,pa"); break;
		case 0xc1: sprintf(op, "mov a,pb"); break;
		case 0xc2: sprintf(op, "mov a,pc"); break;
		case 0xc3: sprintf(op, "mov a,mk"); break;
		case 0xc4: sprintf(op, "mov a,mb"); break;	// 未定義?
		case 0xc5: sprintf(op, "mov a,mc"); break;	// 未定義?
		case 0xc6: sprintf(op, "mov a,tm0"); break;	// 未定義?
		case 0xc7: sprintf(op, "mov a,tm1"); break;	// 未定義?
		case 0xc8: sprintf(op, "mov a,s"); break;
		default:
			if(b < 0xc0) {
				sprintf(op, "in %2xh", getb()); break;
			}
			sprintf(op, "db 4ch,%2xh", b);
		}
		break;
	case 0x4d:
		switch(b = getb())
		{
		case 0xc0: sprintf(op, "mov pa,a"); break;
		case 0xc1: sprintf(op, "mov pb,a"); break;
		case 0xc2: sprintf(op, "mov pc,a"); break;
		case 0xc3: sprintf(op, "mov mk,a"); break;
		case 0xc4: sprintf(op, "mov mb,a"); break;
		case 0xc5: sprintf(op, "mov mc,a"); break;
		case 0xc6: sprintf(op, "mov tm0,a"); break;
		case 0xc7: sprintf(op, "mov tm1,a"); break;
		case 0xc8: sprintf(op, "mov s,a"); break;
		default:
			if(b < 0xc0) {
				sprintf(op, "out %2xh", getb()); break;
			}
			sprintf(op, "db 4dh,%2xh", b);
		}
		break;
	case 0x4e: b = getb(); sprintf(op, "jre %4xh", pc + b); break;
	case 0x4f: b = getb(); sprintf(op, "jre %4xh", (pc + b - 256) & 0xffff); break;
	
//	case 0x50:
	case 0x51: sprintf(op, "dcr a"); break;
	case 0x52: sprintf(op, "dcr b"); break;
	case 0x53: sprintf(op, "dcr c"); break;
	case 0x54: sprintf(op, "jmp %4xh", getw()); break;
	case 0x55: wa = getwa(); sprintf(op, "offiw v.%xh,%2xh", wa, getb()); break;
	case 0x56: sprintf(op, "aci a,%2xh", getb()); break;
	case 0x57: sprintf(op, "offi a,%2xh", getb()); break;
	case 0x58: sprintf(op, "bit 0,v.%xh", getwa()); break;
	case 0x59: sprintf(op, "bit 1,v.%xh", getwa()); break;
	case 0x5a: sprintf(op, "bit 2,v.%xh", getwa()); break;
	case 0x5b: sprintf(op, "bit 3,v.%xh", getwa()); break;
	case 0x5c: sprintf(op, "bit 4,v.%xh", getwa()); break;
	case 0x5d: sprintf(op, "bit 5,v.%xh", getwa()); break;
	case 0x5e: sprintf(op, "bit 6,v.%xh", getwa()); break;
	case 0x5f: sprintf(op, "bit 7,v.%xh", getwa()); break;
	
	case 0x60:
		switch(b = getb())
		{
		case 0x08: sprintf(op, "ana v,a"); break;
		case 0x09: sprintf(op, "ana a,a"); break;
		case 0x0a: sprintf(op, "ana b,a"); break;
		case 0x0b: sprintf(op, "ana c,a"); break;
		case 0x0c: sprintf(op, "ana d,a"); break;
		case 0x0d: sprintf(op, "ana e,a"); break;
		case 0x0e: sprintf(op, "ana h,a"); break;
		case 0x0f: sprintf(op, "ana l,a"); break;
		case 0x10: sprintf(op, "xra v,a"); break;
		case 0x11: sprintf(op, "xra a,a"); break;
		case 0x12: sprintf(op, "xra b,a"); break;
		case 0x13: sprintf(op, "xra c,a"); break;
		case 0x14: sprintf(op, "xra d,a"); break;
		case 0x15: sprintf(op, "xra e,a"); break;
		case 0x16: sprintf(op, "xra h,a"); break;
		case 0x17: sprintf(op, "xra l,a"); break;
		case 0x18: sprintf(op, "ora v,a"); break;
		case 0x19: sprintf(op, "ora a,a"); break;
		case 0x1a: sprintf(op, "ora b,a"); break;
		case 0x1b: sprintf(op, "ora c,a"); break;
		case 0x1c: sprintf(op, "ora d,a"); break;
		case 0x1d: sprintf(op, "ora e,a"); break;
		case 0x1e: sprintf(op, "ora h,a"); break;
		case 0x1f: sprintf(op, "ora l,a"); break;
		case 0x20: sprintf(op, "addnc v,a"); break;
		case 0x21: sprintf(op, "addnc a,a"); break;
		case 0x22: sprintf(op, "addnc b,a"); break;
		case 0x23: sprintf(op, "addnc c,a"); break;
		case 0x24: sprintf(op, "addnc d,a"); break;
		case 0x25: sprintf(op, "addnc e,a"); break;
		case 0x26: sprintf(op, "addnc h,a"); break;
		case 0x27: sprintf(op, "addnc l,a"); break;
		case 0x28: sprintf(op, "gta v,a"); break;
		case 0x29: sprintf(op, "gta a,a"); break;
		case 0x2a: sprintf(op, "gta b,a"); break;
		case 0x2b: sprintf(op, "gta c,a"); break;
		case 0x2c: sprintf(op, "gta d,a"); break;
		case 0x2d: sprintf(op, "gta e,a"); break;
		case 0x2e: sprintf(op, "gta h,a"); break;
		case 0x2f: sprintf(op, "gta l,a"); break;
		case 0x30: sprintf(op, "subnb v,a"); break;
		case 0x31: sprintf(op, "subnb a,a"); break;
		case 0x32: sprintf(op, "subnb b,a"); break;
		case 0x33: sprintf(op, "subnb c,a"); break;
		case 0x34: sprintf(op, "subnb d,a"); break;
		case 0x35: sprintf(op, "subnb e,a"); break;
		case 0x36: sprintf(op, "subnb h,a"); break;
		case 0x37: sprintf(op, "subnb l,a"); break;
		case 0x38: sprintf(op, "lta v,a"); break;
		case 0x39: sprintf(op, "lta a,a"); break;
		case 0x3a: sprintf(op, "lta b,a"); break;
		case 0x3b: sprintf(op, "lta c,a"); break;
		case 0x3c: sprintf(op, "lta d,a"); break;
		case 0x3d: sprintf(op, "lta e,a"); break;
		case 0x3e: sprintf(op, "lta h,a"); break;
		case 0x3f: sprintf(op, "lta l,a"); break;
		case 0x40: sprintf(op, "add v,a"); break;
		case 0x41: sprintf(op, "add a,a"); break;
		case 0x42: sprintf(op, "add b,a"); break;
		case 0x43: sprintf(op, "add c,a"); break;
		case 0x44: sprintf(op, "add d,a"); break;
		case 0x45: sprintf(op, "add e,a"); break;
		case 0x46: sprintf(op, "add h,a"); break;
		case 0x47: sprintf(op, "add l,a"); break;
		case 0x50: sprintf(op, "adc v,a"); break;
		case 0x51: sprintf(op, "adc a,a"); break;
		case 0x52: sprintf(op, "adc b,a"); break;
		case 0x53: sprintf(op, "adc c,a"); break;
		case 0x54: sprintf(op, "adc d,a"); break;
		case 0x55: sprintf(op, "adc e,a"); break;
		case 0x56: sprintf(op, "adc h,a"); break;
		case 0x57: sprintf(op, "adc l,a"); break;
		case 0x60: sprintf(op, "sub v,a"); break;
		case 0x61: sprintf(op, "sub a,a"); break;
		case 0x62: sprintf(op, "sub b,a"); break;
		case 0x63: sprintf(op, "sub c,a"); break;
		case 0x64: sprintf(op, "sub d,a"); break;
		case 0x65: sprintf(op, "sub e,a"); break;
		case 0x66: sprintf(op, "sub h,a"); break;
		case 0x67: sprintf(op, "sub l,a"); break;
		case 0x68: sprintf(op, "nea v,a"); break;
		case 0x69: sprintf(op, "nea a,a"); break;
		case 0x6a: sprintf(op, "nea b,a"); break;
		case 0x6b: sprintf(op, "nea c,a"); break;
		case 0x6c: sprintf(op, "nea d,a"); break;
		case 0x6d: sprintf(op, "nea e,a"); break;
		case 0x6e: sprintf(op, "nea h,a"); break;
		case 0x6f: sprintf(op, "nea l,a"); break;
		case 0x70: sprintf(op, "sbb v,a"); break;
		case 0x71: sprintf(op, "sbb a,a"); break;
		case 0x72: sprintf(op, "sbb b,a"); break;
		case 0x73: sprintf(op, "sbb c,a"); break;
		case 0x74: sprintf(op, "sbb d,a"); break;
		case 0x75: sprintf(op, "sbb e,a"); break;
		case 0x76: sprintf(op, "sbb h,a"); break;
		case 0x77: sprintf(op, "sbb l,a"); break;
		case 0x78: sprintf(op, "eqa v,a"); break;
		case 0x79: sprintf(op, "eqa a,a"); break;
		case 0x7a: sprintf(op, "eqa b,a"); break;
		case 0x7b: sprintf(op, "eqa c,a"); break;
		case 0x7c: sprintf(op, "eqa d,a"); break;
		case 0x7d: sprintf(op, "eqa e,a"); break;
		case 0x7e: sprintf(op, "eqa h,a"); break;
		case 0x7f: sprintf(op, "eqa l,a"); break;
		case 0x88: sprintf(op, "ana a,v"); break;
		case 0x89: sprintf(op, "ana a,a"); break;
		case 0x8a: sprintf(op, "ana a,b"); break;
		case 0x8b: sprintf(op, "ana a,c"); break;
		case 0x8c: sprintf(op, "ana a,d"); break;
		case 0x8d: sprintf(op, "ana a,e"); break;
		case 0x8e: sprintf(op, "ana a,h"); break;
		case 0x8f: sprintf(op, "ana a,l"); break;
		case 0x90: sprintf(op, "xra a,v"); break;
		case 0x91: sprintf(op, "xra a,a"); break;
		case 0x92: sprintf(op, "xra a,b"); break;
		case 0x93: sprintf(op, "xra a,c"); break;
		case 0x94: sprintf(op, "xra a,d"); break;
		case 0x95: sprintf(op, "xra a,e"); break;
		case 0x96: sprintf(op, "xra a,h"); break;
		case 0x97: sprintf(op, "xra a,l"); break;
		case 0x98: sprintf(op, "ora a,v"); break;
		case 0x99: sprintf(op, "ora a,a"); break;
		case 0x9a: sprintf(op, "ora a,b"); break;
		case 0x9b: sprintf(op, "ora a,c"); break;
		case 0x9c: sprintf(op, "ora a,d"); break;
		case 0x9d: sprintf(op, "ora a,e"); break;
		case 0x9e: sprintf(op, "ora a,h"); break;
		case 0x9f: sprintf(op, "ora a,l"); break;
		case 0xa0: sprintf(op, "addnc a,v"); break;
		case 0xa1: sprintf(op, "addnc a,a"); break;
		case 0xa2: sprintf(op, "addnc a,b"); break;
		case 0xa3: sprintf(op, "addnc a,c"); break;
		case 0xa4: sprintf(op, "addnc a,d"); break;
		case 0xa5: sprintf(op, "addnc a,e"); break;
		case 0xa6: sprintf(op, "addnc a,h"); break;
		case 0xa7: sprintf(op, "addnc a,l"); break;
		case 0xa8: sprintf(op, "gta a,v"); break;
		case 0xa9: sprintf(op, "gta a,a"); break;
		case 0xaa: sprintf(op, "gta a,b"); break;
		case 0xab: sprintf(op, "gta a,c"); break;
		case 0xac: sprintf(op, "gta a,d"); break;
		case 0xad: sprintf(op, "gta a,e"); break;
		case 0xae: sprintf(op, "gta a,h"); break;
		case 0xaf: sprintf(op, "gta a,l"); break;
		case 0xb0: sprintf(op, "subnb a,v"); break;
		case 0xb1: sprintf(op, "subnb a,a"); break;
		case 0xb2: sprintf(op, "subnb a,b"); break;
		case 0xb3: sprintf(op, "subnb a,c"); break;
		case 0xb4: sprintf(op, "subnb a,d"); break;
		case 0xb5: sprintf(op, "subnb a,e"); break;
		case 0xb6: sprintf(op, "subnb a,h"); break;
		case 0xb7: sprintf(op, "subnb a,l"); break;
		case 0xb8: sprintf(op, "lta a,v"); break;
		case 0xb9: sprintf(op, "lta a,a"); break;
		case 0xba: sprintf(op, "lta a,b"); break;
		case 0xbb: sprintf(op, "lta a,c"); break;
		case 0xbc: sprintf(op, "lta a,d"); break;
		case 0xbd: sprintf(op, "lta a,e"); break;
		case 0xbe: sprintf(op, "lta a,h"); break;
		case 0xbf: sprintf(op, "lta a,l"); break;
		case 0xc0: sprintf(op, "add a,v"); break;
		case 0xc1: sprintf(op, "add a,a"); break;
		case 0xc2: sprintf(op, "add a,b"); break;
		case 0xc3: sprintf(op, "add a,c"); break;
		case 0xc4: sprintf(op, "add a,d"); break;
		case 0xc5: sprintf(op, "add a,e"); break;
		case 0xc6: sprintf(op, "add a,h"); break;
		case 0xc7: sprintf(op, "add a,l"); break;
		case 0xc8: sprintf(op, "ona a,v"); break;
		case 0xc9: sprintf(op, "ona a,a"); break;
		case 0xca: sprintf(op, "ona a,b"); break;
		case 0xcb: sprintf(op, "ona a,c"); break;
		case 0xcc: sprintf(op, "ona a,d"); break;
		case 0xcd: sprintf(op, "ona a,e"); break;
		case 0xce: sprintf(op, "ona a,h"); break;
		case 0xcf: sprintf(op, "ona a,l"); break;
		case 0xd0: sprintf(op, "adc a,v"); break;
		case 0xd1: sprintf(op, "adc a,a"); break;
		case 0xd2: sprintf(op, "adc a,b"); break;
		case 0xd3: sprintf(op, "adc a,c"); break;
		case 0xd4: sprintf(op, "adc a,d"); break;
		case 0xd5: sprintf(op, "adc a,e"); break;
		case 0xd6: sprintf(op, "adc a,h"); break;
		case 0xd7: sprintf(op, "adc a,l"); break;
		case 0xd8: sprintf(op, "offa a,v"); break;
		case 0xd9: sprintf(op, "offa a,a"); break;
		case 0xda: sprintf(op, "offa a,b"); break;
		case 0xdb: sprintf(op, "offa a,c"); break;
		case 0xdc: sprintf(op, "offa a,d"); break;
		case 0xdd: sprintf(op, "offa a,e"); break;
		case 0xde: sprintf(op, "offa a,h"); break;
		case 0xdf: sprintf(op, "offa a,l"); break;
		case 0xe0: sprintf(op, "sub a,v"); break;
		case 0xe1: sprintf(op, "sub a,a"); break;
		case 0xe2: sprintf(op, "sub a,b"); break;
		case 0xe3: sprintf(op, "sub a,c"); break;
		case 0xe4: sprintf(op, "sub a,d"); break;
		case 0xe5: sprintf(op, "sub a,e"); break;
		case 0xe6: sprintf(op, "sub a,h"); break;
		case 0xe7: sprintf(op, "sub a,l"); break;
		case 0xe8: sprintf(op, "nea a,v"); break;
		case 0xe9: sprintf(op, "nea a,a"); break;
		case 0xea: sprintf(op, "nea a,b"); break;
		case 0xeb: sprintf(op, "nea a,c"); break;
		case 0xec: sprintf(op, "nea a,d"); break;
		case 0xed: sprintf(op, "nea a,e"); break;
		case 0xee: sprintf(op, "nea a,h"); break;
		case 0xef: sprintf(op, "nea a,l"); break;
		case 0xf0: sprintf(op, "sbb a,v"); break;
		case 0xf1: sprintf(op, "sbb a,a"); break;
		case 0xf2: sprintf(op, "sbb a,b"); break;
		case 0xf3: sprintf(op, "sbb a,c"); break;
		case 0xf4: sprintf(op, "sbb a,d"); break;
		case 0xf5: sprintf(op, "sbb a,e"); break;
		case 0xf6: sprintf(op, "sbb a,h"); break;
		case 0xf7: sprintf(op, "sbb a,l"); break;
		case 0xf8: sprintf(op, "eqa a,v"); break;
		case 0xf9: sprintf(op, "eqa a,a"); break;
		case 0xfa: sprintf(op, "eqa a,b"); break;
		case 0xfb: sprintf(op, "eqa a,c"); break;
		case 0xfc: sprintf(op, "eqa a,d"); break;
		case 0xfd: sprintf(op, "eqa a,e"); break;
		case 0xfe: sprintf(op, "eqa a,h"); break;
		case 0xff: sprintf(op, "eqa a,l"); break;
		default: sprintf(op, "db 60h,%2xh", b);
		}
		break;
	case 0x61: sprintf(op, "daa"); break;
	case 0x62: sprintf(op, "reti"); break;
	case 0x63: sprintf(op, "calb"); break;
	case 0x64:
		switch(b = getb())
		{
		case 0x08: sprintf(op, "ani v,%2xh", getb()); break;
		case 0x09: sprintf(op, "ani a,%2xh", getb()); break;
		case 0x0a: sprintf(op, "ani b,%2xh", getb()); break;
		case 0x0b: sprintf(op, "ani c,%2xh", getb()); break;
		case 0x0c: sprintf(op, "ani d,%2xh", getb()); break;
		case 0x0d: sprintf(op, "ani e,%2xh", getb()); break;
		case 0x0e: sprintf(op, "ani h,%2xh", getb()); break;
		case 0x0f: sprintf(op, "ani l,%2xh", getb()); break;
		case 0x10: sprintf(op, "xri v,%2xh", getb()); break;
		case 0x11: sprintf(op, "xri a,%2xh", getb()); break;
		case 0x12: sprintf(op, "xri b,%2xh", getb()); break;
		case 0x13: sprintf(op, "xri c,%2xh", getb()); break;
		case 0x14: sprintf(op, "xri d,%2xh", getb()); break;
		case 0x15: sprintf(op, "xri e,%2xh", getb()); break;
		case 0x16: sprintf(op, "xri h,%2xh", getb()); break;
		case 0x17: sprintf(op, "xri l,%2xh", getb()); break;
		case 0x18: sprintf(op, "ori v,%2xh", getb()); break;
		case 0x19: sprintf(op, "ori a,%2xh", getb()); break;
		case 0x1a: sprintf(op, "ori b,%2xh", getb()); break;
		case 0x1b: sprintf(op, "ori c,%2xh", getb()); break;
		case 0x1c: sprintf(op, "ori d,%2xh", getb()); break;
		case 0x1d: sprintf(op, "ori e,%2xh", getb()); break;
		case 0x1e: sprintf(op, "ori h,%2xh", getb()); break;
		case 0x1f: sprintf(op, "ori l,%2xh", getb()); break;
		case 0x20: sprintf(op, "adinc v,%2xh", getb()); break;
		case 0x21: sprintf(op, "adinc a,%2xh", getb()); break;
		case 0x22: sprintf(op, "adinc b,%2xh", getb()); break;
		case 0x23: sprintf(op, "adinc c,%2xh", getb()); break;
		case 0x24: sprintf(op, "adinc d,%2xh", getb()); break;
		case 0x25: sprintf(op, "adinc e,%2xh", getb()); break;
		case 0x26: sprintf(op, "adinc h,%2xh", getb()); break;
		case 0x27: sprintf(op, "adinc l,%2xh", getb()); break;
		case 0x28: sprintf(op, "gti v,%2xh", getb()); break;
		case 0x29: sprintf(op, "gti a,%2xh", getb()); break;
		case 0x2a: sprintf(op, "gti b,%2xh", getb()); break;
		case 0x2b: sprintf(op, "gti c,%2xh", getb()); break;
		case 0x2c: sprintf(op, "gti d,%2xh", getb()); break;
		case 0x2d: sprintf(op, "gti e,%2xh", getb()); break;
		case 0x2e: sprintf(op, "gti h,%2xh", getb()); break;
		case 0x2f: sprintf(op, "gti l,%2xh", getb()); break;
		case 0x30: sprintf(op, "suinb v,%2xh", getb()); break;
		case 0x31: sprintf(op, "suinb a,%2xh", getb()); break;
		case 0x32: sprintf(op, "suinb b,%2xh", getb()); break;
		case 0x33: sprintf(op, "suinb c,%2xh", getb()); break;
		case 0x34: sprintf(op, "suinb d,%2xh", getb()); break;
		case 0x35: sprintf(op, "suinb e,%2xh", getb()); break;
		case 0x36: sprintf(op, "suinb h,%2xh", getb()); break;
		case 0x37: sprintf(op, "suinb l,%2xh", getb()); break;
		case 0x38: sprintf(op, "lti v,%2xh", getb()); break;
		case 0x39: sprintf(op, "lti a,%2xh", getb()); break;
		case 0x3a: sprintf(op, "lti b,%2xh", getb()); break;
		case 0x3b: sprintf(op, "lti c,%2xh", getb()); break;
		case 0x3c: sprintf(op, "lti d,%2xh", getb()); break;
		case 0x3d: sprintf(op, "lti e,%2xh", getb()); break;
		case 0x3e: sprintf(op, "lti h,%2xh", getb()); break;
		case 0x3f: sprintf(op, "lti l,%2xh", getb()); break;
		case 0x40: sprintf(op, "adi v,%2xh", getb()); break;
		case 0x41: sprintf(op, "adi a,%2xh", getb()); break;
		case 0x42: sprintf(op, "adi b,%2xh", getb()); break;
		case 0x43: sprintf(op, "adi c,%2xh", getb()); break;
		case 0x44: sprintf(op, "adi d,%2xh", getb()); break;
		case 0x45: sprintf(op, "adi e,%2xh", getb()); break;
		case 0x46: sprintf(op, "adi h,%2xh", getb()); break;
		case 0x47: sprintf(op, "adi l,%2xh", getb()); break;
		case 0x48: sprintf(op, "oni v,%2xh", getb()); break;
		case 0x49: sprintf(op, "oni a,%2xh", getb()); break;
		case 0x4a: sprintf(op, "oni b,%2xh", getb()); break;
		case 0x4b: sprintf(op, "oni c,%2xh", getb()); break;
		case 0x4c: sprintf(op, "oni d,%2xh", getb()); break;
		case 0x4d: sprintf(op, "oni e,%2xh", getb()); break;
		case 0x4e: sprintf(op, "oni h,%2xh", getb()); break;
		case 0x4f: sprintf(op, "oni l,%2xh", getb()); break;
		case 0x50: sprintf(op, "aci v,%2xh", getb()); break;
		case 0x51: sprintf(op, "aci a,%2xh", getb()); break;
		case 0x52: sprintf(op, "aci b,%2xh", getb()); break;
		case 0x53: sprintf(op, "aci c,%2xh", getb()); break;
		case 0x54: sprintf(op, "aci d,%2xh", getb()); break;
		case 0x55: sprintf(op, "aci e,%2xh", getb()); break;
		case 0x56: sprintf(op, "aci h,%2xh", getb()); break;
		case 0x57: sprintf(op, "aci l,%2xh", getb()); break;
		case 0x58: sprintf(op, "offi v,%2xh", getb()); break;
		case 0x59: sprintf(op, "offi a,%2xh", getb()); break;
		case 0x5a: sprintf(op, "offi b,%2xh", getb()); break;
		case 0x5b: sprintf(op, "offi c,%2xh", getb()); break;
		case 0x5c: sprintf(op, "offi d,%2xh", getb()); break;
		case 0x5d: sprintf(op, "offi e,%2xh", getb()); break;
		case 0x5e: sprintf(op, "offi h,%2xh", getb()); break;
		case 0x5f: sprintf(op, "offi l,%2xh", getb()); break;
		case 0x60: sprintf(op, "sui v,%2xh", getb()); break;
		case 0x61: sprintf(op, "sui a,%2xh", getb()); break;
		case 0x62: sprintf(op, "sui b,%2xh", getb()); break;
		case 0x63: sprintf(op, "sui c,%2xh", getb()); break;
		case 0x64: sprintf(op, "sui d,%2xh", getb()); break;
		case 0x65: sprintf(op, "sui e,%2xh", getb()); break;
		case 0x66: sprintf(op, "sui h,%2xh", getb()); break;
		case 0x67: sprintf(op, "sui l,%2xh", getb()); break;
		case 0x68: sprintf(op, "nei v,%2xh", getb()); break;
		case 0x69: sprintf(op, "nei a,%2xh", getb()); break;
		case 0x6a: sprintf(op, "nei b,%2xh", getb()); break;
		case 0x6b: sprintf(op, "nei c,%2xh", getb()); break;
		case 0x6c: sprintf(op, "nei d,%2xh", getb()); break;
		case 0x6d: sprintf(op, "nei e,%2xh", getb()); break;
		case 0x6e: sprintf(op, "nei h,%2xh", getb()); break;
		case 0x6f: sprintf(op, "nei l,%2xh", getb()); break;
		case 0x70: sprintf(op, "sbi v,%2xh", getb()); break;
		case 0x71: sprintf(op, "sbi a,%2xh", getb()); break;
		case 0x72: sprintf(op, "sbi b,%2xh", getb()); break;
		case 0x73: sprintf(op, "sbi c,%2xh", getb()); break;
		case 0x74: sprintf(op, "sbi d,%2xh", getb()); break;
		case 0x75: sprintf(op, "sbi e,%2xh", getb()); break;
		case 0x76: sprintf(op, "sbi h,%2xh", getb()); break;
		case 0x77: sprintf(op, "sbi l,%2xh", getb()); break;
		case 0x78: sprintf(op, "eqi v,%2xh", getb()); break;
		case 0x79: sprintf(op, "eqi a,%2xh", getb()); break;
		case 0x7a: sprintf(op, "eqi b,%2xh", getb()); break;
		case 0x7b: sprintf(op, "eqi c,%2xh", getb()); break;
		case 0x7c: sprintf(op, "eqi d,%2xh", getb()); break;
		case 0x7d: sprintf(op, "eqi e,%2xh", getb()); break;
		case 0x7e: sprintf(op, "eqi h,%2xh", getb()); break;
		case 0x7f: sprintf(op, "eqi l,%2xh", getb()); break;
		case 0x88: sprintf(op, "ani pa,%2xh", getb()); break;
		case 0x89: sprintf(op, "ani pb,%2xh", getb()); break;
		case 0x8a: sprintf(op, "ani pc,%2xh", getb()); break;
		case 0x8b: sprintf(op, "ani mk,%2xh", getb()); break;
		case 0x90: sprintf(op, "xri pa,%2xh", getb()); break;
		case 0x91: sprintf(op, "xri pb,%2xh", getb()); break;
		case 0x92: sprintf(op, "xri pc,%2xh", getb()); break;
		case 0x93: sprintf(op, "xri mk,%2xh", getb()); break;
		case 0x98: sprintf(op, "ori pa,%2xh", getb()); break;
		case 0x99: sprintf(op, "ori pb,%2xh", getb()); break;
		case 0x9a: sprintf(op, "ori pc,%2xh", getb()); break;
		case 0x9b: sprintf(op, "ori mk,%2xh", getb()); break;
		case 0xa0: sprintf(op, "adinc pa,%2xh", getb()); break;
		case 0xa1: sprintf(op, "adinc pb,%2xh", getb()); break;
		case 0xa2: sprintf(op, "adinc pc,%2xh", getb()); break;
		case 0xa3: sprintf(op, "adinc mk,%2xh", getb()); break;
		case 0xa8: sprintf(op, "gti pa,%2xh", getb()); break;
		case 0xa9: sprintf(op, "gti pb,%2xh", getb()); break;
		case 0xaa: sprintf(op, "gti pc,%2xh", getb()); break;
		case 0xab: sprintf(op, "gti mk,%2xh", getb()); break;
		case 0xb0: sprintf(op, "suinb pa,%2xh", getb()); break;
		case 0xb1: sprintf(op, "suinb pb,%2xh", getb()); break;
		case 0xb2: sprintf(op, "suinb pc,%2xh", getb()); break;
		case 0xb3: sprintf(op, "suinb mk,%2xh", getb()); break;
		case 0xb8: sprintf(op, "lti pa,%2xh", getb()); break;
		case 0xb9: sprintf(op, "lti pb,%2xh", getb()); break;
		case 0xba: sprintf(op, "lti pc,%2xh", getb()); break;
		case 0xbb: sprintf(op, "lti mk,%2xh", getb()); break;
		case 0xc0: sprintf(op, "adi pa,%2xh", getb()); break;
		case 0xc1: sprintf(op, "adi pb,%2xh", getb()); break;
		case 0xc2: sprintf(op, "adi pc,%2xh", getb()); break;
		case 0xc3: sprintf(op, "adi mk,%2xh", getb()); break;
		case 0xc8: sprintf(op, "oni pa,%2xh", getb()); break;
		case 0xc9: sprintf(op, "oni pb,%2xh", getb()); break;
		case 0xca: sprintf(op, "oni pc,%2xh", getb()); break;
		case 0xcb: sprintf(op, "oni mk,%2xh", getb()); break;
		case 0xd0: sprintf(op, "aci pa,%2xh", getb()); break;
		case 0xd1: sprintf(op, "aci pb,%2xh", getb()); break;
		case 0xd2: sprintf(op, "aci pc,%2xh", getb()); break;
		case 0xd3: sprintf(op, "aci mk,%2xh", getb()); break;
		case 0xd8: sprintf(op, "offi pa,%2xh", getb()); break;
		case 0xd9: sprintf(op, "offi pb,%2xh", getb()); break;
		case 0xda: sprintf(op, "offi pc,%2xh", getb()); break;
		case 0xdb: sprintf(op, "offi mk,%2xh", getb()); break;
		case 0xe0: sprintf(op, "sui pa,%2xh", getb()); break;
		case 0xe1: sprintf(op, "sui pb,%2xh", getb()); break;
		case 0xe2: sprintf(op, "sui pc,%2xh", getb()); break;
		case 0xe3: sprintf(op, "sui mk,%2xh", getb()); break;
		case 0xe8: sprintf(op, "nei pa,%2xh", getb()); break;
		case 0xe9: sprintf(op, "nei pb,%2xh", getb()); break;
		case 0xea: sprintf(op, "nei pc,%2xh", getb()); break;
		case 0xeb: sprintf(op, "nei mk,%2xh", getb()); break;
		case 0xf0: sprintf(op, "sbi pa,%2xh", getb()); break;
		case 0xf1: sprintf(op, "sbi pb,%2xh", getb()); break;
		case 0xf2: sprintf(op, "sbi pc,%2xh", getb()); break;
		case 0xf3: sprintf(op, "sbi mk,%2xh", getb()); break;
		case 0xf8: sprintf(op, "eqi pa,%2xh", getb()); break;
		case 0xf9: sprintf(op, "eqi pb,%2xh", getb()); break;
		case 0xfa: sprintf(op, "eqi pc,%2xh", getb()); break;
		case 0xfb: sprintf(op, "eqi mk,%2xh", getb()); break;
		default: sprintf(op, "db 64h,%2xh", b);
		}
		break;
	case 0x65: wa = getwa(); sprintf(op, "neiw v.%xh,%2xh", wa, getb()); break;
	case 0x66: sprintf(op, "sui a,%2xh", getb()); break;
	case 0x67: sprintf(op, "nei a,%2xh", getb()); break;
	case 0x68: sprintf(op, "mvi v,%2xh", getb()); break;
	case 0x69: sprintf(op, "mvi a,%2xh", getb()); break;
	case 0x6a: sprintf(op, "mvi b,%2xh", getb()); break;
	case 0x6b: sprintf(op, "mvi c,%2xh", getb()); break;
	case 0x6c: sprintf(op, "mvi d,%2xh", getb()); break;
	case 0x6d: sprintf(op, "mvi e,%2xh", getb()); break;
	case 0x6e: sprintf(op, "mvi h,%2xh", getb()); break;
	case 0x6f: sprintf(op, "mvi l,%2xh", getb()); break;
	
	case 0x70:
		switch(b = getb())
		{
		case 0x0e: sprintf(op, "sspd %4xh", getw()); break;
		case 0x0f: sprintf(op, "lspd %4xh", getw()); break;
		case 0x1e: sprintf(op, "sbcd %4xh", getw()); break;
		case 0x1f: sprintf(op, "lbcd %4xh", getw()); break;
		case 0x2e: sprintf(op, "sded %4xh", getw()); break;
		case 0x2f: sprintf(op, "lded %4xh", getw()); break;
		case 0x3e: sprintf(op, "shld %4xh", getw()); break;
		case 0x3f: sprintf(op, "lhld %4xh", getw()); break;
		case 0x68: sprintf(op, "mov v,%4xh", getw()); break;
		case 0x69: sprintf(op, "mov a,%4xh", getw()); break;
		case 0x6a: sprintf(op, "mov b,%4xh", getw()); break;
		case 0x6b: sprintf(op, "mov c,%4xh", getw()); break;
		case 0x6c: sprintf(op, "mov d,%4xh", getw()); break;
		case 0x6d: sprintf(op, "mov e,%4xh", getw()); break;
		case 0x6e: sprintf(op, "mov h,%4xh", getw()); break;
		case 0x6f: sprintf(op, "mov l,%4xh", getw()); break;
		case 0x78: sprintf(op, "mov %4xh,v", getw()); break;
		case 0x79: sprintf(op, "mov %4xh,a", getw()); break;
		case 0x7a: sprintf(op, "mov %4xh,b", getw()); break;
		case 0x7b: sprintf(op, "mov %4xh,c", getw()); break;
		case 0x7c: sprintf(op, "mov %4xh,d", getw()); break;
		case 0x7d: sprintf(op, "mov %4xh,e", getw()); break;
		case 0x7e: sprintf(op, "mov %4xh,h", getw()); break;
		case 0x7f: sprintf(op, "mov %4xh,l", getw()); break;
		case 0x89: sprintf(op, "anax b"); break;
		case 0x8a: sprintf(op, "anax d"); break;
		case 0x8b: sprintf(op, "anax h"); break;
		case 0x8c: sprintf(op, "anax d+"); break;
		case 0x8d: sprintf(op, "anax h+"); break;
		case 0x8e: sprintf(op, "anax d-"); break;
		case 0x8f: sprintf(op, "anax h-"); break;
		case 0x91: sprintf(op, "xrax b"); break;
		case 0x92: sprintf(op, "xrax d"); break;
		case 0x93: sprintf(op, "xrax h"); break;
		case 0x94: sprintf(op, "xrax d+"); break;
		case 0x95: sprintf(op, "xrax h+"); break;
		case 0x96: sprintf(op, "xrax d-"); break;
		case 0x97: sprintf(op, "xrax h-"); break;
		case 0x99: sprintf(op, "orax b"); break;
		case 0x9a: sprintf(op, "orax d"); break;
		case 0x9b: sprintf(op, "orax h"); break;
		case 0x9c: sprintf(op, "orax d+"); break;
		case 0x9d: sprintf(op, "orax h+"); break;
		case 0x9e: sprintf(op, "orax d-"); break;
		case 0x9f: sprintf(op, "orax h-"); break;
		case 0xa1: sprintf(op, "addncx b"); break;
		case 0xa2: sprintf(op, "addncx d"); break;
		case 0xa3: sprintf(op, "addncx h"); break;
		case 0xa4: sprintf(op, "addncx d+"); break;
		case 0xa5: sprintf(op, "addncx h+"); break;
		case 0xa6: sprintf(op, "addncx d-"); break;
		case 0xa7: sprintf(op, "addncx h-"); break;
		case 0xa9: sprintf(op, "gtax b"); break;
		case 0xaa: sprintf(op, "gtax d"); break;
		case 0xab: sprintf(op, "gtax h"); break;
		case 0xac: sprintf(op, "gtax d+"); break;
		case 0xad: sprintf(op, "gtax h+"); break;
		case 0xae: sprintf(op, "gtax d-"); break;
		case 0xaf: sprintf(op, "gtax h-"); break;
		case 0xb1: sprintf(op, "subnbx b"); break;
		case 0xb2: sprintf(op, "subnbx d"); break;
		case 0xb3: sprintf(op, "subnbx h"); break;
		case 0xb4: sprintf(op, "subnbx d+"); break;
		case 0xb5: sprintf(op, "subnbx h+"); break;
		case 0xb6: sprintf(op, "subnbx d-"); break;
		case 0xb7: sprintf(op, "subnbx h-"); break;
		case 0xb9: sprintf(op, "ltax b"); break;
		case 0xba: sprintf(op, "ltax d"); break;
		case 0xbb: sprintf(op, "ltax h"); break;
		case 0xbc: sprintf(op, "ltax d+"); break;
		case 0xbd: sprintf(op, "ltax h+"); break;
		case 0xbe: sprintf(op, "ltax d-"); break;
		case 0xbf: sprintf(op, "ltax h-"); break;
		case 0xc1: sprintf(op, "addx b"); break;
		case 0xc2: sprintf(op, "addx d"); break;
		case 0xc3: sprintf(op, "addx h"); break;
		case 0xc4: sprintf(op, "addx d+"); break;
		case 0xc5: sprintf(op, "addx h+"); break;
		case 0xc6: sprintf(op, "addx d-"); break;
		case 0xc7: sprintf(op, "addx h-"); break;
		case 0xc9: sprintf(op, "onax b"); break;
		case 0xca: sprintf(op, "onax d"); break;
		case 0xcb: sprintf(op, "onax h"); break;
		case 0xcc: sprintf(op, "onax d+"); break;
		case 0xcd: sprintf(op, "onax h+"); break;
		case 0xce: sprintf(op, "onax d-"); break;
		case 0xcf: sprintf(op, "onax h-"); break;
		case 0xd1: sprintf(op, "adcx b"); break;
		case 0xd2: sprintf(op, "adcx d"); break;
		case 0xd3: sprintf(op, "adcx h"); break;
		case 0xd4: sprintf(op, "adcx d+"); break;
		case 0xd5: sprintf(op, "adcx h+"); break;
		case 0xd6: sprintf(op, "adcx d-"); break;
		case 0xd7: sprintf(op, "adcx h-"); break;
		case 0xd9: sprintf(op, "offax b"); break;
		case 0xda: sprintf(op, "offax d"); break;
		case 0xdb: sprintf(op, "offax h"); break;
		case 0xdc: sprintf(op, "offax d+"); break;
		case 0xdd: sprintf(op, "offax h+"); break;
		case 0xde: sprintf(op, "offax d-"); break;
		case 0xdf: sprintf(op, "offax h-"); break;
		case 0xe1: sprintf(op, "subx b"); break;
		case 0xe2: sprintf(op, "subx d"); break;
		case 0xe3: sprintf(op, "subx h"); break;
		case 0xe4: sprintf(op, "subx d+"); break;
		case 0xe5: sprintf(op, "subx h+"); break;
		case 0xe6: sprintf(op, "subx d-"); break;
		case 0xe7: sprintf(op, "subx h-"); break;
		case 0xe9: sprintf(op, "neax b"); break;
		case 0xea: sprintf(op, "neax d"); break;
		case 0xeb: sprintf(op, "neax h"); break;
		case 0xec: sprintf(op, "neax d+"); break;
		case 0xed: sprintf(op, "neax h+"); break;
		case 0xee: sprintf(op, "neax d-"); break;
		case 0xef: sprintf(op, "neax h-"); break;
		case 0xf1: sprintf(op, "sbbx b"); break;
		case 0xf2: sprintf(op, "sbbx d"); break;
		case 0xf3: sprintf(op, "sbbx h"); break;
		case 0xf4: sprintf(op, "sbbx d+"); break;
		case 0xf5: sprintf(op, "sbbx h+"); break;
		case 0xf6: sprintf(op, "sbbx d-"); break;
		case 0xf7: sprintf(op, "sbbx h-"); break;
		case 0xf9: sprintf(op, "eqax b"); break;
		case 0xfa: sprintf(op, "eqax d"); break;
		case 0xfb: sprintf(op, "eqax h"); break;
		case 0xfc: sprintf(op, "eqax d+"); break;
		case 0xfd: sprintf(op, "eqax h+"); break;
		case 0xfe: sprintf(op, "eqax d-"); break;
		case 0xff: sprintf(op, "eqax h-"); break;
		default: sprintf(op, "db 70h,%2xh", b);
		}
		break;
	case 0x71: wa = getwa(); sprintf(op, "mviw v.%xh,%2xh", wa, getb()); break;
	case 0x72: sprintf(op, "softi"); break;
	case 0x73: sprintf(op, "jb"); break;
	case 0x74:
		switch(b = getb())
		{
		case 0x88: sprintf(op, "anaw v.%x", getwa()); break;
		case 0x90: sprintf(op, "xraw v.%x", getwa()); break;
		case 0x98: sprintf(op, "oraw v.%x", getwa()); break;
		case 0xa0: sprintf(op, "addncw v.%x", getwa()); break;
		case 0xa8: sprintf(op, "gtaw v.%x", getwa()); break;
		case 0xb0: sprintf(op, "subnbw v.%x", getwa()); break;
		case 0xb8: sprintf(op, "ltaw v.%x", getwa()); break;
		case 0xc0: sprintf(op, "addw v.%x", getwa()); break;
		case 0xc8: sprintf(op, "onaw v.%x", getwa()); break;
		case 0xd0: sprintf(op, "adcw v.%x", getwa()); break;
		case 0xd8: sprintf(op, "offaw v.%x", getwa()); break;
		case 0xe0: sprintf(op, "subw v.%x", getwa()); break;
		case 0xe8: sprintf(op, "neaw v.%x", getwa()); break;
		case 0xf0: sprintf(op, "sbbw v.%x", getwa()); break;
		case 0xf8: sprintf(op, "eqaw v.%x", getwa()); break;
		default: sprintf(op, "db 74h,%2xh", b);
		}
		break;
	case 0x75: wa = getwa(); sprintf(op, "eqiw v.%xh,%2xh", wa, getb()); break;
	case 0x76: sprintf(op, "sbi a,%2xh", getb()); break;
	case 0x77: sprintf(op, "eqi a,%2xh", getb()); break;
	case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7e: case 0x7f:
		sprintf(op, "calf %3x", 0x800 | ((b & 7) << 8) | getb()); break;
	
	case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87:
	case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8e: case 0x8f:
	case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
	case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9e: case 0x9f:
	case 0xa0: case 0xa1: case 0xa2: case 0xa3: case 0xa4: case 0xa5: case 0xa6: case 0xa7:
	case 0xa8: case 0xa9: case 0xaa: case 0xab: case 0xac: case 0xad: case 0xae: case 0xaf:
	case 0xb0: case 0xb1: case 0xb2: case 0xb3: case 0xb4: case 0xb5: case 0xb6: case 0xb7:
	case 0xb8: case 0xb9: case 0xba: case 0xbb: case 0xbc: case 0xbd: case 0xbe: case 0xbf:
		sprintf(op, "calt %2xh", 0x80 | ((b & 0x3f) << 1)); break;
		
	case 0xc0: case 0xc1: case 0xc2: case 0xc3: case 0xc4: case 0xc5: case 0xc6: case 0xc7:
	case 0xc8: case 0xc9: case 0xca: case 0xcb: case 0xcc: case 0xcd: case 0xce: case 0xcf:
	case 0xd0: case 0xd1: case 0xd2: case 0xd3: case 0xd4: case 0xd5: case 0xd6: case 0xd7:
	case 0xd8: case 0xd9: case 0xda: case 0xdb: case 0xdc: case 0xdd: case 0xde: case 0xdf:
		sprintf(op, "jr %4xh", pc + (b & 0x1f)); break;
	
	case 0xe0: case 0xe1: case 0xe2: case 0xe3: case 0xe4: case 0xe5: case 0xe6: case 0xe7:
	case 0xe8: case 0xe9: case 0xea: case 0xeb: case 0xec: case 0xed: case 0xee: case 0xef:
	case 0xf0: case 0xf1: case 0xf2: case 0xf3: case 0xf4: case 0xf5: case 0xf6: case 0xf7:
	case 0xf8: case 0xf9: case 0xfa: case 0xfb: case 0xfc: case 0xfd: case 0xfe: case 0xff:
		sprintf(op, "jr %4xh", pc + ((b & 0x1f) - 0x20)); break;
	
	default: sprintf(op, "db %2xh", b); break;
	}
	
	if(pc - prv == 4)
		sprintf(out, "%4x\t%2x,%2x,%2x,%2x\t%c%c%c%c\t%s", prv, b0,b1,b2,b3, asc(b0),asc(b1),asc(b2),asc(b3), op);
	else if(pc - prv == 3)
		sprintf(out, "%4x\t%2x,%2x,%2x   \t%c%c%c\t%s", prv, b0,b1,b2, asc(b0),asc(b1),asc(b2), op);
	else if(pc - prv == 2)
		sprintf(out, "%4x\t%2x,%2x      \t%c%c\t%s", prv, b0,b1, asc(b0),asc(b1), op);
	else
		sprintf(out, "%4x\t%2x         \t%c\t%s", prv, b0, asc(b0), op);
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
		printf("\to : top address of srcfile (0000-ffff, default=8000)\n");
		printf("\ts : start address to be disassembled (0000-ffff, default=8001)\n");
		printf("\tl : last address to be disassembled (0000-ffff, default=ffff)\n"); 
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
