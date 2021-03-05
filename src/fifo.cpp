/*
	Skelton for retropc emulator

	Author : Takeda.Toshiya
	Date   : 2014.12.19-

	Modified for Libretro-EmuSCV
	Author : MARCONATO Maxime (aka MaaaX)
	Date   : 2019-12-05 - 

	[ fifo buffer ]
*/

#include <stdlib.h>
#ifndef _OSX 
	#include <malloc.h>
#endif
#include "fifo.h"
#include "fileio.h"
#include "state.h"

FIFO::FIFO(int s)
{
	size = s;
	buf = (int*)malloc(size * sizeof(int));
	cnt = rpt = wpt = 0;
}

void FIFO::release()
{
	free(buf);
}

void FIFO::clear()
{
	cnt = rpt = wpt = 0;
}

void FIFO::write(int val)
{
	if(cnt < size) {
		buf[wpt++] = val;
		if(wpt >= size) {
			wpt = 0;
		}
		cnt++;
	}
}

int FIFO::read()
{
	int val = 0;
	if(cnt) {
		val = buf[rpt++];
		if(rpt >= size) {
			rpt = 0;
		}
		cnt--;
	}
	return val;
}

int FIFO::read_not_remove(int pt)
{
	if(pt >= 0 && pt < cnt) {
		pt += rpt;
		if(pt >= size) {
			pt -= size;
		}
		return buf[pt];
	}
	return 0;
}

void FIFO::write_not_push(int pt, int d)
{
	if(pt >= 0 && pt < cnt) {
		pt += wpt;
		if(pt >= size) {
			pt -= size;
		}
		buf[pt] = d;
	}
}

int FIFO::count()
{
	return cnt;
}

bool FIFO::full()
{
	return (cnt == size);
}

bool FIFO::empty()
{
	return (cnt == 0);
}

#define FIFO_STATE_ID	1101

void FIFO::save_state(STATE *state, bool max_size)
{
	state->SetValue((uint16_t)FIFO_STATE_ID);
	state->SetValue(size);
	state->SetArray(buf, size * sizeof(int), 1);
	state->SetValue(cnt);
	state->SetValue(rpt);
	state->SetValue(wpt);
}

bool FIFO::load_state(STATE *state)
{
	if(!state->CheckValue((uint16_t)FIFO_STATE_ID))
		return false;
	state->GetValue(size);
	state->GetArray(buf, size * sizeof(int), 1);
	state->GetValue(cnt);
	state->GetValue(rpt);
	state->GetValue(wpt);
	return true;
}
