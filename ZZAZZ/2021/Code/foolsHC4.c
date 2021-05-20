#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
//allow use of fopen and make vs shut up about null pointers bc it seems to warn about them even if it's impossible
#pragma warning (disable : 4996 6385 6387 6011)

#define biome (hChunkExitsAndBiome >> 4 & 3)
#define exits (hChunkExitsAndBiome & 0x0F)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#pragma region Variables

u8* MapExitConfigurations;
u8* workingBuffer;

u8 wRNGVarX;
u8 wRNGVarA;
u8 wRNGVarB;
u8 wRNGVarC;

//i only really learned c array syntax after i was basically done with this
u8* seed;
u8* corruptedTiles;

u8 hChunkStartXY;
u8 hChunkExitsAndBiome;

#pragma endregion

#pragma region Constants
const u8 ITEM_PATH = 0x08;
const u8 PATH_TILE = 0x0A;
const u8 GRASS_TILE = 0x0B;
const u8 FOREST_TILE = 0x0F;
const u8 PILLARS_TILE = 0x13;
const u8 TILED_TILE = 0x31;
const u8 ITEM_RIGHT = 0x32;
const u8 ITEM_BOTTOM = 0x33;
const u8 ITEM_TOP = 0x34;
const u8 PILLARS_RIGHT = 0x4D;
const u8 PILLARS_LEFT = 0x4E;
const u8 PILLARS_TOP = 0x51;
const u8 PILLARS_BOTTOM = 0x52;
const u8 BRICKS_TILE = 0x55;
const u8 ITEM_LEFT = 0x60;
const u8 TREES_TOP = 0x6C;
const u8 TREES_LEFT = 0x6D;
const u8 TREES_RIGHT = 0x6E;
const u8 TREES_BOTTOM = 0x6F;
const u8 FLOWERS_TILE = 0x74;
const u8 PATCH_TILE = 0x7A;
const u8 FLOWERS_SMALL = 0x7B;
const u8 CORRUPTION_C9 = 0xC9;
const u8 CORRUPTION_CA = 0xCA;
const u8 CORRUPTION_CB = 0xCB;
const u8 CORRUPTION_D9 = 0xD9;
const u8 CORRUPTION_DB = 0xDB;
const u8 CORRUPTION_EC = 0xEC;
#pragma endregion

#pragma region Util
void CopyToLargerArray(u8* from, u8* to, int fromX, int fromY, int stride, int offsetX, int offsetY) {
	int toIndex = offsetY * stride + offsetX;
	int fromIndex = 0;
	for (int i = 0; i < fromY; i++) {
		memcpy(to + toIndex, from + fromIndex, fromX);
		toIndex += stride;
		fromIndex += fromX;
	}
}

void PrintBuffer(u8* buffer, int w, int h) {
	u8* hl = buffer;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			printf("%02x ", *hl);
			hl++;
		}
		putchar('\n');
	}
	putchar('\n');
}

void Set(u32 _seed) {
	seed[3] = _seed & 0xFF;
	seed[2] = (_seed >> 8) & 0xFF;
	seed[1] = (_seed >> 16) & 0xFF;
	seed[0] = (_seed >> 24) & 0xFF;
}

void PrintFullMap(u8* fullMap) {
	u8* hl = fullMap;
	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			printf("%02x ", *hl);
			if (j % 8 == 7)
				printf("| ");
			hl++;
		}
		putchar('\n');
		if (i % 8 == 7) {
			for (int k = 0; k < 24 * 3 + 4; k++) {
				putchar('-');
			}
			putchar('\n');
		}
			
	}
	putchar('\n');
}

u8* ReadBytesFromFile(const char* path, size_t size) {
	//this was set to my working directory
	//i know jack shit about io and strings in c
	const char* lol = "";
	char* fullPath = malloc(strlen(path) + strlen(lol));
	strcpy(fullPath, lol);
	strcpy(fullPath + strlen(lol), path);
	FILE* f = fopen(fullPath, "rb");
	if (!f) {
		perror(path);
		exit(1);
	}
	u8* buffer = malloc(size);
	fread(buffer, sizeof(u8), size, f);
	fclose(f);
	return buffer;
}

