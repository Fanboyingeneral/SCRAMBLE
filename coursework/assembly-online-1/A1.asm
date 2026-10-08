.MODEL SMALL
.STACK 100H

.DATA
ARRAY DW 50,40,30,20,20 
ASC DB 'ASCENDING$'
DESC DB 'DESCENDING$'
NONE DB 'NONE$'


.CODE


MAIN PROC
    MOV AX, @DATA
    MOV DS,AX
    
    MOV AX,0;I
    MOV BX,0;J
       
    
    
    LOOP1:
        CMP AX,8
        JGE ASCENDING
         
        XOR BX,BX
        ADD BX,AX
        ADD BX,2 ;J=I+2
        
       LOOP2:
       
        CMP BX,10
        JGE EXITLOOP2
        
        
        MOV SI,AX 
        MOV CX, ARRAY[SI]
        
        MOV SI,BX
        MOV DX, ARRAY[SI]
        
        CMP CX,DX
        JG EXIT
        
        ADD BX,2
        
        JMP LOOP2
        
       EXITLOOP2:
       
       ADD AX,2
       JMP LOOP1      
       
EXIT:   
    
    
         
             MOV AX,0;I
    MOV BX,0;J
    
    
    LOOP12:
        CMP AX,8
        JGE DESCENDING
         
        ADD BX,AX
        ADD BX,2 ;J=I+2
        
       LOOP22:
       
        CMP BX,10
        JGE EXITLOOP22
        
        
        MOV SI,AX 
        MOV CX, ARRAY[SI]
        
        MOV SI,BX
        MOV DX, ARRAY[SI]
        
        CMP CX,DX
        JL EXIT2
        
         ADD BX,2
        JMP LOOP22
        
       EXITLOOP22:
       
        ADD AX,2
       JMP LOOP12      
       
EXIT2:   
  MOV AH,09H
  LEA DX,NONE
  INT 21H
  JMP DONE  
       
ASCENDING:
      MOV AH,09H
  LEA DX,ASC
  INT 21H
  JMP DONE   
  DESCENDING:
      MOV AH,09H
  LEA DX,DESC
  INT 21H
  JMP DONE 
       
       
         
DONE:         
   
MOV AH,4CH
INT 21H
MAIN ENDP







END MAIN