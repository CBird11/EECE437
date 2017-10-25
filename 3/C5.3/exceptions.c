/********************************************************************
Copyright 2010-2017 K.C. Wang.
********************************************************************/

int kprintf(char *fmt, ...);
/*********
//void __attribute__((interrupt)) kc_handler() {
void kc_handler() {
  int vicstatus, sicstatus;
  int ustatus, kstatus;

  // read VIC status register to find out which interrupt
  vicstatus = VIC_STATUS;
  sicstatus = SIC_STATUS;  
  //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);

    if (vicstatus & 0x0010){
         timer0_handler();
    }
    if (vicstatus & 0x1000){
         uart0_handler();
    }
    if (vicstatus & 0x2000){
         uart1_handler();
    }
    if (vicstatus & 0x80000000){
       if (sicstatus & 0x08){
          kbd_handler();
       }
    }
    VIC_VADDR = 0xFF;
    return;
}
***********/
/* all other handlers are infinite loops */
void __attribute__((interrupt)) undef_handler(void) { for(;;); }
void __attribute__((interrupt)) swi_handler(void) { for(;;); }
void __attribute__((interrupt)) prefetch_abort_handler(void) { for(;;); }
void __attribute__((interrupt)) data_abort_handler(void) { for(;;); }
void __attribute__((interrupt)) fiq_handler(void) { for(;;); }
