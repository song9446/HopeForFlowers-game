#include <random>
#include <openssl/sha.h>
#include <stdio.h>
namespace tool {
    char* sha256(const char *string, size_t len, char outputBuffer[64]){
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, string, len);
        SHA256_Final(hash, &sha256);
        int i = 0;
        for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
            sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
        }
        outputBuffer[64] = 0;
    }
    int rand(const int& f, const int& t){
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist(f,t);
        return dist(mt);
    }
    template<typename T>
    const T& randomSelect(const T* pool, int size){
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist(0,size-1);
        return pool[dist(mt)];
    } 
    void randStr(char* s, int len){
        for(int i=0; i<len; ++i){
            switch(rand(0, 2)){
            case 0:
                s[i] = (char)rand(static_cast<int>('a'), static_cast<int>('z'));
                break;
            case 1:
                s[i] = (char)rand(static_cast<int>('A'), static_cast<int>('Z'));
                break;
            case 2:
                s[i] = (char)rand(static_cast<int>('0'), static_cast<int>('9'));
                break;
            }
        }
    }
    char* randStr(int len){
        char* s = new char[len];
        for(int i=0; i<len; ++i){
            switch(rand(0, 2)){
            case 0:
                s[i] = rand(static_cast<int>('a'), static_cast<int>('z'));
                break;
            case 1:
                s[i] = rand(static_cast<int>('A'), static_cast<int>('Z'));
                break;
            case 2:
                s[i] = rand(static_cast<int>('1'), static_cast<int>('0'));
                break;
            }
        }
        return s;
    }
};
