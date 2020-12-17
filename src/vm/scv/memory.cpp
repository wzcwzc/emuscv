/*
	EPOCH Super Cassette Vision Emulator 'eSCV'

	Author : Takeda.Toshiya
	Date   : 2006.08.21 -

	Modified by MaaaX for EmuSCV

	[ memory ]
*/

#include "memory.h"
#include "../../md5.h"

#define MEM_WAIT_VDP 0

#define SET_BANK(s, e, w, r)\
{ \
	int sb = (s) >> 7, eb = (e) >> 7; \
	for(int i = sb; i <= eb; i++)\
{ \
		if((w) == wdmy)\
{ \
			wbank[i] = wdmy; \
		}\
		else\
		{ \
			wbank[i] = (w) + 0x80 * (i - sb); \
		} \
		if((r) == rdmy)\
		{ \
			rbank[i] = rdmy; \
		}\
		else\
		{ \
			rbank[i] = (r) + 0x80 * (i - sb); \
		} \
	} \
}

void MEMORY::initialize()
{
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

	memset(bios, 0xff, sizeof(bios));
	memset(&cart_header, 0, sizeof(cart_header));
	memset(&ram_header, 0, sizeof(ram_header));
	memset(cart, 0xff, sizeof(cart));
	memset(sram, 0xff, sizeof(sram));
	memset(rdmy, 0xff, sizeof(rdmy));
	
	// load bios
	// first try with the file "upd7801.s01"
	FILEIO* fio = new FILEIO();
	if(fio->Fopen(create_local_path(_T("upd7801g.s01")), FILEIO_READ_BINARY))
	{
		bios_found = true;
		fio->Fread(bios, 0x1000, 1);
		bios_present = true;

		if(config.scv_checkbios == false)
		{
				bios_ok = true;
		}
		else
		{
			// check bios MD5 hash
			_TCHAR bios_md5[33];
			memset(bios_md5, 0, sizeof(bios_md5));
			strncpy(bios_md5, get_md5(bios, sizeof(bios)), sizeof(bios_md5));
			if(strcmp(bios_md5, _T("635a978fd40db9a18ee44eff449fc126")) == 0)
			{
				bios_ok = true;
			}
			else
			{
				bios_ok = false;
				memset(bios, 0xff, sizeof(bios));
				bios_present = false;
			}
		}
		
		fio->Fclose();
	}

	if(!bios_found || !bios_present || !bios_ok)
	{
		if(fio->Fopen(create_local_path(_T("upd7801g.bin")), FILEIO_READ_BINARY))
		{
			bios_found = true;
			fio->Fread(bios, 0x1000, 1);
			bios_present = true;

			if(config.scv_checkbios == false)
			{
				bios_ok = true;
			}
			else
			{
				// check bios MD5 hash
				_TCHAR bios_md5[33];
				memset(bios_md5, 0, sizeof(bios_md5));
				strncpy(bios_md5, get_md5(bios, sizeof(bios)), sizeof(bios_md5));
				if(strcmp(bios_md5, _T("635a978fd40db9a18ee44eff449fc126")) == 0)
				{
					bios_ok = true;
				}
				else
				{
					bios_ok = false;
					memset(bios, 0xff, sizeof(bios));
					bios_present = false;
				}
			}

			fio->Fclose();
		}
	}

	if(!bios_found || !bios_present || !bios_ok)
	{
		if(fio->Fopen(create_local_path(_T("upd7801g.bios")), FILEIO_READ_BINARY))
		{
			bios_found = true;
			fio->Fread(bios, 0x1000, 1);
			bios_present = true;

			if(config.scv_checkbios == false)
			{
				bios_ok = true;
			}
			else
			{
				// check bios MD5 hash
				_TCHAR bios_md5[33];
				memset(bios_md5, 0, sizeof(bios_md5));
				strncpy(bios_md5, get_md5(bios, sizeof(bios)), sizeof(bios_md5));
				if(strcmp(bios_md5, _T("635a978fd40db9a18ee44eff449fc126")) == 0)
				{
					bios_ok = true;
				}
				else
				{
					bios_ok = false;
					memset(bios, 0xff, sizeof(bios));
					bios_present = false;
				}
			}

			fio->Fclose();
		}
	}


	if(!bios_found || !bios_present || !bios_ok)
	{
		if(fio->Fopen(create_local_path(_T("bios.rom")), FILEIO_READ_BINARY))
		{
			bios_found = true;
			fio->Fread(bios, 0x1000, 1);
			bios_present = true;

			if(config.scv_checkbios == false)
			{
				bios_ok = true;
			}
			else
			{
				// check bios MD5 hash
				_TCHAR bios_md5[33];
				memset(bios_md5, 0, sizeof(bios_md5));
				strncpy(bios_md5, get_md5(bios, sizeof(bios)), sizeof(bios_md5));
				if(strcmp(bios_md5, _T("635a978fd40db9a18ee44eff449fc126")) == 0)
				{
					bios_ok = true;
				}
				else
				{
					bios_ok = false;
					memset(bios, 0xff, sizeof(bios));
					bios_present = false;
				}
			}

			fio->Fclose();
		}
	}

	if(!bios_found || !bios_present || !bios_ok)
	{
		if(fio->Fopen(create_local_path(_T("bios.bin")), FILEIO_READ_BINARY))
		{
			bios_found = true;
			fio->Fread(bios, 0x1000, 1);
			bios_present = true;

			if(config.scv_checkbios == false)
			{
				bios_ok = true;
			}
			else
			{
				// check bios MD5 hash
				_TCHAR bios_md5[33];
				memset(bios_md5, 0, sizeof(bios_md5));
				strncpy(bios_md5, get_md5(bios, sizeof(bios)), sizeof(bios_md5));
				if(strcmp(bios_md5, _T("635a978fd40db9a18ee44eff449fc126")) == 0)
				{
					bios_ok = true;
				}
				else
				{
					bios_ok = false;
					memset(bios, 0xff, sizeof(bios));
					bios_present = false;
				}
			}

			fio->Fclose();
		}
	}

	delete fio;

	// $0000-$0fff : cpu internal rom
	// $2000-$3fff : vram
	// $8000-$ff7f : cartridge rom
	// ($e000-$ff7f : 8kb sram)
	// $ff80-$ffff : cpu internam ram
	SET_BANK(0x0000, 0x0fff, wdmy, bios);
	SET_BANK(0x1000, 0x1fff, wdmy, rdmy);
	SET_BANK(0x2000, 0x3fff, vram, vram);
	SET_BANK(0x4000, 0x7fff, wdmy, rdmy);
	SET_BANK(0x8000, 0xff7f, wdmy, cart);
	SET_BANK(0xff80, 0xffff, wreg, wreg);
}

void MEMORY::release()
{
	close_cart();

	// reinit
 	bios_found		= false;
 	bios_present	= false;
 	bios_ok			= false;
	memset(bios, 0xff, sizeof(bios));
}

