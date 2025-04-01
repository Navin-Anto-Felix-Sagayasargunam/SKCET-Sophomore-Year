#include <iostream>
#include <queue>
#include <map>
#include <string>
#include <vector>
#include <functional>

using namespace std;

// Instruction Structure
struct Instruction {
    string operation;       // Operation type (ADD, SUB, etc.)
    int operand1;           // First operand
    int operand2;           // Second operand
    int destination;        // Destination register
    string addressingMode;  // Addressing mode (Immediate, Register, Memory)
};

// Pipeline Stages
enum PipelineStage {
    FETCH,
    DECODE,
    EXECUTE,
    MEMORY,
    WRITEBACK
};

// Condition Codes
struct ConditionFlags {
    bool Zero = false;      // Zero flag
    bool Negative = false;  // Negative flag
    bool Carry = false;     // Carry flag
};

// Instruction Pipeline Class
class InstructionPipeline {
private:
    queue<Instruction> instructionQueue;     // Instruction queue
    vector<PipelineStage> pipelineStages;    // Stages of the pipeline
    map<int, int> registers;                 // General-purpose registers
    map<int, int> memory;                    // Memory for load/store instructions
    ConditionFlags flags;                    // Condition flags
    int clockCycle;                          // Clock cycle count

    // Detect and resolve structural hazards
    bool detectStructuralHazard() {
        // Example: Memory and ALU conflict
        if (pipelineStages[MEMORY] != WRITEBACK && pipelineStages[EXECUTE] != FETCH) {
            cout << "Structural Hazard Detected: Memory conflict resolved.\n";
            return true; // Hazard detected
        }
        return false; // No hazard
    }

    // Detect and resolve data hazards
    bool detectDataHazard(const Instruction& currentInst, const Instruction& prevInst) {
        // RAW Hazard: Current instruction depends on the result of the previous instruction
        if (currentInst.operand1 == prevInst.destination || currentInst.operand2 == prevInst.destination) {
            cout << "Data Hazard Detected (RAW): Stalling pipeline to resolve...\n";
            return true; // Hazard detected
        }
        return false; // No hazard
    }

    // Detect and resolve control hazards
    bool detectControlHazard(const Instruction& currentInst) {
        if (currentInst.operation == "BRANCH") {
            cout << "Control Hazard Detected: Branch instruction causes pipeline flush.\n";
            return true; // Hazard detected
        }
        return false; // No hazard
    }

    // Update condition codes based on result
    void updateConditionFlags(int result) {
        flags.Zero = (result == 0);
        flags.Negative = (result < 0);
        flags.Carry = (result > 255); // Example for an 8-bit system
    }

    // Execute a single instruction
    void executeInstruction(const Instruction& instruction) {
        int result = 0;
        if (instruction.addressingMode == "Immediate") {
            if (instruction.operation == "ADD") {
                result = instruction.operand1 + instruction.operand2;
            } else if (instruction.operation == "SUB") {
                result = instruction.operand1 - instruction.operand2;
            }
        } else if (instruction.addressingMode == "Register") {
            if (instruction.operation == "ADD") {
                result = registers[instruction.operand1] + registers[instruction.operand2];
            } else if (instruction.operation == "SUB") {
                result = registers[instruction.operand1] - registers[instruction.operand2];
            }
        } else if (instruction.addressingMode == "Memory") {
            if (instruction.operation == "LOAD") {
                result = memory[instruction.operand1];
            } else if (instruction.operation == "STORE") {
                memory[instruction.operand1] = registers[instruction.operand2];
            }
        }

        // Update destination register
        registers[instruction.destination] = result;
        updateConditionFlags(result);

        cout << "Executed Instruction: " << instruction.operation << ", Result = " << result << "\n";
    }

public:
    InstructionPipeline() : clockCycle(0) {
        pipelineStages = {FETCH, DECODE, EXECUTE, MEMORY, WRITEBACK};
    }

    void loadInstructions(const vector<Instruction>& instructions) {
        for (const auto& inst : instructions) {
            instructionQueue.push(inst);
        }
    }

    void runPipeline() {
        vector<Instruction> pipelineRegister(pipelineStages.size()); // Pipeline register for instructions
        Instruction prevInstruction = {"", 0, 0, 0, ""}; // Empty previous instruction for hazard detection

        while (!instructionQueue.empty() || !pipelineRegister.empty()) {
            clockCycle++;
            cout << "\nClock Cycle: " << clockCycle << "\n";

            // Fetch next instruction
            if (!instructionQueue.empty()) {
                pipelineRegister[FETCH] = instructionQueue.front();
                instructionQueue.pop();
                cout << "Fetched Instruction: " << pipelineRegister[FETCH].operation << "\n";
            }

            // Detect Hazards
            if (detectStructuralHazard()) continue; // Resolve structural hazard
            if (detectDataHazard(pipelineRegister[DECODE], prevInstruction)) continue; // Resolve data hazard
            if (detectControlHazard(pipelineRegister[DECODE])) {
                pipelineRegister.clear(); // Flush pipeline on control hazard
                continue;
            }

            // Decode Instruction
            cout << "Decoded Instruction: " << pipelineRegister[DECODE].operation << "\n";

            // Execute Instruction
            executeInstruction(pipelineRegister[EXECUTE]);

            // Shift pipeline stages
            for (int i = pipelineStages.size() - 1; i > 0; i--) {
                pipelineRegister[i] = pipelineRegister[i - 1];
            }

            prevInstruction = pipelineRegister[WRITEBACK];
            pipelineRegister[WRITEBACK] = {"", 0, 0, 0, ""}; // Clear writeback stage
        }
    }

    void printRegisters() {
        cout << "\n--- Register Status ---\n";
        for (const auto& reg : registers) {
            cout << "R" << reg.first << ": " << reg.second << "\n";
        }
    }

    void printMemory() {
        cout << "\n--- Memory Status ---\n";
        for (const auto& mem : memory) {
            cout << "Address " << mem.first << ": " << mem.second << "\n";
        }
    }
};

int main() {
    InstructionPipeline pipeline;

    // Example instructions
    vector<Instruction> instructions = {
        {"ADD", 5, 10, 0, "Immediate"},    // R0 = 5 + 10
        {"SUB", 0, 0, 1, "Register"},      // R1 = R0 - R0
        {"LOAD", 100, 0, 2, "Memory"},     // R2 = Mem[100]
        {"STORE", 2, 0, 200, "Memory"},    // Mem[200] = R2
        {"BRANCH", 0, 0, 0, "Control"}     // Branch (control hazard)
    };

    pipeline.loadInstructions(instructions);
    pipeline.runPipeline();
    pipeline.printRegisters();
    pipeline.printMemory();

    return 0;
}
