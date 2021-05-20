#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
//allow use of fopen
#pragma warning (disable : 4996)

uint8_t* wCurMapBlocks;
uint8_t* MapExitConfigurations;
uint8_t* workingBuffer;

uint8_t wRNGVarX;
uint8_t wRNGVarA;
uint8_t wRNGVarB;
uint8_t wRNGVarC;

uint8_t* seed;

const uint8_t PATH_TILE = 0x0A;
const uint8_t GRASS_TILE = 0x0B;
const uint8_t FOREST_TILE = 0x0F;
const uint8_t ITEM_RIGHT = 0x32;
const uint8_t ITEM_BOTTOM = 0x33;
const uint8_t ITEM_TOP = 0x34;
const uint8_t ITEM_LEFT = 0x60;
const uint8_t TREES_TOP = 0x6C;
const uint8_t TREES_LEFT = 0x6D;
const uint8_t TREES_RIGHT = 0x6E;
const uint8_t TREES_BOTTOM = 0x6F;
const uint8_t FLOWERS_TILE = 0x74;
const uint8_t PATCH_TILE = 0x7A;

void PrintBuffer(uint8_t* buffer, int w, int h) {
    uint8_t* hl = buffer;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            printf("%02x ", *hl);
            hl++;
        }
        putchar('\n');
    }
    putchar('\n');
}

uint8_t PRNG() {
    //thanks ghidra
    wRNGVarA = wRNGVarA ^ wRNGVarC ^ wRNGVarX + 1;
    wRNGVarB += wRNGVarA;
    wRNGVarX++;
    wRNGVarC += (wRNGVarB >> 1 ^ wRNGVarA);
    return wRNGVarC;
}

void PreparePRNGState(int16_t x, int16_t y) {
    // Seed the RNG with the chunk seed.
    // Chunk Y in BC. Chunk X in DE.

    wRNGVarX = seed[0] ^ (x >> 8 & 0xFF);
    wRNGVarA = seed[1] ^ (x & 0xFF);
    wRNGVarB = seed[2] ^ (y >> 8 & 0xFF);
    wRNGVarC = seed[3] ^ (y & 0xFF);
    for (int i = 0; i < 16; i++) {
       PRNG();
    }
    return;
}

uint8_t GetChunkExitsAndBiome(int16_t x, int16_t y) {
    // Calculate the biome and exit locations for a given chunk.
    // Seed RNG with chunk coordinates divided by 4 (so each 4x4 chunk area
    // gets its own biome and layout
    PreparePRNGState(x & 0xfffc, y & 0xfffc);
    uint8_t a = PRNG() & 7;
    int mapExitOffset = (a << 4) + y * 4 + x;
    uint8_t ret = MapExitConfigurations[mapExitOffset] & 0x0f;
    //The rest of this func is unused for the initial runs but we need to simulate the RNG
    PRNG();
    return ret;
}


void MapGen_WriteTileAt(uint8_t location, uint8_t tile) {
    //Write tile at location B.
    uint8_t* ptr = workingBuffer;
    uint8_t y = location & 0x0F;
    uint8_t x = location >> 4;
    ptr += 8 * y + x;
    *ptr = tile;
}