#pragma endregion

#pragma region MapGenTools
u8 PRNG() {
	//thanks ghidra
	wRNGVarA = wRNGVarA ^ wRNGVarC ^ wRNGVarX + 1;
	wRNGVarB += wRNGVarA;
	wRNGVarX++;
	wRNGVarC += (wRNGVarB >> 1 ^ wRNGVarA);
	return wRNGVarC;
}

void PreparePRNGState(u16 x, u16 y) {
	// seed the RNG with the chunk seed.
	wRNGVarX = seed[0] ^ (x >> 8 & 0xFF);
	wRNGVarA = seed[1] ^ (x & 0xFF);
	wRNGVarB = seed[2] ^ (y >> 8 & 0xFF);
	wRNGVarC = seed[3] ^ (y & 0xFF);
	for (int i = 0; i < 16; i++) {
		PRNG();
	}
	return;
}

void GetChunkExitsAndBiome(u16 x, u16 y) {
	// Calculate the biome and exit locations for a given chunk.
	// seed RNG with chunk coordinates divided by 4 (so each 4x4 chunk area
	// gets its own biome and layout
	PreparePRNGState(x & 0xfffc, y & 0xfffc);
	u8 a = PRNG() & 7;
	int mapExitOffset = (a << 4) + (y & 3) * 4 + (x & 3);
	u8 c = MapExitConfigurations[mapExitOffset] & 0x0f;
	c |= PRNG() & 0x30;
	hChunkExitsAndBiome = c;
	/*
	if (x < 4 && y < 4) {
		printf("checking biome in initial area");
		exit(1);
	}
	*/
}

void MapGen_WriteTileAt(u8 location, u8 tile) {
	//Write tile at location.
	u8* ptr = workingBuffer;
	u8 y = location & 0x0F;
	u8 x = location >> 4;
	ptr += 8 * y + x;
	*ptr = tile;
}

void MapGen_ConnectDots_Simple(u8 start, u8 end, u8 block) {
	//Connect two points with a block, with a direct path.
	int startX = start >> 4;
	int startY = start & 0x0F;
	int endX = end >> 4;
	int endY = end & 0x0F;
	int xOffset;
	int yOffset;
	if (startX <= endX)
		xOffset = 16;
	else
		xOffset = -16;
	if (startY <= endY)
		yOffset = 1;
	else
		yOffset = -1;

	u8 curPos = start;
	while (curPos != end) {
		MapGen_WriteTileAt(curPos, block);
		if (curPos >> 4 != endX)
			curPos += xOffset;
		MapGen_WriteTileAt(curPos, block);
		if ((curPos & 0x0F) != endY)
			curPos += yOffset;
		MapGen_WriteTileAt(curPos, block);
	}

}

void MapGen_ConnectDots(u8 start, u8 end, u8 block) {
	//Connect two points with a block, with an intermediate
	//random point.
	u8 x;
	u8 y;
	do {
		x = PRNG() & 7;
	} while (x == 0 || x == 7);
	x <<= 4;
	do {
		y = PRNG() & 7;
	} while (y == 0 || y == 7);

	u8 point = x | y;
	MapGen_ConnectDots_Simple(start, point, block);
	MapGen_ConnectDots_Simple(point, end, block);
}

