/*
	Emulator state

	Created for Libretro-EmuSCV
	Author : Maxime MARCONATO
	Date   : 2021.02.23 -

	[ Emulatior state ]
*/

#include "state.h"

#define STATE_VERSION 1

// Constructor
STATE::STATE()
{
	data = (uint8_t*)malloc(STATE_MAX_DATA_SIZE);
	memset(data, 0, STATE_MAX_DATA_SIZE);
	Init();
}

// Destructor
STATE::~STATE()
{
	if(data)
		free(data);
}

// Init
void STATE::Init()
{
	ready_to_load = false;
	data_current_ptr = data;
	data_current_size = 0;
}

void STATE::SetReadyToLoad(bool is_ready)
{
	if(is_ready)
		data_current_ptr = data;
	ready_to_load = is_ready;
}

bool STATE::IsReadyToLoad()
{
	return ready_to_load;
}

// Return the size of data
size_t STATE::GetDataSize()
{
	return data_current_size;
}

// Return data pointer to data
void* STATE::GetDataPtr()
{
	return (void*)data;
}

#define STATE_GET_VALUE(type) \
	uint8_t buffer[sizeof(type)] = {0}; \
	size_t size = sizeof(buffer); \
	memcpy(buffer, data_current_ptr, size); \
	data_current_ptr += size; \
	return *(type *)buffer

#define STATE_SET_VALUE(type, v) \
	size_t size = sizeof(type); \
	memcpy(data_current_ptr, &v, size); \
	data_current_ptr += size; \
	data_current_size += size

bool STATE::GetBool()
{
	STATE_GET_VALUE(bool);
}

void STATE::SetBool(bool val)
{
	STATE_SET_VALUE(bool, val);
}

uint8_t STATE::GetUint8()
{
	STATE_GET_VALUE(uint8_t);
}

void STATE::SetUint8(uint8_t val)
{
	STATE_SET_VALUE(uint8_t, val);
}

uint16_t STATE::GetUint16()
{
	STATE_GET_VALUE(uint16_t);
}

void STATE::SetUint16(uint16_t val)
{
	STATE_SET_VALUE(uint16_t, val);
}

uint32_t STATE::GetUint32()
{
	STATE_GET_VALUE(uint32_t);
}

void STATE::SetUint32(uint32_t val)
{
	STATE_SET_VALUE(uint32_t, val);
}

uint64_t STATE::GetUint64()
{
	STATE_GET_VALUE(uint64_t);
}

void STATE::SetUint64(uint64_t val)
{
	STATE_SET_VALUE(uint64_t, val);
}

int8_t STATE::GetInt8()
{
	STATE_GET_VALUE(int8_t);
}

void STATE::SetInt8(int8_t val)
{
	STATE_SET_VALUE(int8_t, val);
}

int16_t STATE::GetInt16()
{
	STATE_GET_VALUE(int16_t);
}

void STATE::SetInt16(int16_t val)
{
	STATE_SET_VALUE(int16_t, val);
}

int32_t STATE::GetInt32()
{
	STATE_GET_VALUE(int32_t);
}

void STATE::SetInt32(int32_t val)
{
	STATE_SET_VALUE(int32_t, val);
}

int64_t STATE::GetInt64()
{
	STATE_GET_VALUE(int64_t);
}

void STATE::SetInt64(int64_t val)
{
	STATE_SET_VALUE(int64_t, val);
}

float STATE::GetFloat()
{
	STATE_GET_VALUE(float);
}

void STATE::SetFloat(float val)
{
	STATE_SET_VALUE(float, val);
}

double STATE::GetDouble()
{
	STATE_GET_VALUE(double);
}

void STATE::SetDouble(double val)
{
	STATE_SET_VALUE(double, val);
}

uint16_t STATE::GetUint16_LE()
{
	pair16_t tmp;
	tmp.b.l = GetUint8();
	tmp.b.h = GetUint8();
	return tmp.w;
}

void STATE::SetUint16_LE(uint16_t val)
{
	pair16_t tmp;
	tmp.w = val;
	SetUint8(tmp.b.l);
	SetUint8(tmp.b.h);
}

uint32_t STATE::GetUint32_LE()
{
	pair32_t tmp;
	tmp.b.l  = GetUint8();
	tmp.b.h  = GetUint8();
	tmp.b.h2 = GetUint8();
	tmp.b.h3 = GetUint8();
	return tmp.d;
}

