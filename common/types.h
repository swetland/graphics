#ifndef _TYPES_H_
#define _TYPES_H_

/* basic datatypes */
typedef unsigned int u32;
typedef int s32;
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;
typedef signed char s8;

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&); \
	void operator=(const TypeName&)

#define RGBA(r,g,b,a) ((r&0xFF)|((g&0xFF)<<8)|((b&0xFF)<<16)|((a&0xFF)<<24))

#endif

