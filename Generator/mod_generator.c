#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include "generator.h"
#define COMPILE_COMMAND "gcc -S -masm=intel "
#define MAX_PATH 256



const char* header_code[15] = {
    "\tnop rcx\n\t", "push rax\n\t", "push rbx\n\t", "push rcx\n\t", "push rdx\n\t", 
	"push rsi\n\t", "push rdi\n\t", "push r8\n\t", "push r9\n\t", "push r10\n\t", 
    "push r11\n\t", "push r12\n\t", "push r13\n\t", "push r14\n\t", "push r15\n"
};

const char* pop_code[14] = {
    "pop r15\n\t", "pop r14\n\t", "pop r13\n\t", "pop r12\n\t", 
	"pop r11\n\t", "pop r10\n\t", "pop r9\n\t", "pop r8\n\t", 
	"pop rdi\n\t", "pop rsi\n\t", "pop rdx\n\t", "pop rcx\n\t", 
	"pop rbx\n\t", "pop rax\n\t",
};

const char* ending_code[11] = {
    "mov temp_rax[rip + 0], rax\n\t", 
	"pop rax\n\t", "mov temp_ret[rip + 0], rax\n\t", "mov rax, temp_rax[rip + 0]\n\t", 
	"nop rax\n\t", "call   0xffffffffffabcdef\n\t", "mov temp_rax[rip + 0], rax\n\t", 
	"mov rax, temp_ret[rip + 0]\n\t", "push rax\n\t", "mov rax, temp_rax[rip + 0]\n\t", 
	"ret\n\t", 
};

