#include <vector>
#include <string>
#include <bitset>
#include <map>

#pragma once

using namespace std;

// Structure for each address line
struct Line {
    bitset<32> address;
    int set = -1;
    int tag = -1;
    int counter = 0;
};

class Cache {
    vector<Line> cache;

    unsigned int cacheSize; // Size of the cache (bytes)
    unsigned int lineSize; // Size of each line (bytes)
    unsigned int setSize; // Size of each set (number of lines per set)

    unsigned int linesCache; // Number of lines in the cache

    int offset; // Size of the offset field (in bits)
    //int lineField; // Size of line field (in bits)
    int setField; // Size of each set field (in bits). Determines associativity

    bool isLRU; // Indicates if LRU (Least Recently Used) is enabled. If false, defaults to FIFO.

    unsigned int hitCount = 0;
    unsigned int missCount = 0;

    public:
        Cache(unsigned int cSize, 
              unsigned int lSize, 
              unsigned int sSize,
              unsigned int LRU);

        int getTag(bitset<32> address);
        int getSet(bitset<32> address);

        void HitRatio();

        bool loadFile(string fileName);
};