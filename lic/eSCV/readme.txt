Binary archive of retro pc emulator common source code
								8/1/2013

--- What's this ?

This archive includes the binaries of the emulators listed below:

	Emu5		SORD m5
	EmuGaki		CASIO PV-2000
	EmuLTI8		MITSUBISHI Electric MULTI8
	EmuPIA		TOSHIBA PASOPIA
	EmuPIA7		TOSHIBA PASOPIA7
	EmuZ-80B	SHARP MZ-80B
	EmuZ-80K	SHARP MZ-80K
	EmuZ-700	SHARP MZ-700
	EmuZ-800	SHARP MZ-800
	EmuZ-1200	SHARP MZ-1200
	EmuZ-1500	SHARP MZ-1500
	EmuZ-2200	SHARP MZ-2200
	EmuZ-2500	SHARP MZ-2500
	EmuZ-2800	SHARP MZ-2800
	EmuZ-3500	SHARP MZ-3500
	EmuZ-5500	SHARP MZ-5500
	EmuZ-6500	SHARP MZ-6500
	eBabbage-2nd	Gijutsu Hyoron Sha Babbage-2nd
	eFamilyBASIC	Nintendo Family BASIC
	eFM16pi		FUJITSU FM16pi
	eFMR-30		FUJITSU FMR-30
	eFMR-50		FUJITSU FMR-50
	eFMR-60		FUJITSU FMR-60
	eFP-200		CASIO FP-200
	eFP-1100	CASIO FP-1100
	eHANDY98	NEC PC-98HA
	eHC-20		EPSON HC-20/HX-20
	eHC-40		EPSON HC-40/PX-4
	eHC-80		EPSON HC-80/PX-8/Geneva
	eJX		IBM Japan Ltd PC/JX
	eMAP-1010	SEIKO MAP-1010
	eMYCOMZ-80A	Japan Electronics College MYCOMZ-80A
	ePC-8001mkIISR	NEC PC-8001mkIISR
	ePC-8201	NEC PC-8201/PC-8201A
	ePC-8801MA	NEC PC-8801MA
	ePC-9801	NEC PC-9801
	ePC-9801E	NEC PC-9801E/F/M
	ePC-9801VM	NEC PC-9801VM
	ePC-98DO	NEC PC-98DO
	ePC-98LT	NEC PC-98LT
	ePC-100		NEC PC-100
	ePCEngine	NEC-HE PC Engine / SuperGrafx
	ePHC-25		SANYO PHC-20
	ePHC-25		SANYO PHC-25
	ePV-1000	CASIO PV-1000
	ePyuTa		TOMY PyuTa/PyuTa Jr.
	eQC-10		EPSON QC-10/QX-10
	eRX-78		BANDAI RX-78
	eSC-3000	SEGA SC-3000
	eSCV		EPOCH Super Cassette Vision
	eTK-80BS	NEC TK-80BS (COMPO BS/80)
	eX-07		CANON X-07
	eX1		SHARP X1
	eX1twin		SHARP X1twin
	eX1turbo	SHARP X1turbo
	eYS-6464A	Shinko Sangyo YS-6464A

These binaries are for Windows 2000/XP/Vista/7.
They are tested on Windows XP Professional with SP3.


--- How to use

Common menus:

	Control
		Reset		Reset the virtual machine
		-
		CPU x1		Set CPU clock multipler
		CPU x2
		CPU x4
		CPU x8
		CPU x16
		-
		Paste		Auto key hitting from the clip board text
		Stop		Stop the auto key hitting
		-
		Exit		Terminate the emulator

	Cart
		Insert		Insert the cart image
		Eject		Eject the cart image
		-
		History		Insert the cart image

	FDD*
		Insert		Insert the floppy disk image
		Eject		Eject the floppy disk image
		-
		History		Insert the floppy disk image

	Tape
		Play		Insert the cassette tape image to play
		Rec		Insert the cassette tape image to record
		Eject		Eject the cassette tape image
		-
		History		Insert the cassette tape image to play

	Screen
		Rec 30fps	Record the movie to avi and wav files
		Rec 15fps
		Rec 10fps
		Stop		Stop recording the movie
		Capture		Capture the screen to bitmap file
		-
		Window x1	Set the window size
			:
		Window x8
		Fullscreen ?x?	Set the fullscreen size

	Sound
		Rec		Record the sound to wav file
		Stop		Stop recording the sound
		-
		2000Hz		Set the sound frequency
		4000Hz		You need to restart the emulator
		8000Hz
		11025Hz
		22050Hz
		44100Hz
		48000Hz
		96000Hz
		-
		50msec		Set the sound buffer size (latency)
		100msec		You need to restart the emulator
		200msec
		300msec
		400msec


