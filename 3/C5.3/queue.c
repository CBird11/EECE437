/********************************************************************
Copyright 2010-2017 K.C. Wang.
********************************************************************/

extern PROC *freeList;	// global freeList
PROC *get_proc(PROC **list)	// return a PROC type pointer
{
  
}

int put_proc(PROC **list, PROC *p)
{
  
}


int enqueue(PROC **queue, PROC *p)
{
  

PROC *dequeue(PROC **queue)
{
  
}

int printQ(PROC *p)
{
  kprintf("readyQueue = ");
  while(p){
    kprintf("[%d prt %d]->", p->pid,p->priority);
    p = p->next;
  }
  kprintf("NULL\n");
}

int printList(PROC *p)
{
   kprintf("freeList = ");
   while(p){
     kprintf("[%d]->", p->pid);
     p = p->next;
  }
  kprintf("NULL\n"); 
}
