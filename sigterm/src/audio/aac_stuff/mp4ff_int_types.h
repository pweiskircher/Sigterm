#ifndef _MP4FF_INT_TYPES_H_
#define _MP4FF_INT_TYPES_H_

#ifdef _WIN32

typedef signed __int8		int8_t;
typedef unsigned __int8		uint8_t;
typedef signed __int16		int16_t;
typedef unsigned __int16	uint16_t;
typedef signed __int32		int32_t;
typedef unsigned __int32	uint32_t;
typedef signed __int64		int64_t;
typedef unsigned __int64	uint64_t;

#else

#include <stdint.h>

#endif


#endif

