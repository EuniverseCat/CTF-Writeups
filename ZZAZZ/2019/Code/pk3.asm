copyDword:
  03:A084                  ld   a,(de)
  03:A085                  ldi  (hl),a
  03:A086                  inc  de
  03:A087                  ld   a,(de)
  03:A088                  ldi  (hl),a
  03:A089                  inc  de
  03:A08A                  ld   a,(de)
  03:A08B                  ldi  (hl),a
  03:A08C                  inc  de
  03:A08D                  ld   a,(de)
  03:A08E                  ld   (hl),a
  03:A08F                  ret

addDword:
  03:A090                  ld   a,(de)
  03:A091                  add  (hl)
  03:A092                  ldi  (hl),a
  03:A093                  inc  de
  03:A094                  ld   a,(de)
  03:A095                  adc  (hl)
  03:A096                  ldi  (hl),a
  03:A097                  inc  de
  03:A098                  ld   a,(de)
  03:A099                  adc  (hl)
  03:A09A                  ldi  (hl),a
  03:A09B                  inc  de
  03:A09C                  ld   a,(de)
  03:A09D                  adc  (hl)
  03:A09E                  ld   (hl),a
  03:A09F                  ret
  
varA0A0:
  03:A0A0                  rst  38
  03:A0A1                  rst  38
  03:A0A2                  rst  38
  03:A0A3                  rst  38
  
doesSumToZero:
  03:A0A4                  push hl
  03:A0A5                  ld   de,varA0A0
  03:A0A8                  call addDword
  03:A0AB                  pop  hl
  03:A0AC                  ldi  a,(hl)
  03:A0AD                  and  a
  03:A0AE                  ret  nz
  03:A0AF                  ldi  a,(hl)
  03:A0B0                  and  a
  03:A0B1                  ret  nz
  03:A0B2                  ldi  a,(hl)
  03:A0B3                  and  a
  03:A0B4                  ret  nz
  03:A0B5                  ldi  a,(hl)
  03:A0B6                  and  a
  03:A0B7                  ret  

varA0B8:
  03:A0B8                  nop  
  03:A0B9                  nop  
  03:A0BA                  nop  
  03:A0BB                  nop  
  03:A0BC                  nop  
  03:A0BD                  nop  
  03:A0BE                  nop  
  03:A0BF                  nop  
  03:A0C0                  nop  
  03:A0C1                  nop  
  03:A0C2                  nop  
  03:A0C3                  nop 
  
multiplyDword:  
  03:A0C4                  push hl
  03:A0C5                  push hl
  03:A0C6                  push de
  03:A0C7                  ld   hl,varA0B8
  03:A0CA                  call copyDword
  03:A0CD                  pop  de
  03:A0CE                  pop  hl
  03:A0CF                  push hl
  03:A0D0                  ld   hl,varA0B8 + 4
  03:A0D3                  ld   de,varA0B8 + 8
  03:A0D6                  call copyDword
  03:A0D9                  pop  de
.loop
  03:A0DA                  push de
  03:A0DB                  ld   hl,varA0B8 + 4
  03:A0DE                  call addDword
  03:A0E1                  pop  de
  03:A0E2                  push de
  03:A0E3                  ld   hl,varA0B8
  03:A0E6                  call doesSumToZero
  03:A0E9                  pop  de
  03:A0EA                  jr   nz,.loop
  03:A0EC                  pop  hl
  03:A0ED                  ld   de,varA0B8 + 4
  03:A0F0             tail call copyDword

varA0F3:
  03:A0F3                  ld   de,0B1C
  03:A0F6                  ld   e,l
  03:A0F7                  dec  h
  03:A0F8                  sub  c
  03:A0F9                  rst  20
  03:A0FA                  dec  (hl)
  03:A0FB                  <corrupted stop>
  03:A0FD                  ld   e,c
  03:A0FE                  ld   d,(hl)
  03:A0FF                  ld   a,(bc)
  03:A100                  ei   
  03:A101                  rst  38
  03:A102                  ld   a,a
varA103:
  03:A103                  inc  sp
  03:A104                  rlca 
  03:A105                  ld   (001B),
varA107:
  03:A108                  nop  
  03:A109                  nop  
  03:A10A                  nop 
varA10B:  
  03:A10B                  nop  
  03:A10C                  nop  
  03:A10D                  nop  
  03:A10E                  nop
 
funcA10F:
  03:A10F                  ld   hl,varA10B
  03:A112                  ldi  a,(hl)
  03:A113                  ld   h,(hl)
  03:A114                  ld   l,a
  03:A115                  and  a
  03:A116                  srl  h
  03:A118                  rr   l
  03:A11A                  ld   d,h
  03:A11B                  ld   e,l
  03:A11C                  ld   hl,0
  03:A11F                  ld   a,(varA10B + 2)
  03:A122                  ld   c,a
  03:A123                  and  a
  03:A124                  jr   z,.skiploop
.loop
  03:A126                  add  hl,de
  03:A127                  dec  c
  03:A128                  jr   nz,.loop
.skiploop
  03:A12A                  ld   a,(varA10B + 3)
  03:A12D                  ld   c,a
  03:A12E                  ld   b,00
  03:A130                  add  hl,bc
  03:A131                  ld   a,h
  03:A132                  ld   (varA10B + 1),a
  03:A135                  ld   a,l
  03:A136                  ld   (varA10B),a
  03:A139                  ret
  
mainDecryptionFunction:
  03:A13A                  ld   hl,varA107
  03:A13D                  ld   de,varA103
  03:A140                  call copyDword
  03:A143                  ld   hl,varA0F3 + 1
  03:A146                  ld   de,varA10B
  03:A149                  ldi  a,(hl)
  03:A14A                  ld   (de),a
  03:A14B                  inc  de
  03:A14C                  xor  a
  03:A14D                  ld   (de),a
  03:A14E                  inc  de
  03:A14F                  ldi  a,(hl)
  03:A150                  ld   (de),a
  03:A151                  inc  de
  03:A152                  ld   a,(hl)
  03:A153                  ld   (de),a
  03:A154                  ld   hl,varA567
  03:A157                  ld   bc,NULL
.loop
  03:A15A                  push hl
  03:A15B                  push bc
  03:A15C                  call funcA10F
  03:A15F                  pop  bc
  03:A160                  pop  hl
  03:A161                  xor  (hl)
  03:A162                  ldi  (hl),a
  03:A163                  inc  bc
  03:A164                  ld   a,b
  03:A165                  cp   a,02
  03:A167                  jr   nz,.else
  03:A169                  ld   bc,NULL
  03:A16C                  ld   hl,varA567
.else
  03:A16F                  push hl
  03:A170                  ld   hl,A107
  03:A173                  call doesSumToZero
  03:A176                  pop  hl
  03:A177                  jr   nz,.loop
  03:A179                  ld   hl,varA0F3
  03:A17C                  ld   de,varA0F3 + 4
  03:A17F                  call multiplyDword
  03:A182                  ld   hl,varA0F3
  03:A185                  ld   de,varA0F3 + 8
  03:A188                  call addDword
  03:A18B                  ld   hl,varA0F3 + 12
  03:A18E                  call doesSumToZero
  03:A191             tail call nz,mainDecryptionFunction
  03:A193                  ret
