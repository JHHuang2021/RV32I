#include "include/cache.h"
Cache::Cache(unsigned char *mem) { this->mem = mem; }

void Cache::Write(int addr, unsigned char value) {
    int a = addr % CACHE;
    if (cache[a].key != addr && cache[a].dirty) {
        mem[cache[a].key] = cache[a].value;
        cache[a].dirty = false;
    }
    cache[a].key = addr;
    cache[a].dirty = true;
    cache[a].value = value;
}

unsigned char Cache::Read(int addr) {
    int a = addr % CACHE;
    if (cache[a].key == addr) return cache[a].value;
    if (cache[a].key != addr && cache[a].dirty) {
        mem[cache[a].key] = cache[a].value;
        cache[a].dirty = false;
    }
    cache[a].key = addr;
    cache[a].dirty = false;
    cache[a].value = mem[addr];
    return cache[a].value;
}