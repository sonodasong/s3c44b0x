#ifndef __DEFINE_H__
#define __DEFINE_H__

#include <s3c44b0x.h>
#include <ucos_ii.h>

typedef unsigned char boolean;
typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned int uint32;
typedef signed int int32;

#define ex(n)		(0x01<<(n))
#define rex(n)		(~ex(n))

#ifndef NULL
#define NULL		0
#endif

#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE		0
#endif

#endif
