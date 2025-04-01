#include <iostream>
#include <vector>
#include <cstdlib> // For random errors
#include <fstream> // For saving logs to a file
#include <string>

using namespace std;

// Abstract Base Class for Memory
class Memory {
public:
    virtual void read(int address) = 0; // Abstract read function
    virtual void write(int address, int data) = 0; // Abstract write function
    virtual ~Memory() {} // Virtual destructor
};

// Logger Class
class Logger {
private:
    vector<string> logs; // Logs for operations
    int invalidAddressErrors = 0;
    int writeProtectionErrors = 0;
    int bitFlipErrors = 0;

public:
    void logOperation(const string& log) {
        logs.push_back(log);
    }

    void incrementInvalidAddressError() {
        invalidAddressErrors++;
    }

    void incrementWriteProtectionError() {
        writeProtectionErrors++;
    }

    void incrementBitFlipError() {
        bitFlipErrors++;
    }

    void printLogs() {
        cout << "\n--- Simulation Logs ---" << endl;
        for (const auto& log : logs) {
            cout << log << endl;
        }
    }

    void printErrorStatistics() {
        cout << "\n--- Error Statistics ---" << endl;
        cout << "Invalid Address Errors: " << invalidAddressErrors << endl;
        cout << "Write Protection Errors: " << writeProtectionErrors << endl;
        cout << "Bit-Flip Errors: " << bitFlipErrors << endl;
    }

    void saveLogsToFile(const string& filename) {
        ofstream file(filename);
        if (!file) {
            cout << "Error: Unable to save logs to file." << endl;
            return;
        }
        for (const auto& log : logs) {
            file << log << endl;
        }
        file.close();
        cout << "Logs saved to file: " << filename << endl;
    }
};

// TTL RAM Class
class TTLLRAM : public Memory {
private:
    vector<int> memory;
    Logger* logger;

public:
    TTLLRAM(int size, Logger* logger) : memory(size, 0), logger(logger) {}

    void read(int address) override {
        if (address < 0 || address >= memory.size()) {
            logger->logOperation("Error: TTL RAM Invalid Address Access");
            logger->incrementInvalidAddressError();
            return;
        }
        if (rand() % 10 == 0) { // Simulate bit-flip error (10% chance)
            int corruptedData = memory[address] ^ (1 << (rand() % 8)); // Flip a random bit
            logger->logOperation("TTL RAM Read (Corrupted): Address = " + to_string(address) + ", Data = " + to_string(corruptedData));
            logger->incrementBitFlipError();
        } else {
            logger->logOperation("TTL RAM Read: Address = " + to_string(address) + ", Data = " + to_string(memory[address]));
        }
    }

    void write(int address, int data) override {
        if (address < 0 || address >= memory.size()) {
            logger->logOperation("Error: TTL RAM Invalid Address Access");
            logger->incrementInvalidAddressError();
            return;
        }
        memory[address] = data;
        logger->logOperation("TTL RAM Write: Address = " + to_string(address) + ", Data = " + to_string(data));
    }
};

// MOS RAM Class
class MOSRAM : public Memory {
private:
    vector<int> memory;
    Logger* logger;

public:
    MOSRAM(int size, Logger* logger) : memory(size, 0), logger(logger) {}

    void read(int address) override {
        if (address < 0 || address >= memory.size()) {
            logger->logOperation("Error: MOS RAM Invalid Address Access");
            logger->incrementInvalidAddressError();
            return;
        }
        logger->logOperation("MOS RAM Read: Address = " + to_string(address) + ", Data = " + to_string(memory[address]));
    }

    void write(int address, int data) override {
        if (address < 0 || address >= memory.size()) {
            logger->logOperation("Error: MOS RAM Invalid Address Access");
            logger->incrementInvalidAddressError();
            return;
        }
        memory[address] = data;
        logger->logOperation("MOS RAM Write: Address = " + to_string(address) + ", Data = " + to_string(data));
    }
};

