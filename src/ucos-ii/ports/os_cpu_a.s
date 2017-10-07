	AREA	os_cpu_a, CODE, READONLY
	CODE32

	IMPORT	LINK_SAVE
	IMPORT	HandleADC

	IMPORT	OSTCBCur
	IMPORT	OSTCBHighRdy
	IMPORT	OSPrioCur
	IMPORT	OSPrioHighRdy

	IMPORT	OSRunning
;	IMPORT	OSTaskSwHook

	EXPORT	OSStartHighRdy
	EXPORT	OSCtxSw
	EXPORT	OSIntCtxSw
	EXPORT	OSIsrIRQ
	EXPORT	ARMDisableInt
	EXPORT	ARMEnableInt

I_ISPR		EQU		0x01e00020
I_ISPC		EQU		0x1e00024
NOINT		EQU		0xC0


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Function: 	OSStartHighRdy																			;
; Purpose:		To start the task with the highest priority during OS startup							;
; Processing:	See uC/OS-II Task Level Context Switch flow chart										;
; Parameters: 	void																					;
; Outputs:  	None																					;
; Returns:  	void																					;
; Notes:		Called once during OSStart()															;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

OSStartHighRdy
;	BL		OSTaskSwHook
	LDR		r0, = OSRunning			; OSRunning = 1
	MOV		r1, #1
	STRB	r1, [r0]

	LDR		r5, = OSTCBHighRdy
	LDR		r5, [r5]				; get stack pointer
	LDR		sp, [r5]				; switch to the new stack

	LDMFD	sp!, {r4}				; get new state from top of the stack
	MSR		CPSR_cxsf, r4			; CPSR should be SVC32Mode
	LDMFD	sp!, {r0 - r12, pc}	; start the new task


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Function: 	OS_TASK_SW 																				;
; Purpose: 		To perform a context switch from the Task Level.										;
; Processing:	See uC/OS-II Task Level Context Switch flow chart										;
; Parameters:	void																					;
; Outputs:  	None																					;
; Returns:		void																					;
; Notes:		The whole function is executed in CRITICAL state. See OSSched().						;
;																										;
; On entry, OSTCBCur and OSPrioCur hold the current TCB and priority									;
; and OSTCBHighRdy and OSPrioHighRdy contain the same for the task										;
; to be switched to.																					;
; The following code assumes that the virtual memory is directly										;
; mapped into  physical memory. If this is not true, the cache must 									;
; be flushed at context switch to avoid address aliasing.												;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

OSCtxSw
OSIntCtxSw
	STMFD	sp!, {lr}				; save lr
	STMFD	sp!, {r0 - r12}			; save register file and ret address
	MRS		r4, CPSR
	STMFD	sp!, {r4}				; save current PSR

	LDR		r4, = OSTCBCur
	LDR		r5, [r4]
	STR		sp, [r5]				; store sp in preempted taskss TCB

;	BL		OSTaskSwHook
	LDR		r4, = OSPrioCur
	LDR		r5, = OSPrioHighRdy
	LDRB	r6, [r5]
	STRB	r6, [r4]

	LDR		r4, = OSTCBHighRdy
	LDR		r5, = OSTCBCur
	LDR		r4, [r4]
	STR		r4, [r5]

	LDR		sp, [r4]
	LDMFD	sp!, {r4}
	MSR		CPSR_cxsf, r4
; have to restore lr, do not know why
	LDMFD	sp!, {r0 - r12, pc}


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Function:		OSIsrIRQ																				;
; Purpose:																								;
; Processing:																							;
; Parameters:																							;
; Outputs:																								;
; Returns:																								;
; Notes:																								;					
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
OSIsrIRQ
; SPSR_irq and r14_irq must be saved on the stack of corresponding task
	STMFD	sp!, {r0}
	LDR		r0, = LINK_SAVE
	STR		lr, [r0]
	MRS		lr, spsr
	STR		lr, [r0, #4]
	LDMFD	sp!, {r0}
	ORR		lr, lr, #0x80
	MSR		cpsr_cxsf, lr

	SUB		sp, sp, #4
; in usual procedure call, lr is saved for "current function" in case of being corrupted
; the lr below is saved for the "caller function" instead of "current function"
; (IRQ is seen as "current function")
	STMFD	sp!, {r0 - r4, r12, lr}
	LDR		r0, = LINK_SAVE
	LDR		r1, [r0]
	SUB		r1, r1, #4
	STR		r1, [sp, #(7 * 4)]
	LDR		r1, [r0, #4]
	STMFD	sp!, {r1}

	LDR		r0, = I_ISPR
	LDR		r0, [r0]
	MOV		r1, #0x00
	MOV		r2, #0x01
0
	MOVS	r0, r0, lsr #1
	BCS		%F1
	MOV		r2, r2, lsl #1
	ADD		r1, r1, #4
	B		%B0
1
	LDR		r0, = I_ISPC
	STR		r2, [r0]

	LDR		r0, = HandleADC
	ADD		r0, r0, r1
	LDR		r0, [r0]

	MOV		lr, pc
	MOV		pc, r0

	LDMFD	sp!, {r0}
	MSR		cpsr_cxsf, r0
	LDMFD	sp!, {r0 - r4, r12, lr, pc}


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Functions: 	ARMDisableInt	ARMEnableInt															;
; Purpose:		Disable and enable IRQ and FIQ preserving current CPU mode.								;
; Processing:	Push the cpsr onto the stack, Disable IRQ and FIQ interrupts, Return					;
; Parameters: 	void																					;
; Outputs:  	None																					;
; Returns:  	void																					;
; Notes:																								;
;   (1) Can be called from SVC mode to protect Critical Sections. 										;
;   (2) Do not use these calls at interrupt level.														;
;   (3) Used in pairs within the same function level;													;
;   (4) Will restore interrupt state when called; i.e., if interrupts									;
;       are disabled when DisableInt is called, interrupts will still									;
;       still be disabled when the matching EnableInt is called.										;
;   (5) Uses the method described by Labrosse as "Method 2".											;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ARMDisableInt
	MRS		r0, CPSR
	ORR		r12, r0, #NOINT
	MSR		CPSR_c, r12
	MOV		pc, lr

ARMEnableInt
	MSR		CPSR_c, r0
	MOV		pc, lr


	END
