/********************************************************************
Copyright 2010-2017 K.C. Wang
********************************************************************/

.text
.code 32

.global reset
.global vectors_start, vectors_end
.global lock, unlock, IRQ_handler

reset:
reset_handler:
  /* set SVC stack */
  LDR sp, =svc_stack
  /* copy vector table to address 0 */
  BL copy_vectors
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
	sub lr,lr, #4
	stmfd sp!, {r0-r12, lr}
	bl IRQ_handler
	ldmfd sp!, {r0-r3, r12, pc}^

lock:
	MRS r0, cpsr
	ORR r0, r0, #0x80
	MSR cpsr, r0
	mov pc,lr

unlock:
	MRS r0, cpsr
	BIC r0, r0, #0x80
	MSR cpsr, r0
	mov pc, lr



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

.end
