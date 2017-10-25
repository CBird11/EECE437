/********************************************************************
Copyright 2010-2017 K.C. Wang.
********************************************************************/

#include "type.h"
#include "string.c"
#include "queue.c"
#include "kbd.c"
#include "vid.c"
#include "exceptions.c"
#include "kernel.c"


void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);
void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC status register to find out which interrupt
    vicstatus = VIC_STATUS; // VIC_STATUS=0x10140000=status reg
    sicstatus = SIC_STATUS;  
    //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);
    if (vicstatus & 0x80000000){
       if (sicstatus & 0x08){
          kbd_handler();
       }
    }
}
int body();

int main()
{ 
   int i; 
   char line[128]; 
   u8 kbdstatus, key, scode;

   color = WHITE;
   row = col = 0; 

   fbuf_init();
   kbd_init();

   /* enable KBD IRQ */
   VIC_INTENABLE |= 1<<31;    // SIC to VIC's IRQ31
   SIC_ENSET |= 1<<3;  // KBD int=3 on SIC

   kprintf("Welcome to Dynamic Processes in Arm\n");
   kernel_init();
   kfork((int)body, 1);   // fetch the first available free PROC (PROC1)
   while(1){
     if (readyQueue)
        tswitch();
   }
}
