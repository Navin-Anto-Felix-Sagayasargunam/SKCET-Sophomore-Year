#include <iostream>
#include <vector>
#include <unordered_map>
#include <deque>
#include <ctime>
#include <cstdlib>

using namespace std;

// Helper Functions for Randomization
int getRandomIndex(int size) {
    return rand() % size;
}

// Cache Block Structure
struct CacheBlock {
    int tag;
    int data;
    bool valid;
    bool dirty; // For Write-back policy
    CacheBlock() : tag(-1), data(0), valid(false), dirty(false) {}
};

// Replacement Policies
enum ReplacementPolicy {
    LRU,
    FIFO,
    LFU,
    Random
};

// Write Policies
enum WritePolicy {
    WriteThrough,
    BufferedWriteThrough,
    WriteBack
};

// Mapping Functions
enum MappingFunction {
    Direct,
    Associative,
    SetAssociative
};

// Cache Coherency Mechanisms
enum CoherencyMechanism {
    BusWatching,
    HardwareTransparency,
    CacheFlushing
};

// Cache Memory Class
class CacheMemory {
private:
    vector<CacheBlock> cache;
    vector<int> mainMemory; // Simulated DRAM
    unordered_map<int, int> accessFrequency; // For LFU policy
    deque<int> fifoQueue; // For FIFO policy
    vector<bool> nonCacheableMemory; // Tracks non-cacheable memory regions
    vector<int> writeBuffer; // For Buffered Write-Through

    int cacheSize;
    int blockSize;
    int mainMemorySize;
    int associativity; // For set-associative mapping
    ReplacementPolicy replacementPolicy;
    WritePolicy writePolicy;
    MappingFunction mappingFunction;
    CoherencyMechanism coherencyMechanism;

    void fetchBlockFromMemory(int address, int blockNumber, int setIndex, bool isWrite, int writeData);

public:
    CacheMemory(int cacheSize, int blockSize, int mainMemorySize, int associativity,
                ReplacementPolicy replacementPolicy, WritePolicy writePolicy,
                MappingFunction mappingFunction, CoherencyMechanism coherencyMechanism)
        : cacheSize(cacheSize), blockSize(blockSize), mainMemorySize(mainMemorySize),
          associativity(associativity), replacementPolicy(replacementPolicy),
          writePolicy(writePolicy), mappingFunction(mappingFunction),
          coherencyMechanism(coherencyMechanism) {
        cache.resize(cacheSize);
        mainMemory.resize(mainMemorySize, 0); // Initialize main memory
        nonCacheableMemory.resize(mainMemorySize, false); // Default: all memory cacheable
    }

    void markNonCacheableMemory(int start, int end) {
        for (int i = start; i <= end; i++) {
            nonCacheableMemory[i] = true;
        }
    }

    void accessMemory(int address, int writeData = -1, bool isWrite = false) {
        // Handle Non-Cacheable Memory
        if (nonCacheableMemory[address]) {
            cout << "Accessing Non-Cacheable Memory: Address = " << address;
            if (isWrite) {
                mainMemory[address] = writeData;
                cout << ", Write Data = " << writeData << endl;
            } else {
                cout << ", Read Data = " << mainMemory[address] << endl;
            }
            return;
        }

        int blockNumber = address / blockSize;
        int offset = address % blockSize;
        int setIndex = blockNumber % cacheSize; // Default for direct mapping
        if (mappingFunction == Associative) setIndex = 0; // Ignore index for fully associative
        if (mappingFunction == SetAssociative) setIndex = blockNumber % associativity;

        if (cache[setIndex].valid && cache[setIndex].tag == blockNumber) {
            // Cache Hit
            cout << "Cache Hit: Address = " << address << ", Data = " << cache[setIndex].data << endl;
            if (isWrite) {
                cache[setIndex].data = writeData;
                if (writePolicy == WriteBack) cache[setIndex].dirty = true;
                if (writePolicy == WriteThrough) mainMemory[address] = writeData;
                if (writePolicy == BufferedWriteThrough) writeBuffer.push_back(address);
            }
        } else {
            // Cache Miss
            cout << "Cache Miss: Address = " << address << endl;

            // Write-back policy: Write dirty block to memory during eviction
            if (cache[setIndex].valid && cache[setIndex].dirty && writePolicy == WriteBack) {
                int mainMemoryAddress = cache[setIndex].tag * blockSize;
                mainMemory[mainMemoryAddress] = cache[setIndex].data;
                cout << "Write-Back: Address = " << mainMemoryAddress << ", Data = " << cache[setIndex].data << endl;
            }

            fetchBlockFromMemory(address, blockNumber, setIndex, isWrite, writeData);
        }

        // Replacement policies
        if (replacementPolicy == LFU) accessFrequency[blockNumber]++;
        if (replacementPolicy == FIFO) fifoQueue.push_back(blockNumber);
    }

