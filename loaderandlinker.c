#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants for real-world simulation
#define MAX_SYMBOLS 100
#define MAX_MODULES 10
#define MAX_CODE_LENGTH 1024
#define MAX_RELOCATIONS 50

// Symbol table entry structure
typedef struct {
    char name[50];          // Symbol name
    int address;            // Memory address
    int isExternal;         // 1 for external, 0 for internal
} Symbol;

// Relocation table entry structure
typedef struct {
    int offset;             // Offset in code that needs relocation
    char symbolName[50];    // Symbol to resolve
} RelocationEntry;

// Module structure representing an object file
typedef struct {
    char name[50];                // Module name
    Symbol symbols[MAX_SYMBOLS];  // Module-level symbols
    int symbolCount;              // Number of symbols
    char code[MAX_CODE_LENGTH];   // Machine code
    int codeLength;               // Length of the code
    RelocationEntry relocations[MAX_RELOCATIONS]; // Relocations
    int relocationCount;          // Number of relocation entries
} Module;

// Globals
Module modules[MAX_MODULES];
int moduleCount = 0;

Symbol globalSymbolTable[MAX_SYMBOLS];
int globalSymbolCount = 0;

// Function declarations
void loadModule(const char *filename);
void linkModules();
void resolveRelocations(Module *module);
int findSymbolInGlobalTable(const char *name);
void executeProgram(const char *entryModule);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <module1.obj> <module2.obj> ...\n", argv[0]);
        return 1;
    }

    // Step 1: Load modules
    for (int i = 1; i < argc; i++) {
        loadModule(argv[i]);
    }

    // Step 2: Link modules
    linkModules();

    // Step 3: Execute the program from the entry module "main"
    executeProgram("main");

    return 0;
}

// Function to load an object module
void loadModule(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    Module module;
    memset(&module, 0, sizeof(Module));
    strcpy(module.name, filename);

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "SYM", 3) == 0) {
            // Parse symbol: SYM <name> <address> <isExternal>
            Symbol symbol;
            sscanf(line, "SYM %s %d %d", symbol.name, &symbol.address, &symbol.isExternal);
            module.symbols[module.symbolCount++] = symbol;
        } else if (strncmp(line, "REL", 3) == 0) {
            // Parse relocation: REL <offset> <symbolName>
            RelocationEntry relocation;
            sscanf(line, "REL %d %s", &relocation.offset, relocation.symbolName);
            module.relocations[module.relocationCount++] = relocation;
        } else {
            // Add code line to machine code
            strcat(module.code, line);
            module.codeLength += strlen(line);
        }
    }

    modules[moduleCount++] = module;
    fclose(file);
}

// Function to link modules
void linkModules() {
    printf("Linking modules...\n");

    // Build the global symbol table
    for (int i = 0; i < moduleCount; i++) {
        for (int j = 0; j < modules[i].symbolCount; j++) {
            Symbol *symbol = &modules[i].symbols[j];
            if (!symbol->isExternal) {
                globalSymbolTable[globalSymbolCount++] = *symbol;
            }
        }
    }

    // Resolve relocations for each module
    for (int i = 0; i < moduleCount; i++) {
        resolveRelocations(&modules[i]);
    }
    printf("Linking completed.\n");
}

// Resolve relocations for a module
void resolveRelocations(Module *module) {
    for (int i = 0; i < module->relocationCount; i++) {
        RelocationEntry *relocation = &module->relocations[i];
        int address = findSymbolInGlobalTable(relocation->symbolName);
        if (address == -1) {
            fprintf(stderr, "Error: Undefined symbol '%s' in module '%s'\n", relocation->symbolName, module->name);
            exit(EXIT_FAILURE);
        }

        // Apply relocation
        module->code[relocation->offset] = (char)address;
    }
}

// Find a symbol in the global symbol table
int findSymbolInGlobalTable(const char *name) {
    for (int i = 0; i < globalSymbolCount; i++) {
        if (strcmp(globalSymbolTable[i].name, name) == 0) {
            return globalSymbolTable[i].address;
        }
    }
    return -1; // Symbol not found
}

// Simulate execution of the program
void executeProgram(const char *entryModule) {
    for (int i = 0; i < moduleCount; i++) {
        if (strcmp(modules[i].name, entryModule) == 0) {
            printf("\nExecuting program '%s'...\n", entryModule);
            printf("Machine Code:\n%s\n", modules[i].code);
            printf("Execution completed.\n");
            return;
        }
    }

    fprintf(stderr, "Error: Entry module '%s' not found\n", entryModule);
    exit(EXIT_FAILURE);
}
