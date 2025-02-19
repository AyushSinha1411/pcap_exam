#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKENS 100
#define MAX_ID_LEN 50

typedef struct {
    int index;
    char name[MAX_ID_LEN];
    char type[10];
    int size;
} Symbol;

Symbol symbolTable[MAX_TOKENS];
int symbolCount = 0;

int lookupSymbol(char *name) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0)
            return symbolTable[i].index;
    }
    return -1;
}

int insertSymbol(char *name, char *type, int size) {
    int index = lookupSymbol(name);
    if (index == -1) {
        symbolTable[symbolCount].index = symbolCount + 1;
        strcpy(symbolTable[symbolCount].name, name);
        strcpy(symbolTable[symbolCount].type, type);
        symbolTable[symbolCount].size = size;
        symbolCount++;
        return symbolTable[symbolCount - 1].index;
    }
    return index;
}

void getNextToken(FILE *fp) {
    char ch, buffer[MAX_ID_LEN];
    int line = 1, col = 0, tokenIndex = 1;

    while ((ch = fgetc(fp)) != EOF) {
        col++;
        if (isspace(ch)) {
            if (ch == '\n') {
                line++;
                col = 0;
            }
            continue;
        }

        if (ch == '#') { // Preprocessor directive
            int i = 0;
            buffer[i++] = ch;
            while (isalpha(ch = fgetc(fp))) {
                buffer[i++] = ch;
            }
            buffer[i] = '\0';
            ungetc(ch, fp);
            printf("%d. < Preprocessor , %d , %d, %s>\n", tokenIndex++, line, col, buffer);
        } else if (ch == '<') { // Handling library names
            int i = 0;
            buffer[i++] = ch;
            while ((ch = fgetc(fp)) != '>' && ch != EOF) {
                buffer[i++] = ch;
            }
            buffer[i++] = '>';
            buffer[i] = '\0';
            printf("%d. < Library , %d , %d, %s>\n", tokenIndex++, line, col, buffer);
        } else if (isalpha(ch) || ch == '_') {
            int i = 0;
            buffer[i++] = ch;
            while (isalnum(ch = fgetc(fp)) || ch == '_') {
                buffer[i++] = ch;
            }
            buffer[i] = '\0';
            ungetc(ch, fp);
            int index = insertSymbol(buffer, "var", 4);
            printf("%d. < Identifier , %d , %d, %s>\n", tokenIndex++, line, col, buffer);
        } else if (isdigit(ch)) {
            int i = 0;
            buffer[i++] = ch;
            while (isdigit(ch = fgetc(fp))) {
                buffer[i++] = ch;
            }
            buffer[i] = '\0';
            ungetc(ch, fp);
            printf("%d. < Number , %d , %d, %s>\n", tokenIndex++, line, col, buffer);
        } else {
            buffer[0] = ch;
            buffer[1] = '\0';
            printf("%d. < Symbol , %d , %d, %s>\n", tokenIndex++, line, col, buffer);
        }
    }
}

void printSymbolTable() {
    printf("\nSymbol Table:\n");
    printf("Index   Name    Type    Size\n");
    printf("-----   ----    ----    ----\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("%d       %s      %s      %d\n", symbolTable[i].index, symbolTable[i].name, symbolTable[i].type, symbolTable[i].size);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Error opening file %s\n", argv[1]);
        return 1;
    }

    getNextToken(fp);
    fclose(fp);
    printSymbolTable();
    return 0;
}
/*
t_files> gcc gpt1.c -o gpt1
PS C:\Users\Ayush Sinha\Desktop\code\c\exam prep\test_files> ./gpt1 test.cpp   
1. < Symbol , 1 , 1, />
2. < Symbol , 1 , 2, />
3. < Identifier , 1 , 3, hello>
4. < Identifier , 1 , 5, world>
5. < Symbol , 2 , 1, />
6. < Symbol , 2 , 2, />
7. < Identifier , 2 , 3, hello>
8. < Identifier , 2 , 5, world>
9. < Preprocessor , 3 , 1, #include>
10. < Library , 3 , 3, <iostream>>
11. < Preprocessor , 4 , 1, #include>
12. < Library , 4 , 3, <fstream>>
13. < Preprocessor , 5 , 1, #include>
14. < Library , 5 , 3, <string>>
15. < Identifier , 6 , 1, int>
16. < Identifier , 6 , 3, main>
17. < Symbol , 6 , 4, (>
18. < Symbol , 6 , 5, )>
19. < Symbol , 6 , 6, {>
20. < Identifier , 7 , 5, cout>
21. < Library , 7 , 6, <<"Hello world"
    return 0;
}>>

Symbol Table:
Index   Name    Type    Size
-----   ----    ----    ----
1       hello      var      4
2       world      var      4
3       int      var      4
4       main      var      4
5       cout      var      4
*/
