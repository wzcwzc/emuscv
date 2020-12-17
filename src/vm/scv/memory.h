/*
	EPOCH Super Cassette Vision Emulator 'eSCV'

	Author : Takeda.Toshiya
	Date   : 2006.08.21 -

	Modified by MaaaX for EmuSCV

	[ memory ]
*/

#ifndef _EMUSCV_INC_VM_SCV_MEMORY_H_
#define _EMUSCV_INC_VM_SCV_MEMORY_H_

#include "../device.h"

// Header block
// ......AA BBBBTTSS
// A : Block address
// B : Block banks
// T : Block type
// S : Block size

#define BLOCK_VOID		   0	// 00000000 00000000 / Empty block

#define BLOCK_SIZE_MASK		0x00F0	// 00000000 11110000 / Block size mask
#define BLOCK_SIZE_1KB		0x0010	// 00000000 00010000 /  1KB of data
#define BLOCK_SIZE_2KB		0x0020	// 00000000 00100000 /  2KB of data
#define BLOCK_SIZE_4KB		0x0030	// 00000000 00110000 /  4KB of data => to use for VRAM/SRAM
#define BLOCK_SIZE_8KB		0x0040	// 00000000 01000000 /  8KB of data => to use for VRAM/SRAM
#define BLOCK_SIZE_16KB		0x0050	// 00000000 01010000 / 16KB of data
#define BLOCK_SIZE_32KB		0x0060	// 00000000 01100000 / 32KB of data

#define BLOCK_TYPE_MASK		0x000F	// 00000000 00001111 / Block type mask
#define BLOCK_TYPE_ROM		0x0001	// 00000000 00000001 / ROM (Read Only Memory)
#define BLOCK_TYPE_VRAM		0x0002	// 00000000 00000010 / Volatile RAM (RAM without battery)
#define BLOCK_TYPE_SRAM		0x0003	// 00000000 00000011 / Saved RAM (RAM with battery)

#define BLOCK_BANK_MASK		0xF000	// 11110000 00000000 / Block bank mask
#define BLOCK_BANK_0		0x1000	// 00010000 00000000 / BANK 0 / PC5 = 0 and PC6 = 0
#define BLOCK_BANK_1		0x2000	// 00100000 00000000 / BANK 1 / PC5 = 1 and PC6 = 0 / (for VRAM/SRAM of 8KB)
#define BLOCK_BANK_2		0x4000	// 01000000 00000000 / BANK 2 / PC5 = 0 and PC6 = 1 / (for VRAM/SRAM of 4KB)
#define BLOCK_BANK_3		0x8000	// 10000000 00000000 / BANK 3 / PC5 = 1 and PC6 = 1 / (for VRAM/SRAM of 4KB or 8KB)
#define BLOCK_BANK_02		0x5000	// 01010000 00000000 / BANKS 0 and 2 / PC5 = 0 and PC6 = *
#define BLOCK_BANK_13		0xA000	// 10100000 00000000 / BANKS 1 and 3 / PC5 = 1 and PC6 = *
#define BLOCK_BANK_01		0x3000	// 00110000 00000000 / BANKS 0 and 1 / PC5 = * and PC6 = 0
#define BLOCK_BANK_23		0xC000	// 11000000 00000000 / BANKS 2 and 3 / PC5 = * and PC6 = 1
#define BLOCK_BANK_0123		0xF000	// 11110000 00000000 / ALL BANKS / PC5 = * and PC6 = *

#define BLOCK_ADDR_MASK		0x0F00	// 00001111 00000000 / Address mask
#define BLOCK_ADDR_8000		0x0100	// 00000001 00000000 / Address 0x8000 in memory (offset 0x0000 in bank)
#define BLOCK_ADDR_A000		0x0200	// 00000010 00000000 / Address 0xA000 in memory (offset 0x4000 in bank)
#define BLOCK_ADDR_C000		0x0300	// 00000011 00000000 / Address 0xC000 in memory (offset 0x4000 in bank)
#define BLOCK_ADDR_E000		0x0400	// 00000100 00000000 / Address 0xE000 in memory (offset 0x6000 in bank) => to use for VRAM/SRAM of 8KB
#define BLOCK_ADDR_F000		0x0500	// 00000101 00000000 / Address 0xF000 in memory (offset 0x7000 in bank) => to use for VRAM/SRAM of 4KB
#define BLOCK_ADDR_F800		0x0600	// 00000110 00000000 / Address 0xF800 in memory (offset 0x7800 in bank)
#define BLOCK_ADDR_FC00		0x0700	// 00000111 00000000 / Address 0xFC00 in memory (offset 0x7C00 in bank)

class MEMORY : public DEVICE
{
private:
	DEVICE* d_sound;

