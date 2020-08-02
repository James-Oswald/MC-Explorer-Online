//James Oswald NBT Class

#ifndef _NBT
#define _NBT

#include<string>
#include<vector>
#include"byteUtils.h"

namespace nbt{


class NBT{
    private:
        struct Tag;
        std::vector<Tag> tags;
        std::string pathToString(std::vector<std::string> path);
    public:
        NBT(u8* data);

};

struct NBT::Tag{
    enum Type: u8{
        TAG_End = 0,
        TAG_Byte = 1,
        TAG_Short = 2,
        TAG_Int = 3,
        TAG_Long = 4,
        TAG_Float = 5,
        TAG_Double = 6,
        TAG_Byte_Array = 7,
        TAG_String = 8,
        TAG_List = 9,
        TAG_Compound = 10,
        TAG_Int_Array = 11,
        TAG_Long_Array = 12
    };
    std::vector<std::string> path;
    std::string name;
    u8* location;
    Type type;
    u64 tagLength;
    Tag();
};

std::string NBT::pathToString(std::vector<std::string> path){
    std::string rv = "";
    for(u32 i = 0; i < path.size(); i++)
        rv += "/" + path[i];
    return rv;
}

//this parser is only valid for java edition tags that use an unnamed compound as their root
NBT::NBT(u8* data){
    tags = std::vector<Tag>();
    bool end = false;
    u32 dataIndex = 0;
    std::vector<std::string> path; //path to the tag
    std::vector<u32> listIndexs; //index of self in parent tag
    std::vector<u32> ArrayLength; //num elements if inside fixed length array, if not in one will be 0xffffffff

    while(!end){
        Tag::Type type = (Tag::Type)data[dataIndex];
        u16 nameLength = (u16)u8Atou32(data + dataIndex + 1, 2);
        std::string name;
        if(nameLength != 0)
            name = std::string((char*)(data + dataIndex + 3), nameLength);
        else
            name = "<unnamed#" + std::to_string(listIndexs.back()) + ">";
        switch (type){
            case Tag::Type::TAG_End:
                listIndexs.pop_back();
                path.pop_back();
            case Tag::Type::TAG_Compound:
                path.push_back(name);
                listIndexs.push_back(0);
            case Tag::Type::TAG_Int_Array:
            case Tag::Type::TAG_Long_Array:
            case Tag::Type::TAG_Byte_Array:
                path.push_back(name);
                listIndexs.push_back(0);

            break;
        default:

            break;
        }
    }
}

}

#endif