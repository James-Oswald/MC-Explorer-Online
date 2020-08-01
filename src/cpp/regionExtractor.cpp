#include<cstdio>
#include<string>
#include<memory>
#include<vector>
#include<exception>

namespace zlib{
    extern "C"{
#include<zlib.h>
    }
};

using namespace std;

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
        
    public:
        Chunk(u8* location);
};

Chunk::Chunk(u8* location){
    u32 chunkLength = u8Atou32(location, 3);
    u8 compressionType = location[4];
    if(compressionType != Zlib)
        throw runtime_error("Chunk Data Compression must be of type Zlib!");
    u64 chunkDataBufferLen = 100000; //100 Kb 
    u8* chunkDataBuffer = (u8*)malloc(sizeof(u8) * chunkDataBufferLen); 
    uncompressLoop:{
        int status = zlib::uncompress(chunkDataBuffer, &chunkDataBufferLen, location, chunkLength);
        switch(status){
            case Z_OK:
                break;
            case Z_BUF_ERROR:
                chunkDataBufferLen *= 2;
                realloc(chunkDataBuffer, sizeof(u8) * chunkDataBufferLen); 
                goto uncompressLoop;
            case Z_DATA_ERROR:
                throw runtime_error("Chunk Data Corrupted: Length " + chunkLength);
            case Z_MEM_ERROR:
            default:
                throw runtime_error("Unexpected uncompression Failure");
        }
        realloc(chunkDataBuffer, chunkDataBufferLen);
    }
}

class Region{
    private:
        u64 regionFileSize;
        u8* regionFileData;
    public:
        Region(const string& pathName);

};

Region::Region(const string& pathName){
    FILE* regionFile = fopen(pathName.c_str(), "rb");
    fseek(regionFile, 0, SEEK_END);
    long regionFileSize = ftell(regionFile);
    regionFileData = new u8[regionFileSize];
    fclose(regionFile);
}

int main(){

}