/*
	EPOCH Super Cassette Vision Emulator 'eSCV'

	Author : Takeda.Toshiya
	Date   : 2006.08.21 -

	Modified for Libretro-EmuSCV
	Author : MARCONATO Maxime (aka MaaaX)
	Date   : 2019-12-05 - 

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

		if(config.scv_checkbios != SETTING_CHECKBIOS_YES_VAL)
		{
				bios_ok = true;
		}
		else
		{
			// check bios MD5 hash
			_TCHAR bios_md5[33];
			memset(bios_md5, 0, sizeof(bios_md5));
			memcpy(bios_md5, get_md5(bios, sizeof(bios)), sizeof(bios_md5)-1);
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

			if(config.scv_checkbios != SETTING_CHECKBIOS_YES_VAL)
			{
				bios_ok = true;
			}
			else
			{
				// check bios MD5 hash
				_TCHAR bios_md5[33];
				memset(bios_md5, 0, sizeof(bios_md5));
				memcpy(bios_md5, get_md5(bios, sizeof(bios)), sizeof(bios_md5)-1);
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

			if(config.scv_checkbios != SETTING_CHECKBIOS_YES_VAL)
			{
				bios_ok = true;
			}
			else
			{
				// check bios MD5 hash
				_TCHAR bios_md5[33];
				memset(bios_md5, 0, sizeof(bios_md5));
				memcpy(bios_md5, get_md5(bios, sizeof(bios)), sizeof(bios_md5)-1);
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

			if(config.scv_checkbios != SETTING_CHECKBIOS_YES_VAL)
			{
				bios_ok = true;
			}
			else
			{
				// check bios MD5 hash
				_TCHAR bios_md5[33];
				memset(bios_md5, 0, sizeof(bios_md5));
				memcpy(bios_md5, get_md5(bios, sizeof(bios)), sizeof(bios_md5)-1);
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

			if(config.scv_checkbios != SETTING_CHECKBIOS_YES_VAL)
			{
				bios_ok = true;
			}
			else
			{
				// check bios MD5 hash
				_TCHAR bios_md5[33];
				memset(bios_md5, 0, sizeof(bios_md5));
				memcpy(bios_md5, get_md5(bios, sizeof(bios)), sizeof(bios_md5)-1);
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

// Extract current file in a zip archive
static bool memory_zipfile_currentfile_extract(unzFile zipfile, const _TCHAR* file_path)
{
	unz_file_info zipfile_file_info;
	if(unzOpenCurrentFile(zipfile) != UNZ_OK
	|| unzGetCurrentFileInfo(zipfile, &zipfile_file_info, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK
	|| unzOpenCurrentFile(zipfile) != UNZ_OK)
		return false;

	FILEIO *fionewfile = new FILEIO();
	if(!fionewfile || !fionewfile->Fopen(file_path, FILEIO_WRITE_BINARY))
		return false;
	
	if(zipfile_file_info.uncompressed_size == 0)
	{
		fionewfile->Fclose();
		unzCloseCurrentFile(zipfile);
		return true;
	}

	uint8_t *buffer = (uint8_t*)malloc(zipfile_file_info.uncompressed_size);
	if(!buffer)
		return false;
	memset(buffer, 0, zipfile_file_info.uncompressed_size);
	unsigned int buffer_level = 0;
	unsigned int bytes_to_read = zipfile_file_info.uncompressed_size;
	int bytes_ridden = 0;
	do
	{
		bytes_ridden = unzReadCurrentFile(zipfile, buffer+buffer_level, bytes_to_read);
		if(bytes_ridden < 0)
		{
			// read error
			if(buffer_level > 0)
				fionewfile->Fwrite(buffer, buffer_level, 1);
			fionewfile->Fclose();
			unzCloseCurrentFile(zipfile);
			return false;
		}
		else if(bytes_ridden > 0)
		{
			// Write data to file
			bytes_to_read -= bytes_ridden;
			buffer_level  += bytes_ridden;
		}
	} while(bytes_to_read > 0 && bytes_ridden > 0);

	if(buffer_level > 0)
		fionewfile->Fwrite(buffer, buffer_level, 1);
	
	fionewfile->Fclose();
	unzCloseCurrentFile(zipfile);
	return true;
}

void MEMORY::open_cart(const _TCHAR* file_path)
{
	uint8_t		  index			= 0;
	uint16_t	  size			= 0x0000;
	uint16_t	  type			= 0;
	uint16_t	  bank			= 0;
	uint16_t	  offset		= 0x0000;
	uint32_t	  cart_crc32	= 0;
	size_t		  len			= 0;
	FILEIO		 *fiocart		= new FILEIO();
	uint8_t		  data[0x8000];
	_TCHAR		  newcart_path[_MAX_PATH];
	_TCHAR		  save_file_path[_MAX_PATH];
	_TCHAR		  rom_file_path[_MAX_PATH];
	const _TCHAR *file_name		= get_file_path_file(file_path);
	bool   cart_in_memory		= false;
	uint32_t	  data_index	= 0;
	uint8_t	      cart_data[0x20000];
	uint8_t       zipfile_number = 0;
	_TCHAR        zipfile_file_path[4][_MAX_PATH];


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
	memset(rom_file_path, 0, sizeof(rom_file_path));
	memcpy(rom_file_path, file_path, strlen(file_path));
	memset(&newcart_path, 0, sizeof(newcart_path));
 #if defined(_LIBRETRO)
 	memcpy(save_file_path, get_libretro_save_directory(), sizeof(save_file_path)-1);
 #elif
	memcpy(save_file_path, get_file_path_directory(rom_file_path), sizeof(save_file_path)-1);
 #endif
	memset(cart_data, 0xFF, sizeof(data));

	// Check file path
	len = strlen(rom_file_path);
	if(len == 0)
		goto lbl_rom_error;	// Fatal error

	// Extract zip file
	if(rom_file_path[len - 4] == _T('.')
	&& (rom_file_path[len - 3] == _T('z') || rom_file_path[len - 3] == _T('Z'))
	&& (rom_file_path[len - 2] == _T('i') || rom_file_path[len - 2] == _T('I'))
	&& (rom_file_path[len - 1] == _T('p') || rom_file_path[len - 1] == _T('P')))
	{
		unzFile zipfile = unzOpen(rom_file_path);
		if(zipfile == NULL)
			goto lbl_rom_error;

		unz_global_info zipfile_global_info;
		// Get info about the zip file
		if(unzGetGlobalInfo(zipfile, &zipfile_global_info) != UNZ_OK || zipfile_global_info.number_entry == 0)
		{
			unzClose(zipfile);
			goto lbl_rom_error;
		}
		_TCHAR zipfile_file[4][_MAX_PATH];
		memset(zipfile_file[0], 0, sizeof(zipfile_file[0]));
		memset(zipfile_file[1], 0, sizeof(zipfile_file[1]));
		memset(zipfile_file[2], 0, sizeof(zipfile_file[2]));
		memset(zipfile_file[3], 0, sizeof(zipfile_file[3]));
		memset(zipfile_file_path[0], 0, sizeof(zipfile_file_path[0]));
		memset(zipfile_file_path[1], 0, sizeof(zipfile_file_path[1]));
		memset(zipfile_file_path[2], 0, sizeof(zipfile_file_path[2]));
		memset(zipfile_file_path[3], 0, sizeof(zipfile_file_path[3]));
		zipfile_number = 0;
		size_t filename_len = strlen(file_name);
		memcpy(zipfile_file[zipfile_number], file_name, filename_len);

		zipfile_file[zipfile_number][filename_len - 3] = _T('c');
		zipfile_file[zipfile_number][filename_len - 2] = _T('a');
		zipfile_file[zipfile_number][filename_len - 1] = _T('r');
		zipfile_file[zipfile_number][filename_len]     = _T('t');
		if(unzLocateFile(zipfile, zipfile_file[zipfile_number], 2) == UNZ_OK)	// .cart (1 file)
		{
			memset(zipfile_file_path[zipfile_number], 0, sizeof(zipfile_file_path[zipfile_number]));
			memcpy(zipfile_file_path[zipfile_number], save_file_path, strlen(save_file_path));
			memcpy(zipfile_file_path[zipfile_number]+strlen(save_file_path), zipfile_file[zipfile_number], strlen(zipfile_file[zipfile_number]));
			memory_zipfile_currentfile_extract(zipfile, zipfile_file_path[zipfile_number]);
			zipfile_number++;
		}
		else
		{
			zipfile_file[zipfile_number][filename_len - 3] = _T('b');
			zipfile_file[zipfile_number][filename_len - 2] = _T('i');
			zipfile_file[zipfile_number][filename_len - 1] = _T('n');
			zipfile_file[zipfile_number][filename_len]     = 0;
			if(unzLocateFile(zipfile, zipfile_file[zipfile_number], 2) == UNZ_OK)	// .bin (1 file)
			{
				memset(zipfile_file_path[zipfile_number], 0, sizeof(zipfile_file_path[zipfile_number]));
				memcpy(zipfile_file_path[zipfile_number], save_file_path, strlen(save_file_path));
				memcpy(zipfile_file_path[zipfile_number]+strlen(save_file_path), zipfile_file[zipfile_number], strlen(zipfile_file[zipfile_number]));
				memory_zipfile_currentfile_extract(zipfile, zipfile_file_path[zipfile_number]);
				zipfile_number++;
			}
			else
			{
				zipfile_file[zipfile_number][filename_len - 3] = _T('r');
				zipfile_file[zipfile_number][filename_len - 2] = _T('o');
				zipfile_file[zipfile_number][filename_len - 1] = _T('m');
				if(unzLocateFile(zipfile, zipfile_file[zipfile_number], 2) == UNZ_OK)	// .rom (1 file)
				{
					memset(zipfile_file_path[zipfile_number], 0, sizeof(zipfile_file_path[zipfile_number]));
					memcpy(zipfile_file_path[zipfile_number], save_file_path, strlen(save_file_path));
					memcpy(zipfile_file_path[zipfile_number]+strlen(save_file_path), zipfile_file[zipfile_number], strlen(zipfile_file[zipfile_number]));
					memory_zipfile_currentfile_extract(zipfile, zipfile_file_path[zipfile_number]);
					zipfile_number++;
				}
				else
				{
					zipfile_file[zipfile_number][filename_len - 3] = _T('0');
					zipfile_file[zipfile_number][filename_len - 2] = 0;
					zipfile_file[zipfile_number][filename_len - 1] = 0;
					if(unzLocateFile(zipfile, zipfile_file[zipfile_number], 2) == UNZ_OK)	// .0 (1, 2 or 4 files)
					{
						memset(zipfile_file_path[zipfile_number], 0, sizeof(zipfile_file_path[zipfile_number]));
						memcpy(zipfile_file_path[zipfile_number], save_file_path, strlen(save_file_path));
						memcpy(zipfile_file_path[zipfile_number]+strlen(save_file_path), zipfile_file[zipfile_number], strlen(zipfile_file[zipfile_number]));
						memory_zipfile_currentfile_extract(zipfile, zipfile_file_path[zipfile_number]);
						zipfile_number++;

						memcpy(zipfile_file[zipfile_number], zipfile_file[zipfile_number-1], filename_len);
						zipfile_file[zipfile_number][filename_len - 3] = _T('1');
						if(unzLocateFile(zipfile, zipfile_file[zipfile_number], 2) == UNZ_OK)	// .1 (2 or 4 files)
						{
							memset(zipfile_file_path[zipfile_number], 0, sizeof(zipfile_file_path[zipfile_number]));
							memcpy(zipfile_file_path[zipfile_number], save_file_path, strlen(save_file_path));
							memcpy(zipfile_file_path[zipfile_number]+strlen(save_file_path), zipfile_file[zipfile_number], strlen(zipfile_file[zipfile_number]));
							memory_zipfile_currentfile_extract(zipfile, zipfile_file_path[zipfile_number]);
							zipfile_number++;

							memcpy(zipfile_file[zipfile_number], zipfile_file[zipfile_number-1], filename_len);
							zipfile_file[zipfile_number][filename_len - 3] = _T('2');
							if(unzLocateFile(zipfile, zipfile_file[zipfile_number], 2) == UNZ_OK)	// .2 (3 or 4 files)
							{
								memset(zipfile_file_path[zipfile_number], 0, sizeof(zipfile_file_path[zipfile_number]));
								memcpy(zipfile_file_path[zipfile_number], save_file_path, strlen(save_file_path));
								memcpy(zipfile_file_path[zipfile_number]+strlen(save_file_path), zipfile_file[zipfile_number], strlen(zipfile_file[zipfile_number]));
								memory_zipfile_currentfile_extract(zipfile, zipfile_file_path[zipfile_number]);
								zipfile_number++;

								memcpy(zipfile_file[zipfile_number], zipfile_file[zipfile_number-1], filename_len);
								zipfile_file[zipfile_number][filename_len - 3] = _T('3');
								if(unzLocateFile(zipfile, zipfile_file[zipfile_number], 2) == UNZ_OK)	// .3 (4 files)
								{
									memset(zipfile_file_path[zipfile_number], 0, sizeof(zipfile_file_path[zipfile_number]));
									memcpy(zipfile_file_path[zipfile_number], save_file_path, strlen(save_file_path));
									memcpy(zipfile_file_path[zipfile_number]+strlen(save_file_path), zipfile_file[zipfile_number], strlen(zipfile_file[zipfile_number]));
									memory_zipfile_currentfile_extract(zipfile, zipfile_file_path[zipfile_number]);
									zipfile_number++;
								}
								else
								{
									// Missing file 4
									unzClose(zipfile);
									goto lbl_rom_error;
								}
							}
						}
					}
					else
					{
						// Missing file 1
						unzClose(zipfile);
						goto lbl_rom_error;
					}
				}
			}
		}
		unzClose(zipfile);
		if(zipfile_number > 0)
		{
			memset(rom_file_path, 0, sizeof(rom_file_path));
			len = strlen(zipfile_file_path[0]);
			memcpy(rom_file_path, zipfile_file_path[0], len);
		}
	}

	if(rom_file_path[len - 5] != _T('.')
	|| (rom_file_path[len - 4] != _T('c') && rom_file_path[len - 4] != _T('C'))
	|| (rom_file_path[len - 3] != _T('a') && rom_file_path[len - 3] != _T('A'))
	|| (rom_file_path[len - 2] != _T('r') && rom_file_path[len - 2] != _T('R'))
	|| (rom_file_path[len - 1] != _T('t') && rom_file_path[len - 1] != _T('T')))
	{
		uint8_t	      raw[0x20000];
		uint32_t      cart_size				= 0;
		_TCHAR        raw_md5[33];
		FILEIO       *fionewcart			= new FILEIO();
		size_t        file_size;
		size_t        read_size;

		memset(newcart_path, 0, sizeof(newcart_path));
		memcpy(newcart_path, save_file_path, strlen(save_file_path));
		memcpy(newcart_path+strlen(save_file_path), file_name, strlen(file_name));
		uint8_t newcart_path_len = strlen(newcart_path);

		if(newcart_path_len+4 > _MAX_PATH-1)
		{
			newcart_path[newcart_path_len-5] = _T('.');
			newcart_path[newcart_path_len-4] = _T('c');
			newcart_path[newcart_path_len-3] = _T('a');
			newcart_path[newcart_path_len-2] = _T('r');
			newcart_path[newcart_path_len-1] = _T('t');
		}
		else if(newcart_path[newcart_path_len - 4] == _T('.')
		&& (newcart_path[newcart_path_len - 3] != _T('b') || newcart_path[newcart_path_len - 3] != _T('B') || newcart_path[newcart_path_len - 3] != _T('r') || newcart_path[newcart_path_len - 3] != _T('R'))
		&& (newcart_path[newcart_path_len - 2] != _T('i') || newcart_path[newcart_path_len - 2] != _T('I') || newcart_path[newcart_path_len - 2] != _T('o') || newcart_path[newcart_path_len - 2] != _T('O'))
		&& (newcart_path[newcart_path_len - 1] != _T('n') || newcart_path[newcart_path_len - 1] != _T('N') || newcart_path[newcart_path_len - 1] != _T('m') || newcart_path[newcart_path_len - 1] != _T('M')))
		{
			newcart_path[newcart_path_len-4] = _T('.');
			newcart_path[newcart_path_len-3] = _T('c');
			newcart_path[newcart_path_len-2] = _T('a');
			newcart_path[newcart_path_len-1] = _T('r');
			newcart_path[newcart_path_len]   = _T('t');
		}
		else if(newcart_path[newcart_path_len-2] == _T('.') && newcart_path[newcart_path_len-1] == _T('0'))
		{
			newcart_path[newcart_path_len-2] = _T('.');
			newcart_path[newcart_path_len-1] = _T('c');
			newcart_path[newcart_path_len]   = _T('a');
			newcart_path[newcart_path_len+1] = _T('r');				
			newcart_path[newcart_path_len+2] = _T('t');				
		}
		else
		{
			newcart_path[newcart_path_len] = _T('.');
			newcart_path[newcart_path_len+1] = _T('c');
			newcart_path[newcart_path_len+2] = _T('a');
			newcart_path[newcart_path_len+3] = _T('r');				
			newcart_path[newcart_path_len+4] = _T('t');				
		}

		// Delete existing .CART file
		if(fionewcart->IsFileExisting(newcart_path))
			fionewcart->RemoveFile(newcart_path);

		// Open raw file
		if(!fiocart->Fopen(rom_file_path, FILEIO_READ_BINARY))
			goto lbl_rom_end;	// Fatal error
		// Can read 128Ko max only
		file_size = fiocart->FileLength();
		if(file_size > sizeof(raw))
			goto lbl_rom_error;	// Fatal error

		// Read raw file
		memset(raw, 0, sizeof(raw));
		// First chance
		read_size = fiocart->Fread(&raw, sizeof(raw), 1);

		// Get MD5
		memset(raw_md5, 0, sizeof(raw_md5));
		memcpy(raw_md5, get_md5(raw, file_size), sizeof(raw_md5)-1);

		// Different type of cart
		// 
		// === Single file =================================================================================
		// 
		if(strcmp(raw_md5, _T("5c0a8b9e0ae3bdc62cf758c8de3c621c")) == 0		// ASTRO WARS (1 file of 8KB)
		|| strcmp(raw_md5, _T("28b41f62cefae2524f6ae1b980d054d7")) == 0)	// ASTRO WARS II (1 file of 8KB)
		{
			cart_header.nb_block = 1;
			cart_header.block[0] = BLOCK_SIZE_8KB|BLOCK_TYPE_ROM|BLOCK_BANK_0123|BLOCK_ADDR_8000;
			cart_size = 0x2000;
			memcpy(cart_data, raw, cart_size);
		}
		else if(strcmp(raw_md5, _T("c800d70c4a1f9600407d8392b9455015")) == 0	// SUPER BASEBALL (1 file of 16KB)
		|| strcmp(raw_md5, _T("4471a0938102c21a49635d3e4efb61bc")) == 0			// BOULDER DASH (1 file of 16KB)
		|| strcmp(raw_md5, _T("21b1f2432b4ea67dc7162630100b2cd5")) == 0			// BOULDER DASH (alternate) (1 file of 16KB)
		|| strcmp(raw_md5, _T("ec261a6208094fceb7a4f8cc60db24b4")) == 0			// COMIC CIRCUS (1 file of 16KB)
		|| strcmp(raw_md5, _T("3ac6b89ba13e57100d522921abb7319c")) == 0			// ELEVATOR FIGHT (1 file of 16KB)
		|| strcmp(raw_md5, _T("e1547118cf5a9e88825408f12550b890")) == 0			// LUPIN III (1 file of 16KB)
		|| strcmp(raw_md5, _T("07e8dc7cbaacedbbcb483a4e7e6b18f3")) == 0			// LUPIN III (alternate) (1 file of 16KB)
		|| strcmp(raw_md5, _T("f4a3f4f5a08a15ec62a32e959a528eac")) == 0			// MINER 2049ER (1 file of 16KB)
		|| strcmp(raw_md5, _T("e86aab083fc9722f8a44b356139522c2")) == 0			// NEBULA (1 file of 16KB)
		|| strcmp(raw_md5, _T("ac629947980bbd14478c29c1645efa41")) == 0			// NEBULA (alternate) (1 file of 16KB)
		|| strcmp(raw_md5, _T("7c10f8df512ce0bbfad7505ccca88827")) == 0			// PUNCH BOY (1 file of 16KB)
		|| strcmp(raw_md5, _T("c9e1042402b5a0ff6f75b737fec8a08a")) == 0			// REAL MAHJONG (1 file of 16KB)
		|| strcmp(raw_md5, _T("5cd60ad0a9bfb818db9c4e5accc05537")) == 0			// SUPER SOCCER (1 file of 16KB)
		|| strcmp(raw_md5, _T("dd60e91b361fdc8bc8ead3d76c45897c")) == 0			// SUPER SOCCER (alternate) (1 file of 16KB)
		|| strcmp(raw_md5, _T("734091f00d410fc4251f0aae7d47e4c1")) == 0			// SUPER GOLF (1 file of 16KB)
		|| strcmp(raw_md5, _T("622cbb0451fbf82b9b7834c5ca589443")) == 0			// TON TON BALL (1 file of 16KB)
		|| strcmp(raw_md5, _T("ef4ab0300b9e056d1ba9873b63a1b6cf")) == 0)		// WHEELY RACER (1 file of 16KB)
		{
			cart_header.nb_block = 1;
			cart_header.block[0] = BLOCK_SIZE_16KB|BLOCK_TYPE_ROM|BLOCK_BANK_0123|BLOCK_ADDR_8000;
			cart_size = 0x4000;
			memcpy(cart_data, raw, cart_size);
		}
		else if(strcmp(raw_md5, _T("801619e946eb962fe13f4582751c7588")) == 0	// MAPPY (1 file of 32KB)
		|| strcmp(raw_md5, _T("9449bddc1056d51c7147dc2e6329d2ed")) == 0			// MILKY PRINCESS (1 file of 32KB)
		|| strcmp(raw_md5, _T("05f2b3b1a5e450c8ead224c3ebf5b75f")) == 0)		// SUPER SANSUPYUTA (1 file of 32KB)
		{
			cart_header.nb_block = 1;
			cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_0123|BLOCK_ADDR_8000;
			cart_size = 0x8000;
			memcpy(cart_data, raw, cart_size);
		}
		else if(strcmp(raw_md5, _T("6da4e79f9dd60abe222ae8f95023ad48")) == 0	// BASIC NYUMON (1 file of 32KB)
		|| strcmp(raw_md5, _T("9c688c8f43b142a84b90181d717df6d2")) == 0			// DRAGON SLAYER (1 file of 32KB)
		|| strcmp(raw_md5, _T("e2488c33d92ef9985e6b62b45644303c")) == 0)		// POP & CHIPS (1 file of 32KB)
		{
			cart_header.nb_block = 2;
			cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_0123|BLOCK_ADDR_8000;
			cart_header.block[1] = BLOCK_SIZE_8KB|BLOCK_TYPE_SRAM|BLOCK_BANK_13|BLOCK_ADDR_E000;
			cart_size = 0x8000;
			memcpy(cart_data, raw, cart_size);
		}
		else if(strcmp(raw_md5, _T("b248fff8526d5c3cfba82a6e036815ca")) == 0)	// SHOGI (1 file of 32KB)
		{
			cart_header.nb_block = 2;
			cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_0123|BLOCK_ADDR_8000;
			cart_header.block[1] = BLOCK_SIZE_8KB|BLOCK_TYPE_VRAM|BLOCK_BANK_13|BLOCK_ADDR_E000;
			cart_size = 0x8000;
			memcpy(cart_data, raw, cart_size);
		}
		else if(strcmp(raw_md5, _T("fa05cb7422744b0d8a73125db576708f")) == 0	// KUNG-FU ROAD (1 file of 40KB)
		|| strcmp(raw_md5, _T("3a4e3634b4390241beda0a94fa69564d")) == 0)		// STAR SPEEDER (1 file of 40KB)
		{
			cart_header.nb_block = 2;
			cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_0123|BLOCK_ADDR_8000;
			cart_header.block[1] = BLOCK_SIZE_8KB|BLOCK_TYPE_ROM|BLOCK_BANK_13|BLOCK_ADDR_E000;
			cart_size = 0xA000;
			memcpy(cart_data, raw, cart_size);
		}
		else if(strcmp(raw_md5, _T("e93f06b2ccbafa753a2eac14f92070c2")) == 0	// DORAEMON (1 file of 64KB)
		|| strcmp(raw_md5, _T("9b6dd35dd278bcfaa98d4ecf531654cf")) == 0)		// SKY KID (1 file of 64KB)
		{
			cart_header.nb_block = 2;
			cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_02|BLOCK_ADDR_8000;
			cart_header.block[1] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_13|BLOCK_ADDR_8000;
			cart_size = 0x10000;
			memcpy(cart_data, raw, cart_size);
		}
		else if(strcmp(raw_md5, _T("6d11d49390d0946501b39d9f9688ae9d")) == 0)	// DRAGON BALL (1 file of 128KB)
		{
			cart_header.nb_block = 3;
			cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_01|BLOCK_ADDR_8000;
			cart_header.block[1] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_2|BLOCK_ADDR_8000;
			cart_header.block[2] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_3|BLOCK_ADDR_8000;
			cart_size = 0x18000;
			memcpy(cart_data, raw+0x8000, cart_size);
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
		}
		else if(rom_file_path[len - 1] == _T('0'))
		{
			// 
			// === Multiple files =================================================================================
			// 
			_TCHAR next_rom_file_path[_MAX_PATH];
			memset(&next_rom_file_path, 0, sizeof(next_rom_file_path));
			memcpy(next_rom_file_path, rom_file_path, strlen(rom_file_path));
			fiocart->Fclose();
			next_rom_file_path[len - 1] = _T('1');			
			if(!fiocart->Fopen(next_rom_file_path, FILEIO_READ_BINARY))
				goto lbl_rom_error;	// Fatal error
			// Can read 128Ko max only
			file_size = fiocart->FileLength();
			if(file_size > sizeof(raw))
				goto lbl_rom_error;	// Fatal error

			if(strcmp(raw_md5, _T("364c7dae2b5ee1d0525906c501b276a5")) == 0)	// KUNG-FU ROAD (2 files of 32KB and 8KB) File 0 of 32KB
			{
				cart_header.nb_block = 2;
				cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_0123|BLOCK_ADDR_8000;
				cart_header.block[1] = BLOCK_SIZE_8KB|BLOCK_TYPE_ROM|BLOCK_BANK_13|BLOCK_ADDR_E000;
				cart_size = 0xA000;
				memcpy(cart_data, raw, 0x8000);

				// Read raw file
				memset(raw, 0, sizeof(raw));
				// First chance
				read_size = fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				memcpy(raw_md5, get_md5(raw, file_size), sizeof(raw_md5)-1);

				if(strcmp(raw_md5, _T("4b2f0e32943ed64561a03e267d6f3fd9")) != 0)	// File 1 of 8KB
					goto lbl_rom_error;	// Fatal error
				
				memcpy(cart_data+0x8000, raw, 0x2000);
			}
			else if(strcmp(raw_md5, _T("3060d95c563115bde239375518ee6b1d")) == 0)	// STAR SPEEDER (2 files of 32KB and 8KB) File 0 of 32KB
			{
				cart_header.nb_block = 2;
				cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_0123|BLOCK_ADDR_8000;
				cart_header.block[1] = BLOCK_SIZE_8KB|BLOCK_TYPE_ROM|BLOCK_BANK_13|BLOCK_ADDR_E000;
				cart_size = 0xA000;
				memcpy(cart_data, raw, 0x8000);

				// Read raw file
				memset(raw, 0, sizeof(raw));
				// First chance
				read_size = fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				memcpy(raw_md5, get_md5(raw, file_size), sizeof(raw_md5)-1);

				if(strcmp(raw_md5, _T("19c339d88cef61bde7048409828a42e7")) != 0)	// File 1 of 8KB
					goto lbl_rom_error;	// Fatal error
				
				memcpy(cart_data+0x8000, raw, 0x2000);
			}
			else if(strcmp(raw_md5, _T("8928d34618d98384119ffa351d0293cf")) == 0)	// DORAEMON (2 files of 32KB) File 0 of 32KB
			{
				cart_header.nb_block = 2;
				cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_02|BLOCK_ADDR_8000;
				cart_header.block[1] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_13|BLOCK_ADDR_8000;
				cart_size = 0x10000;
				memcpy(cart_data, raw, 0x8000);

				// Read raw file
				memset(raw, 0, sizeof(raw));
				// First chance
				read_size = fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				memcpy(raw_md5, get_md5(raw, file_size), sizeof(raw_md5)-1);

				if(strcmp(raw_md5, _T("3ac9c39b019a81fabb1e26f75292827c")) != 0)	// File 1 of 32KB
					goto lbl_rom_error;	// Fatal error

				memcpy(cart_data+0x8000, raw, 0x8000);
			}
			else if(strcmp(raw_md5, _T("9fa449f97ed26b328793a2ae7c3a7d51")) == 0)	// SKY KID (2 files of 32KB) File 0 of 32KB
			{
				cart_header.nb_block = 2;
				cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_02|BLOCK_ADDR_8000;
				cart_header.block[1] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_13|BLOCK_ADDR_8000;
				cart_size = 0x10000;
				memcpy(cart_data, raw, 0x8000);

				// Read raw file
				memset(raw, 0, sizeof(raw));
				// First chance
				read_size = fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				memcpy(raw_md5, get_md5(raw, file_size), sizeof(raw_md5)-1);

				if(strcmp(raw_md5, _T("a6f81f3b8351771ed7ba3b2f7e987d65")) != 0)	// File 1 of 32KB
					goto lbl_rom_error;	// Fatal error

				memcpy(cart_data+0x8000, raw, 0x8000);
			}
			else if(strcmp(raw_md5, _T("61a6f0c4ef26db9d073b71b21ef957ea")) == 0)	// DRAGON BALL (4 files of 32KB) File 0 of 32KB
			{
				cart_header.nb_block = 3;
				cart_header.block[0] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_01|BLOCK_ADDR_8000;
				cart_header.block[1] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_2|BLOCK_ADDR_8000;
				cart_header.block[2] = BLOCK_SIZE_32KB|BLOCK_TYPE_ROM|BLOCK_BANK_3|BLOCK_ADDR_8000;
				cart_size = 0x18000;
				memcpy(cart_data, raw, 0x8000);

				// Read raw file
				memset(raw, 0, sizeof(raw));
				// First chance
				read_size = fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				memcpy(raw_md5, get_md5(raw, file_size), sizeof(raw_md5)-1);

				if(strcmp(raw_md5, _T("61a6f0c4ef26db9d073b71b21ef957ea")) != 0)	// File 1 of 32KB (same data as file 0)
					goto lbl_rom_error;	// Fatal error
				
				// Data ignored here because same as file 0

				// Open next file
				fiocart->Fclose();
				next_rom_file_path[len - 1] = _T('2');
				if(!fiocart->Fopen(next_rom_file_path, FILEIO_READ_BINARY))
					goto lbl_rom_error;	// Fatal error
				// Can read 128Ko max only
				file_size = fiocart->FileLength();
				if(file_size > sizeof(raw))
					goto lbl_rom_error;	// Fatal error

				// Read raw file
				memset(raw, 0, sizeof(raw));
				// First chance
				read_size = fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				memcpy(raw_md5, get_md5(raw, file_size), sizeof(raw_md5)-1);

				if(strcmp(raw_md5, _T("122398143f8e538b26b0bd1356431022")) != 0)	// File 2 of 32KB
					goto lbl_rom_error;	// Fatal error
				
				memcpy(cart_data+0x8000, raw, 0x8000);

				// Open next file
				fiocart->Fclose();
				next_rom_file_path[len - 1] = _T('3');
				if(!fiocart->Fopen(next_rom_file_path, FILEIO_READ_BINARY))
					goto lbl_rom_error;	// Fatal error
				// Can read 128Ko max only
				file_size = fiocart->FileLength();
				if(file_size > sizeof(raw))
					goto lbl_rom_error;	// Fatal error

				// Read raw file
				memset(raw, 0, sizeof(raw));
				// First chance
				read_size = fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				memcpy(raw_md5, get_md5(raw, file_size), sizeof(raw_md5)-1);

				if(strcmp(raw_md5, _T("087179b0a06f30fe5b35a82dc45a6df9")) != 0)	// File 3 of 32sKB
					goto lbl_rom_error;	// Fatal error

				memcpy(cart_data+0x10000, raw, 0x8000);
			}
			else if(strcmp(raw_md5, _T("f5020cfd5fa59a06be927540638c5444")) == 0)	// PROFESSIONAL WRESTLING (4 files of 32KB) File 0 of 32KB
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
				// First chance
				read_size = fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				memcpy(raw_md5, get_md5(raw, file_size), sizeof(raw_md5)-1);

				if(strcmp(raw_md5, _T("8d1269381114d330757f91fb07e2d76d")) != 0)	// File 1 of 32KB
					goto lbl_rom_error;	// Fatal error
				
				memcpy(cart_data+0x8000, raw, 0x8000);

				// Open next file
				fiocart->Fclose();
				next_rom_file_path[len - 1] = _T('2');
				if(!fiocart->Fopen(next_rom_file_path, FILEIO_READ_BINARY))
					goto lbl_rom_error;	// Fatal error
				// Can read 128Ko max only
				file_size = fiocart->FileLength();
				if(file_size > sizeof(raw))
					goto lbl_rom_error;	// Fatal error

				// Read raw file
				memset(raw, 0, sizeof(raw));
				// First chance
				read_size = fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				memcpy(raw_md5, get_md5(raw, file_size), sizeof(raw_md5)-1);

				if(strcmp(raw_md5, _T("f8328107cb60e3341e9c25c14af239a4")) != 0)	// File 2 of 32KB
					goto lbl_rom_error;	// Fatal error
				
				memcpy(cart_data+0x10000, raw, 0x8000);

				// Open next file
				fiocart->Fclose();
				next_rom_file_path[len - 1] = _T('3');
				if(!fiocart->Fopen(next_rom_file_path, FILEIO_READ_BINARY))
					goto lbl_rom_error;	// Fatal error
				// Can read 128Ko max only
				file_size = fiocart->FileLength();
				if(file_size > sizeof(raw))
					goto lbl_rom_error;	// Fatal error

				// Read raw file
				memset(raw, 0, sizeof(raw));
				// First chance
				read_size = fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				memcpy(raw_md5, get_md5(raw, file_size), sizeof(raw_md5)-1);

				if(strcmp(raw_md5, _T("a3556d2c9ec743085590645b9bbbe885")) != 0)	// File 3 of 32sKB
					goto lbl_rom_error;	// Fatal error

				memcpy(cart_data+0x18000, raw, 0x8000);
			}
			else if(strcmp(raw_md5, _T("c6baa36261ff82e8c454e59714ab4771")) == 0)	// Y2 MONSTER LAND (4 files of 32KB) File 0 of 32KB
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
				// First chance
				read_size = fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				memcpy(raw_md5, get_md5(raw, file_size), sizeof(raw_md5)-1);

				if(strcmp(raw_md5, _T("bfe6c003f5652ebe344139e3c2ed8052")) != 0)	// File 1 of 32KB
					goto lbl_rom_error;	// Fatal error
				
				memcpy(cart_data+0x8000, raw, 0x8000);

				// Open next file
				fiocart->Fclose();
				next_rom_file_path[len - 1] = _T('2');
				if(!fiocart->Fopen(next_rom_file_path, FILEIO_READ_BINARY))
					goto lbl_rom_error;	// Fatal error
				// Can read 128Ko max only
				file_size = fiocart->FileLength();
				if(file_size > sizeof(raw))
					goto lbl_rom_error;	// Fatal error

				// Read raw file
				memset(raw, 0, sizeof(raw));
				// First chance
				read_size = fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				memcpy(raw_md5, get_md5(raw, file_size), sizeof(raw_md5)-1);

				if(strcmp(raw_md5, _T("533e823555bc42d1c0547068a03f845e")) != 0)	// File 2 of 32KB
					goto lbl_rom_error;	// Fatal error
				
				memcpy(cart_data+0x10000, raw, 0x8000);

				// Open next file
				fiocart->Fclose();
				next_rom_file_path[len - 1] = _T('3');
				if(!fiocart->Fopen(next_rom_file_path, FILEIO_READ_BINARY))
					goto lbl_rom_error;	// Fatal error
				// Can read 128Ko max only
				file_size = fiocart->FileLength();
				if(file_size > sizeof(raw))
					goto lbl_rom_error;	// Fatal error

				// Read raw file
				memset(raw, 0, sizeof(raw));
				// First chance
				read_size = fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				memcpy(raw_md5, get_md5(raw, file_size), sizeof(raw_md5)-1);

				if(strcmp(raw_md5, _T("465f41fc3d3b3a689856b108dbf229a7")) != 0)	// File 3 of 32sKB
					goto lbl_rom_error;	// Fatal error
				
				memcpy(cart_data+0x18000, raw, 0x8000);
			}
			else if(strcmp(raw_md5, _T("3df7b33399422731e8e5615785c0536d")) == 0)	// POLE POSITION II (4 files of 32KB) File 0 of 32KB filled with 0xFF
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
				// First chance
				read_size = fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				memcpy(raw_md5, get_md5(raw, file_size), sizeof(raw_md5)-1);

				if(strcmp(raw_md5, _T("4a6d816aa720a8d921b0e38116d205e7")) != 0)	// File 1 of 32KB
					goto lbl_rom_error;	// Fatal error
				
				memcpy(cart_data, raw, 0x8000);

				// Open next file
				fiocart->Fclose();
				next_rom_file_path[len - 1] = _T('2');
				if(!fiocart->Fopen(next_rom_file_path, FILEIO_READ_BINARY))
					goto lbl_rom_error;	// Fatal error
				// Can read 128Ko max only
				file_size = fiocart->FileLength();
				if(file_size > sizeof(raw))
					goto lbl_rom_error;	// Fatal error

				// Read raw file
				memset(raw, 0, sizeof(raw));
				// First chance
				read_size = fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				memcpy(raw_md5, get_md5(raw, file_size), sizeof(raw_md5)-1);

				if(strcmp(raw_md5, _T("b1c2355e6c5f5eb48ee0d446223149e1")) != 0)	// File 2 of 32KB
					goto lbl_rom_error;	// Fatal error
				
				memcpy(cart_data+0x8000, raw, 0x8000);

				// Open next file
				fiocart->Fclose();
				next_rom_file_path[len - 1] = _T('3');
				if(!fiocart->Fopen(next_rom_file_path, FILEIO_READ_BINARY))
					goto lbl_rom_error;	// Fatal error
				// Can read 128Ko max only
				file_size = fiocart->FileLength();
				if(file_size > sizeof(raw))
					goto lbl_rom_error;	// Fatal error

				// Read raw file
				memset(raw, 0, sizeof(raw));
				// First chance
				read_size = fiocart->Fread(&raw, sizeof(raw), 1);

				// Get MD5
				memset(raw_md5, 0, sizeof(raw_md5));
				memcpy(raw_md5, get_md5(raw, file_size), sizeof(raw_md5)-1);

				if(strcmp(raw_md5, _T("7c3cb94c676502b8b149afdbf6fbd3f0")) != 0)	// File 3 of 32KB
					goto lbl_rom_error;	// Fatal error
				
				memcpy(cart_data+0x10000, raw, 0x8000);
			}
			else	// Unknown ROM
			{
				goto lbl_rom_error;	// Fatal error
			}
		}
		else	// Unknown ROM
		{
			goto lbl_rom_error;	// Fatal error
		}
		
		fionewcart->Fclose();

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
		cart_header.crc32		= 0;	// Willbe calculed later

		// Create new .CART file
		cart_in_memory = true;
		if(fionewcart->Fopen(newcart_path, FILEIO_WRITE_BINARY))
		{
			// Save cart header
			fionewcart->Fwrite(&cart_header, sizeof(cart_header), 1);

			// Save cart data
			fionewcart->Fwrite(&cart_data, cart_size, 1);
			
			fionewcart->Fclose();

			cart_in_memory = false;
		}

		memset(rom_file_path, 0, sizeof(rom_file_path));
		len = strlen(newcart_path);
		memcpy(rom_file_path, newcart_path, len);
	}

	// Get save file path
	memset(ram_path, 0, sizeof(ram_path));
	memcpy(ram_path, save_file_path, strlen(save_file_path));
	memcpy(ram_path+strlen(save_file_path), file_name, strlen(file_name));
	ram_path[len - 4] = (rom_file_path[len - 4] == _T('c') ? _T('s') : _T('S'));
	ram_path[len - 3] = (rom_file_path[len - 3] == _T('a') ? _T('a') : _T('A'));
	ram_path[len - 2] = (rom_file_path[len - 2] == _T('r') ? _T('v') : _T('V'));
	ram_path[len - 1] = (rom_file_path[len - 1] == _T('t') ? _T('e') : _T('E'));
	
	// Open .CART file
	if(!fiocart->Fopen(rom_file_path, FILEIO_READ_BINARY) && !cart_in_memory)
		goto lbl_rom_end;	// Fatal error
	
	cart_found = true;

	// Load header
	if(!cart_in_memory)
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
				if(!cart_in_memory)
					fiocart->Fread(&data, size, 1);
				else
				{
					memcpy(data, cart_data+data_index, size);
					data_index += size;
				}

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

	// Save CRC32 if required
	// else control CRC32 if it's set
	cart_crc32 = get_crc32(cart, sizeof(cart));
	if(cart_header.crc32 == 0)
	{
		cart_header.crc32 = cart_crc32;

		// Write cart CRC32
		FILEIO* fiorw = new FILEIO();
		if(fiorw->Fopen(rom_file_path, FILEIO_READ_WRITE_BINARY))
		{
			fiorw->Fseek(sizeof(cart_header)-sizeof(cart_header.crc32), FILEIO_SEEK_SET);
			fiorw->Fwrite(&cart_header.crc32, sizeof(cart_header.crc32), 1);
			fiorw->Fclose();
		}
		delete(fiorw);
	}
	else if(cart_header.crc32 != cart_crc32)
		goto lbl_rom_error;

	cart_ok = true;

	// Try to load SRAM/VRAM save
	if(ram_used)
	{
		memset(sram, 0, ram_size);
		if(ram_save)
		{
			FILEIO* fioram = new FILEIO();

			// Init RAM
			if(ram_size == 0)
				goto lbl_ram_delete;	// Fatal error
			memset(data, 0, ram_size);
			if(ram_size > 0)
				memset(data+ram_size, 0xFF, sizeof(data)-ram_size);

			// load saved sram
			if(!fioram->Fopen(ram_path, FILEIO_READ_BINARY))
				goto lbl_ram_error;	// Fatal error
			
			// Load header
			fioram->Fread(&ram_header, sizeof(ram_header), 1);
			
			// Check format identifier
			if(ram_header.id[0] != 'E' || ram_header.id[1] != 'm' || ram_header.id[2] != 'u' || ram_header.id[3] != 'S' || ram_header.id[4] != 'C' || ram_header.id[5] != 'V' || (uint8_t)ram_header.id[6] != 0x19 || (uint8_t)ram_header.id[7] != 0x84 || (uint8_t)ram_header.id[8] != 0x07 || (uint8_t)ram_header.id[9] != 0x17)
				goto lbl_ram_delete;	// Fatal error
			
			// Check file type
			if(ram_header.type[0] != 'S' || ram_header.type[1] != 'A' || ram_header.type[2] != 'V' || ram_header.type[3] != 'E')
				goto lbl_ram_delete;	// Fatal error

			// Check format version
			if(ram_header.version != 1)
				goto lbl_ram_delete;	// Fatal error

			// Check number of blocks
			// Only one block for SRAM save
			if(ram_header.nb_block > 1)
				goto lbl_ram_delete;	// Fatal error

			// Block 1 is needed
			if(ram_header.block[0] == BLOCK_VOID)
				goto lbl_ram_delete;	// Fatal error

			// Init cart CRC32 if necessary
			if(ram_header.cart_crc32 == 0)
			{
				ram_header.cart_crc32 = cart_header.crc32;

				// Write cart CRC32
				FILEIO* fiorw = new FILEIO();
				if(fiorw->Fopen(ram_path, FILEIO_WRITE_BINARY))
				{
					fiorw->Fseek(sizeof(ram_header)-sizeof(ram_header.save_crc32)-sizeof(ram_header.cart_crc32), FILEIO_SEEK_SET);
					fiorw->Fwrite(&ram_header.cart_crc32, sizeof(ram_header.cart_crc32), 1);
					fiorw->Fclose();
				}
				delete(fiorw);
			}
			else if(ram_header.cart_crc32 != cart_crc32)
				goto lbl_ram_delete;	// Fatal error

			// Compare SAVE block to CART bock
			if(ram_header.block[0] != ram_block)
				goto lbl_ram_delete;	// Fatal error

			// Init RAM
			memset(data, 0x00, ram_size);
			if(ram_size < sizeof(data))
				memset(data+ram_size, 0xFF, sizeof(data)-ram_size);

			// Read backup
			fioram->Fread(&data, ram_size, 1);

			// Check ram CRC32
			ram_crc32 = get_crc32(data, ram_size);
			// Reinit save file on CRC32 error or save not corresponding to cart
			if(ram_header.save_crc32 == 0)
			{
				ram_header.save_crc32 = ram_crc32;

				// Write ram CRC32
				FILEIO* fiorw = new FILEIO();
				if(fiorw->Fopen(ram_path, FILEIO_WRITE_BINARY))
				{
					fiorw->Fseek(sizeof(ram_header)-sizeof(ram_header.save_crc32), FILEIO_SEEK_SET);
					fiorw->Fwrite(&ram_header.save_crc32, sizeof(ram_header.save_crc32), 1);
					fiorw->Fclose();
				}
				delete(fiorw);
			}
			else if(ram_header.save_crc32 != ram_crc32)
				goto lbl_ram_delete;

			// Put data in RAM
			memcpy(sram, data, ram_size);

			goto lbl_ram_close;

lbl_ram_delete:
			fioram->Fclose();
			fioram->RemoveFile(ram_path);

lbl_ram_error:
			memset(data, 0x00, ram_size);
			if(ram_size > 0)
				memset(data+ram_size, 0xFF, sizeof(data)-ram_size);

			memset(&ram_header, 0, sizeof(ram_header));
			ram_header.id[0]		= 'E';
			ram_header.id[1]		= 'm';
			ram_header.id[2]		= 'u';
			ram_header.id[3]		= 'S';
			ram_header.id[4]		= 'C';
			ram_header.id[5]		= 'V';
			ram_header.id[6]		= 0x19;
			ram_header.id[7]		= 0x84;
			ram_header.id[8]		= 0x07;
			ram_header.id[9]		= 0x17;
			ram_header.type[0]		= 'S';
			ram_header.type[1]		= 'A';
			ram_header.type[2]		= 'V';
			ram_header.type[3]		= 'E';
			ram_header.version		= 1;
			ram_header.nb_block		= 1;
			ram_header.block[0]		= ram_block;
			ram_header.cart_crc32	= cart_header.crc32;
			ram_header.save_crc32	= get_crc32(data, ram_size);

			goto lbl_rom_end;

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

	// Delete files extracted from a zip archive
	if(zipfile_number > 0)
		for(int8_t i = zipfile_number; i-- >= 0; )
			fiocart->RemoveFile(zipfile_file_path[i]);

	delete fiocart;
}

void MEMORY::save_cart()
{
	if(cart_inserted && ram_used && ram_save)
	{
		ram_crc32 = get_crc32(sram, ram_size);
		ram_header.save_crc32	= ram_crc32;
		FILEIO* fiorw = new FILEIO();
		if(fiorw->Fopen(ram_path, FILEIO_WRITE_BINARY))
		{
			fiorw->Fwrite(&ram_header, sizeof(ram_header), 1);
			fiorw->Fwrite(&sram, ram_size, 1);
			fiorw->Fclose();
		}
		delete fiorw;
	}
}

void MEMORY::close_cart()
{
	// save backuped sram
	save_cart();

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

#define MEMORY_STATE_ID	601

void MEMORY::save_state(STATE* state, bool max_size)
{
	uint16_t len;
	state->SetValue((uint16_t)MEMORY_STATE_ID);
	state->SetValue(this_device_id);
	state->SetArray(cart_header.id, sizeof(cart_header.id), 1);
	state->SetArray(cart_header.type, sizeof(cart_header.type), 1);
	state->SetValue(cart_header.version);
	state->SetValue(cart_header.nb_block);
	state->SetArray(cart_header.block, sizeof(cart_header.block), 1);
	state->SetValue(cart_header.crc32);
	state->SetArray(ram_header.id, sizeof(ram_header.id), 1);
	state->SetArray(ram_header.type, sizeof(ram_header.type), 1);
	state->SetValue(ram_header.version);
	state->SetValue(ram_header.nb_block);
	state->SetArray(ram_header.block, sizeof(ram_header.block), 1);
	state->SetArray(ram_header.reserved, sizeof(ram_header.reserved), 1);
	state->SetValue(ram_header.cart_crc32);
	state->SetValue(ram_header.save_crc32);
	state->SetValue(bios_found);
	state->SetValue(bios_present);
	state->SetValue(bios_ok);
	state->SetValue(cart_found);
	state->SetValue(cart_inserted);
	state->SetValue(cart_ok);
	if(max_size)
		len = sizeof(ram_path);
	else
		len = strlen(ram_path)+1;
	state->SetValue(len);
	state->SetArray(ram_path, len, 1);
	state->SetValue(ram_block);
	state->SetValue(ram_used);
	state->SetValue(ram_save);
	state->SetValue(ram_bank0);
	state->SetValue(ram_bank1);
	state->SetValue(ram_bank2);
	state->SetValue(ram_bank3);
	state->SetValue(ram_offset);
	state->SetValue(ram_size);
	state->SetValue(ram_crc32);
	state->SetArray(vram, sizeof(vram), 1);
	state->SetArray(wreg, sizeof(wreg), 1);
	state->SetArray(sram, sizeof(sram), 1);
	state->SetValue(cur_bank);
}

bool MEMORY::load_state(STATE* state)
{
	uint16_t len;
	
	if(!state->CheckValue((uint16_t)MEMORY_STATE_ID))
		return false;
	if(!state->CheckValue(this_device_id))
		return false;
	state->GetArray(cart_header.id, sizeof(cart_header.id), 1);
	state->GetArray(cart_header.type, sizeof(cart_header.type), 1);
	state->GetValue(cart_header.version);
	state->GetValue(cart_header.nb_block);
	state->GetArray(cart_header.block, sizeof(cart_header.block), 1);
	state->GetValue(cart_header.crc32);
	state->GetArray(ram_header.id, sizeof(ram_header.id), 1);
	state->GetArray(ram_header.type, sizeof(ram_header.type), 1);
	state->GetValue(ram_header.version);
	state->GetValue(ram_header.nb_block);
	state->GetArray(ram_header.block, sizeof(ram_header.block), 1);
	state->GetArray(ram_header.reserved, sizeof(ram_header.reserved), 1);
	state->GetValue(ram_header.cart_crc32);
	state->GetValue(ram_header.save_crc32);
	state->GetValue(bios_found);
	state->GetValue(bios_present);
	state->GetValue(bios_ok);
	state->GetValue(cart_found);
	state->GetValue(cart_inserted);
	state->GetValue(cart_ok);
	state->GetValue(len);
	state->GetArray(ram_path, len, 1);
	state->GetValue(ram_block);
	state->GetValue(ram_used);
	state->GetValue(ram_save);
	state->GetValue(ram_bank0);
	state->GetValue(ram_bank1);
	state->GetValue(ram_bank2);
	state->GetValue(ram_bank3);
	state->GetValue(ram_offset);
	state->GetValue(ram_size);
	state->GetValue(ram_crc32);
	state->GetArray(vram, sizeof(vram), 1);
	state->GetArray(wreg, sizeof(wreg), 1);
	state->GetArray(sram, sizeof(sram), 1);
	state->GetValue(cur_bank);
	set_bank(cur_bank);
	
	return true;
}