void MapGen_ConnectDots_Simple(uint8_t start, uint8_t end, uint8_t block) {
    //Connect point at B to point at C with block E, with a direct path.
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

    uint8_t curPos = start;
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

void MapGen_ConnectDots(uint8_t start, uint8_t end, uint8_t block) {
    //Connect point at B to point at C with block E, with an intermediate
    //random point.
    uint8_t x;
    uint8_t y;
    do {
        x = PRNG() & 7;
    } while (x == 0 || x == 7);
    x <<= 4;
    do {
        y = PRNG() & 7;
    } while (y == 0 || y == 7);

    uint8_t point = x | y;
    MapGen_ConnectDots_Simple(start, point, block);
    MapGen_ConnectDots_Simple(point, end, block);
}

void MapGen_SetTileHLNorth(uint8_t tile, uint8_t* hl) {
    //Write tile A north of HL.
    *(hl - 8) = tile;
}

void MapGen_SetTileHLSouth(uint8_t tile, uint8_t* hl) {
    //Write tile A south of HL.
    *(hl + 8) = tile;
}

void MapGen_SetTileHLWest(uint8_t tile, uint8_t* hl) {
    //Write tile A west of HL.
    *(hl - 1) = tile;
}

void MapGen_SetTileHLEast(uint8_t tile, uint8_t* hl) {
    //Write tile A east of HL.
    *(hl + 1) = tile;
}

//Loop through each tile on the map.If it is tile[hChunkWriteTile],
//it has a 50 % chance of spawning a new tile with ID[hChunkWriteTile],
//from each cardinal direction(1 tile up, 1 tile down, 1 tile left,
// 1 tile right).We call this procedure "expanding".
void MapGen_Expand(uint8_t tile) {
    uint8_t* tempBuffer = calloc(64, sizeof(uint8_t));
    memcpy(tempBuffer + 8, workingBuffer + 8, 64 - 16);
    for (int i = 8; i < 64 - 8; i++) {
        if (i % 8 == 0 || i % 8 == 7)
            continue;
        if (tempBuffer[i] != tile)
            continue;
        uint8_t b = PRNG();
        if (b & 1)
            MapGen_SetTileHLWest(tile, workingBuffer + i);
        if (b & 2)
            MapGen_SetTileHLEast(tile, workingBuffer + i);
        if (b & 4)
            MapGen_SetTileHLNorth(tile, workingBuffer + i);
        if (b & 8)
            MapGen_SetTileHLSouth(tile, workingBuffer + i);
    }
    free(tempBuffer);
}

void MapGen_Seed(uint8_t origTile, uint8_t newTile, uint8_t chance) {
    //In the map data, replace each tile B with C with a D / 256 chance.
    //We call this procedure "seeding".
    for (int i = 0; i < 64; i++) {
        if (workingBuffer[i] != origTile)
            continue;
        if (PRNG() < chance)
            workingBuffer[i] = newTile;
    }
}

void MapGen_SeedInside(uint8_t b, uint8_t c, uint8_t d) {
    //MapGen_Seed, but does not check tiles on map borders
    //(X = 0, 7 or Y = 0, 7)
    for (int i = 8; i < 64 - 8; i++) {
        if (i % 8 == 0 || i % 8 == 7 || workingBuffer[i] != b)
            continue;
        if (PRNG() < d)
            workingBuffer[i] = c;
    }
}

void MapGen_AdvancedSeed(uint8_t origTile, uint8_t newTile, uint8_t chance, uint8_t top, uint8_t bottom, uint8_t left, uint8_t right) {
    //MapGen_SeedInside, but only changes tiles which are next to other tiles,
    //based on values in[hChunkSeedTileTop], [hChunkSeedTileBottom],
    //[hChunkSeedTileLeft] , [hChunkSeedTileRight].
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

uint8_t hChunkStartXY;

void GenerateChunk(int16_t x, int16_t y, uint8_t* buffer) {
    workingBuffer = buffer;
    //Generate a chunk.
    uint8_t exits = GetChunkExitsAndBiome(x, y);
    //memset(workingBuffer, 0x0F, 64);
    for (int i = 0; i < 64; i++) {
        workingBuffer[i] = FOREST_TILE;
    }
    if (exits & 1)
        hChunkStartXY = 0x74;
    if (exits & 2)
        hChunkStartXY = 0x04;
    if (exits & 4)
        hChunkStartXY = 0x47;
    if (exits & 8)
        hChunkStartXY = 0x40;
    PreparePRNGState(x, y);

    uint8_t start = hChunkStartXY;
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
    GenerateChunk_GrasslandsBiome();
}

uint8_t* GenerateSingleChunk(int16_t x, int16_t y) {
    uint8_t* buffer = malloc(64 * sizeof(uint8_t));
    GenerateChunk(x, y, buffer);
    return buffer;
}

int CheckSpawnMap(uint8_t* map, uint8_t* compareAgainst) {
    //i'm making a pretty big assumption that only two map chunks are being shown
    //bc i'm kinda sick of C
    //i'll come back tomorrow if i'm wrong

    //check at top-right x between 0-3 (range where only two maps are shown)
    for (int k = 0; k <= 3; k++) {
        int mapIndex = k;
        int spawnIndex = 0;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 5; j++) {
                if (compareAgainst[spawnIndex] != map[mapIndex])
                    goto nomatch;
                mapIndex++;
                spawnIndex++;
            }
            mapIndex += 3;
        }
        return k;
    nomatch:
        continue;
    }
    return -1;
}

int CheckUpperSpawnMap(uint8_t* map, uint8_t* compareAgainst, int startIndex) {
    int mapIndex = startIndex + 8 * 6;
    int spawnIndex = 0;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 5; j++) {
            if (compareAgainst[spawnIndex] != map[mapIndex])
                return 0;
            mapIndex++;
            spawnIndex++;
        }
        mapIndex += 3;
    }
    return 1;
}

int main() {
    seed = calloc(4, sizeof(uint8_t));
    MapExitConfigurations = malloc(256);

    uint8_t correctTopAnswer[] = {
        GRASS_TILE, GRASS_TILE, GRASS_TILE, FLOWERS_TILE, PATH_TILE,
        FOREST_TILE, GRASS_TILE, FOREST_TILE, PATH_TILE, PATH_TILE
    };
    uint8_t correctBottomAnswer[] = {
        FOREST_TILE, FOREST_TILE, PATH_TILE, PATH_TILE, GRASS_TILE,
        FOREST_TILE, GRASS_TILE, PATH_TILE, PATH_TILE, PATH_TILE,
        GRASS_TILE, GRASS_TILE, PATH_TILE, PATH_TILE, FLOWERS_TILE
    };

    FILE* f = fopen("MapExitConfigurations.bin", "rb");
    if (!f) {
        perror("Could not open file");
        exit(1);
    }
    fread(MapExitConfigurations, sizeof(uint8_t), 256, f);
    fclose(f);

    for (uint32_t _seed = 1; _seed < 0xFFFFFFF1; _seed += 16) {
        seed[3] = _seed & 0xFF;
        seed[2] = (_seed >> 8) & 0xFF;
        seed[1] = (_seed >> 16) & 0xFF;
        seed[0] = (_seed >> 24) & 0xFF;

        uint8_t* map = GenerateSingleChunk(1, 2);

        int startX = CheckSpawnMap(map, correctBottomAnswer);
        if (startX != -1) {
            map = GenerateSingleChunk(1, 1);
            if (CheckUpperSpawnMap(map, correctTopAnswer, startX))
                printf("%08x\n", _seed);
        }
        free(map);
    }
}