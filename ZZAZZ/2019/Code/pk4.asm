  02:AE11:	call funcAEDB
  02:AE14:	ld sp,ropFuncAE18
  02:AE17:	ret

funcAEDB: ;appears to set up some registers and wait for hblank
  02:AEDB                  ld   hl,9A12
  02:AEDE                  ld   b,C0
  02:AEE0                  ld   a,(ADB9)
  02:AEE3                  inc  a
  02:AEE4                  and  a,03
  02:AEE6                  ld   (ADB9),a
  02:AEE9                  add  b
  02:AEEA                  ld   b,a
.loop
  02:AEEB                  ld   a,(ff00+41) ; lcd stat
  02:AEED                  and  a,03
  02:AEEF                  jr   nz,.loop
  02:AEF1                  ld   (hl),b
  02:AEF2                  ret

ropFuncAE18: 
  1898	ld	a,(de)
  106A	inc	de
  3E3D	add a
  08F1	pop bc		;0000
  0D8E	ld	c,a
  0831	pop	hl		;AE2C, this basically serves as the start of a jump table
					;sp is set to the value at arrAE2C[(de)]
  1D17	add hl,bc
  1C85	ldi a,(hl)
  1C86	ld	h,(hl)
  1C87  ld	l,a
  1708	ld	sp,hl
  1709	ret
  
byte01_ropFuncAEF3: ;01 d16
  0831	pop	hl		;ADAE
  1898	ld	a,(de)
  2250	ldi (hl),a
  106A	inc	de
  1898	ld	a,(de)
  2250	ldi (hl),a
  106A	inc	de
  0831	pop	hl		;AE18
  1708	ld	sp,hl
  1709	ret
  
byte02_ropFuncAF09: ;02 d8
  0831	pop	hl		;ADB0
  1898	ld	a,(de)
  106A	inc	de
  2250	ldi (hl),a
  0831	pop	hl		;AE18
  1708	ld	sp,hl
  1709	ret
  
byte03_ropFuncAF19: ;03
  0831	pop	hl		;ADAE
  1C85	ldi a,(hl)
  1C86	ld	h,(hl)
  1C87  ld	l,a
  ADBA	ldi	a,(hl)
  4404	ld	b,h
  18DC	ld	c,l
  0831	pop	hl		;ADAE
  ADBE	ld	(hl),c
  1447	inc	hl
  1FEE	ld	(hl),b
  0831	pop	hl		;C800
  2250	ldi (hl),a
  0831	pop	hl		;AE18
  1708	ld	sp,hl
  1709	ret
  
byte04_ropFuncAE4A:
  09A2	pop bc		;0100
  09A3	pop de		;CB00
  09A4	pop hl		;AE58
  09A5	pop af		;FF00
  3026	CopyBytes
  CB00	ld  sp,C100
  CB03	call funcD92E
  D92E	ld	a,03
  D930	jr	D922
  D922	ld	(DF35),a
  D925	jr	(4000),a
  D928	ld	a,0A
  D92A	ld (0000), a
  D92D	ret
  CB06	ld	bc,01B0
  CB09	ld	de,A100
  CB0C	ld	hl,C800
  CB0F	call CopyBytes
  CB12	call D920
  ;can analyze this more if necessary
  
byte05_ropFuncAF3B: ;05
  0831	pop	hl		;ADB1
  65E1	inc (hl)
  ADC0	ld  b,(hl)
  ADBA	ldi	a,(hl)
  ADBA	ldi	a,(hl)
  ADBA	ldi	a,(hl)
  ADBC	ldd	a,(hl)
  0933	xor b
  1489	ld	b,a
  ADBC	ldd	a,(hl)
  ADBC	ldd	a,(hl)
  1447	inc	hl
  0933	xor b
  2250	ldi (hl),a
  1489	ld	b,a
  ADBC	ldd	a,(hl)
  1447	inc	hl
  ADC2	add b
  2250	ldi (hl),a
  ADC4	srl a
  1489	ld	b,a
  ADBC	ldd	a,(hl)
  ADC2	add b
  1489	ld	b,a
  ADBC	ldd	a,(hl)
  ADBA	ldi	a,(hl)
  0933	xor b
  1447	inc	hl
  2250	ldi (hl),a
  0831	pop	hl		;C800
  ADC7	xor (hl)
  2250	ldi (hl),a
  0831	pop	hl		;AE18
  1708	ld	sp,hl
  1709	ret
  
byte06_ropFuncAF85:
  0831	pop	hl		;ADB8
  ADBC	ldd	a,(hl)
  37C2	inc a
  0831	pop	hl		;07FC
  2DE0	or	(hl)
  08F1	pop bc		;C000
  0933	xor b
  0831	pop	hl		;ADB8
  2250	ldi (hl),a
  0394	and a
  ADC9	ret z		;to AEDB. graphics? rest of the code runs regardless
  ADCA	pop hl
  0831	pop	hl		;ADB6
  18D0	ld	a,e
  2250	ldi (hl),a
  3351	ld	(hl),d
  0831	pop	hl		;C800
  ADBA	ldi	a,(hl)
  0831	pop	hl		;ADB5
  2250	ldi (hl),a
  09A2	pop bc		;01B0
  09A3	pop de		;C800
  09A4	pop hl		;C801
  09A5	pop af		;FF00
  3026	CopyBytes
  0831	pop	hl		;ADB5
  ADBA	ldi	a,(hl)
  0831	pop	hl		;C9AF 
  2250	ldi (hl),a
  0831	pop	hl		;ADB6
  ADBA	ldi	a,(hl)
  5E0C	ld	d,(hl)
  1FA5	ld	e,a
  0831	pop	hl		;AE18
  1708	ld	sp,hl
  1709	ret
  
