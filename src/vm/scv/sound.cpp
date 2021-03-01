/*
	EPOCH Super Cassette Vision Emulator 'eSCV'

	Author : Takeda.Toshiya
	Date   : 2006.08.21 -

	Modified for Libretro-EmuSCV
	Author : MARCONATO Maxime (aka MaaaX)
	Date   : 2019-12-05 - 

	[ NEC uPD1771C ]
*/

#include <math.h>
#include "sound.h"
#include "sound_tbl.h"
#include "../upd7801.h"
#include "../state.h"

//#define SOUND_DEBUG
#define ACK_WAIT 100

void SOUND::reset()
{
	touch_sound();

	clear_channel(&tone);
	clear_channel(&noise);
	clear_channel(&square1);
	clear_channel(&square2);
	clear_channel(&square3);
	clear_channel(&pcm);

	memset(params, 0, sizeof(params));
	param_cnt = param_ptr = 0;
	register_id = -1;
	cmd_addr = 0;
}

void SOUND::write_data8(uint32_t addr, uint32_t data)
{
	if(register_id != -1)
		return; // ignore new commands before return ack

	if(param_cnt <= 0)
	{
		// new command
		touch_sound();
		switch(data)
		{
			case CMD_SILENCE:
				param_cnt = 1;
				break;
				
			case CMD_NOISE:
				param_cnt = 10;
				break;

			case CMD_TONE:
				param_cnt = 4;
				break;

			case CMD_PCM:
				param_cnt = MAX_PARAM;
				memset(pcm_table, 0, PCM_TABLE_SIZE+8);
				memset(pcm_table_smooth, 0, PCM_TABLE_SMOOTH_SIZE);
				pcm_len = pcm.ptr = 0;
				pcm_table_smooth_index = 0;
				break;
		}
		param_ptr = 0;
		cmd_addr  = get_cpu_pc(0); // for patch
#ifdef SOUND_DEBUG
		this->out_debug_log(_T("PC=%4x\tSOUND\t"), cmd_addr);
#endif
	}

#ifdef SOUND_DEBUG
	this->out_debug_log(_T("%2x "), data);
#endif
	if(param_cnt > 0)
	{
		params[param_ptr++] = data;

		if(params[0] == 0x1f)
		{
			// pcm command
			if(param_ptr == 5)
			{
				pcm.period = 240000 - (params[1] & 0x06) * 17500;
//				params[1] 0?, 4, 6, 100?
//				params[2] 100?
//				params[3] 0, 1, 3
//				param[4] = begin of data?
			}

			// Patch for Kung-Fu Road
			if(param_ptr == 2 && (params[1] != 4 && params[1] != 6 && params[1] != 100))
			{
				param_cnt = 1;
				clear_channel(&pcm);
			}
			else if(param_ptr >= 7)
			{
				// Patch for Star Speeder: 0xfe,0x00 = end of pcm, intf1 must not be done except star speeder
				if(params[param_ptr - 2] == 0xfe && data == 0x00 && cmd_addr != 0xa765)
				{
					param_cnt = 1;
				}
				else
					// Add pcm wave to buffer
					if(pcm_len < PCM_TABLE_SIZE-8)
					{
						uint8_t data = params[param_ptr - 2];
						// Smooth sound
						pcm_table[pcm_len++] = (data & 0x80) ? MAX_PCM : 0;
						pcm_table[pcm_len++] = (data & 0x40) ? MAX_PCM : 0;
						pcm_table[pcm_len++] = (data & 0x20) ? MAX_PCM : 0;
						pcm_table[pcm_len++] = (data & 0x10) ? MAX_PCM : 0;
						pcm_table[pcm_len++] = (data & 0x08) ? MAX_PCM : 0;
						pcm_table[pcm_len++] = (data & 0x04) ? MAX_PCM : 0;
						pcm_table[pcm_len++] = (data & 0x02) ? MAX_PCM : 0;
						pcm_table[pcm_len++] = (data & 0x01) ? MAX_PCM : 0;

						if(pcm.count == 0)
						{
							pcm.count  = 1;
							pcm.output = pcm_table[0];
						}
					}
			}
		}

		if(--param_cnt)
		{
			if(register_id != -1)
				cancel_event(this, register_id);
			register_event(this, 0, ACK_WAIT, false, &register_id);
		}
	}

	// Process command
	if(param_cnt <= 0)
	{
		touch_sound();
		switch(params[0])
		{

			case CMD_SILENCE:	// Note off
				clear_channel(&tone);
				clear_channel(&noise);
				clear_channel(&square1);
				clear_channel(&square2);
				clear_channel(&square3);
				break;

			case CMD_NOISE:	// Noise & square
				noise.timbre = (params[1] & 0xE0) << 1;
				//noise.offset = (params[1] & 0x1F) << 3;
				noise.ptr    = rand() % (NOISE_TABLE_SIZE - 1);
				noise.period = params[2] << 6;
				noise.volume = MAX_NOISE * (params[3] & 0x1F) / 0x1F;
				if(noise.period)
					noise.output = (noise_table[noise.ptr] * noise.volume) >> 8;
				else
					noise.output = (noise_table[noise.ptr] * noise.volume) >> 9;

				square1.volume = MAX_SQUARE1 * (params[7] & 0x1F) / 0x1F;
				square2.volume = MAX_SQUARE2 * (params[8] & 0x1F) / 0x1F;
				square3.volume = MAX_SQUARE3 * (params[9] & 0x1F) / 0x1F;
				if(params[1] == 0 && params[3] == 0)
				{
					square1.period = params[4] << 7;
					if(params[4] > 1 && params[4] < 12)	// Patch for Kung-Fu Road... Not perfect
						square2.period = (params[5] * params[4]) << 2;
					else
						square2.period = params[5] << 8;
				}
				else
				{
					square1.period = params[4] << 7;
					square2.period = params[5] << 8;
					square1.volume = square1.volume >> 1;
					square2.volume = square2.volume >> 1;
					square3.volume = square3.volume >> 1;
				}
				square3.period = params[6] << 8;
				square1.output = (square_table[square1.ptr] * square1.volume) >> 8;
				square2.output = (square_table[square2.ptr] * square2.volume) >> 8;
				square3.output = (square_table[square3.ptr] * square3.volume) >> 8;

				// tone off
				clear_channel(&tone);
				break;

			case CMD_TONE:	// note on : $02, timbre, period, volume ?
				tone.timbre = (params[1] & 0xE0) >> 5;
				tone.offset = params[1] & 0x1F;
				tone.period = params[2] * detune_table[tone.offset];
				tone.volume = MAX_TONE * (params[3] & 0x1F) / 0x1F;
				tone.output = (timbre_table[tone.timbre][tone.ptr] * tone.volume) >> 8;

				// noise & square off
				clear_channel(&noise);
				clear_channel(&square1);
				clear_channel(&square2);
				clear_channel(&square3);
				break;

			case CMD_PCM:	// PCM
			default:
				break;
		}

		// clear command buffer
		memset(params, 0, MAX_PARAM);
		param_cnt = param_ptr = 0;

#ifdef SOUND_DEBUG
		this->out_debug_log(_T("\n"));
#endif
	}
}

