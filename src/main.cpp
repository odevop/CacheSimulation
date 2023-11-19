#include <iostream>
#include <string>
#include <math.h>
#include "cache.h"

using namespace std;

int main() {

    unsigned int cacheSize, lineSize, setSize, LRU;

    string fileName;

    cout << "Filename to open: ";
    cin >> fileName;

    string autoTest;
    cout << "Perform automated tests? (Y/N) ";
    cin >> autoTest;

    if (autoTest == "Y") {
        // Automated tests: Outermost loop determines the cache size
        for (int i = 9; i <= 14; i++) {
            cacheSize = pow(2, i);

            // Loop to determine set size for associativity (0, 1, 2, 4, 8)
            vector<int> vSet = {0, 1, 2, 4, 8};
            for (auto setIter = vSet.begin(); setIter != vSet.end(); ++setIter) {
                setSize = *setIter;

                // If direct map, only test block size and continue:
                if (setSize == 1) {
                    Cache c32DIRECT(cacheSize, 32, setSize, 0);
                    Cache c64DIRECT(cacheSize, 64, setSize, 0);

                    c32DIRECT.loadFile(fileName);
                    c64DIRECT.loadFile(fileName);

                    c32DIRECT.HitRatio();
                    c64DIRECT.HitRatio();

                    continue;
                }

                // Create cahce for current cache size and set size,
                // vary the block size as well as replacement strategy:
                Cache c32FIFO(cacheSize, 32, setSize, 0);
                Cache c32LRU(cacheSize, 32, setSize, 1);
                Cache c64FIFO(cacheSize, 64, setSize, 0);
                Cache c64LRU(cacheSize, 64, setSize, 1);

                // Load test file into each simulated cache:
                c32FIFO.loadFile(fileName);
                c32LRU.loadFile(fileName);
                c64FIFO.loadFile(fileName);
                c64LRU.loadFile(fileName);

                // Print the results of each cache:
                c32FIFO.HitRatio();
                c32LRU.HitRatio();
                c64FIFO.HitRatio();
                c64LRU.HitRatio();
            }
        }
    } else {
        cout << "Enter the cache size in bytes: ";
        cin >> cacheSize;

        cout << "Enter the line size in bytes: ";
        cin >> lineSize;

        cout << "Enter the size of each set \n(1 for direct map. 0 for fully associative. All else is n-way set associative): ";
        cin >> setSize;

        cout << "Enter 0 for FIFO replacement, 1 for LRU replacement: ";
        cin >> LRU;

        Cache c(cacheSize, lineSize, setSize, LRU);

        c.loadFile(fileName);

        c.HitRatio();
    }

    return 0;
}