The Neo Kobe Emulator Pack 2013-08-13

================================================================================
1. Introduction
================================================================================
This is a collection of emulators for almost every Japanese pasokon (= personal 
computer) architecture developed between 1979 and 1995. The emulators are 
preconfigured with common settings and necessary BIOS/system ROMs wherever 
possible.

This emulator pack, in combination with MESS, gives you access to practically 
the entire history of Japanese computing.

================================================================================
2. Recommended Emulators
================================================================================
For systems with multiple emulators, these are the recommended emulators for 
general use. If you're unsure of where to start, use these.

[Fujitsu FM-7]	XM7_3460
[MSX]			OpenMSX
[NEC PC-88VA]	vaegx4
[NEC PC-6001]	PC6001VW3 (PC6001VW3.exe)
[NEC PC-8001]	j80 (run _j80.choice.bat and choose option 1)
[NEC PC-8801]	M88kai or j80 option 9
[NEC PC-9801]	(1989) PC-9801RS
[Sharp X1]		xmil026tt143_ika5 (Xmillenium.exe)
[Sharp X68000]	XM6 Pro-68k

================================================================================
3. Setup
================================================================================
All emulators are pre-configured as much as possible, but some things need to 
be done manually.

=== Fujitsu FM Towns ===
[Unz]
	You need to specify a CD-ROM drive letter on the CD-ROM1 tab.

=== MSX ===
[OpenMSX]
	You first need to launch the Catapult frontend and run an MSX hardware 
	scan (File->Test MSX Hardware).

