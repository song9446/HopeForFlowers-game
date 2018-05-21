#include <random>

namespace tool {
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
    char* randStr(int len){
        char* s = new char[len];    
        for(int i=0; i<len; ++i){
            s[i] = rand(static_cast<int>('a'), static_cast<int>('f'));
        }
    }
};
