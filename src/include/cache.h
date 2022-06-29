#ifndef CACHE_H
#define CACHE_H
const int CACHE = 4096;
class Cache {
   private:
    struct CacheItem {
        int key = -1;  // address on mem
        unsigned char value;
        bool dirty = false;
    };
    CacheItem cache[CACHE];
    unsigned char *mem;

   public:
    Cache(unsigned char *mem);
    void Write(int addr, unsigned char value);
    unsigned char Read(int addr);
};

#endif