    void fetchBlockFromMemory(int address, int blockNumber, int setIndex, bool isWrite, int writeData) {
        int blockStartAddress = blockNumber * blockSize;
        cache[setIndex].tag = blockNumber;
        cache[setIndex].valid = true;
        cache[setIndex].dirty = false; // Reset dirty bit
        cache[setIndex].data = mainMemory[blockStartAddress];
        cout << "Fetched Block: Address = " << blockStartAddress << ", Data = " << cache[setIndex].data << endl;

        if (isWrite) {
            cache[setIndex].data = writeData; // Write data to the block
            if (writePolicy == WriteBack) cache[setIndex].dirty = true;
        }
    }

    void flushCache() {
        cout << "\nFlushing Cache..." << endl;
        for (int i = 0; i < cacheSize; i++) {
            if (cache[i].valid && cache[i].dirty && writePolicy == WriteBack) {
                int mainMemoryAddress = cache[i].tag * blockSize;
                mainMemory[mainMemoryAddress] = cache[i].data;
                cout << "Flushed Dirty Block: Address = " << mainMemoryAddress << ", Data = " << cache[i].data << endl;
                cache[i].dirty = false;
            }
        }
    }

    void printCacheStatus() {
        cout << "\n--- Cache Status ---" << endl;
        for (int i = 0; i < cacheSize; i++) {
            cout << "Cache Block " << i << ": Tag = " << cache[i].tag << ", Data = " << cache[i].data
                 << ", Valid = " << cache[i].valid << ", Dirty = " << cache[i].dirty << endl;
        }
    }

    void printMainMemory() {
        cout << "\n--- Main Memory Status ---" << endl;
        for (int i = 0; i < mainMemorySize; i++) {
            cout << "Address " << i << ": Data = " << mainMemory[i] << endl;
        }
    }
};

int main() {
    srand(static_cast<unsigned>(time(0))); // Seed for random replacement policy

    CacheMemory cacheMemory(
        4, // Cache Size
        16, // Block Size
        256, // Main Memory Size
        2, // Associativity
        ReplacementPolicy::LRU, // Replacement Policy: LRU
        WritePolicy::BufferedWriteThrough, // Write Policy: Buffered Write-Through
        MappingFunction::SetAssociative, // Mapping Function: Set-Associative Mapping
        CoherencyMechanism::BusWatching // Coherency Mechanism: Bus Watching
    );

    cacheMemory.markNonCacheableMemory(240, 255); // Mark addresses 240-255 as non-cacheable

    cacheMemory.accessMemory(20); // Read from address 20
    cacheMemory.accessMemory(36); // Read from address 36
    cacheMemory.accessMemory(52, 25, true); // Write to address 52
    cacheMemory.accessMemory(20); // Read from address 20
    cacheMemory.accessMemory(240); // Access non-cacheable memory (read)
    cacheMemory.accessMemory(245, 50, true); // Access non-cacheable memory (write)

    cacheMemory.flushCache(); // Flush the cache

    cacheMemory.printCacheStatus();
    cacheMemory.printMainMemory();

    return 0;
}