	// memory

	// New EmuSCV version
	struct
	{
		char     id[10];		// "EmuSCV...."" (0x45, 0x6D , 0x75, 0x53, 0x43, 0x56, 0x19, 0x84, 0x07, 0x17) => 17th july 1984 :-)
		char     type[4];		// "CART" or "SRAM"
		uint8_t  version;		// File format version = 1
		uint8_t  nb_block;		// Number of blocks (4 max. used for genuine carts)
		uint16_t block[6];		// For each block of data you can indicate a combination of HEADER_SIZE_xxx + HEADER_TYPE_xxx + HEADER_BANK_xxx + HEADER_ADDR_xxx
		char     game_tag[4];	// Tag identifier for the game (used to link the .CART to the .SAVE file)
	} cart_header, ram_header;

	bool bios_found;			// Is the BIOS file found?
	bool bios_present;			// Is the BIOS present in memory?
	bool bios_ok;				// Is the BIOS controlled OK? (if not correct the BIOS will be automatically unloaded)

	bool cart_found;			// Is the cart file found?
	bool cart_inserted;			// Is a cart inserted?
	bool cart_ok;				// Is cart controlled OK? (if not correct the cart will be automatically ejected)

	_TCHAR 	 ram_path[_MAX_PATH];
	uint16_t ram_block;			// VRAM/SRAM block
	bool     ram_used;			// Is VRAM or SRAM used?
	bool     ram_save;			// Is VRAM ? (saved with battery in genuine carts)
	bool	 ram_bank0;			// Is VRAM/SRAM using bank 0? (PC5 = 0 / PC6 = 0)
	bool	 ram_bank1;			// Is VRAM/SRAM using bank 1? (PC5 = 1 / PC6 = 0)
	bool	 ram_bank2;			// Is VRAM/SRAM using bank 2? (PC5 = 0 / PC6 = 1)
	bool	 ram_bank3;			// Is VRAM/SRAM using bank 3? (PC5 = 1 / PC6 = 1)
	uint16_t ram_offset;		// VRAM/SRAM offset in block
	uint16_t ram_size;			// VRAM/SRAM size
	uint32_t ram_crc32;			// VRAM/SRAM CRC32

	uint8_t *wbank[0x200];	// 1 x 512B
	uint8_t *rbank[0x200];	// 1 x 512B
	uint8_t bios[0x1000];	// 1 x 4KB
	uint8_t vram[0x2000];	// 1 x 8KB
	uint8_t wreg[0x80];		// 1 x 128B
	uint8_t cart[0x8000*4];	// 4 x 32KB
	uint8_t sram[0x8000];	// 1 x 32KB (uselly 8KB for genuine carts)
	uint8_t wdmy[0x80];		// 1 x 128B
	uint8_t rdmy[0x80];		// 1 x 128B

	uint8_t cur_bank;
	void set_bank(uint8_t bank);

public:
	MEMORY(VM_TEMPLATE* parent_vm, EMU* parent_emu) : DEVICE(parent_vm, parent_emu)
	{
		set_device_name(_T("Memory Bus"));
		bios_found		= false;
		bios_present	= false;
		bios_ok			= false;
		cart_found		= false;
		cart_inserted	= false;
		cart_ok			= false;
		ram_block		= BLOCK_VOID;
		ram_used		= false;
		ram_save		= false;
		ram_bank0		= false;
		ram_bank1		= false;
		ram_bank2		= false;
		ram_bank3		= false;
		ram_offset		= 0;
		ram_size		= 0;
		ram_crc32		= 0;
	}
	~MEMORY() {}

	// common functions
	void initialize();
	void release();
	void reset();
	void write_data8(uint32_t addr, uint32_t data);
	uint32_t read_data8(uint32_t addr);
	void write_data8w(uint32_t addr, uint32_t data, int *wait);
	uint32_t read_data8w(uint32_t addr, int *wait);
	void write_io8(uint32_t addr, uint32_t data);
	bool process_state(FILEIO *state_fio, bool loading);

	// unique functions
	void open_cart(const _TCHAR *file_path);
	void close_cart();
	void set_context_sound(DEVICE* device) { d_sound = device; }
	uint8_t *get_font() { return bios + 0x200; }
	uint8_t *get_vram() { return vram; }
	bool is_bios_found(){ return bios_found; }
	bool is_bios_present(){ return bios_present; }
	bool is_bios_ok(){ return bios_ok; }
	bool is_cart_found() { return cart_found; }
	bool is_cart_inserted() { return cart_inserted; }
	bool is_cart_ok() { return cart_ok; }
};

#endif	// _EMUSCV_INC_VM_SCV_MEMORY_H_
