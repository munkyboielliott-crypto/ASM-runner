// src/asmrunner.c
// ASMrunner - Pre-release (v0.1, 3m17d)
// Modular x86 assembler foundation in C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 256
#define MAX_INSTRUCTIONS 128

typedef struct {
    const char* mnemonic;
    unsigned char opcode;
    int operand_count; // number of operands this instruction expects
} Instruction;

// A starting set of common x86 instructions
Instruction instruction_set[] = {
    {"NOP", 0x90, 0},
    {"MOV", 0xB8, 2},  // simplified: MOV reg, imm
    {"ADD", 0x01, 2},
    {"SUB", 0x29, 2},
    {"INC", 0x40, 1},
    {"DEC", 0x48, 1},
    {"JMP", 0xEB, 1},
    {"CALL", 0xE8, 1},
    {"RET", 0xC3, 0},
    {"INT", 0xCD, 1},
    {"CMP", 0x39, 2},
    {"AND", 0x21, 2},
    {"OR", 0x09, 2},
    {"XOR", 0x31, 2},
    {"PUSH", 0x50, 1},
    {"POP", 0x58, 1},
    {"LEA", 0x8D, 2},
    {"TEST", 0x85, 2}
    // You can add hundreds more here gradually
};

// Lookup opcode by mnemonic
int get_opcode(const char* instr, int* operand_count) {
    for (int i = 0; i < sizeof(instruction_set)/sizeof(Instruction); i++) {
        if (strcmp(instr, instruction_set[i].mnemonic) == 0) {
            if (operand_count) *operand_count = instruction_set[i].operand_count;
            return instruction_set[i].opcode;
        }
    }
    return -1;  // unknown instruction
}

// Parse a line of ASM
void parse_line(char* line, FILE* out) {
    char* token = strtok(line, " \t\n");
    if (!token || token[0] == ';') return; // skip empty lines/comments

    for (int i = 0; token[i]; i++) token[i] = toupper(token[i]);

    int operand_count;
    int opcode = get_opcode(token, &operand_count);
    if (opcode == -1) {
        printf("Unknown instruction: %s\n", token);
        return;
    }

    fputc(opcode, out);

    // Parse operands
    for (int i = 0; i < operand_count; i++) {
        token = strtok(NULL, " \t\n");
        if (!token) break;

        int value = 0;
        if (strncmp(token, "0X", 2) == 0) {
            sscanf(token, "%x", &value);
        } else if (isalpha(token[0])) {
            // TODO: parse register codes (AX, BX, etc.) later
            value = 0; // placeholder for registers
        } else {
            value = atoi(token);
        }
        fputc(value, out);
    }
}

// Assemble a file
void assemble_file(const char* input_path, const char* output_path) {
    FILE* in = fopen(input_path, "r");
    if (!in) {
        perror("Error opening input file");
        exit(1);
    }

    FILE* out = fopen(output_path, "wb");
    if (!out) {
        perror("Error opening output file");
        fclose(in);
        exit(1);
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), in)) {
        parse_line(line, out);
    }

    fclose(in);
    fclose(out);
    printf("Assembly complete! Output written to %s\n", output_path);
}

int main() {
    const char* input_file = "examples/test.asm";
    const char* output_file = "output/test.bin";

    // create output directory if it doesn't exist (POSIX)
    system("mkdir -p output");

    assemble_file(input_file, output_file);

    return 0;
}
