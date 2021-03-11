/*
	Libretro-EmuSCV
	
	Author : MARCONATO Maxime (aka MaaaX)
	Date   : 2019-12-05 - 

	[Libretro mapping]
*/

// EmuSCV includes
#include "emuscv.h"

// Static EmuSCV instance
static cEmuSCV oEmuSCV;

//
// Set the libretro environment
//
void retro_set_environment(retro_environment_t cb)
{
	oEmuSCV.RetroSetEnvironment(cb);
}

// 
// Set the Libretro video callback
// 
void retro_set_video_refresh(retro_video_refresh_t cb)
{
	oEmuSCV.RetroSetVideoRefresh(cb);
}

// 
// Set the Libretro audio sample callback
// 
void retro_set_audio_sample(retro_audio_sample_t cb)
{
	oEmuSCV.RetroSetAudioSample(cb);
}

// 
// Set the Libretro audio batch callback
// 
void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
	oEmuSCV.RetroSetAudioSampleBatch(cb);
}

// 
// Set the Libretro input poll callback
// 
void retro_set_input_poll(retro_input_poll_t cb)
{
	oEmuSCV.RetroSetInputPoll(cb);
}

// 
// Set the Libretro input state callback
// 
void retro_set_input_state(retro_input_state_t cb)
{
	oEmuSCV.RetroSetInputState(cb);
}

//
// Return the Libretro version used by the core for compatibility check with the frontend
//
unsigned retro_api_version(void)
{
	return oEmuSCV.RetroGetApiVersion();
}

//
// Return the video standard used
//
unsigned retro_get_region(void)
{
	return oEmuSCV.RetroGetVideoRegion();
}

// 
// Get the system infos
// 
void retro_get_system_info(struct retro_system_info *info)
{
	oEmuSCV.RetroGetSystemInfo(info);
}

// 
// Get the audio/video infos
// 
void retro_get_system_av_info(struct retro_system_av_info *info)
{
	oEmuSCV.RetroGetAudioVideoInfo(info);
}

// 
// Libretro audio callback
// 
void retro_audio_cb(void)
{
	// Log
	oEmuSCV.RetroLogPrintf(RETRO_LOG_INFO, "[%s] retro_audio_cb() => UNUSED\n", EMUSCV_NAME);
/*
	oEmuSCV.RetroAudioCb();
*/
}

// 
// Libretro audio set state enable/disable callback
// 
void retro_audio_set_state_cb(bool enable)
{
	oEmuSCV.RetroAudioSetStateCb(enable);
}

// 
// Libretro frame time callback
// 
void retro_frame_time_cb(retro_usec_t usec)
{
/*
	// Log
	oEmuSCV.RetroLogPrintf(RETRO_LOG_INFO, "[%s] retro_frame_time_cb() => UNUSED\n", EMUSCV_NAME);
*/
	oEmuSCV.RetroFrameTimeCb(usec);
}

//
// Initialize the Libretro core
//
void retro_init(void)
{
	oEmuSCV.RetroInit(retro_audio_cb, retro_audio_set_state_cb, retro_frame_time_cb);
}

//
// Deinitialize the Libretro core
//
void retro_deinit(void)
{
	oEmuSCV.RetroDeinit();
}

// 
// Set a controller port device
// 
void retro_set_controller_port_device(unsigned port, unsigned device)
{
	oEmuSCV.RetroSetControllerPortDevice(port, device);
}

// 
// Load game (or load without game)
// 
bool retro_load_game(const struct retro_game_info *info)
{
	return oEmuSCV.RetroLoadGame(info);
}

// 
// Unload game
// 
void retro_unload_game(void)
{
	oEmuSCV.RetroUnloadGame();
}

// 
// Unused
// Load a "special" kind of game
// 
bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
	// Log
	oEmuSCV.RetroLogPrintf(RETRO_LOG_INFO, "[%s] retro_load_game_special() => UNUSED\n", EMUSCV_NAME);

	return false;
}

// 
// Run for only one frame
// 
void retro_run(void)
{
	oEmuSCV.RetroRun();
}

// 
// Libretro reset
// 
void retro_reset(void)
{
	oEmuSCV.RetroReset(true);
}

// 
// Get save state size
// 
size_t retro_serialize_size(void)
{
	return oEmuSCV.RetroSaveStateSize();
}

// 
// Serialize save state data
// 
bool retro_serialize(void *data, size_t size)
{
	return oEmuSCV.RetroSaveState((void *)data, size);
}

// 
// Unserialize save state data
// 
bool retro_unserialize(const void *data, size_t size)
{
	return oEmuSCV.RetroLoadState((void *)data, size);
}

// 
// Get save state data pointer
// 
void *retro_get_memory_data(unsigned id)
{
	if (id != RETRO_MEMORY_SAVE_RAM)
		return NULL;

	return oEmuSCV.RetroSaveStateData();
}

// 
// Get save state size
// 
size_t retro_get_memory_size(unsigned id)
{
	if (id != RETRO_MEMORY_SAVE_RAM)
		return 0;

	return oEmuSCV.RetroSaveStateSize();
}

// 
// Unused
// 
void retro_cheat_reset(void)
{
	// Log
	oEmuSCV.RetroLogPrintf(RETRO_LOG_INFO, "[%s] retro_cheat_reset() => UNUSED\n", EMUSCV_NAME);
}

// 
// Unused
// 
void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
	// Log
	oEmuSCV.RetroLogPrintf(RETRO_LOG_INFO, "[%s] retro_cheat_set() => UNUSED\n", EMUSCV_NAME);
}
