/********************************************************************
Copyright 2010-2017 K.C. Wang.
********************************************************************/

#include "type.h"
#include "string.c"

#include "exceptions.c"
#include "kbd.c"
#include "vid.c"

#define NPROC 5
PROC proc[NPROC], *running;
int procsize = sizeof(PROC);

void copy_vectors(void) {
    extern u32 vectors_start, vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
      *vectors_dst++ = *vectors_src++;
}

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

int body()
{
  char c; char line[64];
  int pid;
  printf("proc %d resume to body()\n", running->pid);
  while(1){
    pid = running->pid;
    if (pid==0) color=WHITE;
    if (pid==1) color=GREEN;
    if (pid==2) color=RED;
    if (pid==3) color=CYAN;
    if (pid==4) color=PURPLE;

    printf("proc %d in body() input a char [s] : ", running->pid);
    c = kgetc(); 
    printf("%c\n", c);
    
    printf("\n proc %d in body() switches to the next proc \n", running->pid);
    
    // the current process gives up CPU time, switches to the next process
  }
}

// Initialize the initial context/environment of every process
int kernel_init()
{
  int i, j; 
  PROC *p;
  
  kprintf("kernel_init() .... \n");
  for (i=0; i<NPROC; i++){
    // initialize each of the field of the struct proc
    // pid
    // status
    
    // initialize the kernel stack: pretend that each shared CPU register
    // has an initial value. Initialize the following: LR, R12 - R0
    
    
    // make the LR (which is in kstack[SSIZE-1] point to the entry of the
    // function body()
    
    // point ksp to the saved R0 in kstack;
    p->ksp = &(p->kstack[SSIZE-14]);
 
    // initialize the proc next pointer
    
  }
  
  // Form circular proc list by pointing the next pointer of PROC 4 to PROC 0
  
  // initialize the first running PROC to be PROC0;
  
  printf("kernel_init done\n");
}

int scheduler()
{
  int ocolor = color;
  color = YELLOW;
  printf("proc %d in scheduler \n", running->pid);
  
  // make the running proc to be the one next to the current proc
  
  
  printf("next running proc = %d\n", running->pid);
  color = ocolor;
}  

int main()
{ 
   fbuf_init();
   kbd_init();
   color = WHITE;

   printf("Welcome to the basic context switching for processes in Arm\n");
   
   // Initialize the initial context/environment of every process
   
   printf("The initial PROC 0 switches process\n");
   // PROC 0 runs the switching
}