void MEMORY::reset()
{
	// clear memory
	memset(vram, 0, sizeof(vram));
	for(int i = 0x1000; i < 0x1200; i += 32)
	{
		static const uint8_t tmp[32] =
		{
			0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
			0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
		};
		memcpy(vram + i, tmp, 32);
	}
	memset(wreg, 0, sizeof(wreg));
	
	// initialize memory bank
	set_bank(0);
}

void MEMORY::write_data8(uint32_t addr, uint32_t data)
{
	addr &= 0xffff;
	if(addr == 0x3600)
		d_sound->write_data8(addr, data);
	if((addr & 0xfe00) == 0x3400)
		wbank[0x68][addr & 3] = data;
	else
		wbank[addr >> 7][addr & 0x7f] = data;
}

uint32_t MEMORY::read_data8(uint32_t addr)
{
	addr &= 0xffff;
	return rbank[addr >> 7][addr & 0x7f];
}

void MEMORY::write_data8w(uint32_t addr, uint32_t data, int* wait)
{
	*wait = (0x2000 <= addr && addr < 0x3600) ? MEM_WAIT_VDP : 0;
	write_data8(addr, data);
}

uint32_t MEMORY::read_data8w(uint32_t addr, int* wait)
{
	*wait = (0x2000 <= addr && addr < 0x3600) ? MEM_WAIT_VDP : 0;
	return read_data8(addr);
}

void MEMORY::write_io8(uint32_t addr, uint32_t data)
{
	set_bank((data >> 5) & 3);
}

void MEMORY::set_bank(uint8_t bank)
{
//	if(cur_bank != bank) {
		SET_BANK(0x8000, 0xff7f, wdmy, cart + 0x8000 * bank);
		SET_BANK(0x8000, 0xff7f, cart + 0x8000 * bank, cart + 0x8000 * bank);

		if(ram_used)
		{
			if((bank == 0 && ram_bank0)
			|| (bank == 1 && ram_bank1)
			|| (bank == 2 && ram_bank2)
			|| (bank == 3 && ram_bank3))
				SET_BANK(0x8000+ram_offset, (0x8000+ram_offset+ram_size < 0xff7f ? 0x8000+ram_offset+ram_size : 0xff7f), sram, sram);
		}

		cur_bank = bank;
//	}
}