void STATE::SetUint32_LE(uint32_t val)
{
	pair32_t tmp;
	tmp.d = val;
	SetUint8(tmp.b.l);
	SetUint8(tmp.b.h);
	SetUint8(tmp.b.h2);
	SetUint8(tmp.b.h3);
}

uint64_t STATE::GetUint64_LE()
{
	pair64_t tmp;
	tmp.b.l  = GetUint8();
	tmp.b.h  = GetUint8();
	tmp.b.h2 = GetUint8();
	tmp.b.h3 = GetUint8();
	tmp.b.h4 = GetUint8();
	tmp.b.h5 = GetUint8();
	tmp.b.h6 = GetUint8();
	tmp.b.h7 = GetUint8();
	return tmp.q;
}

void STATE::SetUint64_LE(uint64_t val)
{
	pair64_t tmp;
	tmp.q = val;
	SetUint8(tmp.b.l);
	SetUint8(tmp.b.h);
	SetUint8(tmp.b.h2);
	SetUint8(tmp.b.h3);
	SetUint8(tmp.b.h4);
	SetUint8(tmp.b.h5);
	SetUint8(tmp.b.h6);
	SetUint8(tmp.b.h7);
}

int16_t STATE::GetInt16_LE()
{
	pair16_t tmp;
	tmp.b.l = GetUint8();
	tmp.b.h = GetUint8();
	return tmp.sw;
}

void STATE::SetInt16_LE(int16_t val)
{
	pair16_t tmp;
	tmp.sw = val;
	SetUint8(tmp.b.l);
	SetUint8(tmp.b.h);
}

int32_t STATE::GetInt32_LE()
{
	pair32_t tmp;
	tmp.b.l  = GetUint8();
	tmp.b.h  = GetUint8();
	tmp.b.h2 = GetUint8();
	tmp.b.h3 = GetUint8();
	return tmp.sd;
}

void STATE::SetInt32_LE(int32_t val)
{
	pair32_t tmp;
	tmp.sd = val;
	SetUint8(tmp.b.l);
	SetUint8(tmp.b.h);
	SetUint8(tmp.b.h2);
	SetUint8(tmp.b.h3);
}

int64_t STATE::GetInt64_LE()
{
	pair64_t tmp;
	tmp.b.l  = GetUint8();
	tmp.b.h  = GetUint8();
	tmp.b.h2 = GetUint8();
	tmp.b.h3 = GetUint8();
	tmp.b.h4 = GetUint8();
	tmp.b.h5 = GetUint8();
	tmp.b.h6 = GetUint8();
	tmp.b.h7 = GetUint8();
	return tmp.sq;
}

void STATE::SetInt64_LE(int64_t val)
{
	pair64_t tmp;
	tmp.sq = val;
	SetUint8(tmp.b.l);
	SetUint8(tmp.b.h);
	SetUint8(tmp.b.h2);
	SetUint8(tmp.b.h3);
	SetUint8(tmp.b.h4);
	SetUint8(tmp.b.h5);
	SetUint8(tmp.b.h6);
	SetUint8(tmp.b.h7);
}

float STATE::GetFloat_LE()
{
	pair32_t tmp;
	tmp.b.l  = GetUint8();
	tmp.b.h  = GetUint8();
	tmp.b.h2 = GetUint8();
	tmp.b.h3 = GetUint8();
	return tmp.f;
}

void STATE::SetFloat_LE(float val)
{
	pair32_t tmp;
	tmp.f = val;
	SetUint8(tmp.b.l);
	SetUint8(tmp.b.h);
	SetUint8(tmp.b.h2);
	SetUint8(tmp.b.h3);
}

double STATE::GetDouble_LE()
{
	pair64_t tmp;
	tmp.b.l  = GetUint8();
	tmp.b.h  = GetUint8();
	tmp.b.h2 = GetUint8();
	tmp.b.h3 = GetUint8();
	tmp.b.h4 = GetUint8();
	tmp.b.h5 = GetUint8();
	tmp.b.h6 = GetUint8();
	tmp.b.h7 = GetUint8();
	return tmp.df;
}

void STATE::SetDouble_LE(double val)
{
	pair64_t tmp;
	tmp.df = val;
	SetUint8(tmp.b.l);
	SetUint8(tmp.b.h);
	SetUint8(tmp.b.h2);
	SetUint8(tmp.b.h3);
	SetUint8(tmp.b.h4);
	SetUint8(tmp.b.h5);
	SetUint8(tmp.b.h6);
	SetUint8(tmp.b.h7);
}

