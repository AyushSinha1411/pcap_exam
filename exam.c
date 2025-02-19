#include <stdio.h> // Include standard input/output library
#include <stdlib.h> // Include standard library (for malloc, free, etc.)
#include <string.h> // Include string manipulation library
#include <ctype.h> // Include character type checking library

#define MAX_TOKENS 100 // Define maximum number of tokens
#define MAX_ID_LEN 50 // Define maximum length of an identifier

// Structure to store symbol table entries
typedef struct {
    int index; // Index of the symbol in the table
    char name[MAX_ID_LEN]; // Name of the identifier
    char type[10]; // Data type of the identifier
    int size; // Size of the identifier (e.g., in bytes)
} Symbol;

Symbol symbolTable[MAX_TOKENS]; // Declare the symbol table as an array of Symbol structures
int symbolCount = 0; // Initialize the count of symbols in the table

// Function to check if an identifier already exists in the symbol table
int lookupSymbol(char *name) {
    for (int i = 0; i < symbolCount; i++) { // Iterate through the symbol table
        if (strcmp(symbolTable[i].name, name) == 0) // Compare the given name with the name in the symbol table
            return symbolTable[i].index; // Return the index if the name is found
    }
    return -1; // Return -1 if the name is not found
}

// Function to insert a new identifier into the symbol table
int insertSymbol(char *name, char *type, int size) {
    int index = lookupSymbol(name); // Check if the symbol already exists
    if (index == -1) { // If the symbol doesn't exist
        symbolTable[symbolCount].index = symbolCount + 1; // Assign a new index
        strcpy(symbolTable[symbolCount].name, name); // Copy the name
        strcpy(symbolTable[symbolCount].type, type); // Copy the type
        symbolTable[symbolCount].size = size; // Set the size
        symbolCount++; // Increment the symbol count
        return symbolTable[symbolCount - 1].index; // Return the index of the newly inserted symbol
    }
    return index; // Return the existing index if the symbol already exists
}

// Function to tokenize the input file
void getNextToken(FILE *fp) {
    char ch, buffer[MAX_ID_LEN]; // Character to read from file, buffer to store tokens
    int line = 1, col = 0, tokenIndex = 1; // Initialize line number, column number, and token index

    while ((ch = fgetc(fp)) != EOF) { // Read character by character until EOF
        col++; // Increment column number
        if (isspace(ch)) { // Handling whitespace
            if (ch == '\n') { // If it's a newline character
                line++; // Increment line number
                col = 0; // Reset column number
            }
            continue; // Skip whitespace
        }

        if (isalpha(ch) || ch == '_') {  // Identifiers and keywords
            int i = 0;
            buffer[i++] = ch; // Store the first character of the identifier
            while (isalnum(ch = fgetc(fp)) || ch == '_') { // Read until non-alphanumeric or underscore
                buffer[i++] = ch; // Store the subsequent characters
            }
            buffer[i] = '\0'; // Null-terminate the buffer
            ungetc(ch, fp); // Put the last read character back to the stream
            int index = insertSymbol(buffer, "var", 4); // Insert into symbol table
            printf("%d. < Identifier , %d , %d, %s>\n", tokenIndex++, line, col, buffer); // Print token information
        } else if (isdigit(ch)) {  // Numbers
            int i = 0;
            buffer[i++] = ch; // Store the first digit
            while (isdigit(ch = fgetc(fp))) { // Read until non-digit
                buffer[i++] = ch; // Store the subsequent digits
            }
            buffer[i] = '\0'; // Null-terminate
            ungetc(ch, fp); // Put the last read character back
            printf("%d. < Number , %d , %d, %s>\n", tokenIndex++, line, col, buffer); // Print token information
        } else if (ch == '=' || ch == '>' || ch == '<' || ch == '!') {  // Relational and assignment operators
            buffer[0] = ch; // Store the operator
            ch = fgetc(fp); // Read the next character
            if (ch == '=') {  // Handling >=, <=, ==, !=
                buffer[1] = ch;
                buffer[2] = '\0';
            } else {
                buffer[1] = '\0';
                ungetc(ch, fp); // Put the last read character back
            }
            printf("%d. < Symbol , %d , %d, %s>\n", tokenIndex++, line, col, buffer); // Print token information
        } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {  // Arithmetic operators
            if (ch == '/' && (ch = fgetc(fp)) == '/') { // Single-line comment
                while ((ch = fgetc(fp)) != '\n' && ch != EOF); // Read until newline or EOF
                line++; // Increment line number
                col = 0; // Reset column
                continue; // Skip comment
            } else if (ch == '/' && ch == '*') { // Multi-line comment
                while ((ch = fgetc(fp)) != EOF) { // Read until EOF
                    if (ch == '*' && (ch = fgetc(fp)) == '/') break; // Check for end of comment
                }
                continue; // Skip comment
            } else {
                buffer[0] = ch; // Store the operator
                buffer[1] = '\0';
                printf("%d. < Operator , %d , %d, %s>\n", tokenIndex++, line, col, buffer); // Print token information
            }
        } else if (ch == '&' || ch == '|') {  // Logical operators (&&, ||)
            buffer[0] = ch; // Store the operator
            ch = fgetc(fp); // Read the next character
            if (ch == buffer[0]) { // Check for && or ||
                buffer[1] = ch;
                buffer[2] = '\0';
            } else {
                buffer[1] = '\0';
                ungetc(ch, fp); // Put the last read character back
            }
            printf("%d. < Logical Operator , %d , %d, %s>\n", tokenIndex++, line, col, buffer); // Print token information
        } else if (ch == '"') {  // String literals
            int i = 0;
            buffer[i++] = ch; // Store the opening quote
            while ((ch = fgetc(fp)) != '"' && ch != EOF) { // Read until closing quote or EOF
                buffer[i++] = ch; // Store the string characters
            }
            buffer[i++] = '"'; // Store the closing quote
            buffer[i] = '\0'; // Null-terminate
            printf("%d. < String , %d , %d, %s>\n", tokenIndex++, line, col, buffer); // Print token information
        }
    }
}

// Function to print the symbol table
void printSymbolTable() {
    printf("\nSymbol Table:\n"); // Print header
    printf("Index   Name    Type    Size\n"); // Print column headers
    printf("-----   ----    ----    ----\n"); // Print separator
    for (int i = 0; i < symbolCount; i++) { // Iterate through the symbol table
        printf("%d       %s      %s      %d\n", symbolTable[i].index, symbolTable[i].name, symbolTable[i].type, symbolTable[i].size); // Print each entry
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) { // Check if filename is provided
        printf("Usage: %s <filename>\n", argv[0]); // Print usage message
        return 1; // Exit with error
    }
    FILE *fp = fopen(argv[1], "r"); // Open the file in read mode
    if (fp == NULL) { // Check if file opening failed
        printf("Error opening file %s\n", argv[1]); // Print error message
        return 1; // Exit with error
    }

    getNextToken(fp); // Tokenize the file
    fclose(fp); // Close the file

    printSymbolTable(); // Print the symbol table
    return 0; // Exit successfully
}
// gcc exam.c -o exam
//./exam test.py python