void generate_binary(CODE_POSITION run_position, int patch_count, char* mod_name, char** mod_list) {
    FILE* mod_binary;
    mod_binary = fopen("mod.bin", "w+");
    fwrite(FILE_HEADER, sizeof(FILE_HEADER), 1, mod_binary);
    if(mod_name != NULL) {
        printf("Compiling %s\n", mod_name);
        char compile_command[MAX_PATH + sizeof(COMPILE_COMMAND)];
        strcpy(compile_command, COMPILE_COMMAND);
        strcat(compile_command, mod_name);
        printf("issuing %s\n", compile_command);
        system(compile_command);
        //Now insert needed code into assembler source
        printf("Inserting header/ender code\n");
        FILE* original_source;
        FILE* gen_source;
        char file_name[MAX_PATH];
        strcpy(file_name, mod_name);
        file_name[strlen(file_name) - 1] = 's';
        printf("Assembler source: %s\n", file_name);
        original_source = fopen(file_name, "r");
        gen_source = fopen("dkcedit_out.s", "w+");
        if(original_source == NULL) {
            printf("Invalid path for the file\n");
            goto compile_done;
        }
        char file_buffer[MAX_PATH];
        while(fgets(file_buffer, MAX_PATH, original_source) != NULL) {
            if(strcmp(file_buffer, "mod_main:\n") == 0){
                fwrite(file_buffer, strlen(file_buffer), 1, gen_source);
                printf("mod_main found\n");
                for(int i = 0; i < 15; i++){
                    fwrite(header_code[i], strlen(header_code[i]), 1, gen_source);
                }
            }
            else if(strcmp(file_buffer, "\t.seh_endproc\n") == 0) {
                printf("End of code found\n");
                fseek(gen_source, -5, SEEK_CUR); //moves back to the start of the ret\n line
                for(int i = 0; i < 25; i++){
                    fwrite(ending_code[i], strlen(ending_code[i]), 1, gen_source);
                }
                fwrite(file_buffer, strlen(file_buffer), 1, gen_source);
            }
            else {
                fwrite(file_buffer, strlen(file_buffer), 1, gen_source);
            }
        }
        fclose(original_source);
        fclose(gen_source);
        printf("Assembling the code\n");
        system("as -o dkcedit_out.o dkcedit_out.s");
        printf("Dumping text segment\n");
        system("objcopy -O binary --only-section=.text dkcedit_out.o code.bin");
        printf("code.bin created.\nIf there is no mod.bin then there was a compilation error.\n");
        
        printf("Adding variables\n");
        FILE* variables = fopen("variables.txt", "r");
        if(variables == NULL) {
            printf("Missing variables.txt\n");
            goto compile_done;
        }
        uint16_t variable_count = 0;
        char variable_buf[0x100];
        fwrite(VARIABLE_HEADER, sizeof(VARIABLE_HEADER), 1, mod_binary);
        while(fgets(variable_buf, 0x100, variables) != NULL) {
            variable_count++;
        }
        fseek(variables, SEEK_SET, 0);
        fwrite(&variable_count, sizeof(uint16_t), 1, mod_binary);
        while(fgets(variable_buf, 0x100, variables) != NULL) {
            char *endptr;
            uint32_t var_32 = (uint32_t)strtoul(variable_buf, &endptr, 16);
            fwrite(&var_32, sizeof(uint32_t), 1, mod_binary);
            if (*endptr != '\0' && *endptr != '\n') {
                printf("Invalid formatting in variables file. Mod binary failed\n");
                goto compile_done;
            }
        }

        printf("Adding functions\n");
        FILE* functions = fopen("functions.txt", "r");
        if(variables == NULL) {
            printf("Missing functions.txt\n");
            goto compile_done;
        }
        uint16_t function_count = 0;
        char function_buf[0x100];
        fwrite(FUNCTION_HEADER, sizeof(FUNCTION_HEADER), 1, mod_binary);
        while(fgets(function_buf, 0x100, functions) != NULL) {
            function_count++;
        }
        fseek(functions, SEEK_SET, 0);
        fwrite(&function_count, sizeof(uint16_t), 1, mod_binary);
        while(fgets(function_buf, 0x100, functions) != NULL) {
            char *endptr;
            uint32_t fun_32 = (uint32_t)strtoul(function_buf, &endptr, 16);
            fwrite(&fun_32, sizeof(uint32_t), 1, mod_binary);
            if (*endptr != '\0' && *endptr != '\n') {
                printf("Invalid formatting in variables file. Mod binary failed\n");
                goto compile_done;
            }
        }
    char mod_head_buf[0x10];
    strcpy(mod_head_buf, MOD_HEADER);
    fwrite(mod_head_buf, sizeof(MOD_HEADER), 1, mod_binary);
    FILE* code = fopen("code.bin", "rb");
    fseek(code, 0, SEEK_END);
    uint32_t code_size = ftell(code);
    if(code_size == 0xFFFFFFFF) {
        perror("File Size Error");
        fclose(code);
        goto compile_done;
    }
    fseek(code, 0, SEEK_SET);
    char *code_buf = malloc(code_size);
    if(code_buf == NULL) {
        perror("Allocation for code failed");
        fclose(code);
        goto compile_done;
    }
    size_t code_bytes_read = fread(code_buf, 1, code_size, code);
    if(code_bytes_read != code_size) {
        perror("Error reading code.bin");
        close(code);
        free(code_buf);
        goto compile_done;
    }
    fwrite(&code_size, sizeof(uint32_t), 1, mod_binary);
    fwrite(code_buf, 1, code_size, mod_binary);
    free(code_buf);
    fclose(code);
    compile_done:
    }
    if(patch_count){
        printf("Adding %d patches to mod file\n", patch_count);
    } else {
        printf("No patches\n");
        return;
    }
    for(int i = 0; i < patch_count; i++) {
        char* cur_patch = mod_list[i];
        FILE* patch_file = fopen(cur_patch, "rb");
        char header[0x10];
        strcpy(header, PATCH_HEADER);
        fwrite(header, sizeof(header), 1, mod_binary);
        printf("Now writing patch %s... ", cur_patch);
        
        if(fseek(patch_file, 0, SEEK_END) != 0) {
            perror("End of File error");
            fclose(patch_file);
            printf("Skipping to next patch\n");
            continue;
        }

        uint32_t size = ftell(patch_file);
        if(size == -1) {
            perror("File size error");
            fclose(patch_file);
            printf("Skipping to next patch\n");
            continue;
        }
        fwrite(&size, sizeof(uint32_t), 1, mod_binary);
        printf("patch size %d\n", size);
        unsigned char read_byte = 0;
        while(read_byte = fread(&read_byte, 1, 1, patch_file) > 0) {
            fwrite(&read_byte, 1, 1, mod_binary);
        }
        fclose(patch_file);
    }
    char end_buf[0x10];
    strcpy(end_buf, END_OF_FILE_MARK);
    fwrite(end_buf, sizeof(END_OF_FILE_MARK), 1, mod_binary);
    fclose(mod_binary);
    return;
}
