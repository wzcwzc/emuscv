/*
	EPOCH Super Cassette Vision Emulator 'eSCV'

	Author : Takeda.Toshiya
	Date   : 2006.08.21 -

	[ uPD1771C ]
*/

#include <math.h>
#include "sound.h"
#include "sound_tbl.h"
#include "../upd7801.h"

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
				memset(pcm_table, 0, PCM_TABLE_SIZE);
				//memset(pcm_table_smooth, 0, sizeof(pcm_table_smooth));
				//memset(pcm_smooth, 0, sizeof(pcm_smooth));
				pcm_len = pcm.ptr = 0;
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
//				if(params[1] != 4 && params[1] != 6 && params[1] != 100)
//					bool stop = 1;
				pcm.period = 240000 - (params[1] & 0x06) * 17500;
//				//params[1] 0?, 4, 6, 100?
//				//params[2] 100?
//				//params[3] 0, 1, 3
//				param[4] = begin of data?
			}
/*
			// Patch for Kung-Fu Road
			if(param_ptr == 2 && (params[1] != 4 && params[1] != 6 && params[1] != 100))
			{
				param_cnt = 1;
				clear_channel(&pcm);
			}
			else 
*/
			if(param_ptr >= 7)
			{
				// Patch for Star Speeder: 0xfe,0x00 = end of pcm, intf1 must not be done except star speeder
				if(params[param_ptr - 2] == 0xfe && data == 0x00 && cmd_addr != 0xa765)
					param_cnt = 1;
				else
					uint8_t data = params[param_ptr - 2];
					// Add pcm wave to buffer
					if(pcm_len < PCM_TABLE_SIZE-8)
					{
						// Smooth sound
/*
						uint8_t v;
						pcm_table_smooth[0] = (data & 0x80) ? 1 : 0;
						pcm_table_smooth[1] = (data & 0x40) ? 1 : 0;
						pcm_table_smooth[2] = (data & 0x20) ? 1 : 0;
						pcm_table_smooth[3] = (data & 0x10) ? 1 : 0;
						pcm_table_smooth[4] = (data & 0x08) ? 1 : 0;
						pcm_table_smooth[5] = (data & 0x04) ? 1 : 0;
						pcm_table_smooth[6] = (data & 0x02) ? 1 : 0;
						if(pcm_len != 0)
						{
							v = ((pcm_table_smooth[7] << 1 + pcm_table_smooth[7]) + (pcm_table_smooth[0] << 1)) >> 2;
							pcm_table[pcm_len - 1] = (v == 0 ? 0 : 1);
						}
						pcm_table_smooth[7] = (data & 0x01) ? 1 : 0;
						for(uint8_t i = 0; i < 8; i++)
						{
							if(i < 1 && pcm_len == 0)
								v = (pcm_table_smooth[i] >> 1 + pcm_table_smooth[i + 1]) / 3;
							else if(i < 7)
								v = (pcm_table_smooth[i - 1] + pcm_table_smooth[i] >> 1 + pcm_table_smooth[i + 1]) >> 2;
							else
								v = (pcm_table_smooth[i - 1] + pcm_table_smooth[i] >> 1) / 3;
							pcm_table[pcm_len + i] = (v == 0 ? 0 : 1);
						}
						pcm_len += 8;
*/
						pcm_table[pcm_len++] = (data & 0x80) ? 1 : 0;
						pcm_table[pcm_len++] = (data & 0x40) ? 1 : 0;
						pcm_table[pcm_len++] = (data & 0x20) ? 1 : 0;
						pcm_table[pcm_len++] = (data & 0x10) ? 1 : 0;
						pcm_table[pcm_len++] = (data & 0x08) ? 1 : 0;
						pcm_table[pcm_len++] = (data & 0x04) ? 1 : 0;
						pcm_table[pcm_len++] = (data & 0x02) ? 1 : 0;
						pcm_table[pcm_len++] = (data & 0x01) ? 1 : 0;

						if(!pcm.count)
						{
							pcm.count  = pcm.diff;
							pcm.output = pcm_table[pcm_len - 8];
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
		bool stop;
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
				noise.timbre = (params[1] & 0xe0) >> 5;
				noise.offset = (params[1] & 0x1f);
				noise.ptr    = noise.offset;
				noise.period = params[2] << 8;
				//noise.period = ((uint32_t)params[2] + 1) << 7;
				noise.volume = (MAX_NOISE * (params[3] > 0x1f ? 0x1f : params[3])) / 0x1f;
				//noise.volume = volume_table[params[3] & 0x1f];
				noise.output = (noise_table[noise.ptr] * noise.volume) >> 8;

				square1.period = params[4] << 8;
				square1.volume = (MAX_SQUARE * (params[7] > 0x7f ? 0x7f : params[7])) / 0x7f;
				square1.output = (square_table[square1.ptr] * square1.volume) >> 8;

				square2.period = params[5] << 8;
				square2.volume = (MAX_SQUARE * (params[8] > 0x7f ? 0x7f : params[8])) / 0x7f;
				square2.output = (square_table[square2.ptr] * square2.volume) >> 8;

				square3.period = params[6] << 8;
				square3.volume = (MAX_SQUARE * (params[9] > 0x7f ? 0x7f : params[9])) / 0x7f;
				square3.output = (square_table[square3.ptr] * square3.volume) >> 8;

				// tone off
				clear_channel(&tone);
				break;

			case CMD_TONE:	// note on : $02, timbre, period, volume ?
				tone.timbre = (params[1] & 0xe0) >> 5;
				tone.offset = (params[1] & 0x1f);
				tone.ptr    = tone.offset;
				tone.period = (params[2] * detune_table[tone.offset]);
				tone.volume = volume_table[params[3] & 0x1f];
				tone.output = (timbre_table[tone.timbre][tone.ptr] * tone.volume) >> 8;

				// noise & square off
				clear_channel(&noise);
				clear_channel(&square1);
				clear_channel(&square2);
				clear_channel(&square3);
				break;

			case CMD_PCM:	// PCM
//				clear_channel(&pcm);
				break;

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

		if(cmd_addr == 0x8402)
		{
// PATCH y2 monster land
//			bool pause = false;//(get_cpu_pc(0) == 0x96c);
//			if(pause || !(params[0] == 0x1f && param_ptr > 5))
			if(!(params[0] == 0x1f && param_ptr > 5))
			{
				// terminate command
				if(register_id != -1)
					cancel_event(this, register_id);
				memset(params, 0, sizeof(params));
				param_cnt = param_ptr = 0;

				// terminate pcm when pause
// PATCH y2 monster land
//				if(pause)
//					clear_channel(&pcm);
//			} else if(register_id == -1) {
//				vm->register_callback(this, 0, 100, false, &register_id);
			}
		}
		else
		{
			if(params[0])
			{
				// terminate command
				memset(params, 0, sizeof(params));
				param_cnt = param_ptr = 0;
//				clear_channel(&pcm);
			}
//			if(register_id == -1)
//				vm->register_callback(this, 0, 100, false, &register_id);
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
		vol /= 1.12201845439369;//1.258925412;
	}
	volume_table[0] = 0;

	// create detune table
	for(int i = 0; i < 32; i++)
		detune_table[i] = (int) (detune_rate[i] * 256.0 / 100.0 + 0.5);
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

void SOUND::mix(int32_t* buffer, int cnt)
{
	float c, v;
	int vol, vol_l, vol_r;
	int o01, o03, o05, o07, o09, o11, o13, o15, o17, o19;


	// create sound buffer
	for(int i = 0; i < cnt; i++)
	{
		vol = vol_l = vol_r = 0;

		// mix pcm
		if(pcm.count)
		{
			pcm.count -= pcm.diff;
			while(pcm.count <= 0)
			{
				pcm.count += pcm.period;

				if(pcm.ptr < pcm_len)
				{
					// Smooth
//					for(uint8_t i = 0; i < sizeof(pcm_smooth); i++)
//					{
//						if(pcm.ptr + i - 9 < 0 && pcm.ptr + i + 9 >= pcm_len)
//							pcm_smooth[i] = 0;
//						else
//							pcm_smooth[i] = pcm_table[pcm.ptr + i];
//					}
/*
					pcm.output = +MAX_PCM*(pcm_table[pcm.ptr]+pcm_table[pcm.ptr + 1]+pcm_table[pcm.ptr + 2]+pcm_table[pcm.ptr + 3])/4;
					pcm.output = +MAX_PCM*(pcm_table[pcm.ptr]+pcm_table[pcm.ptr + 1]+pcm_table[pcm.ptr + 2]+pcm_table[pcm.ptr + 3]+pcm_table[pcm.ptr + 4]+pcm_table[pcm.ptr + 5]+pcm_table[pcm.ptr + 6]+pcm_table[pcm.ptr + 7])/8;
*/
					for(uint8_t i = 0; i < 19; i++)
					{
/*
						v = c = 0;
						if(pcm.ptr + i - 1 >= 0)
						{
							v += pcm_table[pcm.ptr + i - 1];
							c++;
						}
						if(pcm.ptr + i < pcm_len)
						{
							v += 2*pcm_table[pcm.ptr + i];
							c += 2;
						}
						if(pcm.ptr + i + 1 < pcm_len)
						{
							v += pcm_table[pcm.ptr + i + 1];
							c++;
						}
						if(c != 0)
							v = v / c;
						if(v < 0.5)
							pcm_smooth[i] = 0;
						else
							pcm_smooth[i] = MAX_PCM;
*/
						pcm_smooth[i] = (pcm.ptr + i < pcm_len ? MAX_PCM * pcm_table[pcm.ptr + i] : 0);
					}
					// Mix 10 bands
					// /!\ TODO_MM: MUST BE OPTIMIZED
					o01 = pcm_smooth[9];
					o03 = (181 * pcm_smooth[8] + 256 * pcm_smooth[9] + 181 * pcm_smooth[10]) / 618;
					o05 = (128 * pcm_smooth[7] + 222 * pcm_smooth[8] + 256 * pcm_smooth[ 9] + 222 * pcm_smooth[10] + 128 * pcm_smooth[11]) / 956;
//					o07 = ( 98 * pcm_smooth[6] + 181 * pcm_smooth[7] + 237 * pcm_smooth[ 8] + 256 * pcm_smooth[ 9] + 237 * pcm_smooth[10] + 181 * pcm_smooth[11] +  98 * pcm_smooth[12]) / 1288;
//					o09 = ( 79 * pcm_smooth[5] + 150 * pcm_smooth[6] + 207 * pcm_smooth[ 7] + 243 * pcm_smooth[ 8] + 256 * pcm_smooth[ 9] + 243 * pcm_smooth[10] + 207 * pcm_smooth[11] + 150 * pcm_smooth[12] +  79 * pcm_smooth[13]) / 1614;
					o11 = ( 66 * pcm_smooth[4] + 128 * pcm_smooth[5] + 181 * pcm_smooth[ 6] + 222 * pcm_smooth[ 7] + 247 * pcm_smooth[ 8] + 256 * pcm_smooth[ 9] + 247 * pcm_smooth[10] + 222 * pcm_smooth[11] + 181 * pcm_smooth[12] + 128 * pcm_smooth[13] +  66 * pcm_smooth[14]) / 1944;
//					o13 = ( 57 * pcm_smooth[3] + 111 * pcm_smooth[4] + 160 * pcm_smooth[ 5] + 200 * pcm_smooth[ 6] + 231 * pcm_smooth[ 7] + 250 * pcm_smooth[ 8] + 256 * pcm_smooth[ 9] + 250 * pcm_smooth[10] + 231 * pcm_smooth[11] + 200 * pcm_smooth[12] + 160 * pcm_smooth[13] + 111 * pcm_smooth[14] +  57 * pcm_smooth[15]) / 2274;
//					o15 = ( 50 * pcm_smooth[2] +  98 * pcm_smooth[3] + 142 * pcm_smooth[ 4] + 181 * pcm_smooth[ 5] + 213 * pcm_smooth[ 6] + 237 * pcm_smooth[ 7] + 251 * pcm_smooth[ 8] + 256 * pcm_smooth[ 9] + 251 * pcm_smooth[10] + 237 * pcm_smooth[11] + 213 * pcm_smooth[12] + 181 * pcm_smooth[13] + 142 * pcm_smooth[14] +  98 * pcm_smooth[15] +  50 * pcm_smooth[16]) / 2600;
//					o17 = ( 44 * pcm_smooth[1] +  88 * pcm_smooth[2] + 128 * pcm_smooth[ 3] + 165 * pcm_smooth[ 4] + 196 * pcm_smooth[ 5] + 222 * pcm_smooth[ 6] + 241 * pcm_smooth[ 7] + 252 * pcm_smooth[ 8] + 256 * pcm_smooth[ 9] + 252 * pcm_smooth[10] + 241 * pcm_smooth[11] + 222 * pcm_smooth[12] + 196 * pcm_smooth[13] + 165 * pcm_smooth[14] + 128 * pcm_smooth[15] +  88 * pcm_smooth[16] +  44 * pcm_smooth[17]) / 2928;
					o19 = ( 40 * pcm_smooth[0] +  79 * pcm_smooth[1] + 116 * pcm_smooth[ 2] + 150 * pcm_smooth[ 3] + 181 * pcm_smooth[ 4] + 207 * pcm_smooth[ 5] + 228 * pcm_smooth[ 6] + 243 * pcm_smooth[ 7] + 253 * pcm_smooth[ 8] + 256 * pcm_smooth[ 9] + 253 * pcm_smooth[10] + 243 * pcm_smooth[11] + 228 * pcm_smooth[12] + 207 * pcm_smooth[13] + 181 * pcm_smooth[14] + 150 * pcm_smooth[15] + 116 * pcm_smooth[16] + 79 * pcm_smooth[17] + 40 * pcm_smooth[18]) / 3250;
//					pcm.output = (50 * o01 + 98 * o03 + 142 * o05 + 181 * o07 + 213 * o09 + 237 * o11 + 251 * o13 + 256 * o15 + 256 * o17 + 253 * o19) / (40+79+116+150+181+207+228+243+253+256);
					pcm.output = (98 * o03 + 142 * o05 + 237 * o11 + 253 * o19) / (98+142+237+253);
					pcm.output = (2 * pcm.output) - MAX_PCM;

				}
				else
					pcm.output = 0;
				if(++pcm.ptr >= pcm_len)
				{
					pcm.count = 0;
					break;
				}
			}
			vol += pcm.output;
//			vol_l = apply_volume(vol, pcm_volume_l);
//			vol_r = apply_volume(vol, pcm_volume_r);
		}
//		else
//		{
			// mix tone
			if(tone.volume && tone.period)
			{
				tone.count -= tone.diff;
				while(tone.count <= 0)
				{
					tone.count += tone.period;
					tone.ptr++;
					if(tone.ptr >= 256)
						tone.ptr = tone.offset;
					tone.output = (timbre_table[tone.timbre][tone.ptr] * tone.volume) >> 8;
				}

				vol += tone.output;
			}
			if(noise.volume && noise.period)
			{
				noise.count -= noise.diff;
				while(noise.count <= 0)
				{
					noise.count += noise.period;
					noise.ptr++;
					if(noise.ptr >= 256)
						noise.ptr = noise.offset;
					noise.output = (noise_table[noise.ptr] * noise.volume) >> 8;
				}
				vol += noise.output;
			}
			if(square1.volume && square1.period)
			{
				square1.count -= square1.diff;
				while(square1.count <= 0)
				{
					square1.count  += square1.period;
					square1.ptr     = (square1.ptr + 1) & 0xff;
					square1.output  = (square_table[square1.ptr] * square1.volume) >> 8;
				}
				vol += square1.output;
			}
			if(square2.volume && square2.period)
			{
				square2.count -= square2.diff;
				while(square2.count <= 0)
				{
					square2.count  += square2.period;
					square2.ptr     = (square2.ptr + 1) & 0xff;
					square2.output  = (square_table[square2.ptr] * square2.volume) >> 8;
				}
				vol += square2.output;
			}
			if(square3.volume && square3.period)
			{
				square3.count -= square3.diff;
				while(square3.count <= 0)
				{
					square3.count  += square3.period;
					square3.ptr     = (square3.ptr + 1) & 0xff;
					square3.output  = (square_table[square3.ptr] * square3.volume) >> 8;
				}
				vol += square3.output;
			}
//			vol_l = apply_volume(vol, psg_volume_l);
//			vol_r = apply_volume(vol, psg_volume_r);
//		}
		vol_l = apply_volume(vol, 1024);
		vol_r = apply_volume(vol, 1024);

		*buffer++ += vol_l; // L
		*buffer++ += vol_r; // R
	}
}

void SOUND::set_volume(int ch, int decibel_l, int decibel_r)
{
	if(ch == 0)
	{
		psg_volume_l = decibel_to_volume(decibel_l);
		psg_volume_r = decibel_to_volume(decibel_r);
	}
	else if(ch == 1)
	{
		pcm_volume_l = decibel_to_volume(decibel_l);
		pcm_volume_r = decibel_to_volume(decibel_r);
	}
}

#define STATE_VERSION	2

void process_state_channel(channel_t* val, FILEIO* state_fio)
{
	state_fio->StateValue(val->count);
	state_fio->StateValue(val->diff);
	state_fio->StateValue(val->period);
	state_fio->StateValue(val->timbre);
	state_fio->StateValue(val->volume);
	state_fio->StateValue(val->output);
	state_fio->StateValue(val->ptr);
}

bool SOUND::process_state(FILEIO* state_fio, bool loading)
{
	if(!state_fio->StateCheckUint32(STATE_VERSION))
		return false;
	if(!state_fio->StateCheckInt32(this_device_id))
		return false;
	process_state_channel(&tone, state_fio);
	process_state_channel(&noise, state_fio);
	process_state_channel(&square1, state_fio);
	process_state_channel(&square2, state_fio);
	process_state_channel(&square3, state_fio);
	process_state_channel(&pcm, state_fio);
	state_fio->StateArray(pcm_table, sizeof(pcm_table), 1);
	state_fio->StateValue(cmd_addr);
	state_fio->StateValue(pcm_len);
	state_fio->StateValue(param_cnt);
	state_fio->StateValue(param_ptr);
	state_fio->StateValue(register_id);
	state_fio->StateArray(params, sizeof(params), 1);

	return true;
}
