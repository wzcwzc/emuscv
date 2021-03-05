/*
	EPOCH Super Cassette Vision Emulator 'eSCV'

	Author : Takeda.Toshiya
	Date   : 2006.08.21 -

	Modified for Libretro-EmuSCV
	Author : MARCONATO Maxime (aka MaaaX)
	Date   : 2019-12-05 - 

	[ NEC uPD1771C ]
*/

#ifndef _EMUSCV_INC_VM_SCV_SOUND_H_
#define _EMUSCV_INC_VM_SCV_SOUND_H_

#include "../vm.h"
#include "../../emu.h"
#include "../device.h"

#define TONE_CLOCK    1500000.0
#define NOISE_CLOCK       750.0
#define SQUARE_CLOCK   175000.0
#define PCM_CLOCK     1500000.0

#define MAX_TONE          12000
#define MAX_NOISE          9000
#define MAX_SQUARE1        1000
#define MAX_SQUARE2        1000
#define MAX_SQUARE3        1000
#define MAX_PCM           12000

#define MAX_PARAM        0x8000

#define NOISE_TABLE_SIZE   1024
#define SQUARE_TABLE_SIZE   256
#define PCM_TABLE_SIZE  0x40000
#define PCM_TABLE_DATA_SIZE   8
#define PCM_TABLE_SMOOTH_SIZE 8

#define CMD_SILENCE	       0x00
#define CMD_NOISE	       0x01
#define CMD_TONE	       0x02
#define CMD_PCM		       0x1F

typedef struct
{
	int count;
	int diff;
	int offset;
	int period;
	int timbre;
	int volume;
	int16_t output;
	uint32_t ptr;
} channel_t;

class SOUND : public DEVICE
{
private:
	DEVICE* d_cpu;

	// sound gen
	channel_t tone;
	channel_t noise;
	channel_t square1;
	channel_t square2;
	channel_t square3;
	channel_t pcm;
	inline void clear_channel(channel_t *ch);

	int16_t pcm_table[PCM_TABLE_SIZE+8];
	uint32_t cmd_addr;
	uint32_t pcm_len;
	int16_t pcm_table_data[PCM_TABLE_DATA_SIZE];
	int16_t pcm_table_smooth[PCM_TABLE_SMOOTH_SIZE];
	int8_t pcm_table_smooth_index;

	int16_t volume_table[32];
	int16_t detune_table[32];

	// command buffer
	uint32_t param_cnt;
	uint32_t param_ptr;
	int register_id;
	uint8_t params[MAX_PARAM];

	int16_t noise_table[NOISE_TABLE_SIZE];
	int16_t square_table[SQUARE_TABLE_SIZE];

public:
	SOUND(VM_TEMPLATE* parent_vm, EMU* parent_emu) : DEVICE(parent_vm, parent_emu)
	{
		set_device_name(_T("Sound"));
	}
	~SOUND() {}

	// common functions
	void reset();
	void write_data8(uint32_t addr, uint32_t data);
	void write_io8(uint32_t addr, uint32_t data);
	void event_callback(int event_id, int err);
	void mix(int16_t* buffer, uint32_t cnt);
	void save_state(STATE* state, bool max_size);
	bool load_state(STATE* state);

	// unique functions
	void set_context_cpu(DEVICE* device)
	{
		d_cpu = device;
	}
	void initialize_sound(int rate);
};

#endif	// _EMUSCV_INC_VM_SCV_SOUND_H_
