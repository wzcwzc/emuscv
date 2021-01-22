/*
	Skelton for retropc emulator

	Author : Takeda.Toshiya
	Date   : 2006.09.17-

	Modified for Libretro-EmuSCV
	Author : MARCONATO Maxime (aka MaaaX)
	Date   : 2019-12-05 - 

	[ fifo buffer ]
*/

#ifndef _EMUSCV_INC_FIFO_H_
#define _EMUSCV_INC_FIFO_H_

#include "common.h"

class DLL_PREFIX FIFO
{
private:
	int size;
	int* buf;
	int cnt, rpt, wpt;
public:
	FIFO(int s);
	void release();
	void clear();
	void write(int val);
	int read();
	int read_not_remove(int pt);
	void write_not_push(int pt, int d);
	int count();
	bool full();
	bool empty();
	bool process_state(void *f, bool loading);
};

#endif	// _EMUSCV_INC_FIFO_H_

