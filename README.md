# Cache Simulation
Blake Owen

# Summary
This program is designed to simulate various cache configurations. It takes a trace file as input (with memory reads and writes included) and prompts the user if they would like to run the automated tests. If not, the user is prompted for specific system specification they would like to simulate.

# Features
- Variable Cache Size, Line Size and Set Associativity
- Support for FIFO and LRU
- Automated testing using a range of cache designs
- API for conversion of bitset address to integer tags/sets
- Works for trace files obtained using valgrind lackey