_TCHAR STATE::GetTCHAR_LE()
{
/*
	switch(sizeof(_TCHAR))
	{
		case 1: return (_TCHAR)GetUint8();
		case 2: return (_TCHAR)GetUint16_LE();
		case 4: return (_TCHAR)GetUint32_LE();
		case 8: return (_TCHAR)GetUint64_LE();
	}
*/
	return (_TCHAR)GetUint8();
}

void STATE::SetTCHAR_LE(_TCHAR val)
{
/*
	switch(sizeof(_TCHAR))
	{
		case 1: SetUint8((uint8_t )val); return;
		case 2: SetUint16_LE((uint16_t)val); return;
		case 4: SetUint32_LE((uint32_t)val); return;
		case 8: SetUint32_LE((uint64_t)val); return;
	}
*/
	SetUint8((uint8_t )val);
}

void STATE::SetValue(bool val)
{
	SetBool(val);
}
void STATE::GetValue(bool &val)
{
	val = GetBool();
}

void STATE::SetValue(uint8_t val)
{
	SetUint8(val);
}
void STATE::GetValue(uint8_t &val)
{
	val = GetUint8();
}

void STATE::SetValue(uint16_t val)
{
	SetUint16_LE(val);
}
void STATE::GetValue(uint16_t &val)
{
	val = GetUint16_LE();
}

void STATE::SetValue(uint32_t val)
{
	SetUint32_LE(val);
}
void STATE::GetValue(uint32_t &val)
{
	val = GetUint32_LE();
}

void STATE::SetValue(uint64_t val)
{
	SetUint64_LE(val);
}
void STATE::GetValue(uint64_t &val)
{
	val = GetUint64_LE();
}

void STATE::SetValue(int8_t val)
{
	SetInt8(val);
}
void STATE::GetValue(int8_t &val)
{
	val = GetInt8();
}

void STATE::SetValue(int16_t val)
{
	SetInt16_LE(val);
}
void STATE::GetValue(int16_t &val)
{
	val = GetInt16_LE();
}

void STATE::SetValue(int32_t val)
{
	SetInt32_LE(val);
}
void STATE::GetValue(int32_t &val)
{
	val = GetInt32_LE();
}

void STATE::SetValue(int64_t val)
{
	SetInt64_LE(val);
}
void STATE::GetValue(int64_t &val)
{
	val = GetInt64_LE();
}

void STATE::SetValue(pair16_t val)
{
	SetUint16_LE(val.w);
}
void STATE::GetValue(pair16_t &val)
{
	val.w = GetUint16_LE();
}

void STATE::SetValue(pair32_t val)
{
	SetUint32_LE(val.d);
}
void STATE::GetValue(pair32_t &val)
{
	val.d = GetUint32_LE();
}

void STATE::SetValue(pair64_t val)
{
	SetUint64_LE(val.q);
}
void STATE::GetValue(pair64_t &val)
{
	val.q = GetUint64_LE();
}

void STATE::SetValue(float val)
{
	SetFloat_LE(val);
}
void STATE::GetValue(float &val)
{
	val = GetFloat_LE();
}

void STATE::SetValue(double val)
{
	SetDouble_LE(val);
}
void STATE::GetValue(double &val)
{
	val = GetDouble_LE();
}

void STATE::SetValue(_TCHAR val)
{
//	SetTCHAR_LE(val);
	SetUint8((uint8_t )val);
}
void STATE::GetValue(_TCHAR &val)
{
	val = GetTCHAR_LE();
}

void STATE::SetArray(bool *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		SetBool(buffer[i]);
}
void STATE::GetArray(bool *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		buffer[i] = GetBool();
}

void STATE::SetArray(uint8_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		SetUint8(buffer[i]);
}
void STATE::GetArray(uint8_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		buffer[i] = GetUint8();
}

void STATE::SetArray(uint16_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		SetUint16(buffer[i]);
}
void STATE::GetArray(uint16_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		buffer[i] = GetUint16();
}

void STATE::SetArray(uint32_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		SetUint32(buffer[i]);
}
void STATE::GetArray(uint32_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		buffer[i] = GetUint32();
}

void STATE::SetArray(uint64_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		SetUint64(buffer[i]);
}
void STATE::GetArray(uint64_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		buffer[i] = GetUint64();
}

void STATE::SetArray(int8_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		SetInt8(buffer[i]);
}
void STATE::GetArray(int8_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		buffer[i] = GetInt8();
}