void MEMORY::open_cart(const _TCHAR* file_path)
{
	uint8_t  index			= 0;
	uint16_t size			= 0x0000;
	uint16_t type			= 0;
	uint16_t bank			= 0;
	uint16_t offset			= 0x0000;
	uint32_t crc32			= 0;
	uint8_t  len			= 0;
	FILEIO*  fiocart		= new FILEIO();
	uint8_t  data[0x8000];

	bool     newcart_open	= false;
	_TCHAR   newcart_path[_MAX_PATH];
	memset(&newcart_path, 0, sizeof(newcart_path));

/*
	_TCHAR save_path[_MAX_PATH];
 #if defined(_LIBRETRO)
 	strncpy(save_path, get_libretro_save_directory(), _MAX_PATH);
 #elif
	strncpy(save_path, get_file_path_directory(file_path), _MAX_PATH);
 #endif
*/

	// Close cart and initialize memory
	close_cart();

	// Reinit
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
	memset(&cart_header, 0, sizeof(cart_header));
	memset(&ram_header, 0, sizeof(ram_header));
	memset(cart, 0xFF, sizeof(cart));
	memset(sram, 0xFF, sizeof(sram));
	
	// Check file path
	len = _tcslen(file_path);
	if(len == 0)
		goto lbl_rom_error;	// Fatal error

	if(file_path[len - 5] != _T('.')
	|| (file_path[len - 4] != _T('c') && file_path[len - 4] != _T('C'))
	|| (file_path[len - 3] != _T('a') && file_path[len - 3] != _T('A'))
	|| (file_path[len - 2] != _T('r') && file_path[len - 2] != _T('R'))
	|| (file_path[len - 1] != _T('t') && file_path[len - 1] != _T('T')))
	{
		bool          raw_ok				= false;
		uint8_t	      raw[0x20000];
		uint8_t	      cart_data[0x20000];
		uint32_t      cart_size				= 0;
		_TCHAR        raw_md5[33];
		FILEIO       *fionewcart			= new FILEIO();
		const _TCHAR *file_name				= get_file_path_file(file_path);
/*
strncpy(newcart_path, save_path, _MAX_PATH);
strncpy(newcart_path+strlen(save_path), file_name, strlen(file_name));
*/
my_tcscpy_s(newcart_path, _MAX_PATH, file_path);

		if(len+4 > _MAX_PATH-1)
		{
			newcart_path[len-5] = _T('.');
			newcart_path[len-4] = _T('c');
			newcart_path[len-3] = _T('a');
			newcart_path[len-2] = _T('r');
			newcart_path[len-1] = _T('t');
		}
		else if(file_path[len - 4] == _T('.')
		&& (file_path[len - 3] != _T('b') || file_path[len - 3] != _T('B') || file_path[len - 3] != _T('r') || file_path[len - 3] != _T('R'))
		&& (file_path[len - 2] != _T('i') || file_path[len - 2] != _T('I') || file_path[len - 2] != _T('o') || file_path[len - 2] != _T('O'))
		&& (file_path[len - 1] != _T('n') || file_path[len - 1] != _T('N') || file_path[len - 1] != _T('m') || file_path[len - 1] != _T('M')))
		{
			newcart_path[len-4] = _T('.');
			newcart_path[len-3] = _T('c');
			newcart_path[len-2] = _T('a');
			newcart_path[len-1] = _T('r');
			newcart_path[len]   = _T('t');
		}
		else if(file_path[len-2] == _T('.') && file_path[len-1] == _T('0'))
		{
			newcart_path[len-2] = _T('.');
			newcart_path[len-1] = _T('c');
			newcart_path[len]   = _T('a');
			newcart_path[len+1] = _T('r');				
			newcart_path[len+2] = _T('t');				
		}
		else
		{
			newcart_path[len] = _T('.');
			newcart_path[len+1] = _T('c');
			newcart_path[len+2] = _T('a');
			newcart_path[len+3] = _T('r');				
			newcart_path[len+4] = _T('t');				
		}

		// Check if the .CART file already exists
		// If the file exists -> nothing to do
		if(fionewcart->IsFileExisting(newcart_path))
		{
			newcart_open = true;
			goto lbl_newcart_end;
		}

		// Init cart data
		memset(cart_data, 0xFF, sizeof(data));

		// Open raw file
		if(!fiocart->Fopen(file_path, FILEIO_READ_BINARY))
			goto lbl_rom_end;	// Fatal error

		// Read raw file
		memset(raw, 0, sizeof(raw));
		fiocart->Fread(&raw, sizeof(raw), 1);

		// Get MD5
		memset(raw_md5, 0, sizeof(raw_md5));
		strncpy(raw_md5, get_md5(raw, sizeof(raw)), sizeof(raw_md5));

		// Different type of cart
		// 
		// === Single file =================================================================================
		// 
		if(strcmp(raw_md5, _T("7f29b26a63a18ba6329445b08c8e4bb0")) == 0		// ASTRO WARS (1 file of 8KB)
		|| strcmp(raw_md5, _T("f870601241b733e89fcf9df3a9bf2bfc")) == 0)	// ASTRO WARS II (1 file of 8KB)
		{
			cart_header.nb_block = 1;
			cart_header.block[0] = BLOCK_SIZE_8KB|BLOCK_TYPE_ROM|BLOCK_BANK_0123|BLOCK_ADDR_8000;
			cart_size = 0x2000;
			memcpy(cart_data, raw, cart_size);
			raw_ok = true;
		}
		else if(strcmp(raw_md5, _T("26cebb31b131bd6efb92943a01c72a0c")) == 0	// SUPER BASEBALL (1 file of 16KB)
		|| strcmp(raw_md5, _T("634f57525ced404cb8bd8b9e8ae0a1ff")) == 0			// BOULDER DASH (Japan) (1 file of 16KB)
		|| strcmp(raw_md5, _T("1513f25787c40794da6edbe0f2593012")) == 0			// BOULDER DASH (Europe) (1 file of 16KB)
		|| strcmp(raw_md5, _T("8541d99675490863240eceb866b342bf")) == 0			// COMIC CIRCUS (1 file of 16KB)
		|| strcmp(raw_md5, _T("1d40d3bdb152ecbe212357bd87799dfc")) == 0			// ELEVATOR FIGHT (1 file of 16KB)
		|| strcmp(raw_md5, _T("61e8cabac9d01b1ecd7c39bab3a88080")) == 0			// LUPIN III (1 file of 16KB)
		|| strcmp(raw_md5, _T("b7832d2f905f9d8a43863a5098013b89")) == 0			// MINER 2049ER (1 file of 16KB)
		|| strcmp(raw_md5, _T("572faa17c09d325b972fe3a4fb12fe3b")) == 0			// NEBULA (Japan) (1 file of 16KB)
		|| strcmp(raw_md5, _T("50d984715f9cf35ab747210e95630992")) == 0			// NEBULA (Europe) (1 file of 16KB)
		|| strcmp(raw_md5, _T("5c122b1294b6d87d355af9b86a9511f9")) == 0			// PUNCH BOY (1 file of 16KB)
		|| strcmp(raw_md5, _T("6fec936c4d2511a6f6022721f75abb41")) == 0			// REAL MAHJONG (1 file of 16KB)
		|| strcmp(raw_md5, _T("48271d8cdb1c53e536ebe057ec495a2f")) == 0			// SUPER SOCCER (Japan) (1 file of 16KB)
		|| strcmp(raw_md5, _T("3dc5b1ade9801d7f1284e6f8358134db")) == 0			// SUPER SOCCER (Europe) (1 file of 16KB)
		|| strcmp(raw_md5, _T("f22e632d58bcff9d9c1600a2933f1a09")) == 0			// SUPER GOLF (1 file of 16KB)
		|| strcmp(raw_md5, _T("74e4477c3fa6892527760af4a9c17fc6")) == 0			// TON TON BALL (1 file of 16KB)
		|| strcmp(raw_md5, _T("e99a00541da2778e881e7c775be8e6a4")) == 0)		// WHEELY RACER (1 file of 16KB)
		{
			cart_header.nb_block = 1;
			cart_header.block[0] = BLOCK_SIZE_16KB|BLOCK_TYPE_ROM|BLOCK_BANK_0123|BLOCK_ADDR_8000;
			cart_size = 0x4000;
			memcpy(cart_data, raw, cart_size);
			raw_ok = true;
		}
		else if(strcmp(raw_md5, _T("511b2bcf7ec0d0c7fed72a53bfc3c49d")) == 0	// MAPPY (1 file of 32KB)
		|| strcmp(raw_md5, _T("4609c190580abd137ff4f9a69b3970e5")) == 0			// MILKY PRINCESS (1 file of 32KB)
		|| strcmp(raw_md5, _T("85d692f106f37390a9c2e565e368b5c4")) == 0)		// SUPER SANSUPYUTA (1 file of 32KB)
		{
			cart_header.nb_block = 1;
			cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_0123|BLOCK_ADDR_8000;
			cart_size = 0x8000;
			memcpy(cart_data, raw, cart_size);
			raw_ok = true;
		}
		else if(strcmp(raw_md5, _T("a82ab36b652e8370c6e24f3dde39026b")) == 0	// BASIC NYUMON (1 file of 32KB)
		|| strcmp(raw_md5, _T("e96a36f183913faa2c6d3c602e9f5ce3")) == 0			// DRAGON SLAYER (1 file of 32KB)
		|| strcmp(raw_md5, _T("5a8bea76fdcce04196ddb7dd06afdb2d")) == 0)		// POP & CHIPS (1 file of 32KB)
		{
			cart_header.nb_block = 2;
			cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_0123|BLOCK_ADDR_8000;
			cart_header.block[1] = BLOCK_SIZE_8KB|BLOCK_TYPE_SRAM|BLOCK_BANK_13|BLOCK_ADDR_E000;
			cart_size = 0x8000;
			memcpy(cart_data, raw, cart_size);
			raw_ok = true;
		}
		else if(strcmp(raw_md5, _T("be926511d88c0c6f45162513ba997351")) == 0)	// SHOGI (1 file of 32KB)
		{
			cart_header.nb_block = 2;
			cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_0123|BLOCK_ADDR_8000;
			cart_header.block[1] = BLOCK_SIZE_8KB|BLOCK_TYPE_VRAM|BLOCK_BANK_13|BLOCK_ADDR_E000;
			cart_size = 0x8000;
			memcpy(cart_data, raw, cart_size);
			raw_ok = true;
		}
		else if(strcmp(raw_md5, _T("bfc976cefbd328fb4cf6d626ef81c2bc")) == 0	// KUNG-FU ROAD (1 file of 40KB)
		|| strcmp(raw_md5, _T("03ef1011b3f397eb13d5bae895a5747b")) == 0)		// STAR SPEEDER (1 file of 40KB)
		{
			cart_header.nb_block = 2;
			cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_0123|BLOCK_ADDR_8000;
			cart_header.block[1] = BLOCK_SIZE_8KB|BLOCK_TYPE_ROM|BLOCK_BANK_13|BLOCK_ADDR_E000;
			cart_size = 0xA000;
			memcpy(cart_data, raw, cart_size);
			raw_ok = true;
		}
		else if(strcmp(raw_md5, _T("c80cb2edf47a578b8be10f68402df597")) == 0	// DORAEMON (1 file of 64KB)
		|| strcmp(raw_md5, _T("af5ee1af51614a804ccb71be987e1065")) == 0)		// SKY KID (1 file of 64KB)
		{
			cart_header.nb_block = 2;
			cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_02|BLOCK_ADDR_8000;
			cart_header.block[1] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_13|BLOCK_ADDR_8000;
			cart_size = 0x10000;
			memcpy(cart_data, raw, cart_size);
			raw_ok = true;
		}
		else if(strcmp(raw_md5, _T("6d11d49390d0946501b39d9f9688ae9d")) == 0)	// DRAGON BALL (1 file of 128KB)
		{
			cart_header.nb_block = 3;
			cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_01|BLOCK_ADDR_8000;
			cart_header.block[1] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_2|BLOCK_ADDR_8000;
			cart_header.block[2] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_3|BLOCK_ADDR_8000;
			cart_size = 0x18000;
			memcpy(cart_data, raw+0x8000, cart_size);
			raw_ok = true;
		}
		else if(strcmp(raw_md5, _T("44d432cb0093a1992beb806a68e7aaad")) == 0	// PROFESSIONAL WRESTLING (1 file of 128KB)
		|| strcmp(raw_md5, _T("64d6268aebabedb1552c2cd565ec32a7")) == 0)		// Y2 MONSTER LAND (1 file of 128KB)
		{
			cart_header.nb_block = 4;
			cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_0|BLOCK_ADDR_8000;
			cart_header.block[1] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_1|BLOCK_ADDR_8000;
			cart_header.block[2] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_2|BLOCK_ADDR_8000;
			cart_header.block[3] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_3|BLOCK_ADDR_8000;
			cart_size = 0x20000;
			memcpy(cart_data, raw, cart_size);
			raw_ok = true;
		}
		else if(strcmp(raw_md5, _T("78249cfb10962347a8001ba102992503")) == 0)	// POLE POSITION II (1 file of 128KB)
		{
			cart_header.nb_block = 4;
			cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_1|BLOCK_ADDR_8000;
			cart_header.block[1] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_2|BLOCK_ADDR_8000;
			cart_header.block[2] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_3|BLOCK_ADDR_8000;
			cart_header.block[3] = BLOCK_SIZE_4KB|BLOCK_TYPE_VRAM|BLOCK_BANK_23|BLOCK_ADDR_F000;
			cart_size = 0x18000;
			memcpy(cart_data, raw+0x8000, cart_size);
			raw_ok = true;
		}
		else if(file_path[len - 1] == _T('0'))
		{
			// 
			// === Multiple files =================================================================================
			// 
			_TCHAR next_file_path[_MAX_PATH];
			memset(&next_file_path, 0, sizeof(next_file_path));
			my_tcscpy_s(next_file_path, _MAX_PATH, file_path);
			next_file_path[len - 1] = _T('1');
			
			fiocart->Fclose();
			if(!fiocart->Fopen(next_file_path, FILEIO_READ_BINARY))
				goto lbl_nextfile_end;	// Fatal error

			if(strcmp(raw_md5, _T("c0e0d29c2da0d2bb82d18728355a575c")) == 0)	// KUNG-FU ROAD (2 files of 32KB and 8KB) File 0 of 32KB
			{
				cart_header.nb_block = 2;
				cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_0123|BLOCK_ADDR_8000;
				cart_header.block[1] = BLOCK_SIZE_8KB|BLOCK_TYPE_ROM|BLOCK_BANK_13|BLOCK_ADDR_E000;
				cart_size = 0xA000;
				memcpy(cart_data, raw, 0x8000);

				// Read raw file
				memset(raw, 0, sizeof(raw));
				fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				strncpy(raw_md5, get_md5(raw, sizeof(raw)), sizeof(raw_md5));

				if(strcmp(raw_md5, _T("8eaf39879871eb2d8ea1422b22bdbfed")) != 0)	// File 1 of 8KB
					goto lbl_nextfile_end;	// Fatal error
				
				memcpy(cart_data+0x8000, raw, 0x2000);
				raw_ok = true;
			}
			else if(strcmp(raw_md5, _T("3c156476be42eae154b8c311cf42b934")) == 0)	// STAR SPEEDER (2 files of 32KB and 8KB) File 0 of 32KB
			{
				cart_header.nb_block = 2;
				cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_0123|BLOCK_ADDR_8000;
				cart_header.block[1] = BLOCK_SIZE_8KB|BLOCK_TYPE_ROM|BLOCK_BANK_13|BLOCK_ADDR_E000;
				cart_size = 0xA000;
				memcpy(cart_data, raw, 0x8000);

				// Read raw file
				memset(raw, 0, sizeof(raw));
				fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				strncpy(raw_md5, get_md5(raw, sizeof(raw)), sizeof(raw_md5));

				if(strcmp(raw_md5, _T("3112f2a0a3728d37968354b22db2cba6")) != 0)	// File 1 of 8KB
					goto lbl_nextfile_end;	// Fatal error
				
				memcpy(cart_data+0x8000, raw, 0x2000);
				raw_ok = true;
			}
			else if(strcmp(raw_md5, _T("33211e8405ff9ef3c7ff82ea35a695f1")) == 0)	// DORAEMON (2 files of 32KB) File 0 of 32KB
			{
				cart_header.nb_block = 2;
				cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_02|BLOCK_ADDR_8000;
				cart_header.block[1] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_13|BLOCK_ADDR_8000;
				cart_size = 0x10000;
				memcpy(cart_data, raw, 0x8000);

				// Read raw file
				memset(raw, 0, sizeof(raw));
				fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				strncpy(raw_md5, get_md5(raw, sizeof(raw)), sizeof(raw_md5));

				if(strcmp(raw_md5, _T("fe7e9c44fca1b7ee6e412b0d3287d423")) != 0)	// File 1 of 32KB
					goto lbl_nextfile_end;	// Fatal error

				memcpy(cart_data+0x8000, raw, 0x8000);
				raw_ok = true;
			}
			else if(strcmp(raw_md5, _T("77ce7a225767e6a484533b32ed05703f")) == 0)	// SKY KID (2 files of 32KB) File 0 of 32KB
			{
				cart_header.nb_block = 2;
				cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_02|BLOCK_ADDR_8000;
				cart_header.block[1] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_13|BLOCK_ADDR_8000;
				cart_size = 0x10000;
				memcpy(cart_data, raw, 0x8000);

				// Read raw file
				memset(raw, 0, sizeof(raw));
				fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				strncpy(raw_md5, get_md5(raw, sizeof(raw)), sizeof(raw_md5));

				if(strcmp(raw_md5, _T("8800ab30c7c2e6b2ffcf6cd60d2457b7")) != 0)	// File 1 of 32KB
					goto lbl_nextfile_end;	// Fatal error

				memcpy(cart_data+0x8000, raw, 0x8000);
				raw_ok = true;
			}
			else if(strcmp(raw_md5, _T("93c387242f2c99b5b791314844598f43")) == 0)	// DRAGON BALL (4 files of 32KB) File 0 of 32KB
			{
				cart_header.nb_block = 3;
				cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_01|BLOCK_ADDR_8000;
				cart_header.block[1] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_2|BLOCK_ADDR_8000;
				cart_header.block[2] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_3|BLOCK_ADDR_8000;
				cart_size = 0x18000;
				memcpy(cart_data, raw, 0x8000);

				// Read raw file
				memset(raw, 0, sizeof(raw));
				fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				strncpy(raw_md5, get_md5(raw, sizeof(raw)), sizeof(raw_md5));

				if(strcmp(raw_md5, _T("93c387242f2c99b5b791314844598f43")) != 0)	// File 1 of 32KB (same data as file 0)
					goto lbl_nextfile_end;	// Fatal error
				
				// Data ignored here because same as file 0

				// Open next file
				next_file_path[len - 1] = _T('2');
				fiocart->Fclose();
				if(!fiocart->Fopen(next_file_path, FILEIO_READ_BINARY))
					goto lbl_nextfile_end;	// Fatal error

				// Read raw file
				memset(raw, 0, sizeof(raw));
				fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				strncpy(raw_md5, get_md5(raw, sizeof(raw)), sizeof(raw_md5));

				if(strcmp(raw_md5, _T("aa65f12527a2dea288dd79171ad405f9")) != 0)	// File 2 of 32KB
					goto lbl_nextfile_end;	// Fatal error
				
				memcpy(cart_data+0x8000, raw, 0x8000);

				// Open next file
				next_file_path[len - 1] = _T('3');
				fiocart->Fclose();
				if(!fiocart->Fopen(next_file_path, FILEIO_READ_BINARY))
					goto lbl_nextfile_end;	// Fatal error

				// Read raw file
				memset(raw, 0, sizeof(raw));
				fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				strncpy(raw_md5, get_md5(raw, sizeof(raw)), sizeof(raw_md5));

				if(strcmp(raw_md5, _T("79c99377f1d30ea84d1d37a816c8c3a4")) != 0)	// File 3 of 32sKB
					goto lbl_nextfile_end;	// Fatal error

				memcpy(cart_data+0x10000, raw, 0x8000);
				raw_ok = true;
			}
			else if(strcmp(raw_md5, _T("964c760d49a487f9d98054e6abf4648d")) == 0)	// PROFESSIONAL WRESTLING (4 files of 32KB) File 0 of 32KB
			{
				cart_header.nb_block = 4;
				cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_0|BLOCK_ADDR_8000;
				cart_header.block[1] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_1|BLOCK_ADDR_8000;
				cart_header.block[2] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_2|BLOCK_ADDR_8000;
				cart_header.block[3] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_3|BLOCK_ADDR_8000;
				cart_size = 0x20000;
				memcpy(cart_data, raw, 0x8000);

				// Read raw file
				memset(raw, 0, sizeof(raw));
				fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				strncpy(raw_md5, get_md5(raw, sizeof(raw)), sizeof(raw_md5));

				if(strcmp(raw_md5, _T("ec9baea41e54615e2df3cc0d6c96bcff")) != 0)	// File 1 of 32KB
					goto lbl_nextfile_end;	// Fatal error
				
				memcpy(cart_data+0x8000, raw, 0x8000);

				// Open next file
				next_file_path[len - 1] = _T('2');
				fiocart->Fclose();
				if(!fiocart->Fopen(next_file_path, FILEIO_READ_BINARY))
					goto lbl_nextfile_end;	// Fatal error

				// Read raw file
				memset(raw, 0, sizeof(raw));
				fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				strncpy(raw_md5, get_md5(raw, sizeof(raw)), sizeof(raw_md5));

				if(strcmp(raw_md5, _T("070701dc0891ad7838adb43d7e470bd5")) != 0)	// File 2 of 32KB
					goto lbl_nextfile_end;	// Fatal error
				
				memcpy(cart_data+0x10000, raw, 0x8000);

				// Open next file
				next_file_path[len - 1] = _T('3');
				fiocart->Fclose();
				if(!fiocart->Fopen(next_file_path, FILEIO_READ_BINARY))
					goto lbl_nextfile_end;	// Fatal error

				// Read raw file
				memset(raw, 0, sizeof(raw));
				fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				strncpy(raw_md5, get_md5(raw, sizeof(raw)), sizeof(raw_md5));

				if(strcmp(raw_md5, _T("3314d7c12aeecea8fe3c6794f500da96")) != 0)	// File 3 of 32sKB
					goto lbl_nextfile_end;	// Fatal error

				memcpy(cart_data+0x18000, raw, 0x8000);
				raw_ok = true;
			}
			else if(strcmp(raw_md5, _T("983c1dc78d39442c73716957698f5537")) == 0)	// Y2 MONSTER LAND (4 files of 32KB) File 0 of 32KB
			{
				cart_header.nb_block = 4;
				cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_0|BLOCK_ADDR_8000;
				cart_header.block[1] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_1|BLOCK_ADDR_8000;
				cart_header.block[2] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_2|BLOCK_ADDR_8000;
				cart_header.block[3] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_3|BLOCK_ADDR_8000;
				cart_size = 0x20000;
				memcpy(cart_data, raw, 0x8000);

				// Read raw file
				memset(raw, 0, sizeof(raw));
				fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				strncpy(raw_md5, get_md5(raw, sizeof(raw)), sizeof(raw_md5));

				if(strcmp(raw_md5, _T("4d2b9ea5158f07ad0078393bc3ab0e3b")) != 0)	// File 1 of 32KB
					goto lbl_nextfile_end;	// Fatal error
				
				memcpy(cart_data+0x8000, raw, 0x8000);

				// Open next file
				next_file_path[len - 1] = _T('2');
				fiocart->Fclose();
				if(!fiocart->Fopen(next_file_path, FILEIO_READ_BINARY))
					goto lbl_nextfile_end;	// Fatal error

				// Read raw file
				memset(raw, 0, sizeof(raw));
				fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				strncpy(raw_md5, get_md5(raw, sizeof(raw)), sizeof(raw_md5));

				if(strcmp(raw_md5, _T("2df87cda6b91374117ee9681563cc044")) != 0)	// File 2 of 32KB
					goto lbl_nextfile_end;	// Fatal error
				
				memcpy(cart_data+0x10000, raw, 0x8000);

				// Open next file
				next_file_path[len - 1] = _T('3');
				fiocart->Fclose();
				if(!fiocart->Fopen(next_file_path, FILEIO_READ_BINARY))
					goto lbl_nextfile_end;	// Fatal error

				// Read raw file
				memset(raw, 0, sizeof(raw));
				fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				strncpy(raw_md5, get_md5(raw, sizeof(raw)), sizeof(raw_md5));

				if(strcmp(raw_md5, _T("b860b03a486c5b898dcd34ef6a9f2259")) != 0)	// File 3 of 32sKB
					goto lbl_nextfile_end;	// Fatal error
				
				memcpy(cart_data+0x18000, raw, 0x8000);
				raw_ok = true;
			}
			else if(strcmp(raw_md5, _T("1ce49a4f529489bb76afdfb198ddb57a")) == 0)	// POLE POSITION II (4 files of 32KB) File 0 of 32KB filled with 0xFF
			{
				cart_header.nb_block = 4;
				cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_1|BLOCK_ADDR_8000;
				cart_header.block[1] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_2|BLOCK_ADDR_8000;
				cart_header.block[2] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_3|BLOCK_ADDR_8000;
				cart_header.block[3] = BLOCK_SIZE_4KB|BLOCK_TYPE_VRAM|BLOCK_BANK_23|BLOCK_ADDR_F000;
				cart_size = 0x18000;

				// Data ignored here because filled with 0xFF

				// Read raw file
				memset(raw, 0, sizeof(raw));
				fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				strncpy(raw_md5, get_md5(raw, sizeof(raw)), sizeof(raw_md5));

				if(strcmp(raw_md5, _T("520132da5fd078ce6944845f8d50064d")) != 0)	// File 1 of 32KB
					goto lbl_nextfile_end;	// Fatal error
				
				memcpy(cart_data, raw, 0x8000);

				// Open next file
				next_file_path[len - 1] = _T('2');
				fiocart->Fclose();
				if(!fiocart->Fopen(next_file_path, FILEIO_READ_BINARY))
					goto lbl_nextfile_end;	// Fatal error

				// Read raw file
				memset(raw, 0, sizeof(raw));
				fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				strncpy(raw_md5, get_md5(raw, sizeof(raw)), sizeof(raw_md5));

				if(strcmp(raw_md5, _T("ed2c14fd62a7e04d2c72b3dd07ac189a")) != 0)	// File 2 of 32KB
					goto lbl_nextfile_end;	// Fatal error
				
				memcpy(cart_data+0x8000, raw, 0x8000);

				// Open next file
				next_file_path[len - 1] = _T('3');
				fiocart->Fclose();
				if(!fiocart->Fopen(next_file_path, FILEIO_READ_BINARY))
					goto lbl_nextfile_end;	// Fatal error

				// Read raw file
				memset(raw, 0, sizeof(raw));
				fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				strncpy(raw_md5, get_md5(raw, sizeof(raw)), sizeof(raw_md5));

				if(strcmp(raw_md5, _T("2c502ccd404194370f094aacf958595f")) != 0)	// File 3 of 32KB
					goto lbl_nextfile_end;	// Fatal error
				
				memcpy(cart_data+0x10000, raw, 0x8000);
				raw_ok = true;
			}
		}

lbl_nextfile_end:

		// Raw file(s) is(are) ok -> create .CART file
		if(raw_ok)
		{
			// Standard .CART file header
			cart_header.id[0]		= 'E';
			cart_header.id[1]		= 'm';
			cart_header.id[2]		= 'u';
			cart_header.id[3]		= 'S';
			cart_header.id[4]		= 'C';
			cart_header.id[5]		= 'V';
			cart_header.id[6]		= 0x19;
			cart_header.id[7]		= 0x84;
			cart_header.id[8]		= 0x07;
			cart_header.id[9]		= 0x17;
			cart_header.type[0]		= 'C';
			cart_header.type[1]		= 'A';
			cart_header.type[2]		= 'R';
			cart_header.type[3]		= 'T';
			cart_header.version		= 1;
			cart_header.game_tag[0]	= ' ';
			cart_header.game_tag[1]	= ' ';
			cart_header.game_tag[2]	= ' ';
			cart_header.game_tag[0]	= ' ';

			// Create new .CART file
			if(!fionewcart->Fopen(newcart_path, FILEIO_WRITE_BINARY))
				goto lbl_newcart_close;

			// Save cart header
			fionewcart->Fwrite(&cart_header, sizeof(cart_header), 1);

			// Save cart data
			fionewcart->Fwrite(&cart_data, cart_size, 1);
			
			newcart_open = true;

lbl_newcart_close:
			fionewcart->Fclose();
		}
/*
		else
		{
			_TCHAR md5_path[_MAX_PATH];

			memset(&md5_path, 0, sizeof(md5_path));
			my_tcscpy_s(md5_path, _MAX_PATH, file_path);
			md5_path[len] = _T('.');
			md5_path[len+1] = _T('t');
			md5_path[len+2] = _T('x');
			md5_path[len+3] = _T('t');

			FILEIO* fiomd5 = new FILEIO();
			fiomd5->Fopen(md5_path, FILEIO_WRITE_BINARY);
			fiomd5->Fwrite(&raw_md5, sizeof(raw_md5)-1, 1);
			fiomd5->Fclose();
			delete(fiomd5);
		}
*/

lbl_newcart_end:
		delete(fionewcart);
		goto lbl_rom_close;	// Fatal error
	}

	// Get save file path
	my_tcscpy_s(ram_path, _MAX_PATH, file_path);
	ram_path[len - 4] = (file_path[len - 4] == _T('c') ? _T('s') : _T('S'));
	ram_path[len - 3] = (file_path[len - 3] == _T('a') ? _T('a') : _T('A'));
	ram_path[len - 2] = (file_path[len - 2] == _T('r') ? _T('v') : _T('V'));
	ram_path[len - 1] = (file_path[len - 1] == _T('t') ? _T('e') : _T('E'));
	
	// Open .CART file
	if(!fiocart->Fopen(file_path, FILEIO_READ_WRITE_BINARY))
		goto lbl_rom_end;	// Fatal error
	
	cart_found = true;

	// Load header
	fiocart->Fread(&cart_header, sizeof(cart_header), 1);

	// Check format identifier
	if(cart_header.id[0] != 'E' || cart_header.id[1] != 'm' || cart_header.id[2] != 'u' || cart_header.id[3] != 'S' || cart_header.id[4] != 'C' || cart_header.id[5] != 'V' || (uint8_t)cart_header.id[6] != 0x19 || (uint8_t)cart_header.id[7] != 0x84 || (uint8_t)cart_header.id[8] != 0x07 || (uint8_t)cart_header.id[9] != 0x17)
		goto lbl_rom_error;	// Fatal error
	
	// Check file type
	if(cart_header.type[0] != 'C' || cart_header.type[1] != 'A' || cart_header.type[2] != 'R' || cart_header.type[3] != 'T')
		goto lbl_rom_error;	// Fatal error

	// Check format version
	if(cart_header.version != 1)
		goto lbl_rom_error;	// Fatal error

	// Check number of blocks (6 max, only 4 used by genuine carts)
	if(cart_header.nb_block > 6)
		goto lbl_rom_error;	// Fatal error

	// Check blocks
	// Block 1 is needed
	if(cart_header.block[0] == BLOCK_VOID)
		goto lbl_rom_error;	// Fatal error
	// Avoid hole in blocks sequence
	if(cart_header.block[1] == BLOCK_VOID && (cart_header.nb_block >= 2 || cart_header.block[2] != BLOCK_VOID || cart_header.block[3] != BLOCK_VOID || cart_header.block[4] != BLOCK_VOID || cart_header.block[5] != BLOCK_VOID))
		goto lbl_rom_error;	// Fatal error
	if(cart_header.block[1] != BLOCK_VOID && cart_header.nb_block < 2)
		goto lbl_rom_error;	// Fatal error
	if(cart_header.block[2] == BLOCK_VOID && (cart_header.nb_block >= 3 || cart_header.block[3] != BLOCK_VOID || cart_header.block[4] != BLOCK_VOID || cart_header.block[5] != BLOCK_VOID))
		goto lbl_rom_error;	// Fatal error
	if(cart_header.block[2] != BLOCK_VOID && cart_header.nb_block < 3)
		goto lbl_rom_error;	// Fatal error
	if(cart_header.block[3] == BLOCK_VOID && (cart_header.nb_block >= 4 || cart_header.block[4] != BLOCK_VOID || cart_header.block[5] != BLOCK_VOID))
		goto lbl_rom_error;	// Fatal error
	if(cart_header.block[3] != BLOCK_VOID && cart_header.nb_block < 4)
		goto lbl_rom_error;	// Fatal error
	if(cart_header.block[4] == BLOCK_VOID && (cart_header.nb_block >= 5 || cart_header.block[5] != BLOCK_VOID))
		goto lbl_rom_error;	// Fatal error
	if(cart_header.block[4] != BLOCK_VOID && cart_header.nb_block < 5)
		goto lbl_rom_error;	// Fatal error
	if(cart_header.block[5] != BLOCK_VOID && cart_header.nb_block < 6)
		goto lbl_rom_error;	// Fatal error

	// Load blocks
	for(index = 0; index < cart_header.nb_block; index++)
	{
		// Init data
		memset(data, 0xff, sizeof(data));

		// Reorder bytes
		//cart_header.block[index] = EndianToBig_WORD(cart_header.blockindex0]);
		
		// Get block size
		switch(cart_header.block[index] & BLOCK_SIZE_MASK)
		{
			case BLOCK_SIZE_1KB:
				size = 0x0400;
				break;				
			case BLOCK_SIZE_2KB:
				size = 0x0800;
				break;				
			case BLOCK_SIZE_4KB:
				size = 0x1000;
				break;				
			case BLOCK_SIZE_8KB:
				size = 0x2000;
				break;				
			case BLOCK_SIZE_16KB:
				size = 0x4000;
				break;
			case BLOCK_SIZE_32KB:
				size = 0x8000;
				break;
			default:	// Other block size
				goto lbl_rom_error;	// Fatal error
				break;
		}

		// Get type
		switch(cart_header.block[index] & BLOCK_TYPE_MASK)
		{
			case BLOCK_TYPE_ROM:
				type = BLOCK_TYPE_ROM;
				break;
			case BLOCK_TYPE_VRAM:
				type = BLOCK_TYPE_VRAM;
				// TODO
				break;
			case BLOCK_TYPE_SRAM:
				type = BLOCK_TYPE_SRAM;
				break;
			default:	// Other block type
				goto lbl_rom_error;	// Fatal error
				break;
		}

		// Get bank
		if((cart_header.block[index] & BLOCK_BANK_MASK) == BLOCK_VOID)
			goto lbl_rom_error;	// Fatal error
		bank = (cart_header.block[index] & BLOCK_BANK_MASK);

		// Get block address (offset in the block)
		switch(cart_header.block[index] & BLOCK_ADDR_MASK)
		{
			case BLOCK_ADDR_8000:
				offset = 0x0000;
				break;
			case BLOCK_ADDR_A000:
				offset = 0x2000;
				break;
			case BLOCK_ADDR_C000:
				offset = 0x4000;
				break;
			case BLOCK_ADDR_E000:
				offset = 0x6000;
				break;
			case BLOCK_ADDR_F000:
				offset = 0x7000;
				break;
			case BLOCK_ADDR_F800:
				offset = 0x7800;
				break;
			case BLOCK_ADDR_FC00:
				offset = 0x7C00;
				break;
			default:	// Other block address
				goto lbl_rom_error;	// Fatal error
				break;
		}

		// Avoid overflow in blocks
		if(offset + size > 0x8000)
			goto lbl_rom_error;	// Fatal error

		// Load ROM data
		switch(type)
		{
			case BLOCK_TYPE_ROM:
				// Read data from file
				fiocart->Fread(&data, size, 1);

				// Assign block of data to banks
				if(bank & BLOCK_BANK_0)	// PC5=0, PC6=0
					memcpy(cart+offset, data, size);
				if(bank & BLOCK_BANK_1)	// PC5=1, PC6=0
					memcpy(cart+0x8000+offset, data, size);
				if(bank & BLOCK_BANK_2)	// PC5=0, PC6=1
					memcpy(cart+0x10000+offset, data, size);
				if(bank & BLOCK_BANK_3)	// PC5=1, PC6=1
					memcpy(cart+0x18000+offset, data, size);
				break;

			case BLOCK_TYPE_VRAM:
			case BLOCK_TYPE_SRAM:

				// Only one VRAM/SRAM block
				if(ram_used)
					goto lbl_rom_error;	// Fatal error

				// Memorize RAM infos
				ram_block	= cart_header.block[index];
				ram_used	= true;
				ram_save	= (type == BLOCK_TYPE_SRAM);
				ram_bank0	= (bank & BLOCK_BANK_0);
				ram_bank1	= (bank & BLOCK_BANK_1);
				ram_bank2	= (bank & BLOCK_BANK_2);
				ram_bank3	= (bank & BLOCK_BANK_3);
				ram_offset	= offset;
				ram_size	= size;
				ram_crc32	= 0;	// Will be read later

				break;

			default:
				goto lbl_rom_error;	// Fatal error
				break;
		}
	}

	cart_inserted = true;
/*
	// Check CRC32
	crc32 = get_crc32(cart, sizeof(cart));
	if(cart_header.crc32 == 0)	// If CRC32 don't exists we create it
	{
		cart_header.crc32 = crc32;
		// Save CRC32
		fiocart->Fseek(sizeof(cart_header)-sizeof(cart_header.crc32), FILEIO_SEEK_SET);
		fiocart->Fwrite(&cart_header.crc32, sizeof(cart_header.crc32), 1);
	}
	else if(cart_header.crc32 != crc32)	// Check existing CRC32
		goto lbl_rom_error;
*/
	cart_ok = true;

	// Try to load SRAM/VRAM save
	if(ram_used)
	{
		memset(sram, 0x00, sizeof(ram_size));
		if(ram_save)
		{
			FILEIO* fioram = new FILEIO();

			// load saved sram
			if(!fioram->Fopen(ram_path, FILEIO_READ_WRITE_BINARY))
			{
				memset(&ram_header, 0, sizeof(ram_header));
				ram_header.id[0]	= 'E';
				ram_header.id[1]	= 'm';
				ram_header.id[2]	= 'u';
				ram_header.id[3]	= 'S';
				ram_header.id[4]	= 'C';
				ram_header.id[5]	= 'V';
				ram_header.id[6]	= 0x19;
				ram_header.id[7]	= 0x84;
				ram_header.id[8]	= 0x07;
				ram_header.id[9]	= 0x17;
				ram_header.type[0]	= 'S';
				ram_header.type[1]	= 'A';
				ram_header.type[2]	= 'V';
				ram_header.type[3]	= 'E';
				ram_header.version	= 1;
				ram_header.nb_block	= 1;
				ram_header.block[0]	= ram_block;

				goto lbl_ram_end;	// Fatal error
			}

			// Load header
			fioram->Fread(&ram_header, sizeof(ram_header), 1);

			// Check format identifier
			if(ram_header.id[0] != 'E' || ram_header.id[1] != 'm' || ram_header.id[2] != 'u' || ram_header.id[3] != 'S' || ram_header.id[4] != 'C' || ram_header.id[5] != 'V' || (uint8_t)ram_header.id[6] != 0x19 || (uint8_t)ram_header.id[7] != 0x84 || (uint8_t)ram_header.id[8] != 0x07 || (uint8_t)ram_header.id[9] != 0x17)
				goto lbl_ram_close;	// Fatal error
			
			// Check file type
			if(ram_header.type[0] != 'S' || ram_header.type[1] != 'A' || ram_header.type[2] != 'V' || ram_header.type[3] != 'E')
				goto lbl_ram_close;	// Fatal error

			// Check format version
			if(ram_header.version != 1)
				goto lbl_ram_close;	// Fatal error

			// Check number of blocks
			// Only one block for SRAM save
			if(ram_header.nb_block > 1)
				goto lbl_ram_close;	// Fatal error

			// Block 1 is needed
			if(ram_header.block[0] == BLOCK_VOID)
				goto lbl_ram_close;	// Fatal error

			// Compare SAVE block to CART bock
			if(ram_header.block[0] != ram_block)
				goto lbl_ram_close;	// Fatal error

			// Init RAM
			memset(data, 0x00, ram_size);
			memset(data+ram_size, 0xFF, sizeof(data)-ram_size);

			// Read backup
			fioram->Fread(&data, ram_size, 1);
/*
			// Check CRC32
			ram_crc32 = get_crc32(data, ram_size);
			if(ram_header.crc32 == 0)
			{
				// Save CRC32
				FILEIO* fiorw = new FILEIO();
				if(fiorw->Fopen(ram_path, FILEIO_WRITE_BINARY))
				{
					fiorw->Fseek(sizeof(cart_header)-sizeof(cart_header.crc32), FILEIO_SEEK_SET);
					fiorw->Fwrite(&ram_crc32, sizeof(ram_crc32), 1);
					fiorw->Fclose();
				}
				delete(fiorw);

			}
			else if(ram_header.crc32 != ram_crc32)
				goto lbl_ram_close;
*/
			memcpy(data, sram, ram_size);

lbl_ram_close:
			fioram->Fclose();
lbl_ram_end:
			delete(fioram);
		}
	}

	goto lbl_rom_close;

lbl_rom_error:
	ram_block	= BLOCK_VOID;
	ram_used	= false;
	ram_save	= false;
	ram_bank0	= false;
	ram_bank1	= false;
	ram_bank2	= false;
	ram_bank3	= false;
	ram_offset	= 0;
	ram_size	= 0;
	ram_crc32	= 0;
	memset(&cart_header, 0xff, sizeof(cart_header));
	memset(cart, 0xff, sizeof(cart));
	memset(sram, 0xff, sizeof(sram));

lbl_rom_close:
	fiocart->Fclose();
lbl_rom_end:
	delete fiocart;

	if(newcart_open)
		open_cart(newcart_path);
}