// Synchronous DRAM Class
class SDRAM : public Memory {
private:
    vector<int> memory;
    Logger* logger;

public:
    SDRAM(int size, Logger* logger) : memory(size, 0), logger(logger) {}

    void read(int address) override {
        if (address < 0 || address >= memory.size()) {
            logger->logOperation("Error: SDRAM Invalid Address Access");
            logger->incrementInvalidAddressError();
            return;
        }
        logger->logOperation("SDRAM Read (Synchronous): Address = " + to_string(address) + ", Data = " + to_string(memory[address]));
    }

    void write(int address, int data) override {
        if (address < 0 || address >= memory.size()) {
            logger->logOperation("Error: SDRAM Invalid Address Access");
            logger->incrementInvalidAddressError();
            return;
        }
        memory[address] = data;
        logger->logOperation("SDRAM Write (Synchronous): Address = " + to_string(address) + ", Data = " + to_string(data));
    }
};

// Asynchronous DRAM Class
class ADRAM : public Memory {
private:
    vector<int> memory;
    Logger* logger;

public:
    ADRAM(int size, Logger* logger) : memory(size, 0), logger(logger) {}

    void read(int address) override {
        if (address < 0 || address >= memory.size()) {
            logger->logOperation("Error: ADRAM Invalid Address Access");
            logger->incrementInvalidAddressError();
            return;
        }
        logger->logOperation("ADRAM Read (Asynchronous): Address = " + to_string(address) + ", Data = " + to_string(memory[address]));
    }

    void write(int address, int data) override {
        if (address < 0 || address >= memory.size()) {
            logger->logOperation("Error: ADRAM Invalid Address Access");
            logger->incrementInvalidAddressError();
            return;
        }
        memory[address] = data;
        logger->logOperation("ADRAM Write (Asynchronous): Address = " + to_string(address) + ", Data = " + to_string(data));
    }
};

// PROM Class
class PROM : public Memory {
private:
    vector<int> memory;
    bool programmed = false;
    Logger* logger;

public:
    PROM(int size, Logger* logger) : memory(size, 0), logger(logger) {}

    void read(int address) override {
        if (address < 0 || address >= memory.size()) {
            logger->logOperation("Error: PROM Invalid Address Access");
            logger->incrementInvalidAddressError();
            return;
        }
        logger->logOperation("PROM Read: Address = " + to_string(address) + ", Data = " + to_string(memory[address]));
    }

    void write(int address, int data) override {
        if (programmed) {
            logger->logOperation("Error: PROM Already Programmed and Cannot be Modified");
            logger->incrementWriteProtectionError();
            return;
        }
        if (address < 0 || address >= memory.size()) {
            logger->logOperation("Error: PROM Invalid Address Access");
            logger->incrementInvalidAddressError();
            return;
        }
        memory[address] = data;
        programmed = true;
        logger->logOperation("PROM Programming: Address = " + to_string(address) + ", Data = " + to_string(data));
    }
};

// EPROM Class
class EPROM : public Memory {
private:
    vector<int> memory;
    Logger* logger;

public:
    EPROM(int size, Logger* logger) : memory(size, 0), logger(logger) {}

    void read(int address) override {
        if (address < 0 || address >= memory.size()) {
            logger->logOperation("Error: EPROM Invalid Address Access");
            logger->incrementInvalidAddressError();
            return;
        }
        logger->logOperation("EPROM Read: Address = " + to_string(address) + ", Data = " + to_string(memory[address]));
    }

    void erase() {
        logger->logOperation("EPROM Erased Successfully");
    }

    void write(int address, int data) override {
        logger->logOperation("Error: EPROM Write Not Allowed, Erase Required");
    }
};

// EEPROM Class
class EEPROM : public Memory {
private:
    vector<int> memory;
    Logger* logger;

public:
    EEPROM(int size, Logger* logger) : memory(size, 0), logger(logger) {}

