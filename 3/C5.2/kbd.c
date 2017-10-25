/********************************************************************
Copyright 2010-2017 K.C. Wang.
********************************************************************/

// kbd.c file
#define N_SCAN 64

#include "keymap"
/********* byte offsets; for char *base *********/
#define KCNTL    0x00 // 7- 6-    5(0=AT)  4=RxIntEn 3=TxIntEn  2   1   0
#define KSTAT    0x04 // 7- 6=TxE 5=TxBusy 4=RXFull  3=RxBusy   2   1   0
#define KDATA    0x08 // data register;
#define KCLK     0x0C // clock register;
#define KISTA    0x10 // int status register;

typedef volatile struct kbd{ // base = 0x1000 6000
  char *base;         // base address of KBD, as char *
  char buf[128];
  int head, tail, data, room;
}KBD;

extern int color;
volatile KBD kbd;
int kputc(char);

int kbd_init()
{
   KBD *kp = &kbd;
   kp->base = (char *)0x10006000;
   *(kp->base+KCNTL) = 0x14; // 0001 0100
   *(kp->base+KCLK)  = 8;
   kp->data = 0;kp->room = 128; 
   kp->head = kp->tail = 0;
   /* enable KBD IRQ */
   VIC_INTENABLE |= 1<<31;   // SIC to VIC's IRQ31
   SIC_ENSET |= 1<<3;        // KBD int=3 on SIC
}

void kbd_handler()
{
  u8 scode, c;
  KBD *kp = &kbd;
  scode = *(kp->base+KDATA);
  if (scode & 0x80)
    return;

  c = unsh[scode]; 
  /****************
  kprintf("kbd interrupt: c=");
  if (c != '\r')
    kprintf("%x %c\n", c, c); 
  else
    kprints("0x 0D <cr>\n\r");
  ********************/
  kp->buf[kp->head++] = c;
  kp->head %= 128;
  kp->data++; kp->room--;
}

int kputc(char);  // kputc() in vid.c driver

int kgetc()
{
  char c;
  KBD *kp = &kbd;
  while(kp->data <= 0); // wait for data > 0; RONLY, no need to lock
  c = kp->buf[kp->tail++];
  kp->tail %= 128;

  // updating variables: must disable interrupts
  int_off();
    kp->data--; kp->room++;
  int_on();
  return c;
}