byte07_ropFuncAFE3: ;07 d32
  0831	pop	hl		;ADB1
  1898	ld	a,(de)
  2250	ldi (hl),a
  106A	inc	de
  1898	ld	a,(de)
  2250	ldi (hl),a
  106A	inc	de
  1898	ld	a,(de)
  2250	ldi (hl),a
  106A	inc	de
  1898	ld	a,(de)
  2250	ldi (hl),a
  106A	inc	de
  1898	ld	a,(de)
  0831	pop	hl		;AE18
  1708	ld	sp,hl
  1709	ret
  
byte08_ropFuncB005:
  09A4	pop hl		;ADCC
  09A5	pop af		;8600
  2250	ldi (hl),a
  0831	pop	hl		;B01B
  1708	ld	sp,hl
  1709	ret
  0831	pop	hl		;ADB0
  ADBA	ldi	a,(hl)
  0831	pop	hl		;C800
  ADCC	add (hl)
  0831	pop	hl		;ADB0
  2250	ldi (hl),a
  0831	pop	hl		;AE18
  1708	ld	sp,hl
  1709	ret
  
byte09_ropFuncB013:
  09A4	pop hl		;ADCC
  09A5	pop af		;AE00
  2250	ldi (hl),a
  0831	pop	hl		;ADB0
  ADBA	ldi	a,(hl)
  0831	pop	hl		;C800
  ADCC	xor (hl)
  0831	pop	hl		;ADB0
  2250	ldi (hl),a
  0831	pop	hl		;AE18
  1708	ld	sp,hl
  1709	ret
  
  

byte0B_ropFuncB033:
  0831	pop	hl		;ADB0
  ADBA	ldi	a,(hl)
  0831	pop	hl		;C800
  2250	ldi (hl),a
  0831	pop	hl		;AE18
  1708	ld	sp,hl
  1709	ret
  

byte0C_ropFuncB07D: ;0C a16 d8
  1898	ld	a,(de)
  0D8E	ld	c,a
  106A	inc de
  1898	ld	a,(de)
  1489	ld	b,a
  106A	inc de
  1898	ld	a,(de)
  106A	inc de
  0831	pop	hl		;AE18
  0394	and a
  ADC9	ret z		;real return 
  ADCA	pop hl	
  2EC9	dec a
  1380	dec de
  4128	ld  (de),a
  106A	inc de
  0831	pop	hl		;AE18
  ADC9  ret z		;real return	
  ADCA	pop hl
  4D6A	ld  d,b
  2CAD	ld  a,c
  1FA5	ld	e,a
  0831	pop	hl		;AE18
  1708	ld	sp,hl
  1709	ret
  
byte0D_ropFuncB045:
  0831	pop	hl		;ADB0
  ADC0	ld	b,(hl)
  0831	pop	hl		;ADAE
  1C85	ldi a,(hl)
  1C86	ld	h,(hl)
  1C87  ld	l,a
  1FEE	ld	(hl),b
  1447	inc	hl
  4404	ld	b,h
  18DC	ld	c,l
  0831	pop	hl		;ADAE
  ADBE	ld	(hl),c
  1447	inc	hl
  1FEE	ld	(hl),b
  0831	pop	hl		;AE18
  1708	ld	sp,hl
  1709	ret
  
  
byte0E_ropFuncB069:
  0831	pop	hl		;ADAE
  1C85	ldi a,(hl)
  1C86	ld	h,(hl)
  1C87  ld	l,a
  ADBA	ldi	a,(hl)
  0831	pop	hl		;ADB0
  2250	ldi (hl),a
  0831	pop	hl		;AE18
  1708	ld	sp,hl
  1709	ret
  
exitCon_AEDB:
  02:AEDB                  ld   hl,9A12
  02:AEDE                  ld   b,C0
  02:AEE0                  ld   a,(ADB9)
  02:AEE3                  inc  a
  02:AEE4                  and  a,03
  02:AEE6                  ld   (ADB9),a
  02:AEE9                  add  b
  02:AEEA                  ld   b,a
  02:AEEB                  ld   a,(ff00+41)
  02:AEED                  and  a,03
  02:AEEF                  jr   nz,AEEB
  02:AEF1                  ld   (hl),b
  02:AEF2                  ret  
  

01DFFC027F072FE6108C03050806030905060CBDB0480B06017AFA02C207C228F56A030509060CD5B0140B06014EF803060CE2B0030159F8020607A60573EF03050906030508060CF2B04C0B060141DE0738EC7F2A023C030805060C0AB1210B060199DE02E2071511FC4B030508060C1EB1400B06077C383FA202160103A003050908060C32B1040B06020001E1FF0308016BB10D0601E2FF03090B016CB10D060104FF03080B016DB10D060105FF03090B016EB10D0607DEADBEEF060602550150F30D0D0150F30E080D0E090D05060C78B1D50C78B1D602CC01B1AD090D06090D06090D06090D060150F303050603050604
  
  
  
  
  