void STATE::SetArray(int16_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		SetInt16(buffer[i]);
}
void STATE::GetArray(int16_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		buffer[i] = GetInt16();
}

void STATE::SetArray(int32_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		SetInt32(buffer[i]);
}
void STATE::GetArray(int32_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		buffer[i] = GetInt32();
}

void STATE::SetArray(int64_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		SetInt64(buffer[i]);
}
void STATE::GetArray(int64_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		buffer[i] = GetInt64();
}

void STATE::SetArray(pair16_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		SetUint16_LE(buffer[i].w);
}
void STATE::GetArray(pair16_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		buffer[i].w = GetUint16_LE();
}

void STATE::SetArray(pair32_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		SetUint32_LE(buffer[i].d);
}
void STATE::GetArray(pair32_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		buffer[i].d = GetUint32_LE();
}

void STATE::SetArray(pair64_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		SetUint64_LE(buffer[i].q);
}
void STATE::GetArray(pair64_t *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		buffer[i].q = GetUint64_LE();
}

void STATE::SetArray(float *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		SetFloat(buffer[i]);
}
void STATE::GetArray(float *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		buffer[i] = GetFloat();
}

void STATE::SetArray(double *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		SetDouble_LE(buffer[i]);
}
void STATE::GetArray(double *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
	for(int32_t i = -1; ++i < s; )
		buffer[i] = GetDouble_LE();
}

void STATE::SetArray(_TCHAR *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
/*
	switch(sizeof(_TCHAR))
	{
		case 8:
			for(int32_t i = -1; ++i < s; )
				SetUint32_LE((uint64_t)buffer[i]);
			break;
		case 4:
			for(int32_t i = -1; ++i < s; )
				SetUint32_LE((uint32_t)buffer[i]);
			break;
		case 2:
			for(int32_t i = -1; ++i < s; )
				SetUint16_LE((uint16_t)buffer[i]);
			break;
		case 1:
		default:
*/
			for(int32_t i = -1; ++i < s; )
				SetUint8((uint8_t )buffer[i]);
/*
			break;
	}
*/
}
void STATE::GetArray(_TCHAR *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
/*
	switch(sizeof(_TCHAR))
	{
		case 8:
			for(int32_t i = -1; ++i < s; )
				buffer[i] = (_TCHAR)GetUint64_LE();
			break;
		case 4:
			for(int32_t i = -1; ++i < s; )
				buffer[i] = (_TCHAR)GetUint32_LE();
			break;
		case 2:
			for(int32_t i = -1; ++i < s; )
				buffer[i] = (_TCHAR)GetUint16_LE();
			break;
		default:
*/
			for(int32_t i = -1; ++i < s; )
				buffer[i] = (_TCHAR)GetUint8();
/*
			break;
	}
*/
}

bool STATE::CheckValue(bool val)
{
	return (val == GetBool());
}

bool STATE::CheckValue(uint8_t val)
{
	return (val == GetUint8());
}

bool STATE::CheckValue(uint16_t val)
{
	return (val == GetUint16_LE());
}

bool STATE::CheckValue(uint32_t val)
{
	return (val == GetUint32_LE());
}

bool STATE::CheckValue(uint64_t val)
{
	return (val == GetUint64_LE());
}

bool STATE::CheckValue(int8_t val)
{
	return (val == GetInt8());
}

bool STATE::CheckValue(int16_t val)
{
	return (val == GetInt16_LE());
}

bool STATE::CheckValue(int32_t val)
{
	return (val == GetInt32_LE());
}

bool STATE::CheckValue(int64_t val)
{
	return (val == GetInt64_LE());
}

bool STATE::CheckArray(const _TCHAR *buffer, size_t size, size_t count)
{
	int32_t s = size / sizeof(buffer[0]) * count;
/*
	switch(sizeof(_TCHAR))
	{
		case 8:
			for(int32_t i = -1; ++i < s; )
				if(buffer[i] != (_TCHAR)GetUint64_LE())
					return false;
			break;
		case 4:
			for(int32_t i = -1; ++i < s; )
				if(buffer[i] != (_TCHAR)GetUint32_LE())
					return false;
			break;
		case 2:
			for(int32_t i = -1; ++i < s; )
				if(buffer[i] != (_TCHAR)GetUint16_LE())
					return false;
			break;
		default:
*/
			for(int32_t i = -1; ++i < s; )
				if(buffer[i] != (_TCHAR)GetUint8())
					return false;
/*
			break;
	}
*/
	return true;
}
