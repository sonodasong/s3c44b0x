#ifndef __DEFINE_H__
#define __DEFINE_H__

#include <s3c44b0x.h>
#include <ucos_ii.h>

typedef BOOLEAN boolean;
typedef INT8S int8;
typedef INT8U uint8;
typedef INT16S int16;
typedef INT16U uint16;
typedef INT32S int32;
typedef INT32U uint32;

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
