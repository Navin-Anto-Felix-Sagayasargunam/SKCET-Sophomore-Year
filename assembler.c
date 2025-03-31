#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Constants
#define MAX_SYMBOLS 100
#define MAX_CODE_LINES 100
#define MAX_LINE_LENGTH 100
#define MAX_ERROR_MSG 256

// Structure for symbol table entries
typedef struct {
    char symbol[MAX_LINE_LENGTH];
    int address;
    int isRelocatable;  // New feature: Relocatable flag for modularity
} Symbol;

// Symbol table
Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;

// Machine code output
char machineCode[MAX_CODE_LINES][MAX_LINE_LENGTH];
int codeCount = 0;

// Error buffer
char errorBuffer[MAX_CODE_LINES][MAX_ERROR_MSG];
int errorCount = 0;

// Function prototypes
int addSymbol(const char* symbol, int address, int isRelocatable);
int findSymbol(const char* symbol);
void firstPass(FILE* inputFile);
void secondPass(FILE* inputFile);
void singlePassAssembler(FILE* inputFile);  // Single-pass assembler implementation
void assembleInstruction(const char* line, int currentAddress);
void optimizeMachineCode();  // Optimization function
void reportError(const char* errorMessage, int lineNumber);
void printMachineCode();
void printErrors();

// Opcode table for extended instruction set
typedef struct {
    char mnemonic[MAX_LINE_LENGTH];
    int opcode;
} Opcode;

Opcode opcodeTable[] = {
    {"LOAD", 0x01},
    {"STORE", 0x02},
    {"ADD", 0x03},
    {"SUB", 0x04},
    {"MUL", 0x05},       // New instruction
    {"DIV", 0x06},       // New instruction
    {"MOD", 0x07},       // New instruction
    {"HALT", 0xFF}
};
const int opcodeCount = sizeof(opcodeTable) / sizeof(opcodeTable[0]);

int getOpcode(const char* mnemonic);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE* inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        perror("Error opening input file");
        return 1;
    }

    printf("Choose Assembly Mode: \n1. Two-Pass Assembler\n2. Single-Pass Assembler\n");
    int choice;
    scanf("%d", &choice);

    if (choice == 1) {
        // Two-pass assembler
        firstPass(inputFile);
        rewind(inputFile);  // Reset file pointer to beginning for second pass
        secondPass(inputFile);
    } else if (choice == 2) {
        // Single-pass assembler
        rewind(inputFile);
        singlePassAssembler(inputFile);
    } else {
        printf("Invalid choice. Exiting.\n");
        fclose(inputFile);
        return 1;
    }

    // Optimize the machine code
    optimizeMachineCode();

    fclose(inputFile);

    // Print results
    printf("\nMachine Code:\n");
    printMachineCode();

    printf("\nErrors:\n");
    printErrors();

    return 0;
}

// Add a symbol to the symbol table
int addSymbol(const char* symbol, int address, int isRelocatable) {
    if (findSymbol(symbol) != -1) {
        return -1; // Symbol already exists
    }
    strcpy(symbolTable[symbolCount].symbol, symbol);
    symbolTable[symbolCount].address = address;
    symbolTable[symbolCount].isRelocatable = isRelocatable;
    symbolCount++;
    return 0;
}

// Find a symbol in the symbol table
int findSymbol(const char* symbol) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].symbol, symbol) == 0) {
            return symbolTable[i].address;
        }
    }
    return -1; // Not found
}

// Get the opcode for a given mnemonic
int getOpcode(const char* mnemonic) {
    for (int i = 0; i < opcodeCount; i++) {
        if (strcmp(opcodeTable[i].mnemonic, mnemonic) == 0) {
            return opcodeTable[i].opcode;
        }
    }
    return -1; // Invalid opcode
}