void MEMORY::close_cart()
{
	// save backuped sram
	if(cart_inserted && ram_used && ram_save)
	{
		ram_crc32 = get_crc32(sram, ram_size);
//		ram_header.crc32	= ram_crc32;
		FILEIO* fiorw = new FILEIO();
		if(fiorw->Fopen(ram_path, FILEIO_WRITE_BINARY))
		{
			fiorw->Fwrite(&ram_header, sizeof(ram_header), 1);
			fiorw->Fwrite(&sram, ram_size, 1);
			fiorw->Fclose();
		}
		delete fiorw;
	}

	// reinit
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
	memset(&cart_header, 0, sizeof(cart_header));
	memset(cart, 0xff, sizeof(cart));
	memset(sram, 0xff, sizeof(sram));
}

#define STATE_VERSION	2

bool MEMORY::process_state(FILEIO* state_fio, bool loading)
{
	/*
	if(!state_fio->StateCheckUint32(STATE_VERSION))
		return false;
	if(!state_fio->StateCheckInt32(this_device_id))
		return false;
	state_fio->StateArray(cart_header.id, sizeof(cart_header.id), 1);
	state_fio->StateArray(cart_header.type, sizeof(cart_header.type), 1);
	state_fio->StateValue(cart_header.version);
	state_fio->StateValue(cart_header.nb_block);
	state_fio->StateValue(cart_header.block[0]);
	state_fio->StateValue(cart_header.block[1]);
	state_fio->StateValue(cart_header.block[2]);
	state_fio->StateValue(cart_header.block[3]);
	state_fio->StateValue(cart_header.block[4]);
	state_fio->StateValue(cart_header.block[5]);
	state_fio->StateValue(cart_header.crc32);
	state_fio->StateValue(bios_found);
	state_fio->StateValue(bios_present);
	state_fio->StateValue(bios_ok);
	state_fio->StateValue(cart_found);
	state_fio->StateValue(cart_inserted);
	state_fio->StateValue(cart_ok);
	state_fio->StateArray(ram_path, sizeof(ram_path), 1);
	state_fio->StateValue(ram_block);
	state_fio->StateValue(ram_used);
	state_fio->StateValue(ram_save);
	state_fio->StateValue(ram_bank0);
	state_fio->StateValue(ram_bank1);
	state_fio->StateValue(ram_bank2);
	state_fio->StateValue(ram_bank3);
	state_fio->StateValue(ram_offset);
	state_fio->StateValue(ram_size);
	state_fio->StateValue(ram_crc32);
	state_fio->StateArray(vram, sizeof(vram), 1);
	state_fio->StateArray(wreg, sizeof(wreg), 1);
	state_fio->StateArray(sram, sizeof(sram), 1);
	state_fio->StateValue(cur_bank);
	
	// post process
	if(loading)
		set_bank(cur_bank);
	state_fio->StateValue(ram_used);
	*/
	return true;
}
