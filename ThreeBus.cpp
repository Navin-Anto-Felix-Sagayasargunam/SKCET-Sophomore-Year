#include <iostream>
#include <vector>
#include <map>

using namespace std;

// Control Signals
enum ControlSignal {
    LOAD_REG,
    LOAD_MEMORY,
    STORE_MEMORY,
    PERFORM_ALU,
    FETCH_INSTRUCTION,
    DECODE_INSTRUCTION,
    EXECUTE_INSTRUCTION,
    BRANCH
};

// ALU Operations
enum ALUOperation {
    ADD,
    SUB,
    AND,
    OR,
    NOT
};

// Three-Bus Processor Class
class ThreeBusProcessor {
private:
    vector<int> registers;       // General-purpose registers
    vector<int> memory;          // Memory array
    int programCounter;          // Program Counter (PC)
    int instructionRegister;     // Instruction Register (IR)
    int aluResult;               // Temporary result of ALU operations
    map<string, int> labelAddress; // Label-to-address mapping for branching

    // Bus Signals
    int dataBus;        // Simulates the data bus for data transfer
    int addressBus;     // Simulates the address bus for addressing
    int controlBus;     // Simulates the control bus for control signals

    void transferBetweenRegisters(int srcReg, int destReg) {
        dataBus = registers[srcReg]; // Use data bus for transfer
        registers[destReg] = dataBus;
        cout << "Register Transfer via Data Bus: R" << srcReg << " -> R" << destReg << endl;
    }

    void performALUOperation(ALUOperation op, int regA, int regB, int destReg) {
        cout << "Performing ALU Operation via Data Bus..." << endl;
        switch (op) {
        case ADD:
            aluResult = registers[regA] + registers[regB];
            break;
        case SUB:
            aluResult = registers[regA] - registers[regB];
            break;
        case AND:
            aluResult = registers[regA] & registers[regB];
            break;
        case OR:
            aluResult = registers[regA] | registers[regB];
            break;
        case NOT:
            aluResult = ~registers[regA];
            break;
        }
        registers[destReg] = aluResult;
        cout << "ALU Result stored via Data Bus: R" << destReg << endl;
    }

    void fetchWordFromMemory(int address, int destReg) {
        cout << "Fetching word via Address and Data Buses..." << endl;
        if (address >= 0 && address < memory.size()) {
            addressBus = address; // Address bus holds the memory address
            dataBus = memory[addressBus]; // Data bus retrieves the memory content
            registers[destReg] = dataBus;
            cout << "Memory Fetch: Address = " << addressBus << " -> R" << destReg << endl;
        } else {
            cout << "Error: Invalid Memory Address" << endl;
        }
    }

    void storeWordInMemory(int srcReg, int address) {
        cout << "Storing word via Address and Data Buses..." << endl;
        if (address >= 0 && address < memory.size()) {
            addressBus = address; // Address bus holds the memory address
            dataBus = registers[srcReg]; // Data bus holds the register value
            memory[addressBus] = dataBus; // Store the value in memory
            cout << "Memory Store: R" << srcReg << " -> Address = " << addressBus << endl;
        } else {
            cout << "Error: Invalid Memory Address" << endl;
        }
    }

    void branchInstruction(string label) {
        cout << "Branching via Control Bus..." << endl;
        if (labelAddress.find(label) != labelAddress.end()) {
            programCounter = labelAddress[label];
            controlBus = programCounter; // Control bus updates the program counter
            cout << "Branch Instruction: Jump to Label \"" << label << "\" (Address = " << controlBus << ")" << endl;
        } else {
            cout << "Error: Undefined Label \"" << label << "\"" << endl;
        }
    }

public:
    ThreeBusProcessor(int regCount, int memorySize)
        : registers(regCount, 0), memory(memorySize, 0), programCounter(0), instructionRegister(0), aluResult(0),
          dataBus(0), addressBus(0), controlBus(0) {}

    void defineLabel(string label, int address) {
        labelAddress[label] = address;
        cout << "Label Defined: \"" << label << "\" -> Address " << address << endl;
    }

    void executeInstruction(ControlSignal signal, int regA = -1, int regB = -1, int destReg = -1, int address = -1, ALUOperation op = ADD, string label = "") {
        switch (signal) {
        case LOAD_REG:
            transferBetweenRegisters(regA, destReg);
            break;
        case PERFORM_ALU:
            performALUOperation(op, regA, regB, destReg);
            break;
        case FETCH_INSTRUCTION:
            fetchWordFromMemory(programCounter, instructionRegister);
            programCounter++;
            break;
        case DECODE_INSTRUCTION:
            cout << "Decode Instruction via Control Bus: IR = " << instructionRegister << endl;
            break;
        case EXECUTE_INSTRUCTION:
            cout << "Execute Instruction via Control Bus: IR = " << instructionRegister << endl;
            break;
        case STORE_MEMORY:
            storeWordInMemory(regA, address);
            break;
        case LOAD_MEMORY:
            fetchWordFromMemory(address, destReg);
            break;
        case BRANCH:
            branchInstruction(label);
            break;
        default:
            cout << "Error: Invalid Control Signal" << endl;
        }
    }

    void printRegisters() {
        cout << "\n--- Register Status ---" << endl;
        for (int i = 0; i < registers.size(); i++) {
            cout << "R" << i << ": " << registers[i] << endl;
        }
    }

    void printMemory() {
        cout << "\n--- Memory Status ---" << endl;
        for (int i = 0; i < memory.size(); i++) {
            cout << "Address " << i << ": " << memory[i] << endl;
        }
    }

    void printProgramCounter() {
        cout << "\nProgram Counter (PC): " << programCounter << endl;
    }
};

int main() {
    ThreeBusProcessor processor(8, 16); // 8 registers, 16 memory locations

    // Define labels for branching
    processor.defineLabel("START", 0);
    processor.defineLabel("END", 15);

    // Load data into memory
    processor.executeInstruction(STORE_MEMORY, 0, -1, -1, 0); // Store R0 -> Address 0
    processor.executeInstruction(STORE_MEMORY, 1, -1, -1, 1); // Store R1 -> Address 1

    // Fetch and decode an instruction
    processor.executeInstruction(FETCH_INSTRUCTION);
    processor.executeInstruction(DECODE_INSTRUCTION);

    // Perform arithmetic operations
    processor.executeInstruction(PERFORM_ALU, 0, 1, 2, -1, ADD); // R0 + R1 -> R2
    processor.executeInstruction(PERFORM_ALU, 2, -1, 3, -1, NOT); // NOT R2 -> R3

    // Fetch a word from memory
    processor.executeInstruction(LOAD_MEMORY, -1, -1, 4, 0); // Address 0 -> R4

    // Store a word in memory
    processor.executeInstruction(STORE_MEMORY, 3, -1, -1, 2); // R3 -> Address 2

    // Branch to label "START"
    processor.executeInstruction(BRANCH, -1, -1, -1, -1, ADD, "START");

    // Print system status
    processor.printRegisters();
    processor.printMemory();
    processor.printProgramCounter();

    return 0;
}