For Windows PC environment:

	It is recommended to change the screen mode to 16bpp.

	Support Windows PC compatible joystick devices.

	Shortcut key:
		ALT+RETURN  - Switch window / fullscreen
		CTRL+RETURN - Swtich enable / disenable the mouse control

	*) While the mouse control is enabled, the mouse cursor is hidden.

Floppy disk images:

	Supoort D88, TeleDisk, ImageDisk and DSK format disk images.
	If the image file is read-only on windows file system,
	its disk is recognaized that is write-protected on emulator.
	(note: the protect flag in D88 header is ignored.)

Cassette tape images:

	Support wav files. (PCM only)

Video capture devices:

	Support the direct show compatible video capture devices
	to emulate the super impose function.


--- Thanks

- vm/device.h
	XM6
- vm/fmgen/*
	M88/fmgen
- vm/disk.*
	MESS formats/dsk_dsk.c for CPDRead floppy disk image decorder
- vm/hd63484.*
	MAME HD63484 core
- vm/huc6280.*
	MESS huc6280 core
- vm/i86.*
	MAME i86 core
- vm/i286.*
	MAME i286 core
- vm/i386.*
	MAME i386 core
- vm/i8259.*
	Neko Project 2 and MESS 8259 core
- vm/m6502.*
	MAME m6502 core
- vm/mb8877.*
	XM7
- vm/mc6800.*
	MAME mc6800 core
- vm/mc6809.*
	MAME mc6809 core
- vm/mc6847.*
	MAME mc6847 core
- vm/sn76489an.*
	MAME SN76496 core
- vm/tf20.*
	vfloppy 1.4 by Mr.Justin Mitchell and Mr.Fred Jan Kraan
- vm/tms9918a.*
	MAME TMS9928 core
- vm/tms9995.*
	MAME TMS99xx core
- vm/upd71071.*
	88VA Eternal Grafx
- vm/upd7220.*
	Neko Project 2
- vm/upd765a.*
	M88 fdc/fdu core
- vm/upd7801.*
	MAME uPD7810 core
- vm/w3100a.*
	Mr.Oh!Ishi for the chip specification info
- vm/z80.*
	MAME Z80 core
- vm/z80dma.*
	MAME Z80DMA core
- vm/familybasic
	nester
- vm/fmr50/bios.*
	FM-TOWNS emulator on bochs
	UNZ pseudo BIOS
- vm/fp200/*
	PockEmul gives much hints about LCD driver
- vm/hc40/*
	Mr.Fred Han Kraan for EPSON HC-40/PX-4 hardware design info
- vm/hc80/*
	Mr.Fred Han Kraan for EPSON HC-80/PX-8/Geneva hardware design info
- vm/hc80/io.*
	Mr.Dennis Heynlein for intelligent ram disk unit
- vm/m5/*
	MESS sord driver
	Mr.Moriya for Sord M5 hardware design info
- vm/mycomz80a/mon.c
	Based on MON80 by Mr.Tesuya Suzuki
- vm/mz2500/sasi.*
	X millenium
- vm/pc9801/pc88.*
	M88 and MESS PC-8801 driver
- vm/x1/pce.*
	Ootake (joypad)
	xpce (psg)
	MESS (vdc/vce/vpc)
- vm/phc25/*
	PHC-25 emulator by Mr.Tago
- vm/pv1000/*
	Mr.Enri for CASIO PV-1000 hardware design info
- vm/pv2000/*
	Mr.Enri for CASIO PV-2000 hardware design info
- vm/pyuta/*
	MESS tutor driver
	Mr.Enri for TOMY PyuTa Jr. hardware design info
- vm/qc10/*
	Mr.Fred Han Kraan for EPSON QC-10/QX-10 hardware design info
- vm/scv/*
	Mr.Enri and Mr.333 for Epoch Super Cassette Vision hardware info
- vm/x07/io.*
	x07_emul by Mr.Jacques Brigaud
- vm/x1/*
	Many advices by Mr Y.S.
- vm/x1/sub.*
	X millenium T-tune
- vm/x1/display.*
	KANJI ROM support by X1EMU
- win32_sound.cpp
	XM7 for DirectSound implement
	M88 for wavOut API implement
- res/*.ico
	Mr.Temmaru and Mr.Marukun
	See also res/icon.txt

- emulation core design
	nester and XM6

----------------------------------------
TAKEDA, toshiya
t-takeda@m1.interq.or.jp
http://homepage3.nifty.com/takeda-toshiya/
