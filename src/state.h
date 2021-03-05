/*
	Emulator state

	Created for Libretro-EmuSCV
	Author : Maxime MARCONATO
	Date   : 2021.02.23 -

	[ Emulatior state ]
*/

#ifndef _EMUSCV_INC_STATE_H_
#define _EMUSCV_INC_STATE_H_

#include "common.h"

#define STATE_MAX_DATA_SIZE  128*1024	// 128KB

class STATE
{
public:
	STATE();
	~STATE();

	uint8_t* data;

	void Init();
	void SetReadyToLoad(bool is_ready);
	bool IsReadyToLoad();

	size_t GetDataSize();
	void* GetDataPtr();

	bool GetBool();
	void SetBool(bool val);
	uint8_t GetUint8();
	void SetUint8(uint8_t val);
	uint16_t GetUint16();
	void SetUint16(uint16_t val);
	uint32_t GetUint32();
	void SetUint32(uint32_t val);
	uint64_t GetUint64();
	void SetUint64(uint64_t val);
	int8_t GetInt8();
	void SetInt8(int8_t val);
	int16_t GetInt16();
	void SetInt16(int16_t val);
	int32_t GetInt32();
	void SetInt32(int32_t val);
	int64_t GetInt64();
	void SetInt64(int64_t val);
	float GetFloat();
	void SetFloat(float val);
	double GetDouble();
	void SetDouble(double val);

	uint16_t GetUint16_LE();
	void SetUint16_LE(uint16_t val);
	uint32_t GetUint32_LE();
	void SetUint32_LE(uint32_t val);
	uint64_t GetUint64_LE();
	void SetUint64_LE(uint64_t val);
	int16_t GetInt16_LE();
	void SetInt16_LE(int16_t val);
	int32_t GetInt32_LE();
	void SetInt32_LE(int32_t val);
	int64_t GetInt64_LE();
	void SetInt64_LE(int64_t val);
	float GetFloat_LE();
	void SetFloat_LE(float val);
	double GetDouble_LE();
	void SetDouble_LE(double val);
	_TCHAR GetTCHAR_LE();
	void SetTCHAR_LE(_TCHAR val);

	void SetValue(bool val);
	void SetValue(uint8_t val);
	void SetValue(uint16_t val);
	void SetValue(uint32_t val);
	void SetValue(uint64_t val);
	void SetValue(int8_t val);
	void SetValue(int16_t val);
	void SetValue(int32_t val);
	void SetValue(int64_t val);
	void SetValue(pair16_t val);
	void SetValue(pair32_t val);
	void SetValue(pair64_t val);
	void SetValue(float val);
	void SetValue(double val);
	void SetValue(_TCHAR val);
	void SetArray(bool *buffer, size_t size, size_t count);
	void SetArray(uint8_t *buffer, size_t size, size_t count);
	void SetArray(uint16_t *buffer, size_t size, size_t count);
	void SetArray(uint32_t *buffer, size_t size, size_t count);
	void SetArray(uint64_t *buffer, size_t size, size_t count);
	void SetArray(int8_t *buffer, size_t size, size_t count);
	void SetArray(int16_t *buffer, size_t size, size_t count);
	void SetArray(int32_t *buffer, size_t size, size_t count);
	void SetArray(int64_t *buffer, size_t size, size_t count);
	void SetArray(pair16_t *buffer, size_t size, size_t count);
	void SetArray(pair32_t *buffer, size_t size, size_t count);
	void SetArray(pair64_t *buffer, size_t size, size_t count);
	void SetArray(float *buffer, size_t size, size_t count);
	void SetArray(double *buffer, size_t size, size_t count);
	void SetArray(_TCHAR *buffer, size_t size, size_t count);

	void GetValue(bool &val);
	void GetValue(uint8_t &val);
	void GetValue(uint16_t &val);
	void GetValue(uint32_t &val);
	void GetValue(uint64_t &val);
	void GetValue(int8_t &val);
	void GetValue(int16_t &val);
	void GetValue(int32_t &val);
	void GetValue(int64_t &val);
	void GetValue(pair16_t &val);
	void GetValue(pair32_t &val);
	void GetValue(pair64_t &val);
	void GetValue(float &val);
	void GetValue(double &val);
	void GetValue(_TCHAR &val);	
	void GetArray(bool *buffer, size_t size, size_t count);
	void GetArray(uint8_t *buffer, size_t size, size_t count);
	void GetArray(uint16_t *buffer, size_t size, size_t count);
	void GetArray(uint32_t *buffer, size_t size, size_t count);
	void GetArray(uint64_t *buffer, size_t size, size_t count);
	void GetArray(int8_t *buffer, size_t size, size_t count);
	void GetArray(int16_t *buffer, size_t size, size_t count);
	void GetArray(int32_t *buffer, size_t size, size_t count);
	void GetArray(int64_t *buffer, size_t size, size_t count);
	void GetArray(pair16_t *buffer, size_t size, size_t count);
	void GetArray(pair32_t *buffer, size_t size, size_t count);
	void GetArray(pair64_t *buffer, size_t size, size_t count);
	void GetArray(float *buffer, size_t size, size_t count);
	void GetArray(double *buffer, size_t size, size_t count);
	void GetArray(_TCHAR *buffer, size_t size, size_t count);
	
	bool CheckValue(bool val);
	bool CheckValue(uint8_t val);
	bool CheckValue(uint16_t val);
	bool CheckValue(uint32_t val);
	bool CheckValue(uint64_t val);
	bool CheckValue(int8_t val);
	bool CheckValue(int16_t val);
	bool CheckValue(int32_t val);
	bool CheckValue(int64_t val);
	bool CheckArray(const _TCHAR *buffer, size_t size, size_t count);


private:
	bool ready_to_load;
	uint32_t version;
	uint64_t data_current_size;
	uint8_t *data_current_ptr;
};

#endif	// _EMUSCV_INC_STATE_H_
