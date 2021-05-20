using System;
using System.Collections.Generic;
using System.Text;

namespace fools {
	//*ness voice* PK FOUR!
	class PK4 {
		//most of this stuff is part of the interpreter and ended up being unused
		int ip = 0;
		const ushort IP_OFFSET = 0xB0B3;

		byte[] varA003 = new byte[] { 0x00, 0xAF, 0xA2, 0x03 };

		ushort varAddress;
		byte checksum;
		uint encryptionSeed;

		const int BUFFER_SIZE = 0x1B0;
		byte[] buffer = new byte[BUFFER_SIZE];

		byte varD350;

		byte[] bytecode = new byte[] { 0x01, 0xDF, 0xFC, 0x02, 0x7F, 0x07, 0x2F, 0xE6, 0x10, 0x8C, 0x03, 0x05, 0x08, 0x06, 0x03, 0x09, 0x05, 0x06, 0x0C, 0xBD, 0xB0, 0x48, 0x0B, 0x06, 0x01, 0x7A, 0xFA, 0x02, 0xC2, 0x07, 0xC2, 0x28, 0xF5, 0x6A, 0x03, 0x05, 0x09, 0x06, 0x0C, 0xD5, 0xB0, 0x14, 0x0B, 0x06, 0x01, 0x4E, 0xF8, 0x03, 0x06, 0x0C, 0xE2, 0xB0, 0x03, 0x01, 0x59, 0xF8, 0x02, 0x06, 0x07, 0xA6, 0x05, 0x73, 0xEF, 0x03, 0x05, 0x09, 0x06, 0x03, 0x05, 0x08, 0x06, 0x0C, 0xF2, 0xB0, 0x4C, 0x0B, 0x06, 0x01, 0x41, 0xDE, 0x07, 0x38, 0xEC, 0x7F, 0x2A, 0x02, 0x3C, 0x03, 0x08, 0x05, 0x06, 0x0C, 0x0A, 0xB1, 0x21, 0x0B, 0x06, 0x01, 0x99, 0xDE, 0x02, 0xE2, 0x07, 0x15, 0x11, 0xFC, 0x4B, 0x03, 0x05, 0x08, 0x06, 0x0C, 0x1E, 0xB1, 0x40, 0x0B, 0x06, 0x07, 0x7C, 0x38, 0x3F, 0xA2, 0x02, 0x16, 0x01, 0x03, 0xA0, 0x03, 0x05, 0x09, 0x08, 0x06, 0x0C, 0x32, 0xB1, 0x04, 0x0B, 0x06, 0x02, 0x00, 0x01, 0xE1, 0xFF, 0x03, 0x08, 0x01, 0x6B, 0xB1, 0x0D, 0x06, 0x01, 0xE2, 0xFF, 0x03, 0x09, 0x0B, 0x01, 0x6C, 0xB1, 0x0D, 0x06, 0x01, 0x04, 0xFF, 0x03, 0x08, 0x0B, 0x01, 0x6D, 0xB1, 0x0D, 0x06, 0x01, 0x05, 0xFF, 0x03, 0x09, 0x0B, 0x01, 0x6E, 0xB1, 0x0D, 0x06, 0x07, 0xDE, 0xAD, 0xBE, 0xEF, 0x06, 0x06, 0x02, 0x55, 0x01, 0x50, 0xF3, 0x0D, 0x0D, 0x01, 0x50, 0xF3, 0x0E, 0x08, 0x0D, 0x0E, 0x09, 0x0D, 0x05, 0x06, 0x0C, 0x78, 0xB1, 0xD5, 0x0C, 0x78, 0xB1, 0xD6, 0x02, 0xCC, 0x01, 0xB1, 0xAD, 0x09, 0x0D, 0x06, 0x09, 0x0D, 0x06, 0x09, 0x0D, 0x06, 0x09, 0x0D, 0x06, 0x01, 0x50, 0xF3, 0x03, 0x05, 0x06, 0x03, 0x05, 0x06, 0x04 };

		public static void _Main() {
			new PK4().MainLoop();
		}

		//The decompiler
		void MainLoop() {
			Action[] jumpTable = new Action[] {
				null,
				SetAddress,
				SetChecksum,
				CopyRAMIntoBuffer,
				FinishSave,
				EncryptBuffer,
				RotateBuffer,
				SetEncryptionSeed,
				AddToChecksum,
				XorChecksum,
				null,
				CopyChecksumToBuffer,
				ConditionalJump,
				SetValueAtAddress,
				CopyRAMToWorkingByte
			};

			List<(int, string)> decompiled = new List<(int, string)>();
			while (ip < bytecode.Length) {
				//jumpTable[bytecode[ip++]].Invoke();
				string opcode = jumpTable[bytecode[ip++]].Method.Name;
				switch (opcode) {
					case nameof(SetChecksum):
						opcode = nameof(checksum) + " = " + bytecode[ip++].ToString("x2");
						break;
					case nameof(SetAddress):
						opcode = nameof(varAddress) + " = ";
						opcode += bytecode[ip + 1].ToString("x2");
						opcode += bytecode[ip].ToString("x2");
						ip += 2;
						break;
					case nameof(ConditionalJump):
						int jumpIndex = bytecode[ip] + (bytecode[ip + 1] << 8) - IP_OFFSET;
						ip += 2;
						byte loopsLeft = bytecode[ip];
						ip++;
						opcode = "";
						
						bool found = false;
						for (int i = 0; i < decompiled.Count; i++) {
							if (found)
								decompiled[i] = (decompiled[i].Item1, "    " + decompiled[i].Item2);
							else if (decompiled[i].Item1 == jumpIndex) {
								decompiled.Insert(i, (-1, $"for i in range({loopsLeft}):"));
								found = true;
							}
						}
						
						break;
					case nameof(SetEncryptionSeed):
						opcode = nameof(encryptionSeed) + " = ";
						for (int i = 0; i < 4; i++) {
							opcode += bytecode[ip++].ToString("x2");
						}
						break;
					default:
						opcode += "()";
						break;
				}
				decompiled.Add((ip, opcode));
			}
			foreach (var tuple in decompiled) {
				if (tuple.Item2 != "")
					Console.WriteLine(tuple.Item2);
			}
		}

