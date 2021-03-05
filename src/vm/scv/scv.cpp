/*
	EPOCH Super Cassette Vision Emulator 'eSCV'

	Author : Takeda.Toshiya
	Date   : 2006.08.21 -

	Modified for Libretro-EmuSCV
	Author : MARCONATO Maxime (aka MaaaX)
	Date   : 2019-12-05 - 

	[ virtual machine ]
*/

#include "scv.h"

#include "../device.h"
#include "../event.h"
#include "../upd7801.h"
#include "io.h"
#include "memory.h"
#include "sound.h"
#include "vdp.h"
#ifdef USE_DEBUGGER
#include "../debugger.h"
#endif

// ----------------------------------------------------------------------------
// initialize
// ----------------------------------------------------------------------------

VM::VM(EMU* parent_emu) : VM_TEMPLATE(parent_emu)
{
	// create devices
	first_device = last_device = NULL;
	dummy = new DEVICE(this, emu);	// must be 1st device
	event = new EVENT(this, emu);	// must be 2nd device

	cpu = new UPD7801(this, emu);

	io = new IO(this, emu);
	memory = new MEMORY(this, emu);
	sound = new SOUND(this, emu);
	vdp = new VDP(this, emu);

	// set contexts
	event->set_context_cpu(cpu);
	event->set_context_sound(sound);

	io->set_context_mem(memory);
	io->set_context_sound(sound);
	memory->set_context_sound(sound);
	sound->set_context_cpu(cpu);
	vdp->set_context_cpu(cpu);
	vdp->set_font_ptr(memory->get_font());
	vdp->set_vram_ptr(memory->get_vram());

	// cpu bus
	cpu->set_context_mem(memory);
	cpu->set_context_io(io);
#ifdef USE_DEBUGGER
	cpu->set_context_debugger(new DEBUGGER(this, emu));
#endif

	// initialize all devices
	for(DEVICE* device = first_device; device; device = device->next_device) {
		device->initialize();
	}
}

VM::~VM()
{
	// delete all devices
	for(DEVICE* device = first_device; device;)
	{
		DEVICE *next_device = device->next_device;
		device->release();
		delete device;
		device = next_device;
	}
}

DEVICE* VM::get_device(int id)
{
	for(DEVICE* device = first_device; device; device = device->next_device)
	{
		if(device->this_device_id == id)
		{
			return device;
		}
	}
	return NULL;
}

// ----------------------------------------------------------------------------
// drive virtual machine
// ----------------------------------------------------------------------------

void VM::reset()
{
	// reset all devices
	for(DEVICE* device = first_device; device; device = device->next_device)
	{
		device->reset();
	}
}

void VM::run()
{
	event->drive();
}

// ----------------------------------------------------------------------------
// debugger
// ----------------------------------------------------------------------------

#ifdef USE_DEBUGGER
DEVICE *VM::get_cpu(int index)
{
	if(index == 0)
	{
		return cpu;
	}
	return NULL;
}
#endif

// ----------------------------------------------------------------------------
// draw screen
// ----------------------------------------------------------------------------

void VM::reset_palette()
{
	vdp->reset_palette();
}

void VM::draw_screen()
{
	vdp->draw_screen();
}

// ----------------------------------------------------------------------------
// soud manager
// ----------------------------------------------------------------------------

void VM::initialize_sound(int rate)//, int samples)
{
	// init sound manager
	event->initialize_sound(rate);//, samples);

	// init sound gen
	sound->initialize_sound(rate);
}

int16_t* VM::create_sound(int* extra_frames)
{
	return event->create_sound(extra_frames);
}

void VM::reset_sound()
{
	event->reset();
}

#ifdef USE_SOUND_VOLUME
void VM::set_sound_device_volume(int ch, int decibel_l, int decibel_r)
{
	sound->set_volume(ch, decibel_l, decibel_r);
}
#endif

// ----------------------------------------------------------------------------
// user interface
// ----------------------------------------------------------------------------

void VM::open_cart(int drv, const _TCHAR* file_path)
{
	if(drv == 0)
	{
		memory->open_cart(file_path);
		reset();
	}
}

void VM::close_cart(int drv)
{
	if(drv == 0)
	{
		memory->close_cart();
		reset();
	}
}

bool VM::is_cart_inserted(int drv)
{
	if(drv == 0)
	{
		return memory->is_cart_inserted();
	}
	else
	{
		return false;
	}
}

void VM::update_config()
{
	for(DEVICE* device = first_device; device; device = device->next_device)
	{
		device->update_config();
	}
}

#define VM_STATE_ID	201

void VM::save_state(STATE* state, bool max_size)
{
	uint16_t len;
	state->SetValue((uint16_t)VM_STATE_ID);
	for(DEVICE* device = first_device; device; device = device->next_device)
	{
		const char *name = device->get_device_name();
		len = strlen(name)+1;
		state->SetValue(len);
		state->SetArray((_TCHAR *)name, len, 1);
		device->save_state(state, max_size);
	}
}
bool VM::load_state(STATE* state)
{
	if(!state->CheckValue((uint16_t)VM_STATE_ID))
		return false;
	for(DEVICE* device = first_device; device; device = device->next_device)
	{
		const char *name = device->get_device_name();
		uint16_t len = strlen(name)+1;
		if(!state->CheckValue(len))
			return false;
		if(!state->CheckArray((_TCHAR *)name, len, 1))
			return false;
		if(!device->load_state(state))
			return false;
	}
	return true;
}

bool VM::is_bios_found()
{
	return memory->is_bios_found();
}

bool VM::is_bios_present()
{
	return memory->is_bios_present();
}

bool VM::is_bios_ok()
{
	return memory->is_bios_ok();
}
