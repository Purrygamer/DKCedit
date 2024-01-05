#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMPILE_COMMAND "gcc -S -masm=intel "
#define MAX_PATH 256

const char* header_code[15] = {
    "\tnop rcx\n\t", "push rax\n\t", "push rbx\n\t", "push rcx\n\t", "push rdx\n\t", 
	"push rsi\n\t", "push rdi\n\t", "push r8\n\t", "push r9\n\t", "push r10\n\t", 
    "push r11\n\t", "push r12\n\t", "push r13\n\t", "push r14\n\t", "push r15\n"
};

const char* ending_code[25] = {
    "pop r15\n\t", "pop r14\n\t", "pop r13\n\t", "pop r12\n\t", 
	"pop r11\n\t", "pop r10\n\t", "pop r9\n\t", "pop r8\n\t", 
	"pop rdi\n\t", "pop rsi\n\t", "pop rdx\n\t", "pop rcx\n\t", 
	"pop rbx\n\t", "pop rax\n\t", "mov temp_rax[rip + 0], rax\n\t", 
	"pop rax\n\t", "mov temp_ret[rip + 0], rax\n\t", "mov rax, temp_rax[rip + 0]\n\t", 
	"nop rax\n\t", "call   0xffffffffffabcdef\n\t", "mov temp_rax[rip + 0], rax\n\t", 
	"mov rax, temp_ret[rip + 0]\n\t", "push rax\n\t", "mov rax, temp_rax[rip + 0]\n\t", 
	"ret\n\t", 
};

int main(int argc, char* argv[]){
    if(argc < 2){
        printf("Supply a path to a file (ex. path\\to\\file.c)\n");
        return -1;
    }
    if(strlen(argv[1]) > MAX_PATH){
        printf("File path/name too long\n");
    }
    printf("Compiling %s\n", argv[1]);
    char compile_command[MAX_PATH + sizeof(COMPILE_COMMAND)];
    strcpy(compile_command, COMPILE_COMMAND);
    strcat(compile_command, argv[1]);
    printf("issuing %s\n", compile_command);
    system(compile_command);
    //Now insert needed code into assembler source
    printf("Inserting header/ender code\n");
    FILE* original_source;
    FILE* gen_source;
    char file_name[MAX_PATH];
    strcpy(file_name, argv[1]);
    file_name[strlen(file_name) - 1] = 's';
    printf("Assembler source: %s\n", file_name);
    original_source = fopen(file_name, "r");
    gen_source = fopen("dkcedit_out.s", "w+");
    if(original_source == NULL) {
        printf("Invalid path for the file\n");
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
    system("objcopy -O binary --only-section=.text dkcedit_out.o mod.bin");
    printf("mod.bin created.\nIf there is no mod.bin then there was a compilation error.\n");
    return 0;
}