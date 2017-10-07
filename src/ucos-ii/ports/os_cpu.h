/*
*********************************************************************************************************
*                                               uC/OS-II
*                                        The Real-Time Kernel
*
*                        (c) Copyright 1992-1998, Jean J. Labrosse, Plantation, FL
*                                          All Rights Reserved
*                        (c) Copyright ARM Limited 1999.  All rights reserved.
*
*                                          ARM Specific code
*
* File : OS_CPU.H
*********************************************************************************************************
*/

#ifndef __OS_CPU_H__
#define __OS_CPU_H__

/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
*/

typedef unsigned char		BOOLEAN;
typedef unsigned char		INT8U;
typedef signed char			INT8S;
typedef unsigned short		INT16U;
typedef signed short		INT16S;
typedef unsigned int		INT32U;
typedef signed int			INT32S;
typedef float				FP32;
typedef double				FP64;

typedef unsigned int		OS_STK;
typedef unsigned int		OS_CPU_SR;

/* 
*********************************************************************************************************
*                              ARM, various architectures
*
*********************************************************************************************************
*/

extern void OSStartHighRdy(void);
extern void OSCtxSw(void);
extern void OSIntCtxSw(void);
extern unsigned int ARMDisableInt(void);
extern void ARMEnableInt(unsigned int);

extern void OSTickISR(void);

#define OS_CRITICAL_METHOD	3

#if OS_CRITICAL_METHOD == 3
#define	OS_ENTER_CRITICAL()	cpu_sr = ARMDisableInt()
#define	OS_EXIT_CRITICAL()	ARMEnableInt(cpu_sr)
#endif

#define OS_TASK_SW()		OSCtxSw()

#define OS_STK_GROWTH		1

#endif