void MapGen_Expand(u8 tile) {
	//Loop through each tile on the map.If it is a specified tile,
	//it has a 50 % chance of spawning a new tile
	//from each cardinal direction(1 tile up, 1 tile down, 1 tile left,
	// 1 tile right). We call this procedure "expanding".
	u8* tempBuffer = calloc(64, sizeof(u8));
	memcpy(tempBuffer + 8, workingBuffer + 8, 64 - 16);
	for (int i = 8; i < 64 - 8; i++) {
		if (i % 8 == 0 || i % 8 == 7)
			continue;
		if (tempBuffer[i] != tile)
			continue;
		u8 b = PRNG();
		if (b & 1)
			//West
			*(workingBuffer + i - 1) = tile;
		if (b & 2)
			//East
			*(workingBuffer + i + 1) = tile;
		if (b & 4)
			//North
			*(workingBuffer + i - 8) = tile;
		if (b & 8)
			//South
			*(workingBuffer + i + 8) = tile;
	}
	free(tempBuffer);
}

void MapGen_Seed(u8 origTile, u8 newTile, u8 chance) {
	//In the map data, replace each tile with a new tile with a chance / 256 chance.
	//We call this procedure "seeding".
	for (int i = 0; i < 64; i++) {
		if (workingBuffer[i] != origTile)
			continue;
		if (PRNG() < chance)
			workingBuffer[i] = newTile;
	}
}

void MapGen_SeedInside(u8 b, u8 c, u8 d) {
	//MapGen_Seed, but does not check tiles on map borders
	//(X = 0, 7 or Y = 0, 7)
	for (int i = 8; i < 64 - 8; i++) {
		if (i % 8 == 0 || i % 8 == 7 || workingBuffer[i] != b)
			continue;
		if (PRNG() < d)
			workingBuffer[i] = c;
	}
}

void MapGen_AdvancedSeed(u8 origTile, u8 newTile, u8 chance, u8 top, u8 bottom, u8 left, u8 right) {
	//MapGen_SeedInside, but only changes tiles which are next to other tiles
	//setting a directional parameter to 0 skips the check.
	for (int i = 8; i < 64 - 8; i++) {
		if (i % 8 == 0 || i % 8 == 7 || workingBuffer[i] != origTile)
			continue;
		if (PRNG() >= chance) {
			if ((!top || workingBuffer[i - 8] == top) &&
				(!bottom || workingBuffer[i + 8] == bottom) &&
				(!left || workingBuffer[i - 1] == left) &&
				(!right || workingBuffer[i + 1] == right))
				workingBuffer[i] = newTile;
		}
	}
}
#pragma endregion

#pragma region Biomes

void GenerateChunk_GrasslandsBiome() {
	//Generate a Grasslands biome.
	MapGen_Seed(PATH_TILE, GRASS_TILE, 0x30);
	MapGen_Expand(GRASS_TILE);
	MapGen_AdvancedSeed(FOREST_TILE, TREES_TOP, 0x20, FOREST_TILE, PATH_TILE, 0, 0);
	MapGen_AdvancedSeed(FOREST_TILE, TREES_BOTTOM, 0x20, PATH_TILE, FOREST_TILE, 0, 0);
	MapGen_AdvancedSeed(FOREST_TILE, TREES_RIGHT, 0x20, 0, 0, PATH_TILE, FOREST_TILE);
	MapGen_AdvancedSeed(FOREST_TILE, TREES_LEFT, 0x20, 0, 0, FOREST_TILE, PATH_TILE);
	MapGen_Seed(PATH_TILE, FLOWERS_TILE, 0x30);
	MapGen_Seed(PATH_TILE, PATCH_TILE, 0x30);
	MapGen_SeedInside(TREES_TOP, ITEM_BOTTOM, 0x40);
	MapGen_SeedInside(TREES_LEFT, ITEM_RIGHT, 0x40);
	MapGen_SeedInside(TREES_RIGHT, ITEM_LEFT, 0x40);
	MapGen_SeedInside(TREES_BOTTOM, ITEM_TOP, 0x40);
}

void GenerateChunk_SteppesBiome() {
	MapGen_Seed(PATH_TILE, FLOWERS_SMALL, 0x40);
	MapGen_Seed(PATH_TILE, PATCH_TILE, 0x30);
	MapGen_Seed(PATH_TILE, GRASS_TILE, 0xd0);
	MapGen_SeedInside(PATH_TILE, ITEM_PATH, 0x20);
}

