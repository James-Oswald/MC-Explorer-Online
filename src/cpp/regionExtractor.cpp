#include<cstdio>
#include<string>
#include<memory>
#include<vector>
#include<exception>
#include<iostream>
#include<cstdlib>

namespace zlib{
    extern "C"{
#include<zlib.h>
    }
};

#include"byteUtils.h"

class Chunk{
    private:
        u32 offset;
        u8 sectorCount;
        u64 chunkDataBufferLen;
        u8* chunkDataBuffer;
        enum CompressionMethod{
            GZip = 1,
            Zlib = 2
        };
        friend class Region;
        void free();
    public:
        Chunk(u8* location, u32 offset, u8 sectorCount);
        ~Chunk();
        void toFile();
};

Chunk::Chunk(u8* location, u32 offset_, u8 sectorCount_)
:offset(offset_), sectorCount(sectorCount_){
    u32 chunkLength = u8Atou32(location, 4);
    u8 compressionType = location[4];
    if(compressionType != Zlib)
        throw std::runtime_error("Chunk Data Compression must be of type Zlib!");
    chunkDataBufferLen = 100000; //100 Kb initially
    chunkDataBuffer = (u8*)std::malloc(sizeof(u8) * chunkDataBufferLen);
    uncompressLoop:{
        int status = zlib::uncompress(chunkDataBuffer, (unsigned long int*)(&chunkDataBufferLen), location + 5, chunkLength - 1);
        switch(status){
            case Z_OK:
                break;
            case Z_BUF_ERROR:
                chunkDataBufferLen *= 2;
                chunkDataBuffer = (u8*)std::realloc(chunkDataBuffer, sizeof(u8) * chunkDataBufferLen); 
                goto uncompressLoop;
            case Z_DATA_ERROR:
                throw std::runtime_error("Chunk Data Corrupted at offset " + std::to_string(offset_));
            case Z_MEM_ERROR:
            default:
                throw std::runtime_error("Unexpected uncompression Failure");
        }
    }
    chunkDataBuffer = (u8*)std::realloc(chunkDataBuffer, sizeof(u8) * chunkDataBufferLen);
}

void Chunk::free(){
    std::free(chunkDataBuffer);
}

Chunk::~Chunk(){
}

void Chunk::toFile(){
    std::string filename = "./chunks/chunk" + std::to_string(offset) + ".nbt";
    FILE* outputFile = fopen(filename.c_str(), "wb");
    if(outputFile == NULL)
        throw std::runtime_error("Failed to write chunk with offset " + std::to_string(offset));
    fwrite(chunkDataBuffer ,sizeof(u8), chunkDataBufferLen, outputFile);
    fclose(outputFile);
}

class Region{
    private:
        static const u32 sectorSize = 4096;
        u64 regionDataSize;
        u8* regionData;
        std::vector<Chunk> chunks;
    public:
        Region(const std::string& pathName);
        ~Region();
        std::vector<Chunk> getChunks();
};

Region::Region(const std::string& pathName){
    FILE* regionFile = fopen(pathName.c_str(), "rb");
    if(regionFile == NULL)
        throw std::runtime_error("Could not open file \"" + pathName + "\"");
    fseek(regionFile, 0, SEEK_END);
    regionDataSize = ftell(regionFile);
    rewind(regionFile);
    regionData = (u8*)malloc(sizeof(u8) * regionDataSize);
    u32 sizeRead = fread(regionData, sizeof(u8), regionDataSize, regionFile);
    fclose(regionFile);
    if(sizeRead == 0)
        throw std::runtime_error("Failed to read file \"" + pathName + "\"");
    chunks = std::vector<Chunk>();
    for(u32 i = 0; i < sectorSize; i += 4){
        u32 chunkOffset = u8Atou32(regionData + i, 3);
        u8 sectorCount = regionData[i + 3];
        if(chunkOffset != 0)
            chunks.push_back(Chunk(regionData + chunkOffset * sectorSize, chunkOffset, sectorCount));
    }
}

Region::~Region(){
    free(regionData);
    for(u32 i = 0; i < chunks.size(); i++)
        chunks[i].free();
}

std::vector<Chunk> Region::getChunks(){
    return chunks;
}

int main(){
    //u8 t[] = {0x10, 0x33, 0x22};
    //u32 meme = u8Atou32(t, 3);

    ///*
    Region r("r.0.0.mca");
    std::vector<Chunk> c = r.getChunks();
    for(u32 i = 0; i < c.size(); i++){
        c[i].toFile();
    }
    //*/

    return 0;
}