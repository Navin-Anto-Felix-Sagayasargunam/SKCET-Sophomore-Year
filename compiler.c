#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Constants
#define MAX_TOKENS 100
#define MAX_TOKEN_LENGTH 50
#define MAX_CODE_LINES 500
#define MAX_SYMBOL_TABLE 100

// Token Types
typedef enum {
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_LITERAL,
    TOKEN_OPERATOR,
    TOKEN_DELIMITER,
    TOKEN_EOF
} TokenType;

// Token Structure
typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LENGTH];
} Token;

// Symbol Table Entry
typedef struct {
    char name[MAX_TOKEN_LENGTH];
    char type[20];  // Type (int, float, void)
    int initialized; // Initialization flag
} Symbol;

// Global Variables
Token tokens[MAX_TOKENS];
int tokenCount = 0, currentToken = 0;
Symbol symbolTable[MAX_SYMBOL_TABLE];
int symbolCount = 0;
char intermediateCode[MAX_CODE_LINES][MAX_TOKEN_LENGTH];
int codeLineCount = 0;

// Function Prototypes: Lexical Analysis
void tokenize(const char *sourceCode);
Token getNextToken();
void printTokens();

// Function Prototypes: Syntax Analysis
void parseProgram();
void parseStatement();
void parseExpression();
void parseCondition();
void reportSyntaxError(const char *message);

// Function Prototypes: Semantic Analysis
void addToSymbolTable(const char *name, const char *type);
int isInSymbolTable(const char *name);
void semanticCheck();

// Function Prototypes: Intermediate Code Generation
void generateIntermediateCode();
void emit(const char *instruction);

// Utility Function Prototypes
void clearGlobalState();

int main() {
    char sourceCode[1000];

    while (1) {
        // Prompt the user for input
        printf("\nEnter your program (type 'exit' to quit):\n");
        fgets(sourceCode, sizeof(sourceCode), stdin);

        if (strncmp(sourceCode, "exit", 4) == 0) {
            printf("Exiting compiler...\n");
            break;
        }

        // Clear global state
        clearGlobalState();

        // Step 1: Lexical Analysis
        tokenize(sourceCode);
        printf("\nLexical Analysis:\n");
        printTokens();

        // Step 2: Syntax Analysis
        printf("\nSyntax Analysis:\n");
        parseProgram();

        // Step 3: Semantic Analysis
        printf("\nSemantic Analysis:\n");
        semanticCheck();

        // Step 4: Intermediate Code Generation
        printf("\nGenerated Intermediate Code:\n");
        generateIntermediateCode();
    }

    return 0;
}

// Step 1: Lexical Analysis
void tokenize(const char *sourceCode) {
    const char *ptr = sourceCode;
    while (*ptr != '\0') {
        if (isspace(*ptr)) {
            ptr++;
        } else if (isalpha(*ptr)) {
            // Identifier or Keyword
            char buffer[MAX_TOKEN_LENGTH] = {0};
            int index = 0;
            while (isalnum(*ptr)) {
                buffer[index++] = *ptr++;
            }
            if (strcmp(buffer, "int") == 0 || strcmp(buffer, "if") == 0 || strcmp(buffer, "print") == 0) {
                tokens[tokenCount++] = (Token){TOKEN_KEYWORD, ""};
                strcpy(tokens[tokenCount - 1].value, buffer);
            } else {
                tokens[tokenCount++] = (Token){TOKEN_IDENTIFIER, ""};
                strcpy(tokens[tokenCount - 1].value, buffer);
            }
        } else if (isdigit(*ptr)) {
            // Numeric Literal
            char buffer[MAX_TOKEN_LENGTH] = {0};
            int index = 0;
            while (isdigit(*ptr)) {
                buffer[index++] = *ptr++;
            }
            tokens[tokenCount++] = (Token){TOKEN_LITERAL, ""};
            strcpy(tokens[tokenCount - 1].value, buffer);
        } else if (strchr("+-*/=<>(){}", *ptr)) {
            char buffer[2] = {*ptr, '\0'};
            tokens[tokenCount++] = (Token){TOKEN_OPERATOR, ""};
            strcpy(tokens[tokenCount - 1].value, buffer);
            ptr++;
        } else if (*ptr == ';') {
            tokens[tokenCount++] = (Token){TOKEN_DELIMITER, ";"};
            ptr++;
        } else {
            printf("Unknown character: %c\n", *ptr);
            exit(1);
        }
    }
    tokens[tokenCount++] = (Token){TOKEN_EOF, "EOF"};
}

Token getNextToken() {
    return (currentToken < tokenCount) ? tokens[currentToken++] : (Token){TOKEN_EOF, "EOF"};
}

void printTokens() {
    for (int i = 0; i < tokenCount; i++) {
        printf("Token Type: %d, Value: %s\n", tokens[i].type, tokens[i].value);
    }
}

// Step 2: Syntax Analysis
void parseProgram() {
    while (tokens[currentToken].type != TOKEN_EOF) {
        parseStatement();
    }
}

void parseStatement() {
    Token token = getNextToken();
    if (token.type == TOKEN_KEYWORD && strcmp(token.value, "int") == 0) {
        Token var = getNextToken();
        addToSymbolTable(var.value, "int");
        Token op = getNextToken();
        if (strcmp(op.value, "=") == 0) {
            parseExpression();
        }
    } else if (token.type == TOKEN_IDENTIFIER) {
        Token op = getNextToken();
        if (strcmp(op.value, "=") == 0) {
            parseExpression();
        }
    } else if (strcmp(token.value, "print") == 0) {
        parseExpression();
    } else if (strcmp(token.value, "if") == 0) {
        parseCondition();
    } else {
        reportSyntaxError("Invalid statement.");
    }
}

void parseExpression() {
    Token token = getNextToken();
    if (token.type != TOKEN_LITERAL && token.type != TOKEN_IDENTIFIER) {
        reportSyntaxError("Expected an identifier or literal.");
    }
}

void parseCondition() {
    Token token = getNextToken(); // '('
    parseExpression();
    token = getNextToken(); // Operator
    parseExpression();
    token = getNextToken(); // ')'
}

void reportSyntaxError(const char *message) {
    printf("Syntax Error: %s\n", message);
    exit(1);
}

// Step 3: Semantic Analysis
void addToSymbolTable(const char *name, const char *type) {
    if (!isInSymbolTable(name)) {
        Symbol symbol;
        strcpy(symbol.name, name);
        strcpy(symbol.type, type);
        symbol.initialized = 0;
        symbolTable[symbolCount++] = symbol;
    }
}

int isInSymbolTable(const char *name) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) return 1;
    }
    return 0;
}

void semanticCheck() {
    for (int i = 0; i < symbolCount; i++) {
        if (!symbolTable[i].initialized) {
            printf("Semantic Error: Uninitialized variable %s\n", symbolTable[i].name);
            exit(1);
        }
    }
}

// Step 4: Intermediate Code Generation
void generateIntermediateCode() {
    emit("Intermediate code generation is simulated!");
    for (int i = 0; i < codeLineCount; i++) {
        printf("%s\n", intermediateCode[i]);
    }
}

void emit(const char *instruction) {
    strcpy(intermediateCode[codeLineCount++], instruction);
}

// Clear global state for next input
void clearGlobalState() {
    tokenCount = currentToken = 0;
    symbolCount = codeLineCount = 0;
}
