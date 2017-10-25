/********************************************************************
Copyright 2010-2017 K.C. Wang.
********************************************************************/

// interrupts.c file
/* all other interrupt handlers are while(1) loops */
void __attribute__((interrupt)) undef_handler(void) { while(1); }
void __attribute__((interrupt)) swi_handler(void) { while(1); }
void __attribute__((interrupt)) prefetch_abort_handler(void) { while(1); }
void __attribute__((interrupt)) data_abort_handler(void) { while(1); }
void __attribute__((interrupt)) fiq_handler(void) { while(1); }
