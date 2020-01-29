; Print.s
; Student names: Tyler Anson, Ethan Shen
; Last modification date: 5/5/19
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11

divisor  RN 1
rem RN 5
pRes EQU 0						;binding

LCD_OutDec
	PUSH {R0, R1, R5, LR}
	MOV R1, #10
	CMP R0, R1
	BLO base
	UDIV R0, R1
	BL LCD_OutDec
	LDR R0, [SP, #pRes]
	
	UDIV R2, R0, divisor		;gets n/10 (basically does a right shift in decimal form)
	MUL rem, R2, divisor
	SUB rem, R0, rem
	MOV R0, rem
	ADD R0, #0x30
	BL ST7735_OutChar			;output each additional digit
	B DoneDec
	
base
	ADD R0, #0x30
	BL ST7735_OutChar			;output first digit [MS #]
	
DoneDec
	POP {R0, R1, R5, PC}

;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11


Thousand EQU 0					;binding
Hundred EQU 2
Ten EQU 3

LCD_OutFix

	PUSH {R1, R2, R3, R6, R7, LR}
	CMP R0, #0					;check if value is 0
	BNE PASS
	MOV R1, R0
	MOV R0, #0x30				
	BL ST7735_OutChar			;circumvent recursion
	MOV R0, #0x2E
	BL ST7735_OutChar
	MOV R0, #0x30
	BL ST7735_OutChar
	MOV R0, #0x30
	BL ST7735_OutChar
	MOV R0, #0x30
	BL ST7735_OutChar
	POP {R1, R2, R3, R6, R7, PC}

PASS							;check if value greater than 9999
	MOV R3, #10000
	CMP R0, R3
	BLO inRange
	MOV R1, R0
	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2E
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	POP {R1, R2, R3, R6, R7, PC}
	
inRange		
	MOV R2, #1000				;allocation
	STRH R2, [SP, #Thousand]
	MOV R2, #100
	STRB R2, [SP, #Hundred]
	MOV R2, #10
	STRB R2, [SP, #Ten]
	
	MOV R7, R0
	
	LDRH R6, [SP, #Thousand]
	UDIV R0, R7, R6
	ADD R0, #0x30
	BL ST7735_OutChar			;output first #
	
	MOV R0, #0x2E
	BL ST7735_OutChar			;output '.'
	
	UDIV R0, R7, R6				
	MUL R0, R6
	SUB R7, R0					;get %1000 of #
	
	LDRB R6, [SP, #Hundred]
	UDIV R0, R7, R6
	ADD R0, #0x30
	BL ST7735_OutChar			;output second #
	
	UDIV R0, R7, R6				
	MUL R0, R6
	SUB R7, R0					;get %100 of #
	
	LDRB R6, [SP, #Ten]
	UDIV R0, R7, R6
	ADD R0, #0x30
	BL ST7735_OutChar			;output third #
	
	UDIV R0, R7, R6				
	MUL R0, R6
	SUB R0, R7, R0
	ADD R0, #0x30
	BL ST7735_OutChar			;output fourth #
	
	POP {R1, R2, R3, R6, R7, PC}
    BX   LR
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