void GenerateChunk_ConstructBiome() {
	//Generate a Construct biome.
	MapGen_AdvancedSeed(FOREST_TILE, PILLARS_TILE, 0xc0, 0, 0, 0, PATH_TILE);
	MapGen_AdvancedSeed(FOREST_TILE, PILLARS_TILE, 0xc0, 0, 0, PATH_TILE, 0);
	MapGen_AdvancedSeed(PATH_TILE, PILLARS_LEFT, 0x60, 0, 0, FOREST_TILE, 0);
	MapGen_AdvancedSeed(PATH_TILE, PILLARS_RIGHT, 0x60, 0, 0, 0, FOREST_TILE);
	MapGen_AdvancedSeed(PATH_TILE, PILLARS_TOP, 0x60, FOREST_TILE, 0, 0, 0);
	MapGen_AdvancedSeed(PATH_TILE, PILLARS_BOTTOM, 0x60, 0, FOREST_TILE, 0, 0);
	if (exits & 8)
		MapGen_ConnectDots_Simple(hChunkStartXY, 0x40, BRICKS_TILE);
	if (exits & 4)
		MapGen_ConnectDots_Simple(hChunkStartXY, 0x47, BRICKS_TILE);
	if (exits & 2)
		MapGen_ConnectDots_Simple(hChunkStartXY, 0x04, BRICKS_TILE);
	if (exits & 1)
		MapGen_ConnectDots_Simple(hChunkStartXY, 0x74, BRICKS_TILE);
	MapGen_Seed(PATH_TILE, GRASS_TILE, 0x50);
	MapGen_Seed(PATH_TILE, PATCH_TILE, 0xc0);
	MapGen_Seed(PATH_TILE, GRASS_TILE, 0x74);
	MapGen_SeedInside(PILLARS_LEFT, ITEM_RIGHT, 0x18);
	MapGen_SeedInside(PILLARS_RIGHT, ITEM_LEFT, 0x18);
}

void GenerateChunk_CorruptionBiome() {
	//Generate a Corruption biome.
	MapGen_AdvancedSeed(FOREST_TILE, CORRUPTION_EC, 0x40, 0, 0, PATH_TILE, 0);
	MapGen_AdvancedSeed(FOREST_TILE, CORRUPTION_EC, 0x40, 0, 0, 0, PATH_TILE);
	MapGen_AdvancedSeed(FOREST_TILE, CORRUPTION_EC, 0x40, PATH_TILE, 0, 0, 0);
	MapGen_AdvancedSeed(FOREST_TILE, CORRUPTION_EC, 0x40, 0, PATH_TILE, 0, 0);
	MapGen_Seed(CORRUPTION_EC, CORRUPTION_C9, 0x80);
	MapGen_Seed(PATH_TILE, GRASS_TILE, 0x80);
	//randomly place corrupted tiles
	for (int i = 0; i < 64; i++) {
		if (workingBuffer[i] == FOREST_TILE) {
			if (PRNG() < 0x40) {
				int tileIndex = PRNG() & 3;
				workingBuffer[i] = corruptedTiles[tileIndex];
			}
		}
	}

	if (exits & 8)
		MapGen_ConnectDots_Simple(hChunkStartXY, 0x40, TILED_TILE);
	if (exits & 4)
		MapGen_ConnectDots_Simple(hChunkStartXY, 0x47, TILED_TILE);
	if (exits & 2)
		MapGen_ConnectDots_Simple(hChunkStartXY, 0x04, TILED_TILE);
	if (exits & 1)
		MapGen_ConnectDots_Simple(hChunkStartXY, 0x74, TILED_TILE);
	MapGen_Seed(TILED_TILE, GRASS_TILE, 0x20);
	MapGen_SeedInside(TILED_TILE, ITEM_PATH, 0x10);
}
#pragma endregion

