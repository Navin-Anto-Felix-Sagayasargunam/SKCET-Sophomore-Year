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

// Processor Class
class SingleBusProcessor {
private:
    vector<int> registers;       // General-purpose registers
    vector<int> memory;          // Memory array
    int programCounter;          // Program Counter (PC)
    int instructionRegister;     // Instruction Register (IR)
    int aluResult;               // Temporary result of ALU operations
    map<string, int> labelAddress; // Label-to-address mapping for branching

    void transferBetweenRegisters(int srcReg, int destReg) {
        registers[destReg] = registers[srcReg];
        cout << "Register Transfer: R" << srcReg << " -> R" << destReg << endl;
    }

    void performALUOperation(ALUOperation op, int regA, int regB, int destReg) {
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
        cout << "ALU Operation: Result stored in R" << destReg << endl;
    }

    void fetchWordFromMemory(int address, int destReg) {
        if (address >= 0 && address < memory.size()) {
            registers[destReg] = memory[address];
            cout << "Memory Fetch: Address = " << address << " -> R" << destReg << endl;
        } else {
            cout << "Error: Invalid Memory Address" << endl;
        }
    }

    void storeWordInMemory(int srcReg, int address) {
        if (address >= 0 && address < memory.size()) {
            memory[address] = registers[srcReg];
            cout << "Memory Store: R" << srcReg << " -> Address = " << address << endl;
        } else {
            cout << "Error: Invalid Memory Address" << endl;
        }
    }

    void branchInstruction(string label) {
        if (labelAddress.find(label) != labelAddress.end()) {
            programCounter = labelAddress[label];
            cout << "Branch Instruction: Jump to Label \"" << label << "\" (Address = " << programCounter << ")" << endl;
        } else {
            cout << "Error: Undefined Label \"" << label << "\"" << endl;
        }
    }

public:
    SingleBusProcessor(int regCount, int memorySize)
        : registers(regCount, 0), memory(memorySize, 0), programCounter(0), instructionRegister(0), aluResult(0) {}

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
            cout << "Decode Instruction: IR = " << instructionRegister << endl;
            break;
        case EXECUTE_INSTRUCTION:
            cout << "Execute Instruction: IR = " << instructionRegister << endl;
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
    SingleBusProcessor processor(8, 16); // 8 registers, 16 memory locations

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
