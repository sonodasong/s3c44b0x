/*
*********************************************************************************************************
*                                               uC/OS-II
*                                         The Real-Time Kernel
*
*                        (c) Copyright 1992-1998, Jean J. Labrosse, Plantation, FL
*                                          All Rights Reserved
*                        (c) Copyright ARM Limited 1999.  All rights reserved.
*
*                                          ARM Specific code
*
*
* File : OS_CPU_C.C
*********************************************************************************************************
*/

#include <ucos_ii.h>

#define SYSMODE	0x1F

INT32U LINK_SAVE[2];

#if OS_TMR_EN > 0u
static INT16U OSTmrCtr;

#if OS_TIME_TICK_HOOK_EN == 0u
#error "enable OSTimeTickHook() in order to call OSTmrSignal()"
#endif

#if OS_SCHED_LOCK_EN == 0u
#error "enable schedule lock in order to use timer"
#endif

#endif

/*
*********************************************************************************************************
*                                        INITIALIZE A TASK'S STACK
*
* Description: This function is called by either OSTaskCreate() or OSTaskCreateExt() to initialize the
*              stack frame of the task being created.  This function is highly processor specific.
*
* Arguments  : task          is a pointer to the task code
*
*              pdata         is a pointer to a user supplied data area that will be passed to the task
*                            when the task first executes.
*
*              ptos          is a pointer to the top of stack.  It is assumed that 'ptos' points to
*                            a 'free' entry on the task stack.  If OS_STK_GROWTH is set to 1 then 
*                            'ptos' will contain the HIGHEST valid address of the stack.  Similarly, if
*                            OS_STK_GROWTH is set to 0, the 'ptos' will contains the LOWEST valid address
*                            of the stack.
*
*              opt           specifies options that can be used to alter the behavior of OSTaskStkInit().
*                            (see uCOS_II.H for OS_TASK_OPT_???).
*
* Returns    : Always returns the location of the new top-of-stack' once the processor registers have
*              been placed on the stack in the proper order.
*
* Note(s)    : Interrupts are enabled when your task starts executing. You can change this by setting the
*              PSW to 0x0002 instead.  In this case, interrupts would be disabled upon task startup.  The
*              application code would be responsible for enabling interrupts at the beginning of the task
*              code.  You will need to modify OSTaskIdle() and OSTaskStat() so that they enable 
*              interrupts.  Failure to do this will make your system crash!
*********************************************************************************************************
*/

OS_STK *OSTaskStkInit(void (*task)(void *pd), void *pdata, OS_STK *ptos, INT16U opt)
{
    OS_STK *stk;

    opt    = opt;                       /* 'opt' is not used, prevent warning */
    stk    = ptos;                      /* Load stack pointer                 */
    
    /* build a context for the new task */
    *--stk = (OS_STK) task;             /* pc */
    *--stk = (OS_STK) OS_TaskReturn;    /* lr */

    *--stk = 0;                         /* r12 */
    *--stk = 0;                         /* r11 */
    *--stk = 0;                         /* r10 */
    *--stk = 0;                         /* r9 */
    *--stk = 0;                         /* r8 */
    *--stk = 0;                         /* r7 */
    *--stk = 0;                         /* r6 */
    *--stk = 0;                         /* r5 */
    *--stk = 0;                         /* r4 */
    *--stk = 0;                         /* r3 */
    *--stk = 0;                         /* r2 */
    *--stk = 0;                         /* r1 */
    *--stk = (OS_STK) pdata;            /* r0 */
    *--stk = (OS_STK)SYSMODE;
//  *--stk = (SVC32MODE|0x0);           /* spsr  IRQ, FIQ disable             */


    return ((OS_STK *)stk);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*                                            (BEGINNING)
*
* Description: This function is called by OSInit() at the beginning of OSInit().
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts should be disabled during this call.
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSInitHookBegin (void)
{
#if OS_TMR_EN > 0u
	OSTmrCtr = 0;
#endif
}
#endif

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*                                               (END)
*
* Description: This function is called by OSInit() at the end of OSInit().
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts should be disabled during this call.
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSInitHookEnd (void)
{
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                          TASK CREATION HOOK
*
* Description: This function is called when a task is created.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTaskCreateHook (OS_TCB *ptcb)
{
#if OS_APP_HOOKS_EN > 0u
	App_TaskCreateHook(ptcb);
#else
	(void)ptcb;
#endif
}
#endif

/*
*********************************************************************************************************
*                                           TASK DELETION HOOK
*
* Description: This function is called when a task is deleted.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTaskDelHook (OS_TCB *ptcb)
{
#if OS_APP_HOOKS_EN > 0u
	App_TaskDelHook(ptcb);
#else
	(void)ptcb;
#endif
}
#endif

/*
*********************************************************************************************************
*                                             IDLE TASK HOOK
*
* Description: This function is called by the idle task.  This hook has been added to allow you to do  
*              such things as STOP the CPU to conserve power.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are enabled during this call.
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTaskIdleHook (void)
{
#if OS_APP_HOOKS_EN > 0u
	App_TaskIdleHook();
#endif
}
#endif

/*
*********************************************************************************************************
*                                           STATISTIC TASK HOOK
*
* Description: This function is called every second by uC/OS-II's statistics task.  This allows your 
*              application to add functionality to the statistics task.
*
* Arguments  : none
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTaskStatHook (void)
{
#if OS_APP_HOOKS_EN > 0u
	App_TaskStatHook();
#endif
}
#endif

/*
*********************************************************************************************************
*                                           TASK SWITCH HOOK
*
* Description: This function is called when a task switch is performed.  This allows you to perform other
*              operations during a context switch.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are disabled during this call.
*              2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                 will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the 
*                 task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

#if (OS_CPU_HOOKS_EN > 0u) && (OS_TASK_SW_HOOK_EN > 0u)
void  OSTaskSwHook (void)
{
#if OS_APP_HOOKS_EN > 0u
	App_TaskSwHook();
#endif
}
#endif

/*
*********************************************************************************************************
*                                           OSTCBInit() HOOK
*
* Description: This function is called by OS_TCBInit() after setting up most of the TCB.
*
* Arguments  : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTCBInitHook (OS_TCB *ptcb)
{
#if OS_APP_HOOKS_EN > 0u
	App_TCBInitHook(ptcb);
#else
	(void)ptcb;
#endif
}
#endif

/*
*********************************************************************************************************
*                                               TICK HOOK
*
* Description: This function is called every tick.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if (OS_CPU_HOOKS_EN > 0u) && (OS_TIME_TICK_HOOK_EN > 0u)
void  OSTimeTickHook (void)
{
#if OS_APP_HOOKS_EN > 0u
	App_TimeTickHook();
#endif

#if OS_TMR_EN > 0u
	OSTmrCtr++;
	if (OSTmrCtr >= (OS_TICKS_PER_SEC / OS_TMR_CFG_TICKS_PER_SEC)) {
		OSTmrCtr = 0;
		OSTmrSignal();
	}
#endif
}
#endif

/*
*********************************************************************************************************
*                                            TASK RETURN HOOK
*
* Description:
*
* Arguments  :
*
* Note(s)    :
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTaskReturnHook (OS_TCB  *ptcb)
{
#if OS_APP_HOOKS_EN > 0u
	App_TaskReturnHook(ptcb);
#else
	(void)ptcb;
#endif
}
#endif

/*
*********************************************************************************************************
*                                            OSTickISR()
*
* Description:
*
* Arguments  :
*
* Note(s)    :
*********************************************************************************************************
*/

void OSTickISR(void)
{
	OSIntEnter();
	OSTimeTick();
	OSIntExit();
}
