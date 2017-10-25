/********************************************************************
Copyright 2010-2017 K.C. Wang.
********************************************************************/

	.text
.code 32

.global vectors_start
.global vectors_end
.global proc
.global procsize
.global tswitch, scheduler, running
.global int_off, int_on
	
vectors_start:
  LDR PC, reset_handler_addr
  LDR PC, undef_handler_addr
  LDR PC, swi_handler_addr
  LDR PC, prefetch_abort_handler_addr
  LDR PC, data_abort_handler_addr
  B .
  LDR PC, irq_handler_addr
  LDR PC, fiq_handler_addr

reset_handler_addr:          .word reset_handler
undef_handler_addr:          .word undef_handler
swi_handler_addr:            .word swi_handler
prefetch_abort_handler_addr: .word prefetch_abort_handler
data_abort_handler_addr:     .word data_abort_handler
irq_handler_addr:            .word irq_handler
fiq_handler_addr:            .word fiq_handler

vectors_end:

reset_handler:
  /* set Supervisor stack */
  //LDR sp, =stack_top
  LDR r0, =proc
  LDR r1, =procsize
  LDR r2,[r1,#0]
  ADD r0, r0, r2
  MOV sp, r0
	
  /* copy vector table to address 0 */  BL copy_vectors
  /* get Program Status Register */
  MRS r0, cpsr
  /* go in IRQ mode */
  BIC r1, r0, #0x1F
  ORR r1, r1, #0x12
  MSR cpsr, r1
  /* set IRQ stack */
  LDR sp, =irq_stack
  /* Enable IRQs */
  BIC r0, r0, #0x80
  /* go back in Supervisor mode */
  MSR cpsr, r0
  /* jump to main */
  BL main
  B .

.align 4
irq_handler:

  sub	lr, lr, #4
  stmfd	sp!, {r0-r10, fp, ip, lr}

  bl	IRQ_handler  

  ldmfd	sp!, {r0-r10, fp, ip, pc}^

tswitch:
//       1  2  3  4  5  6  7  8  9  10  11  12  13  14
//       ---------------------------------------------
// stack=r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
//       ---------------------------------------------
  stmfd	sp!, {r0-r12, lr}

  LDR r0, =running // r0=&running
  LDR r1, [r0,#0]  // r1->runningPROC
  str sp, [r1,#4]  // running->ksp = sp
	

  bl	scheduler

  LDR r0, =running
  LDR r1, [r0,#0]     // r1->runningPROC
  lDR sp, [r1,#4]
	
  ldmfd	sp!, {r0-r12, pc}
// int_on()/int_off(): turn on/off IRQ interrupts
int_on: // may pass parameter in r0
  MRS r4, cpsr
  BIC r4, r4, #0x80   // clear bit means UNMASK IRQ interrupt
  MSR cpsr, r4
  mov pc, lr	

int_off: // may pass parameter in r0
  MRS r4, cpsr
  ORR r4, r4, #0x80   // set bit means MASK off IRQ interrupt 
  MSR cpsr, r4
  mov pc, lr	
	
.end
