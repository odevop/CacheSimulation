/************************************************************************************************/
/*                                   Cache Simultor                                             */
/*                                                                                              */
/*                                     Blake Owen                                               */
/*                                                                                              */
/*       This program is designed to simulate various cache configurations. It takes a trace
/*       file as input (with memory reads and writes included) and prompts the user for
/*           */
/************************************************************************************************/
#include "cache.h"
#include <math.h>
#include <bitset>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

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

    // Size of offset field
    offset = log2(lSize); 

    // Cache Specs:
    // cout << "Total Lines: " << linesCache << endl;
    // cout << "Set Field Size: " << setField << endl;
    // cout << "Offset: " << offset << endl;

    isLRU = (LRU == 0 ? false : true);

    // Allocate space onto cache for each line
    for (int i = 0; i < linesCache; i++) {
        Line newLine;
        newLine.set = sSize == 0 ? 0 : i / sSize;
        cache.push_back(newLine);
    }
}

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

bool Cache::loadFile(string fileName) {
    ifstream file(fileName);
    

    if (!file.is_open()) {
        cout << "Error reading file" << endl;
        return false;
    }

    string ls, addr, bytes;

    int i = 0;

    while (!file.eof()) {
        getline(file, ls, ' ');
        getline(file, addr, ' ');

        if (file.eof()) {
            break;
        }

        stringstream hexStream;
        hexStream << hex << addr;

        unsigned int bits;
        hexStream >> bits;

        bitset<32> bitAddr(bits);

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
                        cache.at(j).counter++;
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
                    newLine.counter = cache.at(firstIndex).counter + 1;
                    cache.at(firstIndex) = newLine;
                    missCount++;
                }
            }
            
        }

        i++;
    }
}

void Cache::HitRatio() {
    //cout << "Specs: " << cacheSize << " " << lineSize << " " << setSize << " " << isLRU << endl;
    //cout << "Hits: " << hitCount << " Misses: " << missCount << endl;
    cout << (float)hitCount / ((float)(hitCount) + (float)missCount) << endl;
}