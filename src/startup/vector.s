;********************************************************
;*  NAME       : VECTOR.S                               *
;*  Description: start up codes                         *
;********************************************************

; Interrupt Control
INTPND      EQU 0x01e00004
INTMOD      EQU 0x01e00008
INTMSK      EQU 0x01e0000c
I_ISPR      EQU 0x01e00020
I_CMST      EQU 0x01e0001c

; Watchdog timer
WTCON       EQU 0x01d30000

; Clock Controller
PLLCON      EQU 0x01d80000
CLKCON      EQU 0x01d80004
LOCKTIME    EQU 0x01d8000c

; Memory Controller
BWSCON      EQU 0x01c80000

; Pre-defined constants
USERMODE    EQU 0x10
FIQMODE	    EQU 0x11
IRQMODE	    EQU 0x12
SVCMODE     EQU 0x13
ABORTMODE   EQU 0x17
UNDEFMODE   EQU 0x1b
SYSMODE     EQU 0x1f
MODEMASK    EQU 0x1f
NOINT       EQU 0xc0

; check if tasm.exe is used.
  [ {CONFIG} = 16
    CODE32
  ]
    MACRO
$HandlerLabel HANDLER $Handle

$HandlerLabel
    sub	    sp, sp, #4                  ; decrement sp (to store jump address)
    stmfd   sp!, {r0}                   ; PUSH the work register to stack (lr doest push because it return to original address)
    ldr	    r0, = $Handle               ; load the address of HandleXXX to r0
    ldr     r0, [r0]                    ; load the contents (service routine start address) of HandleXXX
    str	    r0, [sp, #4]                ; store the contents (ISR) of HandleXXX to stack
    ldmfd   sp!, {r0, pc}               ; POP the work register and pc (jump to ISR)
    MEND

    IMPORT  |Image$$RO$$Base|
    IMPORT  |Image$$RO$$Limit|          ; End of ROM code (=start of ROM data)
    IMPORT  |Image$$RW$$Base|           ; Base of RAM to initialise
    IMPORT  |Image$$ZI$$Base|           ; Base and limit of area
    IMPORT  |Image$$ZI$$Limit|          ; to zero initialise

    IMPORT  OSIsrIRQ
    IMPORT  Main                        ; The main entry of the program
    
    EXPORT  BaseOfRO
    EXPORT  TopOfRO
    EXPORT  BaseOfRW
    EXPORT  BaseOfZI
    EXPORT  EndOfZI

    EXPORT HandleADC

    AREA    Init, CODE, READONLY

    ENTRY
    b ResetHandler                      ; for debug
    b HandlerUndef                      ; handlerUndef
    b HandlerSWI                        ; SWI interrupt handler
    b HandlerPabort                     ; handlerPAbort
    b HandlerDabort                     ; handlerDAbort
    b .                                 ; handlerReserved
    b HandlerIRQ
    b HandlerFIQ

HandlerFIQ      HANDLER HandleFIQ
HandlerIRQ      HANDLER HandleIRQ
HandlerUndef    HANDLER HandleUndef
HandlerSWI      HANDLER HandleSWI
HandlerDabort   HANDLER HandleDabort
HandlerPabort   HANDLER HandlePabort

;****************************************************
;*  START                                           *
;****************************************************
ResetHandler
    ldr	    r0, = WTCON                 ; watch dog disable
    ldr	    r1, = 0x0
    str	    r1, [r0]

    ldr	    r0, = INTMSK
    ldr	    r1, = 0x07ffffff            ; all interrupt disable
    str	    r1, [r0]

;****************************************************
;*  Set clock control registers                     *
;****************************************************
    ldr     r0, = LOCKTIME
    ldr     r1, = 4000                  ; count = t_lock * Fin (t_lock = 200us, Fin = 4MHz) = 800
    str     r1, [r0]

    ldr     r0, = PLLCON                ; temporary setting of PLL
; Fin = 10MHz, Fout = 75MHz
    ldr     r1, = ((0x43 << 12) + (0x03 << 4) + 0x01)
    str     r1, [r0]

    ldr     r0, = CLKCON
    ldr     r1, = 0x7ff8                ; All unit block CLK enable
    str     r1, [r0]

;****************************************************
;*  Set memory control registers                    *
;****************************************************
    ldr     r0, = SMRDATA
    ldmia   r0, {r1 - r13}
    ldr     r0, = BWSCON                ; BWSCON Address
    stmia   r0, {r1 - r13}

;****************************************************
;*  Initialize stacks                               *
;****************************************************
    ;ldr    sp, = SVCStack              ; Why?
    bl      InitStacks

;****************************************************
;*  Setup IRQ handler                               *
;****************************************************
    ldr     r0, = HandleIRQ             ; This routine is needed
    ldr     r1, = OSIsrIRQ              ; if there isnt 'subs pc, lr, #4' at 0x18, 0x1c
    str     r1, [r0]

;********************************************************
;*  Copy and paste RW data/zero initialized data        *
;********************************************************
    LDR     r0, = |Image$$RO$$Limit|    ; Get pointer to ROM data
    LDR     r1, = |Image$$RW$$Base|     ; and RAM copy
    LDR     r3, = |Image$$ZI$$Base|     ; Zero init base => top of initialised data

    CMP     r0, r1                      ; Check that they are different
    BEQ     %F1
0
    CMP     r1, r3                      ; Copy init data
    LDRCC   r2, [r0], #4                ; --> LDRCC r2, [r0] + ADD r0, r0, #4
    STRCC   r2, [r1], #4                ; --> STRCC r2, [r1] + ADD r1, r1, #4
    BCC     %B0
1
    LDR     r1, = |Image$$ZI$$Limit|    ; Top of zero init segment
    MOV     r2, #0
2
    CMP     r3, r1                      ; Zero init
    STRCC   r2, [r3], #4
    BCC     %B2


;********************************************************
;*  change to sys or user mode                          *
;********************************************************
    mrs     r0, cpsr
    bic     r0, r0, #MODEMASK | NOINT
    orr     r1, r0, #SYSMODE
    msr     cpsr_cxsf, r1               ; SysMode

    [ {CONFIG} = 32
    bl      Main                        ; Dont use main() because ......
    b       .
    ]

    [ {CONFIG} = 16                     ; for start-up code for Thumb mode
    orr     lr, pc, #1
    bx      lr
    CODE16
    bl      Main                        ; Dont use main() because ......
    b       .
    CODE32
    ]

;****************************************************
;*  The function for initializing stack             *
;****************************************************
InitStacks
; Dont use DRAM,such as stmfd,ldmfd......
; SVCstack is initialized before
; Under toolkit ver 2.50, 'msr cpsr, r1' can be used instead of 'msr cpsr_cxsf, r1'

    mrs     r0, cpsr
    bic     r0, r0, #MODEMASK
    orr     r1, r0, #UNDEFMODE | NOINT
    msr     cpsr_cxsf, r1               ; UndefMode
    ldr     sp, = UndefStack

    orr     r1, r0, #ABORTMODE | NOINT
    msr     cpsr_cxsf, r1               ; AbortMode
    ldr     sp, = AbortStack

    orr     r1, r0, #IRQMODE | NOINT
    msr     cpsr_cxsf, r1               ; IRQMode
    ldr     sp, = IRQStack

    orr     r1, r0, #FIQMODE | NOINT
    msr     cpsr_cxsf, r1               ; FIQMode
    ldr     sp,  =FIQStack

    orr     r1, r0, #SYSMODE | NOINT
    msr     cpsr_cxsf, r1               ; SYSMode
    ldr     sp, = UserStack

    bic     r0, r0, #MODEMASK | NOINT
    orr     r1, r0, #SVCMODE
    msr     cpsr_cxsf, r1               ; SVCMode
    ldr     sp, = SVCStack

; USER mode is not initialized.
    mov     pc, lr                      ; The LR register may be not valid for the mode changes.
; One of the following two routines can be used for non-vectored interrupt.
IsrIRQ                                  ; using I_ISPR register.
    sub     sp, sp, #4                  ; reserved for PC
    stmfd   sp!, {r8 - r9}

; IMPORTANT CAUTION
; if I_ISPC isnt used properly, I_ISPR can be 0 in this routine.

    ldr     r9, = I_ISPR
    ldr     r9, [r9]
    mov     r8, #0x0
0
    movs    r9, r9, lsr #1
    bcs     %F1
    add     r8, r8, #4
    b       %B0

1
    ldr     r9, = HandleADC
    add     r9, r9, r8
    ldr     r9, [r9]
    str     r9, [sp, #8]
    ldmfd   sp!, {r8 - r9, pc}

;*****************************************************************
;* Memory configuration has to be optimized for best performance *
;* The following parameter is not optimized.                     *
;*****************************************************************
SMRDATA
    DCD 0x11001102
    DCD 0x00000600
    DCD 0x00007ffc
    DCD 0x00007ffc
    DCD 0x00007ffc
    DCD 0x000054A8
    DCD 0x00007ffc
    DCD 0x00018000
    DCD 0x00018000
    DCD 0x00820591
    DCD 0x00000016
    DCD 0x00000020
    DCD 0x00000020

BaseOfRO    DCD |Image$$RO$$Base|
TopOfRO     DCD |Image$$RO$$Limit|
BaseOfRW    DCD |Image$$RW$$Base|
BaseOfZI    DCD |Image$$ZI$$Base|
EndOfZI     DCD |Image$$ZI$$Limit|

    ALIGN

_ISR_STARTADDRESS   EQU   0xc7fff00     ; GCS6:64M DRAM/SDRAM
;   AREA RamData, DATA, READWRITE

    ^   (_ISR_STARTADDRESS - 0x600)

UserStack   #   256                     ; c1(c7)ff900
SVCStack    #   256                     ; c1(c7)ffa00
UndefStack  #   256                     ; c1(c7)ffb00
AbortStack  #   512                     ; c1(c7)ffc00
IRQStack    #   256                     ; c1(c7)ffe00
FIQStack    #   0                       ; c1(c7)fff00


    ^   _ISR_STARTADDRESS
HandleReset     #   4
HandleUndef     #   4
HandleSWI       #   4
HandlePabort    #   4
HandleDabort    #   4
HandleReserved  #   4
HandleIRQ       #   4
HandleFIQ       #   4

; Dont use the label 'IntVectorTable',
; because armasm.exe cannot recognize this label correctly.
; the value is different with an address you think it may be.
; IntVectorTable
HandleADC       #   4
HandleRTC       #   4
HandleUTXD1     #   4
HandleUTXD0     #   4
HandleSIO       #   4
HandleIIC       #   4
HandleURXD1     #   4
HandleURXD0     #   4
HandleTIMER5    #   4
HandleTIMER4    #   4
HandleTIMER3    #   4
HandleTIMER2    #   4
HandleTIMER1    #   4
HandleTIMER0    #   4
HandleUERR01    #   4
HandleWDT       #   4
HandleBDMA1     #   4
HandleBDMA0     #   4
HandleZDMA1     #   4
HandleZDMA0     #   4
HandleTICK      #   4
HandleEINT4567  #   4
HandleEINT3     #   4
HandleEINT2     #   4
HandleEINT1     #   4
HandleEINT0     #   4                   ; 0xc1(c7)fff84

    END
