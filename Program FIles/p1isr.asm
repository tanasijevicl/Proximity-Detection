;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------

            .cdecls C,LIST,"msp430.h"       ; Include device header file

;-------------------------------------------------------------------------------

;-------------------------------------------------------------------------------
			.text
PORT1ISR	add.w	&P1IV, PC				; set PC to go to wanted label
			reti							; vector 0 - no interrupt
p0:			reti
p1:			reti
p2:			reti
p3:			reti
p4:			jmp p4isr						; vector 0Ah, interrupt source port 1.4
p5:			reti
p6:			reti
p7:			reti

p4isr:		bis.w	#BIT4, &TA0CTL			; start timer in up mode
			;bic.b	#BIT4, &P1IFG			; clear P1.4 flag (not needed due to fast interrupt decoding)
			bic.b	#BIT4, &P1IE			; disable P1.4 interrupt
			reti

;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".int47"                ; MSP430 PORT1 Vector
            .short  PORT1ISR