// First pass: Build symbol table and calculate addresses
void firstPass(FILE* inputFile) {
    char line[MAX_LINE_LENGTH];
    int currentAddress = 0;

    while (fgets(line, sizeof(line), inputFile)) {
        line[strcspn(line, "\n")] = 0;  // Strip newline
        if (strlen(line) == 0 || line[0] == ';') continue;  // Skip empty lines and comments

        char* label = strtok(line, " ");
        char* instruction = strtok(NULL, " ");

        if (instruction) {
            if (addSymbol(label, currentAddress, 1) == -1) {  // Relocatable by default
                reportError("Duplicate label", currentAddress);
            }
        } else {
            instruction = label;  // No label, instruction starts here
        }

        currentAddress++;
    }
}

// Second pass: Translate instructions to machine code
void secondPass(FILE* inputFile) {
    char line[MAX_LINE_LENGTH];
    int currentAddress = 0;

    while (fgets(line, sizeof(line), inputFile)) {
        line[strcspn(line, "\n")] = 0;  // Strip newline
        if (strlen(line) == 0 || line[0] == ';') continue;  // Skip empty lines and comments

        char* label = strtok(line, " ");
        char* instruction = strtok(NULL, " ");

        if (instruction) {
            assembleInstruction(instruction, currentAddress);
        } else {
            assembleInstruction(label, currentAddress);
        }

        currentAddress++;
    }
}

// Single-pass assembler: Combine symbol resolution and code generation
void singlePassAssembler(FILE* inputFile) {
    char line[MAX_LINE_LENGTH];
    int currentAddress = 0;

    while (fgets(line, sizeof(line), inputFile)) {
        line[strcspn(line, "\n")] = 0;  // Strip newline
        if (strlen(line) == 0 || line[0] == ';') continue;  // Skip empty lines and comments

        char* label = strtok(line, " ");
        char* instruction = strtok(NULL, " ");

        if (instruction) {
            if (findSymbol(label) == -1) {  // Inline symbol resolution
                addSymbol(label, currentAddress, 1);
            }
            assembleInstruction(instruction, currentAddress);
        } else {
            assembleInstruction(label, currentAddress);
        }

        currentAddress++;
    }
}

// Assemble a single instruction
void assembleInstruction(const char* line, int currentAddress) {
    char op[MAX_LINE_LENGTH];
    char operand[MAX_LINE_LENGTH];

    sscanf(line, "%s %s", op, operand);

    int opcode = getOpcode(op);
    if (opcode == -1) {
        reportError("Unknown operation", currentAddress);
        return;
    }

    int address = findSymbol(operand);
    if (address == -1 && strlen(operand) > 0) {
        reportError("Undefined symbol", currentAddress);
        return;
    }

    if (strlen(operand) > 0) {
        sprintf(machineCode[currentAddress], "%02X %04d", opcode, address);
    } else {
        sprintf(machineCode[currentAddress], "%02X 0000", opcode);
    }
    codeCount++;
}

// Optimize the machine code
void optimizeMachineCode() {
    for (int i = 0; i < codeCount - 1; i++) {
        if (strncmp(machineCode[i], "FF", 2) == 0 && strncmp(machineCode[i + 1], "FF", 2) == 0) {
            strcpy(machineCode[i], "");  // Remove redundant HALT
        }
    }
}

// Report an error
void reportError(const char* errorMessage, int lineNumber) {
    snprintf(errorBuffer[errorCount], MAX_ERROR_MSG, "Error at line %d: %s", lineNumber + 1, errorMessage);
    errorCount++;
}

// Print the generated machine code
void printMachineCode() {
    for (int i = 0; i < codeCount; i++) {
        if (strlen(machineCode[i]) > 0) {
            printf("%s\n", machineCode[i]);
        }
    }
}

// Print the errors
void printErrors() {
    if (errorCount == 0) {
        printf("No errors.\n");
        return;
    }
    for (int i = 0; i < errorCount; i++) {
        printf("%s\n", errorBuffer[i]);
    }
}