void SOUND::write_io8(uint32_t addr, uint32_t data)
{
	// PC3 : L->H
	if(data & 0x08)
	{
		// note off
		touch_sound();
		clear_channel(&tone);
		clear_channel(&noise);
		clear_channel(&square1);
		clear_channel(&square2);
		clear_channel(&square3);

// Patch for y2 monster land
		if(cmd_addr == 0x8402)
		{
			if(!(params[0] == 0x1f && param_ptr > 5))
			{
				// terminate command
				if(register_id != -1)
					cancel_event(this, register_id);
				memset(params, 0, sizeof(params));
				param_cnt = param_ptr = 0;
			}
		}
		else
		{
			if(params[0])
			{
				// terminate command
				memset(params, 0, sizeof(params));
				param_cnt = param_ptr = 0;
			}
		}

#ifdef SOUND_DEBUG
		this->out_debug_log(_T("PC3\n"));
#endif
	}
}

void SOUND::event_callback(int event_id, int err)
{
	if(pcm.count && param_ptr == 5 && params[0] == CMD_PCM && params[1] == 0x04 && params[2] == 0x64)
	{
		// wait previous pcm
		register_event(this, 0, ACK_WAIT, false, &register_id);
	}
	else
	{
		d_cpu->write_signal(SIG_UPD7801_INTF1, 1, 1);
		register_id = -1;
	}
}

