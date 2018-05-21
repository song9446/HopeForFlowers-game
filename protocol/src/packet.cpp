#include "packet.hpp"
/*
    Packet::Packet(char *message, size_t length){
        op = *reinterpret_cast<OP*>(message);
        data = message+sizeof(OP);
        length = length - sizeof(OP);
    }
    Packet::Packet(){
        data -= length;
    }
    ~Packet::Packet(){
        if(should_free_memory) delete[] data;
    }
    uint8_t Packet::nextUInt8(){
        if(!length) return 0;
        data += sizeof(uint8_t);
        length -= sizeof(uint8_t);
        return *(reinterpret_cast<uint8_t*>(data)-1);
    }
    int Packet::nextInt(){
        if(!length) return 0;
        data += sizeof(int);
        length -= sizeof(int);
        return *(reinterpret_cast<int*>(data)-1);
    }
    MDB_val Packet::nextMDBVal(){
        if(!length) return MDB_val();
        MDB_val b{*reinterpret_cast<size_t*>(data), data + sizeof(size_t)};
        data += sizeof(size_t) + b.mv_size;
        return b;
    } 
*/
