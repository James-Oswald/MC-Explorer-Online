
#ifndef byteUtils
#define byteUtils

#include<cstdint>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
//typedef unsigned long u64;

//Big edian u8 array to u32 conversion helper function
u32 u8Atou32(u8 const* location, u8 numBytes){
    u32 rv = 0;
    for(u8 i = 0; i < numBytes; i++)
        rv += location[i] << (8 * (numBytes - i - 1));
    return rv;
}

#endif