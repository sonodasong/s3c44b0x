# s3c44b0x_ucos-ii_enhanced_1

* "OSIsrIRQ" is used. "IsrIRQ" is left there without being used.
* "OSIsrIRQ" does the routing.
* The start of the interrupt is "OSIsrIRQ" instead of interrupt handler.
* Enhanced version.
* Only "OSIsrIRQ" is manually written in assembly. Interrupt handler can be written in C without "__irq".
* Clearing interrupt is done in "OSIsrIRQ".
* "OSIntCtxSw" is the same as "OSCtxSw".
* "OSIntEnter" and "OSIntExit" can be optionally used.
* stack structure:
	* lr
	* r12 - r0
	* cpsr
