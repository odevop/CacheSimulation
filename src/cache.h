/************************************************************************************************/
/*                                 Cache Simultor Class                                         */
/************************************************************************************************/
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

// Simulated cache object
class Cache {
    // Lines of the simulated cache
    vector<Line> cache;

    unsigned int cacheSize; // Size of the cache (bytes)
    unsigned int lineSize; // Size of each line (bytes)
    unsigned int setSize; // Size of each set (number of lines per set)

    unsigned int linesCache; // Number of lines in the cache

    int offset; // Size of the offset field (in bits)
    int setField; // Size of each set field (in bits). Determines associativity

    bool isLRU; // Indicates if LRU (Least Recently Used) is enabled. If false, defaults to FIFO.

    unsigned int hitCount = 0; // Number of hits
    unsigned int missCount = 0; // Number of misses

    public:
        // Constructor
        Cache(unsigned int cSize, 
              unsigned int lSize, 
              unsigned int sSize,
              unsigned int LRU = 0);

        // Returns the bitset tag of the address
        int getTag(bitset<32> address);

        // Returns the bitset set of the address
        int getSet(bitset<32> address);

        // Loads file and inputs addresses to cache, tracking hit and miss counts
        bool loadFile(string fileName);

        // Calculates and prints the hit ratio
        void HitRatio();
};