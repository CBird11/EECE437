/********************************************************************
Copyright 2010-2017 K.C. Wang
********************************************************************/

#include "defines.h"
#include "string.c"

char *tab = "0123456789ABCDEF";
int color;

#include "timer.c"
#include "vid.c"
#include "exceptions.c"
#include "kbd.c"

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

// IRQ interrupts handler entry point
//void __attribute__((interrupt)) IRQ_handler()
// timer0 base=0x101E2000; timer1 base=0x101E2020
// timer3 base=0x101E3000; timer1 base=0x101E3020
// currentValueReg=0x04
TIMER *tp[4];



void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC SIV status registers to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;  
    //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);
    // VIC status BITs: timer0,1=4, uart0=13, uart1=14, SIC=31: KBD at 3
    
    if (vicstatus & (1<<4)){   // timer0,1=bit4
         timer_handler(0);
	}
	
	if (vicstatus & (1<<31)){
    	if (sicstatus & (1<<3)){
			kbd_handler();
		}
	}
}


int main()
{
   int i; 
   char line[128];

   color = YELLOW;
   row = col = 0; 
   fbuf_init();

   /* enable timer0,1, uart0,1 SIC interrupts */
   VIC_INTENABLE |= (1<<4);  // timer0,1 at bit4 
   //VIC_INTENABLE |= (1<<5);  // timer2,3 at bit5 

   VIC_INTENABLE |= (1<<31); // SIC to VIC's IRQ31

   /* enable KBD IRQ */
   SIC_ENSET = 1<<3;  // KBD int=3 on SIC
   SIC_PICENSET = 1<<3;  // KBD int=3 on SIC
 
   kprintf("C3.2 start: test timer KBD drivers by interrupts\n");
   timer_init();
   kbd_init();
   
   timer_start(0);
   while(1){
      color = CYAN;
      kprintf("Enter a line from KBD\n");
      kgets(line);
      color = CYAN;
      kprintf("line = %s\n", line);
   }
}
