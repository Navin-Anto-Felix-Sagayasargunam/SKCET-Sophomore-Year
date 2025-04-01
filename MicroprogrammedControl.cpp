#include <iostream>
#include <vector>
#include <map>

using namespace std;

// Control Signals for Microinstructions
enum ControlSignal {
    LOAD_REG,
    ALU_EXECUTE,
    MEMORY_READ,
    MEMORY_WRITE,
    BRANCH,
    HALT
};

// Microinstruction Representation
struct Microinstruction {
    vector<ControlSignal> controlSignals; // Control signals
    int nextAddress;                     // Address for the next microinstruction
};

// Microprogram Control Unit Class
class MicroprogramControlUnit {
private:
    map<int, Microinstruction> microprogramMemory; // Memory for microinstructions
    int microsequenceCounter;                      // Microsequence counter for sequencing
    int branchAddress;                             // Branch address for control flow

    void executeMicroinstruction(const Microinstruction& microinst) {
        cout << "Executing Microinstruction at Address: " << microsequenceCounter << endl;
        for (const auto& signal : microinst.controlSignals) {
            printControlSignal(signal);
        }
        microsequenceCounter = microinst.nextAddress; // Update the microsequence counter
    }

    void printControlSignal(ControlSignal signal) {
        switch (signal) {
        case LOAD_REG:
            cout << "LOAD_REG ";
            break;
        case ALU_EXECUTE:
            cout << "ALU_EXECUTE ";
            break;
        case MEMORY_READ:
            cout << "MEMORY_READ ";
            break;
        case MEMORY_WRITE:
            cout << "MEMORY_WRITE ";
            break;
        case BRANCH:
            cout << "BRANCH ";
            break;
        case HALT:
            cout << "HALT ";
            break;
        default:
            cout << "UNKNOWN_SIGNAL ";
            break;
        }
        cout << endl;
    }

public:
    MicroprogramControlUnit() : microsequenceCounter(0), branchAddress(0) {}

    void initializeMicroprogram() {
        // Vertical grouping: compact microinstructions
        microprogramMemory[0] = {{LOAD_REG, ALU_EXECUTE}, 1};  // Load and execute ALU operation
        microprogramMemory[1] = {{MEMORY_READ}, 2};           // Read from memory
        microprogramMemory[2] = {{MEMORY_WRITE}, 3};          // Write to memory
        microprogramMemory[3] = {{BRANCH}, 0};                // Branch to address 0
        microprogramMemory[4] = {{HALT}, -1};                 // Halt execution

        // Horizontal grouping: explicit control signals
        microprogramMemory[5] = {{LOAD_REG}, 6};              // Load register
        microprogramMemory[6] = {{ALU_EXECUTE}, 7};           // Execute ALU operation
        microprogramMemory[7] = {{MEMORY_READ, MEMORY_WRITE}, 8}; // Read and write simultaneously
        microprogramMemory[8] = {{HALT}, -1};                 // Halt execution
    }

    void executeMicrosequencing() {
        cout << "\nMicrosequencing Execution:" << endl;
        while (microsequenceCounter != -1) { // Halt condition
            executeMicroinstruction(microprogramMemory[microsequenceCounter]);
        }
    }

    void modifyBranchAddress(int newAddress) {
        branchAddress = newAddress;
        cout << "Branch Address Modified to: " << branchAddress << endl;
    }

    void executeBranch() {
        if (microprogramMemory.find(branchAddress) != microprogramMemory.end()) {
            microsequenceCounter = branchAddress; // Update microsequence counter
            cout << "Branching to Address: " << branchAddress << endl;
        } else {
            cout << "Error: Invalid Branch Address!" << endl;
        }
    }
};

int main() {
    MicroprogramControlUnit controlUnit;

    // Initialize microprogram memory
    controlUnit.initializeMicroprogram();

    // Execute microsequencing
    controlUnit.executeMicrosequencing();

    // Modify branch address and execute branch
    controlUnit.modifyBranchAddress(3); // Set branch to address 3
    controlUnit.executeBranch();

    // Execute additional microinstructions post-branching
    controlUnit.executeMicrosequencing();

    return 0;
}
