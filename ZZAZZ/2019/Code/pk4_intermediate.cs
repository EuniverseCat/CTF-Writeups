int ip = 0;
const short IP_OFFSET = 0xBOB3;

  
delegate[] jumpTable = new delegate[] { 
	null, 
	byte01_CopyIntovarADAE,
	byte02_CopyIntovarADB0,
	byte03_ropFuncAF19,
	byte04_FinishSave,
	byte05_someEncryptionThing,
	byte06_ropFuncAF85,
	byte07_ropFuncAFE3,
	byte08_AddvarC800,
	byte09_XorvarC800,
	null,
	byte0B_SetvarC800,
	byte0C_ConditionalJump,
	byte0D_ropFuncB045,
	byte0E_ropFuncB069
};
short varADAE;
byte varADB0;
int varADB1;

byte varC800;
const int SAVE_SIZE = 0x1B0;
byte[] saveData = new byte[SAVE_SIZE];

void MainLoop() {
	while (true) {
		jumpTable[bytecode[ip++]].Invoke();
	}
}

void byte01_CopyIntovarADAE() {
	//check endianness
	varADAE = bytecode[ip] << 8 + bytecode[ip + 1];
	ip += 2;
}
  
void byte02_CopyIntovarADB0() {
	varADB0 = bytecode[ip];
	ip++;
}
	
void byte03_ropFuncAF19() {
  1C85	ldi lh,varADAE
  ADBA	ldi	a,(hl)
  4404	ld	bc,hl
  ADBE	ld  (varADAE),cb
  2250	ld	(varC800),a
}
  
void byte04_FinishSave() {
	CopyBytes(in AE58, out CB00, 100)
	CopyBytes(in C800, out A100, SAVE_SIZE)
  //can analyze this more if necessary
}
  
void byte05_someEncryptionThing() {
	byte b1 = byte(varADB1);
	byte b2 = byte(varADB1 >> 8);
	byte b3 = byte(varADB1 >> 16);
	byte b4 = byte(varADB1 >> 24);
	
	byte working;
	
	b1++;
	working = b4 ^ b1
	b2 ^= working
	working = b2
	b3 += working
	working = b3 >> 1
	working += b4
	b4 = b2 ^ working
	varC800 ^= b4
	
	varADB1 = b1 + b2 << 8 + b3 << 16 + b4 << 24;
}
  
byte06_ropFuncAF85:
  ADBC	ldd	a,(ADB8)
  0831	a |= 07FC
  08F1	a ^= C000
  2250	ldi (ADB8),a
  if (a == 0)
	funcAEDB();	//graphics? rest of the code runs regardless
  0831	(ADB6) = ed
  2250	ld (ADB5),varC800
  3026	CopyBytes(in C801, out C800, SAVE_SIZE)
  2250	ldi (C9AF),(ADB5)
  ADBA	ld	ed,(ADB6)
  1709	ret
  
void byte07_ropFuncAFE3() {
	//some array.copy bs - 4 bytes at ip to start of save
}
  
void byte08_AddvarC800() {
	varADB0 += varC800;
}
  
void byte09_XorvarC800() {
	varADB0 ^= varC800;
}
 

void byte0B_SetvarC800() {
	varC800 = varADB0;
}
  

void byte0C_ConditionalJump() {
	short jumpIndex = bytecode[ip] << 8 + bytecode[ip + 1] - IP_OFFSET;
	ip += 2;
	byte* iterPtr = $bytecode[ip];
	ip++;
	if (*iterPtr == 0)
		return;
	*iterPtr--;
	if (*iterPtr == 0)
		return;
	ip = jumpIndex;
}
  
void byte0D_ropFuncB045() { //step of encryption while preserving checksum?
	b = varADB0
	ldi lh,varADAE
	ldi	(hl),b
	varADAE = lh
	return;
}
  
  
void byte0E_ropFuncB069() {
	ldi lh,varADAE
	ldi	a,(hl)
	varADB0 = a
	return
}
  
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
  

byte[] bytecode = new byte[] {0x01, 0xDF, 0xFC, 0x02, 0x7F, 0x07, 0x2F, 0xE6, 0x10, 0x8C, 0x03, 0x05, 0x08, 0x06, 0x03, 0x09, 0x05, 0x06, 0x0C, 0xBD, 0xB0, 0x48, 0x0B, 0x06, 0x01, 0x7A, 0xFA, 0x02, 0xC2, 0x07, 0xC2, 0x28, 0xF5, 0x6A, 0x03, 0x05, 0x09, 0x06, 0x0C, 0xD5, 0xB0, 0x14, 0x0B, 0x06, 0x01, 0x4E, 0xF8, 0x03, 0x06, 0x0C, 0xE2, 0xB0, 0x03, 0x01, 0x59, 0xF8, 0x02, 0x06, 0x07, 0xA6, 0x05, 0x73, 0xEF, 0x03, 0x05, 0x09, 0x06, 0x03, 0x05, 0x08, 0x06, 0x0C, 0xF2, 0xB0, 0x4C, 0x0B, 0x06, 0x01, 0x41, 0xB0, 0xB3, 0x07, 0x38, 0xEC, 0x7F, 0x2A, 0x02, 0x3C, 0x03, 0x08, 0x05, 0x06, 0x0C, 0x0A, 0xB1, 0x21, 0x0B, 0x06, 0x01, 0x99, 0xB0, 0xB3, 0x02, 0xE2, 0x07, 0x15, 0x11, 0xFC, 0x4B, 0x03, 0x05, 0x08, 0x06, 0x0C, 0x1E, 0xB1, 0x40, 0x0B, 0x06, 0x07, 0x7C, 0x38, 0x3F, 0xA2, 0x02, 0x16, 0x01, 0x03, 0xA0, 0x03, 0x05, 0x09, 0x08, 0x06, 0x0C, 0x32, 0xB1, 0x04, 0x0B, 0x06, 0x02, 0x00, 0x01, 0xE1, 0xFF, 0x03, 0x08, 0x01, 0x6B, 0xB1, 0x0D, 0x06, 0x01, 0xE2, 0xFF, 0x03, 0x09, 0x0B, 0x01, 0x6C, 0xB1, 0x0D, 0x06, 0x01, 0x04, 0xFF, 0x03, 0x08, 0x0B, 0x01, 0x6D, 0xB1, 0x0D, 0x06, 0x01, 0x05, 0xFF, 0x03, 0x09, 0x0B, 0x01, 0x6E, 0xB1, 0x0D, 0x06, 0x07, 0xB0, 0xB3, 0xAD, 0xBE, 0xEF, 0x06, 0x06, 0x02, 0x55, 0x01, 0x50, 0xF3, 0x0D, 0x0D, 0x01, 0x50, 0xF3, 0x0E, 0x08, 0x0D, 0x0E, 0x09, 0x0D, 0x05, 0x06, 0x0C, 0x78, 0xB1, 0xD5, 0x0C, 0x78, 0xB1, 0xD6, 0x02, 0xCC, 0x01, 0xB1, 0xAD, 0x09, 0x0D, 0x06, 0x09, 0x0D, 0x06, 0x09, 0x0D, 0x06, 0x09, 0x0D, 0x06, 0x01, 0x50, 0xF3, 0x03, 0x05, 0x06, 0x03, 0x05, 0x06, 0x04 };
  
  
  
  
  