    void read(int address) override {
        if (address < 0 || address >= memory.size()) {
            logger->logOperation("Error: EEPROM Invalid Address Access");
            logger->incrementInvalidAddressError();
            return;
        }
        logger->logOperation("EEPROM Read: Address = " + to_string(address) + ", Data = " + to_string(memory[address]));
    void write(int address, int data) override {
        if (address < 0 || address >= memory.size()) {
            logger->logOperation("Error: EEPROM Invalid Address Access");
            logger->incrementInvalidAddressError();
            return;
        }
        memory[address] = data;
        logger->logOperation("EEPROM Write: Address = " + to_string(address) + ", Data = " + to_string(data));
    }
};

// Flash Memory Class
class FlashMemory : public Memory {
private:
    vector<int> memory;
    vector<bool> writeProtected;
    Logger* logger;

public:
    FlashMemory(int size, Logger* logger) : memory(size, 0), writeProtected(size, false), logger(logger) {}

    void protectBlock(int address) {
        if (address < 0 || address >= memory.size()) {
            logger->logOperation("Error: Flash Memory Invalid Address for Protection");
            logger->incrementInvalidAddressError();
            return;
        }
        writeProtected[address] = true;
        logger->logOperation("Flash Memory Block Protected: Address = " + to_string(address));
    }

    void read(int address) override {
        if (address < 0 || address >= memory.size()) {
            logger->logOperation("Error: Flash Memory Invalid Address Access");
            logger->incrementInvalidAddressError();
            return;
        }
        logger->logOperation("Flash Memory Read: Address = " + to_string(address) + ", Data = " + to_string(memory[address]));
    }

    void write(int address, int data) override {
        if (address < 0 || address >= memory.size()) {
            logger->logOperation("Error: Flash Memory Invalid Address Access");
            logger->incrementInvalidAddressError();
            return;
        }
        if (writeProtected[address]) {
            logger->logOperation("Error: Flash Memory Block is Write-Protected: Address = " + to_string(address));
            logger->incrementWriteProtectionError();
            return;
        }
        memory[address] = data;
        logger->logOperation("Flash Memory Write: Address = " + to_string(address) + ", Data = " + to_string(data));
    }
};
int main() {
    Logger logger;

    // Create memory objects for all types
    TTLLRAM ttlRam(16, &logger);
    MOSRAM mosRam(16, &logger);
    SDRAM sdram(16, &logger);
    ADRAM adram(16, &logger);
    PROM prom(16, &logger);
    EPROM eprom(16, &logger);
    EEPROM eeprom(16, &logger);
    FlashMemory flashMemory(16, &logger);

    // Simulate TTL RAM Operations
    ttlRam.write(5, 10);
    ttlRam.read(5);
    ttlRam.read(20); // Invalid address

    // Simulate MOS RAM Operations
    mosRam.write(8, 20);
    mosRam.read(8);
    mosRam.read(18); // Invalid address

    // Simulate SDRAM Operations
    sdram.write(10, 30);
    sdram.read(10);

    // Simulate ADRAM Operations
    adram.write(12, 40);
    adram.read(12);

    // Simulate PROM Operations
    prom.write(3, 50);
    prom.write(4, 60); // Already programmed

    // Simulate EPROM Operations
    eprom.read(2);
    eprom.erase();
    eprom.write(2, 70); // Write not allowed

    // Simulate EEPROM Operations
    eeprom.write(7, 70);
    eeprom.read(7);

    // Simulate Flash Memory Operations
    flashMemory.write(9, 80);
    flashMemory.read(9);
    flashMemory.protectBlock(9);
    flashMemory.write(9, 90); // Write-protected block

    // Print logs and error statistics
    logger.printLogs();
    logger.printErrorStatistics();
    logger.saveLogsToFile("simulation_logs.txt");

    return 0;
}