void SOUND::initialize_sound(int rate)
{
	// Clock difference
	tone.diff    = (int)((TONE_CLOCK   / rate) * 128.0 * 16.0 + 0.5);
	noise.diff   = (int)((NOISE_CLOCK  / rate) * 128.0 * 16.0 + 0.5);
	square1.diff = (int)((SQUARE_CLOCK / rate) * 128.0 * 16.0 + 0.5);
	square2.diff = (int)((SQUARE_CLOCK / rate) * 128.0 * 16.0 + 0.5);
	square3.diff = (int)((SQUARE_CLOCK / rate) * 128.0 * 16.0 + 0.5);
	pcm.diff     = (int)((PCM_CLOCK    / rate) * 128.0 * 16.0 + 0.5);

	// create volume table
	double vol = MAX_TONE;
	for(int i = 0; i < 32; i++)
	{
		volume_table[31 - i] = (int) vol;
		vol /= 1.12201845439369;
	}
	volume_table[0] = 0;

	// create detune table
	for(int i = 31; i >= 0; i--)
		detune_table[i] = (int) (detune_rate[i] * 256 / 100 + 0.5);

	// Create noise table
	for(int i = NOISE_TABLE_SIZE-1; i >= 0; i--)
		noise_table[i] = (rand() % 510) - 255; 

	// Create square table
	for(int i = 0; i < SQUARE_TABLE_SIZE>>1; i++)
		square_table[i] = 255;
	for(int i = SQUARE_TABLE_SIZE>>1; i < SQUARE_TABLE_SIZE; i++)
		square_table[i] = -255;
}

inline void SOUND::clear_channel(channel_t *ch)
{
	ch->count  = 0;
	ch->offset = 0;
	ch->output = 0;
	ch->period = 0;
	ch->ptr    = 0;
	ch->timbre = 0;
	ch->volume = 0;
}

void SOUND::mix(int16_t* buffer, uint32_t cnt)
{
	int64_t vol;


	// create sound buffer
	// mix pcm
	if(pcm.count != 0)
	{
		for(int i = cnt+1; --i != 0; )
		{
			vol = 0;

			pcm.count -= pcm.diff;
			while(pcm.count <= 0)
			{
				pcm.count += pcm.period;
				for(int i = PCM_TABLE_DATA_SIZE - 1; i >= 0; i--)
				{
					if(pcm.ptr + i < pcm_len)
						pcm_table_data[i] = pcm_table[pcm.ptr + i];
					else
						pcm_table_data[i] = 0;
				}
				pcm.output =  (pcm_table_data[0] + pcm_table_data[1] << 1 + pcm_table_data[2] + pcm_table_data[2] << 1 + pcm_table_data[3] << 2 + pcm_table_data[4] << 2 + pcm_table_data[5] + pcm_table_data[5] << 1 + pcm_table_data[6] << 1 + pcm_table_data[7]) >> 8;
				if(++pcm.ptr >= pcm_len)
				{
					pcm.count = 0;
					break;
				}
			}
			// Smooth sound
			pcm_table_smooth[pcm_table_smooth_index] = pcm.output;
			if(++pcm_table_smooth_index >= PCM_TABLE_SMOOTH_SIZE)
				pcm_table_smooth_index = 0;
			int64_t v = pcm_table_smooth[0];
			for(int i = PCM_TABLE_SMOOTH_SIZE-1; --i > 0; )
				v += pcm_table_smooth[i];
			vol += v>>3;	// Quick optimization for: vol += v/PCM_TABLE_SMOOTH_SIZE;

			// Mix
			*buffer++ = vol;	// Mono
		}
	}
	// mix tone
	else// if(tone.volume && tone.period)
	{
		for(int i = cnt+1; --i != 0; )
		{
			vol = 0;

			// mix tone
			if(tone.volume && tone.period)
			{
				tone.count -= tone.diff;
				while(tone.count <= 0)
				{
					tone.count += tone.period;
					if(++tone.ptr >= 256)
						tone.ptr = tone.offset;
					tone.output = (timbre_table[tone.timbre][tone.ptr] * tone.volume) >> 8;
				}
				vol += tone.output;
			}

			// Mix noise
			if(noise.volume)
			{
				if(noise.period)
				{		
					noise.count -= noise.diff;
					while(noise.count <= 0)
					{
						noise.count += noise.period;
						noise.ptr    = (noise.ptr + 1) & (NOISE_TABLE_SIZE - 1);
						noise.output = (noise_table[noise.ptr] * noise.volume) >> 8;
					}
				}
				else if(noise.timbre)	// Patch for Star Speeder
				{
					noise.count -= noise.diff;
					while(noise.count <= 0)
					{
						noise.count += noise.timbre;
						noise.ptr    = (noise.ptr + 1) & (NOISE_TABLE_SIZE - 1);
						noise.output = (noise_table[noise.ptr] * noise.volume) >> 9;
					}
				}
				vol += noise.output;
			}

			// Mix square 1
			if(square1.volume && square1.period)
			{
				square1.count -= square1.diff;
				while(square1.count <= 0)
				{
					square1.count  += square1.period;
					square1.ptr     = (square1.ptr + 1) & 0xFF;
					square1.output  = (square_table[square1.ptr] * square1.volume) >> 8;
				}
				vol += square1.output;
			}

			// Mix square 2
			if(square2.volume && square2.period)
			{
				square2.count -= square2.diff;
				while(square2.count <= 0)
				{
					square2.count  += square2.period;
					square2.ptr     = (square2.ptr + 1) & 0xFF;
					square2.output  = (square_table[square2.ptr] * square2.volume) >> 8;
				}
				vol += square2.output;
			}

			// Mix square 3
			if(square3.volume && square3.period)
			{
				square3.count -= square3.diff;
				while(square3.count <= 0)
				{
					square3.count  += square3.period;
					square3.ptr     = (square3.ptr + 1) & 0xFF;
					square3.output  = (square_table[square3.ptr] * square3.volume) >> 8;
				}
				vol += square3.output;
			}

			*buffer++ = vol;	// Mono
		}
	}
}

