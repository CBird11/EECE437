/********************************************************************
Copyright 2010-2017 K.C. Wang
********************************************************************/

#include "defines.h"
#include "string.c"

char *tab = "0123456789ABCDEF";
//int BASE;
int color;

#include "uart.c"
#include "kbd.c"
#include "timer.c"
#include "vid.c"
#include "exceptions.c"

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;
    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);
void timer_handler();

// use vectored interrupts of PL190
void timer0_handler()
{  
  timer_handler(0);
}
void uart0_handler()
{
  uart_handler(&uart[0]);
}
void uart1_handler()
{
  uart_handler(&uart[1]);
}

int vectorInt_init()
{
  kprintf("vectorInterrupt_init()\n");
  kprintf("t=%x u0=%x u1=%x kbd=%x\n", 
	 timer0_handler, uart0_handler, uart1_handler, kbd_handler);
 
  // set up vectored interrupts for (REF: KCW's armHowtoVectorIntPlan file)
  // timer0 at IRQ4, UART0 at IRQ12, UART1 at IRQ13, KBD to IRQ31:

  // (1) write to vectoraddr0 (0x100) with ISR of timer0,
  //              vectoraddr1 (0x104) with ISR of UART0,
  //              vectoraddr2 (0x108) with ISR of UART1,
  //              vectoraddr3 (0x10C) with ISR of KBD
  // all are offsets from VIC base at 0x10140000; (SIC is NOT used at all)
  *((int *)(VIC_BASE_ADDR+0x10C)) = (int)timer0_handler; 
  *((int *)(VIC_BASE_ADDR+0x108)) = (int)uart0_handler; 
  *((int *)(VIC_BASE_ADDR+0x104)) = (int)uart1_handler; 
  *((int *)(VIC_BASE_ADDR+0x100)) = (int)kbd_handler; 
  //(2). write to intControlRegs = E=1|IRQ# =  1xxxxx
  *((int *)(VIC_BASE_ADDR+0x200)) = 0x3F;    //100100 at IRQ 4
  *((int *)(VIC_BASE_ADDR+0x204)) = 0x2D;    //101100 at IRQ 12
  *((int *)(VIC_BASE_ADDR+0x208)) = 0x2C;    //101101 at IRQ 13
  *((int *)(VIC_BASE_ADDR+0x20C)) = 0x24;    //111111 at IRQ 31

  //write 32-bit 0's to IntSelectReg to generate IRQ interrupts (1 for FIQs)
  *((int *)(VIC_BASE_ADDR+0x0C)) = 0;
}

// Must rewrite irq_handler() to use vectors: HOW:
// IRQ => still comes to irq_handler() => no need to read status registers 
// to determine the interrupt source ==> should get the ISR address directly 
// by reading vectorAddrReg at 0x30 => get ISR address, then invoke it.
// upon return, must write to vectorAddrReg (any value) as EOI

void IRQ_handler(int lr, int pc)
{
  int (*f)();                         // f is a function pointer
  f =(void *)*((int *)(VIC_BASE_ADDR+0x30)); // read ISR address in vectorAddr
  f();                                // call the ISR function
  *((int *)(VIC_BASE_ADDR+0x30)) = 1; // write to VIC vectorAddr reg as EOI
}

/**** original IRQ_handler() by polling satus register ****
//void __attribute__((interrupt)) kc_handler()
void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC SIV status registers to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;  
    //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);
    // VIC status BITs: timer0=4, uart0=13, uart1=14, SIC=31: KBD at 3

    if (vicstatus & 0x0010){
         timer_handler(0);
	 // kprintf("TIMER "); // verify timer handler return to here
    }
    if (vicstatus & 0x1000){ // Bit 12 
         uart_handler(&uart[0]);
	 // kprintf("U0 "); 
    }
    if (vicstatus & 0x2000){ // bit 13
         uart_handler(&uart[1]);
    }
    if (vicstatus & 0x80000000){ // SIC interrupts=bit_31=>KBD at bit 3 
       if (sicstatus & 0x08){
          kbd_handler();
       }
    }
}
**********************************************/
int main()
{
   int i; 
   u8 kbdstatus, scode, key;
   char line[128];
   UART *up;
   KBD  *kp;
   color = RED;
   row = col = 0; 

   fbuf_init();
   kbd_init();
   uart_init();


   up = &uart[0];
   kp = &kbd;

   /* enable timer0,1, uart0,1 SIC interrupts */
   VIC_INTENABLE |= (1<<4);  // timer0,1 at bit4 
   VIC_INTENABLE |= (1<<12); // UART0 at bit12
   VIC_INTENABLE |= (1<<13); // UART1 at bit13
   VIC_INTENABLE |= (1<<31); // SIC to VIC's IRQ31
  
   /* enable KBD IRQ */
   SIC_INTENABLE |= (1<<3); // KBD int=bit3 on SIC
   *(kp->base+KCNTL) = 0x12; // 1<<4;

   kprintf("C8 start: test Vectored Interrupts\n");
   vectorInt_init(); // must do thie AFTER driver_init()
   timer_init();
   timer_start(0);

   kprintf("test uart0 I/O: enter text from UART 0\n");
   while(1){
     ugets(up, line);
     uprintf("  line=%s\n", line);
     if (strcmp(line, "end")==0)
	break;
   }

   kprintf("test UART1 I/O: enter text from UART 1\n");
   up = &uart[1];
   while(1){
     ugets(up, line);
     ufprintf(up, "  line=%s\n", line);
     if (strcmp(line, "end")==0)
	break;
   }

   //uprintf("test KBD inputs\n"); // print to uart0
   kprintf("test KBD inputs\n"); // print to LCD
   
   while(1){
      kgets(line);
      color = GREEN;
      kprintf("line=%s\n", line);
      if (strcmp(line, "end")==0)
         break;
   }
   kprintf("END OF run %d\n", 1234);
}