u8* GenerateChunk(u16 x, u16 y) {
	workingBuffer = malloc(64 * sizeof(u8));
	//Generate a chunk.
	GetChunkExitsAndBiome(x, y);
	memset(workingBuffer, FOREST_TILE, 64);

	if (exits & 1)
		hChunkStartXY = 0x74;
	if (exits & 2)
		hChunkStartXY = 0x04;
	if (exits & 4)
		hChunkStartXY = 0x47;
	if (exits & 8)
		hChunkStartXY = 0x40;

	PreparePRNGState(x, y);

	if (exits & 1)
		MapGen_ConnectDots(hChunkStartXY, 0x74, PATH_TILE);
	if (exits & 2)
		MapGen_ConnectDots(hChunkStartXY, 0x04, PATH_TILE);
	if (exits & 4)
		MapGen_ConnectDots(hChunkStartXY, 0x47, PATH_TILE);
	if (exits & 8)	
		MapGen_ConnectDots(hChunkStartXY, 0x40, PATH_TILE);
	MapGen_Expand(PATH_TILE);

	if (exits & 8) {
		MapGen_WriteTileAt(0x30, PATH_TILE);
		MapGen_WriteTileAt(0x40, PATH_TILE);
	}
	if (exits & 4) {
		MapGen_WriteTileAt(0x37, PATH_TILE);
		MapGen_WriteTileAt(0x47, PATH_TILE);
	}
	if (exits & 2) {
		MapGen_WriteTileAt(0x03, PATH_TILE);
		MapGen_WriteTileAt(0x04, PATH_TILE);
	}
	if (exits & 1) {
		MapGen_WriteTileAt(0x73, PATH_TILE);
		MapGen_WriteTileAt(0x74, PATH_TILE);
	}

	switch (biome) {
	case 0: GenerateChunk_GrasslandsBiome();  break;
	case 1: GenerateChunk_SteppesBiome();     break;
	case 2: GenerateChunk_ConstructBiome();   break;
	case 3: GenerateChunk_CorruptionBiome();  break;
	default:
		printf("oob biome array call");
		exit(1);
	}
	return workingBuffer;
}

u32 GetNewSeed(u32 _seed) {
	//first byte >= A0 (put sp in RAM), second byte is a jp/call instr
	if ((_seed & 0xFF000000) == 0xFF000000)
		_seed -= (0xFF000000 - 0xA0000000);
	else {
		_seed += 0x01000000;
		return _seed;
	}
	switch (_seed >> 16 & 0xFF) {
	case 0xC2:
	case 0xC3:
		_seed += 0x010000;
		return _seed;
	case 0xC4:
		_seed += 0x090000;
		return _seed;
	case 0xCD:
		_seed += 0x070000;
		return _seed;
	case 0xD4:
		_seed += 0x080000;
		return _seed;
	case 0xD2:
	case 0xDA:
		_seed += 0x020000;
		return _seed;
	case 0xDC:
		return 0;
	default:
		printf("I fucked up math");
		exit(1);
	}
}

