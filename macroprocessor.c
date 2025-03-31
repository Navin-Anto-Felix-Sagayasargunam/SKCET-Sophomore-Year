#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_MACROS 100
#define MAX_ARGS 10
#define MAX_CODE_LINES 500
#define MAX_LINE_LENGTH 256

// Structure for Macro Definitions
typedef struct {
    char name[50];                  // Name of the macro
    char *arguments[MAX_ARGS];      // List of arguments
    int argCount;                   // Number of arguments
    char definition[MAX_CODE_LINES][MAX_LINE_LENGTH]; // Macro body (code lines)
    int lineCount;                  // Number of lines in the macro body
} Macro;

// Macro Table
Macro macroTable[MAX_MACROS];
int macroCount = 0;

// Function Prototypes
void loadInputFile(const char *filename, char inputCode[MAX_CODE_LINES][MAX_LINE_LENGTH], int *lineCount);
void writeOutputFile(const char *filename, char outputCode[MAX_CODE_LINES][MAX_LINE_LENGTH], int lineCount);
int isMacroDefinition(const char *line);
void processMacroDefinition(char inputCode[MAX_CODE_LINES][MAX_LINE_LENGTH], int *currentLine, int totalLines);
int findMacro(const char *name);
void expandMacro(const char *line, char expandedCode[MAX_CODE_LINES][MAX_LINE_LENGTH], int *expandedCount);
void preprocessCode(char inputCode[MAX_CODE_LINES][MAX_LINE_LENGTH], int totalLines, char outputCode[MAX_CODE_LINES][MAX_LINE_LENGTH], int *outputLineCount);

// Main Function
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    char inputCode[MAX_CODE_LINES][MAX_LINE_LENGTH];
    char outputCode[MAX_CODE_LINES][MAX_LINE_LENGTH];
    int inputLineCount = 0, outputLineCount = 0;

    // Load input file
    loadInputFile(argv[1], inputCode, &inputLineCount);

    // Preprocess the code
    preprocessCode(inputCode, inputLineCount, outputCode, &outputLineCount);

    // Write expanded code to output file
    writeOutputFile(argv[2], outputCode, outputLineCount);

       printf("Macro processing completed. Output written to %s\n", argv[2]);
    return 0;
}

// Function to load input file into memory
void loadInputFile(const char *filename, char inputCode[MAX_CODE_LINES][MAX_LINE_LENGTH], int *lineCount) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening input file");
        exit(1);
    }

    while (fgets(inputCode[*lineCount], MAX_LINE_LENGTH, file)) {
        inputCode[*lineCount][strcspn(inputCode[*lineCount], "\n")] = 0; // Remove newline
        (*lineCount)++;
    }

    fclose(file);
}

// Function to write the expanded output to a file
void writeOutputFile(const char *filename, char outputCode[MAX_CODE_LINES][MAX_LINE_LENGTH], int lineCount) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening output file");
        exit(1);
    }

    for (int i = 0; i < lineCount; i++) {
        fprintf(file, "%s\n", outputCode[i]);
    }

    fclose(file);
}

// Check if a line is a macro definition
int isMacroDefinition(const char *line) {
    return strncmp(line, "#define", 7) == 0;
}

// Process a macro definition and store it in the macro table
void processMacroDefinition(char inputCode[MAX_CODE_LINES][MAX_LINE_LENGTH], int *currentLine, int totalLines) {
    char macroName[50];
    char macroArgs[MAX_ARGS][50];
    int argCount = 0;
    Macro macro;
    memset(&macro, 0, sizeof(Macro));

    sscanf(inputCode[*currentLine], "#define %s", macroName);
    char *argStart = strchr(macroName, '(');
    if (argStart) {
        *argStart = '\0'; // Separate macro name from arguments
        char *argToken = strtok(argStart + 1, ",)");
        while (argToken) {
            macro.arguments[argCount] = strdup(argToken);
            argCount++;
            argToken = strtok(NULL, ",)");
        }
    }

    strcpy(macro.name, macroName);
    macro.argCount = argCount;

    (*currentLine)++;
    while (*currentLine < totalLines && strncmp(inputCode[*currentLine], "#end", 4) != 0) {
        strcpy(macro.definition[macro.lineCount], inputCode[*currentLine]);
        macro.lineCount++;
        (*currentLine)++;
    }

    macroTable[macroCount++] = macro;
}

// Find a macro in the macro table
int findMacro(const char *name) {
    for (int i = 0; i < macroCount; i++) {
        if (strcmp(macroTable[i].name, name) == 0) {
            return i;
        }
    }
    return -1; // Not found
}

// Expand a macro call into its definition
void expandMacro(const char *line, char expandedCode[MAX_CODE_LINES][MAX_LINE_LENGTH], int *expandedCount) {
    char macroName[50];
    char argValues[MAX_ARGS][50];
    sscanf(line, "%s", macroName);

    char *argStart = strchr(line, '(');
    int argCount = 0;
    if (argStart) {
        char *argToken = strtok(argStart + 1, ",)");
        while (argToken) {
            strcpy(argValues[argCount++], argToken);
            argToken = strtok(NULL, ",)");
        }
    }

    int macroIndex = findMacro(macroName);
    if (macroIndex == -1) {
        printf("Error: Undefined macro '%s'\n", macroName);
        exit(1);
    }

    Macro *macro = &macroTable[macroIndex];
    for (int i = 0; i < macro->lineCount; i++) {
        char expandedLine[MAX_LINE_LENGTH];
        strcpy(expandedLine, macro->definition[i]);

        for (int j = 0; j < macro->argCount; j++) {
            char placeholder[10];
            sprintf(placeholder, "$%d", j + 1);
            char *pos = strstr(expandedLine, placeholder);
            if (pos) {
                char temp[MAX_LINE_LENGTH];
                strncpy(temp, expandedLine, pos - expandedLine);
                temp[pos - expandedLine] = '\0';
                strcat(temp, argValues[j]);
                strcat(temp, pos + strlen(placeholder));
                strcpy(expandedLine, temp);
            }
        }

        strcpy(expandedCode[*expandedCount], expandedLine);
        (*expandedCount)++;
    }
}

// Preprocess the code by expanding macros
void preprocessCode(char inputCode[MAX_CODE_LINES][MAX_LINE_LENGTH], int totalLines, char outputCode[MAX_CODE_LINES][MAX_LINE_LENGTH], int *outputLineCount) {
    for (int i = 0; i < totalLines; i++) {
        if (isMacroDefinition(inputCode[i])) {
            processMacroDefinition(inputCode, &i, totalLines);
        } else {
            char line[MAX_LINE_LENGTH];
            strcpy(line, inputCode[i]);

            char macroName[50];
            sscanf(line, "%s", macroName);

            int macroIndex = findMacro(macroName);
            if (macroIndex != -1) {
                expandMacro(line, outputCode, outputLineCount);
            } else {
                strcpy(outputCode[*outputLineCount], inputCode[i]);
                (*outputLineCount)++;
            }
        }
    }
}
