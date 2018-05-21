//#include "lmdb.h"
#include <cstdint>
#include <cstring>
#include <stdio.h>
typedef struct MDB_val {
    size_t  mv_size;
    void *  mv_data;
} MDB_val;
class Packet {
public:
    enum class OP: int{
        ERROR=-1, ECHO=0, JOIN=1, LOGIN=2  
    };
private:
    char* data=NULL;
    size_t cursor=sizeof(OP);
    size_t length=0;

    int sumUpLength(){ return 0; }
    template <class T, class ...Rest>
    int sumUpLength(T& t, Rest... rest){
        return sizeof(t) + sumUpLength(rest...);
    }
    template <class ...Rest>
    int sumUpLength(char* t, size_t l, Rest... rest){
        return l + sumUpLength(rest...);
    }
    template <class ...Rest>
    int sumUpLength(const char* t, size_t l, Rest... rest){
        return l + sumUpLength(rest...);
    }
    template <class ...Rest>
    int sumUpLength(size_t l, char* t, Rest... rest){
        return sizeof(l) + l + sumUpLength(rest...);
    }
    template <class ...Rest>
    int sumUpLength(size_t l, const char* t, Rest... rest){
        return sizeof(l) + l + sumUpLength(rest...);
    }
    template <class ...Rest>
    void push(size_t length, char* message, Rest... rest){
        memcpy(data+cursor, &length, sizeof(length));
        cursor += sizeof(length);
        memcpy(data+cursor, message, length);
        cursor += length;
        push(rest...);
    }
    template <class ...Rest>
    void push(size_t length, const char* message, Rest... rest){
        memcpy(data+cursor, &length, sizeof(length));
        cursor += sizeof(length);
        memcpy(data+cursor, message, length);
        cursor += length;
        push(rest...);
    }
    template <class ...Rest>
    void push(char* message, size_t length, Rest... rest){
        memcpy(data+cursor, message, length);
        cursor += sizeof(length);
        push(rest...);
    }
    template <class ...Rest>
    void push(const char* message, size_t length, Rest... rest){
        memcpy(data+cursor, message, length);
        cursor += sizeof(length);
        push(rest...);
    }
    template <class T, class ...Rest>
    void push(T& t, Rest... rest){
        printf("not used this asdfasdfasdf\n");
        memcpy(data+cursor, &t, sizeof(t));
        cursor += sizeof(t);
        push(rest...);
    }
    void push(){
        cursor = sizeof(OP);
    }
public:
    Packet(void *data_, size_t length): length(length) { 
        data = new char[length];
        memcpy(data, data_, length);
    }
    ~Packet(){
        if(data) delete[] data;
    }
    template <class ...Rest>
    Packet(OP op, Rest... rest){
        length = sumUpLength(op, rest...);
        data = new char[length];
        memcpy(data, &op, sizeof(op));
        cursor = sizeof(op);
        push(rest...);
    }
    uint8_t nextUInt8(int len){
        if(!length) return 0;
        cursor += sizeof(uint8_t)*len;
        return *(reinterpret_cast<uint8_t*>(data + cursor)-sizeof(uint8_t)*len);
    }
    uint8_t nextUInt8(){
        if(!length) return 0;
        cursor += sizeof(uint8_t);
        return *(reinterpret_cast<uint8_t*>(data + cursor)-1);
    }
    int nextInt(){
        if(!length) return 0;
        cursor += sizeof(int);
        return *(reinterpret_cast<int*>(data + cursor)-1);
    }
    MDB_val nextMDBVal(){
        if(!length) return MDB_val();
        printf("cursor: %d, size_t : %d\n", cursor, *reinterpret_cast<size_t*>(data + cursor));
        MDB_val b{*reinterpret_cast<size_t*>(data + cursor), data + cursor + sizeof(size_t)};
        cursor += sizeof(size_t) + b.mv_size;
        return b;
    } 
    void rewind(){ cursor = sizeof(OP); }
    OP getOP(){ return length? *reinterpret_cast<OP*>(data) : OP::ERROR; }
    void setOP(OP op){ if(length) *reinterpret_cast<OP*>(data) = op; }
    char* bytes(){ return data; }
    char* getData(){ return data + sizeof(OP); }
    size_t size(){ return length; }
    size_t getDataSize(){ return length-sizeof(OP); }
};
