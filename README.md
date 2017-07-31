# s3c44b0x_ucos-ii_book_modified

* "OSIsrIRQ" is used. "IsrIRQ" is left there without being used.
* "OSIsrIRQ" does the routing.
* The start of the interrupt is "OSIsrIRQ" instead of interrupt handler.
* Official version - OSIsrIRQ.
* Only "OSIsrIRQ" is manually written in assembly. Interrupt handler can be written in C without "__irq".
* Clearing interrupt is done in "OSIsrIRQ".
* "OSIntEnter" and "OSIntExit" are used in "OSIsrIRQ". "OSIntEnter" and "OSIntExit" cannot be used in interrupt handler.
