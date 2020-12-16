===========================================================
Libretro-EmuSCV (EPOCH/YENO Super Cassette Vision Emulator)
===========================================================

-----
About
-----
Libretro-EmuSCV emulte the "EPOCH/YENO Super Cassette Vision", a home video
game console made by EPOCH CO. and released in Japan on July 17, 1984 and
relaesed in Europe (France only) later in 1984 under the YENO brand.

This emulator is purposed as a crossplatform core for Libretro compatible
fondends as Retroarch, Recalbox, RetroPie, etc.
It is written by MARCONATO Maxime (aka MaaaX) and based on eSCV, an "EPOCH Super
Cassette Vision Emulator for Win32" written by TAKEDA toshiya.


-------
install
-------

To do....


---------------
For developpers
---------------
Please use Visual Studio Code as editor on Windows, Mac and Linux, 
On Windows, install Mingw64 as described on the Libretro dev page.
On Linux install build-essential and retroarch (not from flatpak).
On Mac please install "xcode-select --install" (debug with lldb on OS X 10.9+). Unable to compile on recent OS X versions.


-------
Authors
-------

Libretro-EmuSCV:
- MARCONATO Maxime (aka MaaaX) | maxime@maaax.com | http://www.maaax.com/

eSCV:
- TAKEDA toshiya | takeda@m1.interq.or.jp | http://takeda-toshiya.my.coocan.jp/





--- Internal ROM image

	BIOS.ROM	$0000-$0FFF

--- Cart ROM images

	Cart ROM image header:

		db id[4]	'SCV^Z' (53h,43h,56h,1ah)
		db ctype	0 = 16KB, 32KB, 32KB + 8KB
				    1 = 32KB + 8KB SRAM
			    	2 = 32KB + 32KB, 32KB + 32KB + 32KB + 32KB
		db reserver[11]

		(note: if no header, it is recognized that ctype must be 0)

