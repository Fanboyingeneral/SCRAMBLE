.MODEL SMALL
.STACK 100H

.DATA

STRING DB 'eruiaaageruiaaag'   
V DB 'VOWEL COUNT: $'
C DB 'CONSONANT COUNT: $'
CR EQU 0DH
LF EQU 0AH 

NUMBER_STRING DB '00$' ;ADJUST THIS TO STRING LENGTH

.CODE


MAIN PROC
    
    MOV AX, @DATA
    MOV DS,AX
      
    MOV SI, 0   ; i 
    
    MOV BL,0 ;VOWEL
    MOV BH,0 ; CONSONANTS

LOOP1:    
    CMP SI,16
    JGE EXIT
    
    MOV CL, STRING[SI]
    
    CMP CL,61H
    JE VOWEL 
        CMP CL,65H
    JE VOWEL
        CMP CL,69H
    JE VOWEL
        CMP CL,6FH
    JE VOWEL
        CMP CL,75H
    JE VOWEL
    
    INC BH 
    INC SI
    JMP LOOP1
    
VOWEL:  
    INC SI
    INC BL
    JMP LOOP1
    
    



EXIT:
    MOV AH,09H
    LEA DX,V
    INT 21H  
    
    MOV AH,0
    MOV AL, BL
    CALL PRINT
    
    MOV AH,02H
    MOV DL,CR
    INT 21H
    
    MOV DL,LF
    INT 21H  
    
    MOV AH,09H
    LEA DX,C
    INT 21H  
    
    MOV AH,0
    MOV AL, BH
    CALL PRINT   
   
    MOV AH,4CH
    INT 21H
    
MAIN ENDP


PRINT PROC  ;LOAD THE NUMBER INTO AX THEN CALL
    
    LEA SI, NUMBER_STRING
    ADD SI, 2  ;ADJUST THIS TO LENGTH OF STRING
    
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





END MAIN