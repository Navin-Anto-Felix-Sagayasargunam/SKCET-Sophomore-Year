#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <thread>

using namespace std;

// Control Signals
enum ControlSignal {
    FETCH,
    DECODE,
    EXECUTE,
    MEMORY_READ,
    MEMORY_WRITE,
    HALT
};

// Hardwired Control Unit Class
class HardwiredControlUnit {
private:
    map<int, vector<ControlSignal>> stateTable; // State table for State Table Method
    int sequenceCounter;                        // Counter for Sequence Counter Method
    vector<ControlSignal> plaOutputs;           // Outputs for PLA Method
    int currentState;                           // Current state for State Table Method

public:
    HardwiredControlUnit() : sequenceCounter(0), currentState(0) {}

    // State Table Method Initialization
    void initializeStateTable() {
        stateTable[0] = {FETCH};
        stateTable[1] = {DECODE};
        stateTable[2] = {EXECUTE};
        stateTable[3] = {MEMORY_READ};
        stateTable[4] = {MEMORY_WRITE};
        stateTable[5] = {HALT};
    }

    void executeStateTableMethod() {
        cout << "\nState Table Method Execution:" << endl;
        for (const auto& [state, signals] : stateTable) {
            cout << "State " << state << ": ";
            for (const auto& signal : signals) {
                printControlSignal(signal);
            }
        }
    }

    // Delay Element Method
    void executeDelayElementMethod(int delayMilliseconds) {
        cout << "\nDelay Element Method Execution:" << endl;
        vector<ControlSignal> signals = {FETCH, DECODE, EXECUTE, HALT};
        for (const auto& signal : signals) {
            printControlSignal(signal);
            this_thread::sleep_for(chrono::milliseconds(delayMilliseconds)); // Simulates delay
        }
    }

    // Sequence Counter Method
    void executeSequenceCounterMethod() {
        cout << "\nSequence Counter Method Execution:" << endl;
        vector<ControlSignal> sequenceSignals = {FETCH, DECODE, EXECUTE, MEMORY_READ, HALT};
        for (sequenceCounter = 0; sequenceCounter < sequenceSignals.size(); sequenceCounter++) {
            printControlSignal(sequenceSignals[sequenceCounter]);
        }
    }

    // PLA Method Initialization
    void initializePLAMethod() {
        plaOutputs = {FETCH, DECODE, EXECUTE, MEMORY_WRITE, HALT};
    }

    void executePLAMethod() {
        cout << "\nPLA Method Execution:" << endl;
        for (const auto& signal : plaOutputs) {
            printControlSignal(signal);
        }
    }

    void printControlSignal(ControlSignal signal) {
        switch (signal) {
        case FETCH:
            cout << "FETCH ";
            break;
        case DECODE:
            cout << "DECODE ";
            break;
        case EXECUTE:
            cout << "EXECUTE ";
            break;
        case MEMORY_READ:
            cout << "MEMORY_READ ";
            break;
        case MEMORY_WRITE:
            cout << "MEMORY_WRITE ";
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
};

int main() {
    HardwiredControlUnit controlUnit;

    // State Table Method
    controlUnit.initializeStateTable();
    controlUnit.executeStateTableMethod();

    // Delay Element Method
    controlUnit.executeDelayElementMethod(500); // 500 milliseconds delay

    // Sequence Counter Method
    controlUnit.executeSequenceCounterMethod();

    // PLA Method
    controlUnit.initializePLAMethod();
    controlUnit.executePLAMethod();

    return 0;
}