=== NEC PC-8001 ===
[j80]
	The j80 emulator requires a working Java path (something like 
	C:\Program Files\Java\jre6\bin\). The easiest thing to do is install 
	the Java JRE (http://java.com/en/download/index.jsp).

	You can also add a "set javabin=" line to the individual .bat files 
	(see the rem lines for samples), or modify your computer's PATH 
	environment variable. More information: 
	http://docs.oracle.com/javase/tutorial/essential/environment/paths.html

=== NEC PC-9801 ===
[T98-Next]
	You need to specify a CD-ROM drive letter. Go to Device tab (3rd from 
	left)and double-click SCSI4CDN.NHW.

=== Panasonic JR-100&200 ===
[JR-100 (Kemusiro)]
	Runs in Java - see the PC-8001 j80 note above.

=== Sharp X68000 ===
[XM6 Type G]
	Unfortunately will not start unless the computer is set to a Japanese 
	locale. I think it is sufficient to set the Control Panel->Region->Format
	setting to Japanese (Japan), no restart required.
	You can then enable English menus by clicking the last menu on the right,
	first option.

	XM6 Type G also requires the following redistributable packages:
	http://www.microsoft.com/en-us/download/details.aspx?id=5638
	http://www.microsoft.com/en-us/download/details.aspx?id=14431
	http://www.microsoft.com/en-us/download/details.aspx?id=26347

================================================================================
4. Emulator List and Notes
================================================================================

=== Fujitsu FM Towns ===
[Unz] http://townsemu.world.coocan.jp/
	Capable of running most FM Towns software without issues. Also emulates 
	the Marty. Does not support installing Windows.

=== Fujitsu FM-7 ===
[XM7] http://retropc.net/ryu/xm7/xm7.shtml

	The FM-7 emulator XM7 is split into four different families: V1.0, V1.1, V2,
	and V3.
	V1.0	emulates FM-7 only. Deprecated, and not included in this pack.
	V1.1	emulates FM-7, FM-77L4, FM-8 (does not emulate AV models)
	V2		emulates FM-7, FM77AV (does not emulate FM-8 or AV40 models)
	V3		emulates FM-7, FM77AV, FM77AV40EX/SX (does not emulate FM-8)

	FDD motor sounds for different FM-7 models are also included in the 
	WavFile directory. Extract the one you want, rename it to fddseek.wav, 
	and copy it into the Win32 folders. 
	Then start the emulator and enable FDD sounds on the Sound tab.
	Note this is purely cosmetic and does not affect emulation.

[XM7dash] http://tomatoma.s54.xrea.com/xm7/index.shtml
	XM7dash is an unofficial build with numerous enhancements, notably 
	support for external graphics filters and FM synth modules.
	After running xm7dash for the first time, open the XM7.ini file in a 
	text editor and find the lines "FilterDll=" and "FMGenDll=".
	Then type the name of the DLL you want to use.

	Included graphics filters: 2xRpi.dll, 2xRpi+.dll, stretch.dll
	Included FM synth modules: fmgen.dll, mamefm.dll, mamefm2.dll, 
	mamefmv.dll, mamefmv2.dll

=== Fujitsu FM-11 ===
[FM11] http://nanno.dip.jp/softlib/man/fm11/
	Some operating system disk images are included.

=== Hitachi BASIC Master ===
[bm2win32] http://www16.ocn.ne.jp/~ver0/pc/
	Emulates the Hitachi BASIC Master Jr.

[bml3mk5] http://s-sasaji.ddo.jp/bml3mk5/
	Emulates the Hitachi BASIC Master Level 3.

=== MSX ===
[OpenMSX] http://openmsx.sourceforge.net/
	32-bit (x86) binaries for OpenMSX are included. Rename openmsx.exe.x86 
	to openmsx.exe, and catapult.exe.x86 to catapult.exe.

[BlueMSX] http://www.bluemsx.com/
	Last updated in 2009.
	
=== NEC PC-88VA ===
[vaeg] http://www.pc88.gr.jp/vaeg/
	88VA Eternal Grafx. Last updated in 2008.

[vaegx] no website, posted on 2ch board
	Unofficial version of 88VA-EG by Rednow, with various fixes.
	
=== NEC PC-6001 ===
[iP6 Plus] http://www.eonet.ne.jp/~windy/pc6001/ip6plus/ip6.html
	Last updated in 2004. Emulates 2-3 titles that PC6001VW3 fails at.

[PC6001V] http://www.papicom.net/p6v/
	Does not emulate the SR models.
	A 32-bit (x86) build of PC6001V is included. Rename pc6001v.exe.x86 to 
	pc6001v.exe, and SDL.dll.x86 to SDL.dll.

[PC6001VW3] http://p6ers.net/bernie/
	My favorite PC-6001 emulator overall. Mostly English menus.

[PC6001VX] http://eighttails.seesaa.net/article/254095573.html
	Primarily a port of PC6001V for Linux. Currently not included in the 
	pack.

=== NEC PC-8001 ===
[j80] http://www.geocities.jp/upd780c1/pc-8001/
	Great PC-8001 emulator, and fast becoming one of the best PC-8801 
	emulators, particularly for older software.
	
	Launch with _j80.choice.bat in the j80 folder, and select a system model.
	Options 1-3 emulate PC-8001 models, options 4-9 emulate PC-8801 models.
	
=== NEC PC-8801 ===
[M88kai] no website, posted on 2ch board
	Unofficial version of M88 by the author of np2fmgen for the PC-9801.
	Excellent emulation of later PC-8801 software, but tape support is not 
	very good. For early software, use j80, X88x, or Quasi88.
	
	Also supports external FM synth modules. Specify with fmgendll= in the 
	.ini. 

[j80] (see above entry under PC-8001)
	Although originally a PC-8001 emulator, j80 also offers superb
	emulation of the PC-8801 too. Launch with _j80.choice.bat and select from
	options 4-9.

[Quasi88] http://www.eonet.ne.jp/~showtime/quasi88/
	ROM files need to be all lowercase or all uppercase.
	
	QUASI88.exe				Japanese SDL menus
	QUASI88(old-snd).exe	Use old sound driver. Runs faster.
	QUASI88mon.exe			Starts emulator with machine monitor functions.
	QUASI88win32.exe		English Win32API drop-down menus. Some functions
							limited.

[X88x] no website, posted on 2ch board
	Greatly improved unofficial version of X88000 by Rednow. 
	
	Supports Kega Fusion RPI graphics filters, but only at x2 scaling (set 
	zoom=16 in the .ini).
	The filter must also support 640x200 resolution and x2 scaling.
	Put .rpi files in the \plugins\ directory and specify a filter with 
	rpi= in the .ini.
	Linear.rpi is enabled by default.
	
	Also supports external FM synth modules. Specify with fmgendll= in the 
	.ini.
	
=== NEC PC-9801 ===
[Anex86] website closed (http://web.archive.org/web/20101231232247/http://motor.geocities.jp/aqua_marine_blue_556/Anex86/homepage2/ans/ax/axdown.htm)
	Once the preeminent PC-9801 emulator, now Anex86 old, no longer maintained,
	with poor sound emulation. The ZSNES of PC-9801 emulation.
	Included because some people still insist on using it, and because it 
	works with AGTH.

[T98-Next] http://www.geocities.jp/t98next/
	Last updated in 2010.
	
[np2fmgen] no website, posted on 2ch board or http://jbbs.livedoor.jp/computer/43774/
	Unofficial, extensive rewrite of Neko Project II with emphasis on sound 
	quality. By far the best PC-9801/9821 emulator.
	
	There are several executables:
	np2.exe/np2nt.exe		emulates PC-9801
	np2sx.exe/np2sxnt.exe	emulates PC-9801 with 386SX CPU core
	np21.exe/np21nt.exe		emulates PC-9821
	
	The non-NT versions are for running the emulator on Win9x. In most cases,
	you should use np2sxnt.exe or np21nt.exe.

	The VM, RS, and As folders contain preconfigured setups based on the 
	latest np2fmgen. Choose one according to the software you want to run.
	For example, use the VM setup for games released before 1989 or so.

	Also supports external FM synth modules. Specify with fmgendll= in the 
	.ini.
	
	The VM, RS, and As setups are configured for FM synth quality, but CDDA 
	and PCM may sound worse as a result (Policenauts, for example).
	To fix this, turn off QFM and set the sample rate to 44kHz under 
	Emulate->Configure.

	MIDI is set to Microsoft GS Wavetable Synth for compatibility, but 
	changing settings is strongly recommended if you have a better MIDI 
	setup.

[np2x] no website, posted on 2ch board
	Another unofficial Neko Project II version by Rednow. The next best 
	choice after np2fmgen.
	
	Supports Kega Fusion RPI graphics filters, but only at x2 scaling.
	The filter must also support 640x200 resolution and x2 scaling.
	Put .rpi files in the \plugins\ directory and specify a filter with 
	rpi_name= in the .ini.
	
	Also supports external FM synth modules. Specify with fmgendll= in the 
	.ini.
	
=== Panasonic JR-100&200 ===
[JR-100] http://www.asamomiji.jp/kemusiro/index.php
	There are two versions of Kemusiro's JR-100 emulator:
	jr100	simulated system rom
	jr100v2	uses real system rom (JR100ROM.PRG)

[VPanaJR] website closed (www.geocities.com/emucompboy)
	Old emulator from 2002.
	
=== Sanyo PHC-25 ===
[phc25] http://www.geocities.jp/sanyo_phc_25/
	Japanese system rom is enabled by default.
	To emulate the Western model, backup the Japanese rom and rename 
	phc25rom.bin.EN to phc25rom.bin.

=== Sharp MZ-80B&2000 ===
[EmuZ2000TF] http://www7a.biglobe.ne.jp/~tsuneoka/mz_download.html

=== Sharp MZ-80K&700&1500 ===
[MZ700WIN] http://www.retropc.net/mz-memories/mz700/

=== Sharp X1 ===
The world of Sharp X1 emulators is confusing, and the download pages are mostly 
gone. Some info still available here: http://www.x1center.org/emu.html

In general, use xmil026tt143_ika5 or Xmil106R. Toshiya Takeda's X1 emulation is 
also progressing.

[WinX1]
	An old emulator from 2000. WinX1_ika is an unofficial version that adds 
	save states. Japanese menus.

[Xmil026e] http://www.turboz.to/
	X millenium 0.26e, last updated in 2002. Windows binary used to be 
	available on the bulletin board, but that's closed now.

[xmil026tt143_ika5]
	"X millennium v0.26 T-tune STEP 1.43 + ikaTune release 5"
	A hack of a hack of X millenium, released in 2009.
	Xmil-STC2.exe is a custom build of T-Tune specifically for Star 
	Cruiser that enables a 256-color hack.

[Xmil106R]
	Another version based on T-tune 1.43, also released in 2009. Corrects a 
	few timing issues, and adds some minor features.
	XmilMK2.exe is a previous version from 2007.

=== Sharp X68000 ===
[winx68k_v095] http://www.geocities.jp/winx68khighspeed/
	An older emulator last updated in 2004. Kousoku has Japanese menus, 
	HighSpeed has English menus.
	
[XM6 Pro-68k] http://mijet.eludevisibility.org/XM6%20Pro-68k/XM6%20Pro-68k.html
	An updated version of XM6 with a rich set of hacking/debugging 
	functions.
	This is the best X68000 emulator available today.

[XM6 Type G] http://www.geocities.jp/kugimoto0715/xm6g_win32.html
	Another fork of XM6 with many improvements.
	
	There are two versions of XM6 Type G, with different CPU emulation 
	cores.
	The regular version uses the StarScream core, which is fast, but does 
	not support x68030 mode.
	The MAME version uses the MAME 680x0 core, which is slower, but more 
	accurate.

=== Sony SMC-777 ===
[WinSMC017] http://www.smc777.com/winsmc.htm

=== Toshiya Takeda ===
http://homepage3.nifty.com/takeda-toshiya/

Mr. Takeda is a one-man Japanese MESS. His common source code project emulates 
a huge number of rare systems.

Most, but not all, systems have the necessary BIOS roms.

[Toshiya Takeda - m5]
	It seems Takeda's Emu5 emulator have problems to load Tape images in 
	.cas format. The emulator will not detect the header. So I suggest to 
	convert the .cas into .wav format.

	Use the MESS tool called "castool" and type following (command line): 
	castool convert sordm5 mytapefile.cas mytapefile.wav

	Depending on the tape type enter following commands to load/run a 
	program: chain (press Enter) tape (press Enter)

	Example for the Zac Banic game (not the Cartridge version) : Mount the 
	Basic-I cartridge Type: tape and press Enter Mount the Zac Banic.wav 
	tape file

[Toshiya Takeda - pyuta]
	Three ipl roms are included - one for the Japanese Pyuta, one for the 
	Pyuta Jr. console, and one for the American Tutor. Japanese text will 
	not display correctly with the Tutor ipl.

================================================================================
5. Thanks
================================================================================
Sincere thanks to all Neo Kobe members, Tokugawa regulars, 2ch trolls m9(^_^), 
the emulator authors, and all the anonymous dumpers and uploaders who keep the 
scene alive.