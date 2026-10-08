.MODEL SMALL
.STACK 100H

.DATA


CR EQU 0DH
LF EQU 0AH 

NUMBER_STRING DB '000$' ;ADJUST THIS TO STRING LENGTH

.CODE


MAIN PROC
    
    MOV AX, @DATA
    MOV DS,AX 
    

    
    

    
 
EXIT: MOV AH,4CH
      INT 21H
    
MAIN ENDP


PRINT PROC  ;LOAD THE NUMBER INTO AX THEN CALL
    
    LEA SI, NUMBER_STRING
    ADD SI, 3  ;ADJUST THIS TO LENGTH OF STRING
    
    PRINT_LOOP:
        DEC SI
        
        MOV DX, 0
        ; DX:AX = 0000:AX
        
        MOV CX, 10
        DIV CX
        
        ADD DL, '0'
        MOV [SI], DL
        
        CMP AX, 0
        JNE PRINT_LOOP
    
    MOV DX, SI
    MOV AH, 9
    INT 21H
    
    RET

PRINT ENDP

NEWLINE PROC
    MOV AH, 2
    MOV DL, CR
    INT 21H
    MOV DL, LF
    INT 21H
    RET
NEWLINE ENDP



END MAIN   



  MOV DX,0          ; initialize result to 0
    
TAKE_INPUT:
    MOV AH,1
    INT 21H           ; read character
    CMP AL, 13        ; check if Enter pressed
    JE EXIT
    SUB AL, '0'
    MOV CL,AL       ; convert ASCII to number

    MOV AX, DX        ; move current result to CX
    MOV BX, 10
    MUL BX            ; AX = DX * 10 (AX=DX, since DX will be 0 initially)
    ADD AL, CL        ; AX = AX + previous result
        

    MOV DX, AX        ; store new result back to DX
    JMP TAKE_INPUT
