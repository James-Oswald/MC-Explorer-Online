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

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long u64;


//Big edian u8 array to u32 conversion helper function
u32 u8Atou32(u8 const* location, u8 numBytes){
    u32 rv = 0;
    for(u8 i = 0; i < numBytes; i++)
        rv += location[i] << (8 * (numBytes - i - 1));
    return rv;
}

class Chunk{
    private:
        enum Compression{
            GZip = 1,
            Zlib = 2
        };
        u32 offset;
        u8 sectorCount;
        u64 chunkDataBufferLen;
        std::shared_ptr<u8> chunkDataBuffer;
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
    chunkDataBuffer = std::shared_ptr<u8>((u8*)std::malloc(chunkDataBufferLen * sizeof(u8)));
    uncompressLoop:{
        int status = zlib::uncompress(chunkDataBuffer.get(), &chunkDataBufferLen, location + 5, chunkLength - 1);
        switch(status){
            case Z_OK:
                break;
            case Z_BUF_ERROR:
                chunkDataBufferLen *= 2;
                chunkDataBuffer.reset((u8*)std::realloc(chunkDataBuffer.get(), sizeof(u8) * chunkDataBufferLen)); 
                goto uncompressLoop;
            case Z_DATA_ERROR:
                throw std::runtime_error("Chunk Data Corrupted at offset " + std::to_string(offset_));
            case Z_MEM_ERROR:
            default:
                throw std::runtime_error("Unexpected uncompression Failure");
        }
    }
    chunkDataBuffer.reset((u8*)std::realloc(chunkDataBuffer.get(), sizeof(u8) * chunkDataBufferLen));
}

Chunk::~Chunk(){
}

void Chunk::toFile(){
    std::string filename = "chunk" + std::to_string(offset) + ".nbt";
    FILE* outputFile = fopen(filename.c_str(), "wb");
    fwrite(chunkDataBuffer.get() ,sizeof(u8), chunkDataBufferLen, outputFile);
    fclose(outputFile);
}

class Region{
    private:
        static const u32 sectorSize = 4096;
        u64 regionDataSize;
        std::shared_ptr<u8> regionData;
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
    regionData = std::shared_ptr<u8>((u8*)std::malloc(sizeof(u8) * regionDataSize));
    u32 sizeRead = fread(regionData.get(), sizeof(u8), regionDataSize, regionFile);
    fclose(regionFile);
    if(sizeRead == 0)
        throw std::runtime_error("Failed to read file \"" + pathName + "\"");
    chunks = std::vector<Chunk>();
    for(u32 i = 0; i < sectorSize; i += 4){
        u32 chunkOffset = u8Atou32(regionData.get() + i, 3);
        u8 sectorCount = regionData.get()[i + 3];
        if(chunkOffset != 0)
            try{
                chunks.push_back(Chunk(regionData.get() + chunkOffset * sectorSize, chunkOffset, sectorCount));
            }catch(std::exception& e){
                std::cout<<e.what()<<"\n";
            }
    }
}

Region::~Region(){
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
    c[3].toFile();
    //*/

    return 0;
}