		//The interpreter. Didn't end up using it in the end, but it showed me
		//where values were being read/written, and served as a way to check my RE work
		byte GuessValue(ushort address) {
			//ECHO RAM
			if (address >= 0xE000 && address < 0xFE00)
				address -= 0x2000;
			//pokemon data.
			if (address >= 0xDCDF && address < 0xDD6F)
				return 0;
			//pokemon nickname data
			if (address >= 0xDE41 && address <= 0xDE61)
				return 0;
			//pokedex data
			if (address >= 0xDE99 && address <= 0xDED8)
				return 0;
			//item data.
			if (address >= 0xD843 && address <= 0xD8F0)
				return 0;
			//event flag data
			if (address >= 0xDA7A && address <= 0xDA8D)
				return 0;
			//Custom values
			if (address >= 0xA003 && address <= 0xA006)
				return varA003[address - 0xA003];
			if (address == 0xD350)
				return varD350;
			Console.WriteLine($"attempted read at address {address.ToString("x4")}");
			return 0;
		}

		void SetValue(ushort address, byte value) {
			if (address >= 0xE000 && address < 0xFE00)
				address -= 0x2000;
			if (address == 0xD350) {
				varD350 = value;
				return;
			}
			if (address == 0xADB1) {
				encryptionSeed &= 0xFFFFFF00;
				encryptionSeed += value;
				return;
			}

			Console.WriteLine($"attempted write at address {address.ToString("x4")}, value {value}");
		}

		//Checked
		void SetAddress() {
			varAddress = (ushort)(bytecode[ip] + (bytecode[ip + 1] << 8));
			ip += 2;
		}

		//Checked
		void SetChecksum() {
			checksum = bytecode[ip];
			ip++;
		}

		//Checked
		void CopyRAMIntoBuffer() {
			buffer[0] = GuessValue(varAddress);
			varAddress++;
		}

		void FinishSave() {
			//CopyBytes(in AE58, out CB00, 100);
			//CopyBytes(in C800, out A100, SAVE_SIZE);
			//can analyze this more if necessary
		}

		//Checked
		void EncryptBuffer() {
			byte b1 = (byte)(encryptionSeed);
			byte b2 = (byte)(encryptionSeed >> 8);
			byte b3 = (byte)(encryptionSeed >> 16);
			byte b4 = (byte)(encryptionSeed >> 24);

			byte working;

			b1++;
			working = (byte)(b4 ^ b1);
			b2 ^= working;
			working = b2;
			b3 += working;
			working = (byte)(b3 >> 1);
			working += b4;
			b4 = (byte)(b2 ^ working);
			buffer[0] ^= b4;

			encryptionSeed = (uint)(b1 + (b2 << 8) + (b3 << 16) + (b4 << 24));
		}

		void RotateBuffer() {
			byte firstByte = buffer[0];
			Array.Copy(buffer, 1, buffer, 0, BUFFER_SIZE - 1);
			buffer[BUFFER_SIZE - 1] = firstByte;
			return;
		}

		//Checked
		void SetEncryptionSeed() {
			encryptionSeed = BitConverter.ToUInt32(bytecode, ip);
			ip += 4;
		}

		//Checked
		void AddToChecksum() {
			checksum += buffer[0];
		}


		//Checked
		void XorChecksum() {
			checksum ^= buffer[0];
		}

		//Checked
		void CopyChecksumToBuffer() {
			buffer[0] = checksum;
		}

		//Checked
		void ConditionalJump() {
			int jumpIndex = bytecode[ip] + (bytecode[ip + 1] << 8) - IP_OFFSET;
			ip += 2;
			byte loopsLeft = bytecode[ip];
			ip++;
			if (loopsLeft == 0)
				return;
			loopsLeft--;
			bytecode[ip - 1] = loopsLeft;
			if (loopsLeft == 0)
				return;
			ip = jumpIndex;
		}

		void SetValueAtAddress() {
			SetValue(varAddress, checksum);
			return;
		}


		void CopyRAMToWorkingByte() {
			checksum = GuessValue(varAddress);
		}
	}
}