void save_channel_state(STATE* state, channel_t* val)
{
	state->SetValue(val->count);
	state->SetValue(val->diff);
	state->SetValue(val->period);
	state->SetValue(val->timbre);
	state->SetValue(val->volume);
	state->SetValue(val->output);
	state->SetValue(val->ptr);
}

void load_channel_state(STATE* state, channel_t* val)
{
	state->GetValue(val->count);
	state->GetValue(val->diff);
	state->GetValue(val->period);
	state->GetValue(val->timbre);
	state->GetValue(val->volume);
	state->GetValue(val->output);
	state->GetValue(val->ptr);
}

#define SOUND_STATE_ID	701

void SOUND::save_state(STATE* state)
{
	state->SetValue((uint16_t)SOUND_STATE_ID);
	state->SetValue(this_device_id);
	save_channel_state(state, &tone);
	save_channel_state(state, &noise);
	save_channel_state(state, &square1);
	save_channel_state(state, &square2);
	save_channel_state(state, &square3);
	save_channel_state(state, &pcm);
	state->SetValue(pcm_len);
	if(pcm.count > 0 && pcm_len > 0 && pcm_len - pcm.ptr > 0)
		state->SetArray(&pcm_table[pcm.ptr], pcm_len - pcm.ptr, 1);
	state->SetValue(cmd_addr);
	state->SetValue(param_cnt);
	state->SetValue(param_ptr);
	state->SetValue(register_id);
	state->SetArray(params, sizeof(params), 1);
}

bool SOUND::load_state(STATE* state)
{
	if(!state->CheckValue((uint16_t)SOUND_STATE_ID))
		return false;
	if(!state->CheckValue(this_device_id))
		return false;
	load_channel_state(state, &tone);
	load_channel_state(state, &noise);
	load_channel_state(state, &square1);
	load_channel_state(state, &square2);
	load_channel_state(state, &square3);
	load_channel_state(state, &pcm);
	state->GetValue(pcm_len);
	if(pcm.count > 0 && pcm_len > 0 && pcm_len - pcm.ptr > 0)
		state->GetArray(&pcm_table[pcm.ptr], pcm_len - pcm.ptr, 1);
	state->GetValue(cmd_addr);
	state->GetValue(param_cnt);
	state->GetValue(param_ptr);
	state->GetValue(register_id);
	state->GetArray(params, sizeof(params), 1);

	return true;
}