void Loop() {
	//Test all seeds
	//Doesn't take parameters just modify the first two for loops
	u32 _seed = 0xA0C282B8;
	while (true) {
		Set(_seed);
#pragma warning(suppress : 6292)
		//Ill defined for loop is intended behaviour. These are signed, but it's messy to treat them like they are.
		for (u16 x = 0xFF00; x != 0x0100; x++) {
#pragma warning(suppress : 6292)
			for (u16 y = 0xF032; y != 0x1132; y += 0x100) {
				if (x < 4 && y < 4)
					continue;

				u8* map = GenerateChunk(x, y);
				int mapIndex = 0x100 + map[0x1C];
				if (map[0x1D] == CORRUPTION_D9) {
					//printf("found d9 at %04x, %04x\n", x, y);
					u8* fullMap = malloc(64 * 9);
					CopyToLargerArray(map, fullMap, 8, 8, 24, 8, 16);
					for (int _y = 0; _y < 3; _y++) {
						for (int _x = 0; _x < 3; _x++) {
							if (_x == 1 && _y == 2)
								continue;
							free(map);
							map = GenerateChunk(x + (_x - 1), y + _y - 2);
							CopyToLargerArray(map, fullMap, 8, 8, 24, 8 * _x, 8 * _y);
						}
					}
					int foundSP = 0;
					while (true) {
						if (mapIndex >= 64 * 9)
							break;
						u8 curByte = fullMap[mapIndex];
						switch (curByte) {
						//ld sp,hl
						case 0x31:
							if (foundSP) {
								goto continueLoop;
							}
							//printf("found 31 at %04x, %04x\n", x, y);
							//printf("seed: 0x%08x\n", _seed);
							if (fullMap[mapIndex + 1] == 0xEC && fullMap[mapIndex + 2] == 0x0A) {
								//printf("found ld sp,0AEC at %04x\n", 0xC800 + mapIndex);
								foundSP = 1;
								mapIndex += 3;
								continue;
							}
							else {
								mapIndex += 3;
								continue;
							}
						//ret, reti
						case 0xC9:
						case 0xD9:
							if (foundSP) {
								printf("found return at %04x\n", 0xC800 + mapIndex);
								printf("seed: 0x%08x\n", _seed);
								printf("(x, y): 0x%04x, 0x%04x\n", x, y);
								PrintFullMap(fullMap);
							}
							goto continueLoop;
						//crashes
						case 0x60:
						case 0xDB:
						case 0xEC:
							goto continueLoop;
						//everything else
						case 0x0A:
						case 0x0B:
						case 0x0F:
						case 0x13:
						case 0x32:
						case 0x33:
						case 0x34:
						case 0x4D:
						case 0x4E:
						case 0x51:
						case 0x52:
						case 0x55:
						case 0x6C:
						case 0x6D:
						case 0x6E:
						case 0x6F:
						case 0x74:
						case 0x7A:
						case 0x7B:
							mapIndex += 1;
							break;
						case 0xCB:
							mapIndex += 2;
							break;
						case 0x08:
						case 0xCA:
							mapIndex += 3;
							break;

						}
					}

				continueLoop:
					free(fullMap);
				}
				free(map);
			}
		}
		_seed = GetNewSeed(_seed);

		if (!_seed)
			break;
	}
}

void SaveEntireMap() {
	//Save the entire map traversed on the way to the correct map, for analysis in c#
	Set(0xe8c382b8);

	const int w = 0x62;
	const int h = 0x10000 - 0xf932;

	u8* fullMap = malloc(64 * w * h);

#pragma warning(suppress : 6292)
	for (u16 x = 0; x != w; x++) {
#pragma warning(suppress : 6292)
		for (u16 y = 0xf933; y != 0; y++) {
			u8* map = GenerateChunk(x, y);
			CopyToLargerArray(map, fullMap, 8, 8, w * 8, x % 0x10000 * 8, (y + h) % 0x10000 * 8);
			free(map);
		}
	}

	FILE* f = fopen("C:\\Users\\Maya\\source\\repos\\foolsHC4\\Debug\\hugefuckingmap.bin", "wb+");
	if (!f) {
		perror("well fuck");
		exit(42069);
	}
	fwrite(fullMap, sizeof(u8), 64 * w * h, f);
	fclose(f);
}

int main() {
	//haha array? what's an array?
	//didn't use an int for the seed bc i don't trust endianness lol
	seed = calloc(4, sizeof(u8));
	corruptedTiles = calloc(4, sizeof(u8));
	// I used to be using more files, but they ended up not being necessary
	MapExitConfigurations = ReadBytesFromFile("MapExitConfigurations.bin", 256);

	corruptedTiles[0] = CORRUPTION_D9;
	corruptedTiles[1] = CORRUPTION_DB;
	corruptedTiles[2] = CORRUPTION_CA;
	corruptedTiles[3] = CORRUPTION_CB;

	//Loop();
	SaveEntireMap();

	exit(0);
}