/********************************************************************
Copyright 2010-2017 K.C. Wang.
********************************************************************/

/********************
#define  SSIZE 1024
#define  NPROC  9
#define  FREE   0
#define  READY  1
#define  SLEEP  2
#define  BLOCK  3
#define  ZOMBIE 4
#define  printf  kprintf
 
typedef struct proc{
  struct proc *next;
  int    *ksp;
  int    status;
  int    pid;
  int    priority;
  int    kstack[SSIZE];
}PROC;
***************************/
#define NPROC 9
PROC proc[NPROC], *running, *freeList, *readyQueue;
int procsize = sizeof(PROC);
int body();

int kernel_init()
{
  int i, j; 
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->status = FREE;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0; // circular proc list
  freeList = &proc[0];
  readyQueue = 0;

  p = get_proc(&freeList);
  p->priority = 0;
  
  running = p;
  
  kprintf("The running process is %d\n", running->pid);
  printList(freeList);
}

void kexit()
{
  printf("proc %d kexit\n", running->pid);
  running->status = FREE;
  put_proc(&freeList, running);   // return to the freeList
  tswitch();
}
  
PROC *kfork(int func, int priority)
{
  int i;
  PROC *p = get_proc(&freeList);
  if (p==0){
    printf("no more free PROC, kfork failed\n");
    return 0;
  }
  p->status = READY;
  p->priority = priority;
  // set kstack to resume to body
  // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
  //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
  for (i=1; i<15; i++)
      p->kstack[SSIZE-i] = 0;
  p->kstack[SSIZE-1] = (int)func;  
  
  p->ksp = &(p->kstack[SSIZE-14]);
  enqueue(&readyQueue, p);
  printf("%d kforked a child %d\n", running->pid, p->pid);
  printList(freeList);
  printQ(readyQueue);
  return p;
}

int scheduler()
{
  kprintf("proc %d in scheduler ", running->pid);
  if (running->status == READY)    
    enqueue(&readyQueue, running);
  running = dequeue(&readyQueue);
  kprintf("next running = %d\n", running->pid);
}  


int body()
{
  char c; char line[64];
  int pid;
  kprintf("proc %d resume to body()\n", running->pid);
  while(1){
    pid = running->pid;
    if (pid==0) color=BLUE;
    if (pid==1) color=RED;
    if (pid==2) color=GREEN;
    if (pid==3) color=CYAN;
    if (pid==4) color=YELLOW;
    if (pid==5) color=RED;
    if (pid==6) color=GREEN;   
    if (pid==7) color=WHITE;
    if (pid==8) color=CYAN;

    kprintf("proc %d in body() input a char [s|f|x] : ", running->pid);
    c = kgetc(); 
    printf("%c\n", c);

    switch(c){
      case 's': tswitch();           break;
      case 'f': kfork((int)body, 1); break;
      case 'x': kexit();             break;
    }
  }
}
