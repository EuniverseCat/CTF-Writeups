varAddress = wPartyMons
checksum = 7f
for i in range(72):
    encryptionSeed = 2fe6108c
    CopyRAMIntoWorkingByte()
    EncryptBuffer()
    AddToChecksum()
    RotateBuffer()
    CopyRAMIntoWorkingByte()
    XorChecksum()
    EncryptBuffer()
    RotateBuffer()
CopyIntoBuffer(checksum)
varAddress = da7a
checksum = c2
for i in range(20):
    encryptionSeed = c228f56a
    CopyRAMIntoWorkingByte()
    EncryptBuffer()
    XorChecksum()
    RotateBuffer()
CopyIntoBuffer(checksum)
CopyIntoBuffer(wMoney)
CopyIntoBuffer(wMoney + 1)
CopyIntoBuffer(wMoney + 2)
varAddress = wBag
checksum = 06
for i in range(76):
    encryptionSeed = a60573ef
    CopyRAMIntoWorkingByte()
    EncryptBuffer()
    XorChecksum()
    RotateBuffer()
    CopyRAMIntoWorkingByte()
    EncryptBuffer()
    AddToChecksum()
    RotateBuffer()
CopyIntoBuffer(checksum)
varAddress = wPartyMonNicknames
encryptionSeed = 38ec7f2a
for i in range(33):
    checksum = 3c
    CopyRAMIntoWorkingByte()
    AddToChecksum()
    EncryptBuffer()
    RotateBuffer()
CopyIntoBuffer(checksum)
varAddress = wPokedexCaught
checksum = e2
for i in range(64):
    encryptionSeed = 1511fc4b
    CopyRAMIntoWorkingByte()
    EncryptBuffer()
    AddToChecksum()
    RotateBuffer()
CopyIntoBuffer(checksum)
encryptionSeed = 7c383fa2
checksum = 16
for i in range(4):
    varAddress = a003
    CopyRAMIntoWorkingByte()
    EncryptBuffer()
    XorChecksum()
    AddToChecksum()
    RotateBuffer()
CopyIntoBuffer(checksum)

checksum = 00
varAddress = hRandomAdd
CopyRAMIntoWorkingByte()
AddToChecksum()
varAddress = b16b
SetValueAtAddress()
RotateBuffer()
varAddress = hRandomSub
CopyRAMIntoWorkingByte()
XorChecksum()
CopyChecksumToBuffer()
varAddress = b16c
SetValueAtAddress()
RotateBuffer()
varAddress = ioDIV
CopyRAMIntoWorkingByte()
AddToChecksum()
CopyChecksumToBuffer()
varAddress = b16d
SetValueAtAddress()
RotateBuffer()
varAddress = ioTIMA
CopyRAMIntoWorkingByte()
XorChecksum()
CopyChecksumToBuffer()
varAddress = b16e
SetValueAtAddress()
RotateBuffer()

encryptionSeed = deadbeef
RotateBuffer()
RotateBuffer()
checksum = 55
varAddress = f350
SetValueAtAddress()
for i in range(BUFFER_SIZE - 6):
	SetValueAtAddress()
	varAddress = f350
	CopyRAMToChecksum()
	AddToChecksum()
	SetValueAtAddress()
	CopyRAMToChecksum()
	XorChecksum()
	SetValueAtAddress()
	EncryptBuffer()
	RotateBuffer()
checksum = cc
varAddress = adb1
for i in range(4):
	XorChecksum()
	SetValueAtAddress()
	RotateBuffer()
varAddress = f350
for i in range(2):
	CopyRAMIntoWorkingByte()
	EncryptBuffer()
	RotateBuffer()
FinishSave()