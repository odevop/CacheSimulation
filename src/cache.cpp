/************************************************************************************************/
/*                                 Cache Simultor Methods                                       */
/************************************************************************************************/
#include "cache.h"
#include <math.h>
#include <bitset>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

using namespace std;

// Constructor. Inputs are: cache size (bytes), line size (bytes), set size (lines per set) LRU (1 for enabled, 0 for disabled)
Cache::Cache(unsigned int cSize, unsigned int lSize, unsigned int sSize, unsigned int LRU) {
    linesCache = cSize / lSize; // Lines in cache
    
    // Size of each set field. Fully associative sets all lines to the same set, direct sets each line to a different set.
    // Direct map: log2(linesCache) -> or -1
    // Fully associative: 0
    // N-way associative: log2(cSize / (lSize * sSize))
    if (sSize == 0) {
        setField = 0;
    } else if (sSize <= linesCache) {
        setField = log2(cSize / (lSize * sSize));
    } else {
        sSize = linesCache;
        setField = log2(linesCache);
    }

    cacheSize = cSize; // Bytes per cache
    lineSize = lSize; // Bytes per line
    setSize = sSize; // Lines per set

    offset = log2(lSize); // Size of offset field

    isLRU = (LRU == 0 ? false : true); // LRU is enabled or disabled

    // Allocate space onto cache for each line
    for (int i = 0; i < linesCache; i++) {
        Line newLine;
        newLine.set = sSize == 0 ? 0 : i / sSize;
        cache.push_back(newLine);
    }
}

// Returns the bitset tag of the address
int Cache::getTag(bitset<32> bitAddr) {
    // Calculate tag integer
    int tagInt = 0;
    int currentPower = 0;
    for (int j = offset + setField; j < 32; j++) {
        tagInt += bitAddr.test(j) * pow(2, currentPower);

        currentPower++;
    }

    return tagInt;
}

// Returns the bitset set of the address
int Cache::getSet(bitset<32> bitAddr) {
    // Calculate set integer
    int setInt = 0;
    int currentPower = 0;
    for (int j = offset; j < offset + setField; j++) {
        setInt += bitAddr.test(j) ? pow(2, currentPower) : 0;

        currentPower++;
    }

    return setInt;
}

// Loads file and inputs addresses to cache, tracking hit and miss counts
bool Cache::loadFile(string fileName) {
    ifstream file(fileName);
    
    if (!file.is_open()) {
        cout << "Error reading file" << endl;
        return false;
    }

    string ls, addr, bytes; // Initialize each value of row (load/store, address, and number of bytes)

    int i = 0; // Number of cache checks

    int fifoCounter = 0;

    while (!file.eof()) {
        getline(file, ls, ' ');
        getline(file, addr, ' ');

        if (file.eof()) {
            break;
        }

        // Convert hex address to bitset
        stringstream hexStream;
        hexStream << hex << addr;

        unsigned int bits;
        hexStream >> bits;

        bitset<32> bitAddr(bits);

        // Store the address, set, and tag to the new line object
        Line newLine;
        newLine.address = bitAddr;
        newLine.set = getSet(bitAddr);
        newLine.tag = getTag(bitAddr);
        newLine.counter = 0;

        getline(file, bytes);

        // Find the block in the cache, utilizing proper replacement strategy
        // Case for direct map:
        if (setSize == 1) {
            if (cache.at(newLine.set).tag == newLine.tag) {
                hitCount++;
            } else {
                cache.at(newLine.set) = newLine;
                missCount++;
            }
        } 
        // Case for associative (using selected replacement strategy):
        else {
            // Least Recently Used Strategy (LRU)
            if (isLRU) {
                int minIndex = newLine.set;
                bool hit = false;
                for (int j = newLine.set; j < (setSize == 0 ? linesCache : newLine.set + setSize); j++) {   
                    // Case for hit (LRU)               
                    if (cache.at(j).tag == newLine.tag) {
                        cache.at(j).counter = i + 1;
                        hitCount++;
                        hit = true;
                        break;
                    } 
                    
                    if (cache.at(j).counter < cache.at(minIndex).counter) {
                        minIndex = j;
                    }
                }

                // Case for miss (LRU)
                if (!hit) {
                    newLine.counter = i + 1;
                    cache.at(minIndex) = newLine;
                    missCount++;
                }
            }
            // First In First Out Strategy (FIFO)
            else {
                int firstIndex = newLine.set;
                bool hit = false;
                for (int j = newLine.set; j < (setSize == 0 ? linesCache : newLine.set + setSize); j++) { 
                    // Case for hit (FIFO)              
                    if (cache.at(j).tag == newLine.tag) {
                        //cache.at(j).counter++;
                        hitCount++;
                        hit = true;
                        break;
                    }

                    if (cache.at(j).counter < cache.at(firstIndex).counter) {
                        firstIndex = j;
                    }
                }

                // Case for miss (FIFO)
                if (!hit) {
                    newLine.counter = fifoCounter++;
                    cache.at(firstIndex) = newLine;
                    missCount++;
                }
            }
            
        }

        i++; // Iterate number of cache checks
    }

    file.close();

    return true;
}

// Calculates and prints the hit ratio
void Cache::HitRatio() {
    cout << "\nSpecs: " << cacheSize << " byte cache, " 
    << lineSize << " byte lines, " << linesCache << " lines in cache, " 
    <<  (setSize == 0 ? linesCache : setSize) << " lines per set, " 
    << (setSize != 1 ? (isLRU ? "LRU replacement." : "FIFO replacement.") : "Direct map.") << endl;

    cout << "Hits: " << hitCount << " Misses: " << missCount << endl;
    cout << "Hit Ratio: " << fixed << setprecision(6) << (float)hitCount / ((float)(hitCount) + (float)missCount) << endl;
}