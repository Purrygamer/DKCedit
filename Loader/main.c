#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "../Generator/generator.h"

#define GAME_NAME "DkkStm.exe"
#define BUILD_VER "DKCedit: Ver 1.1 .%04d.%02d/%02d %s"
#define BUILD_DATE "Jan 04 2024"
#define BUILD_TIME "19:33:06"

#define OLD_DATE_OFFSET 0x4B47D8
#define OLD_TIME_OFFSET 0x4B47F8
#define OLD_STRING_OFFSET 0x279C7
#define NEW_STRING_OFFSET 0x00ADCA35

#define MOD_SIZE_ADDR 0x5DEA24


#define MAX_STRING 255
#define SECTION_OFFSET 0x15E
#define SECTION_HEADER_OFFSET 0x378
#define DEFAULT_SECTIONS 7
#define SECTION_NAME 0x74696465636b642e
#define VIRTUAL_OFFSET 0xB05000 //0xAFE000
#define VIRTUAL_SIZE 0x2000
#define NEW_SECTION_SIZE 0x1000
#define RAW_OFFSET 0x5DEA00 //0x5D6E00
#define CHARACTERISTICS_MAGIC 0xE0000040
#define IMAGE_SIZE 0xB07000 //0xB00000
#define IMAGE_OFFSET 0x1A8

#define DEFAULT_MOD_START 0x5DEA28
#define DEFAULT_MOD_START_VIRT 0xB05028

typedef enum COMMAND {
    HELP,
    ERROR,
    EXIT,
    LOAD,
    GENERATE,
} COMMAND;

unsigned char prompt_user(char* text);

void load_mod(FILE* game);

void loader_loop();

void generator_loop();

COMMAND get_cli_input(char* buf, int size);

void process_input(char* buf, COMMAND type);

void newline_remover(char* input){
    for(int i = 0; i < MAX_STRING; i++){
        if(input[i] == '\n'){
            input[i] = 0;
            return;
        }
    }
    return;
}

void mod_bootloader(FILE *game){
    printf("You chose the bootloader. Writing new section count\n");
    fseek(game, SECTION_OFFSET, SEEK_SET);
    short new_sections = DEFAULT_SECTIONS + 1;
    fwrite(&new_sections, 2, 1, game);
    printf("Section Count is now %d. Moving on to creating section name.\n", new_sections);
    fseek(game, SECTION_HEADER_OFFSET, SEEK_SET);
    unsigned long long name = SECTION_NAME;
    fwrite(&name, 8, 1, game);
    unsigned int sec_field = VIRTUAL_SIZE;
    fwrite(&sec_field, 4, 1, game);
    sec_field = VIRTUAL_OFFSET;
    fwrite(&sec_field, 4, 1, game);
    sec_field = NEW_SECTION_SIZE;
    fwrite(&sec_field, 4, 1, game);
    sec_field = RAW_OFFSET;
    fwrite(&sec_field, 4, 1, game);
    sec_field = 0;
    fwrite(&sec_field, 4, 1, game);
    fwrite(&sec_field, 4, 1, game);
    fwrite(&sec_field, 4, 1, game);
    sec_field = CHARACTERISTICS_MAGIC;
    fwrite(&sec_field, 4, 1, game);
    printf("Section header added, now adding new Image size\n");
    fseek(game, IMAGE_OFFSET, SEEK_SET);
    sec_field = IMAGE_SIZE;
    fwrite(&sec_field, 4, 1, game);
    char empty = 0;

    char debug_info[] = BUILD_VER;
    fseek(game, 0, SEEK_END);
    fwrite(debug_info, sizeof(debug_info), 1, game);
    for(int i = 0; i < NEW_SECTION_SIZE - sizeof(debug_info); i++){
        fwrite(&empty, 1, 1, game);
    }
    fseek(game, OLD_DATE_OFFSET, SEEK_SET);
    char new_date_info[] = BUILD_DATE;
    fwrite(new_date_info, sizeof(new_date_info), 1, game);
    fseek(game, OLD_TIME_OFFSET, SEEK_SET);
    char new_time_info[] = BUILD_TIME;
    fwrite(new_time_info, sizeof(new_time_info), 1, game);
    fseek(game, OLD_STRING_OFFSET, SEEK_SET);
    uint32_t new_jump = NEW_STRING_OFFSET;
    fwrite(&new_jump, sizeof(uint32_t), 1, game);
}

int main(int argc, char* argv[]){
    printf("                    ___           ___           ___                                             \n");
    printf("     _____         /|  |         /\\__\\         /\\__\\         _____                              \n");
    printf("    /::\\  \\       |:|  |        /:/  /        /:/ _/_       /::\\  \\       ___           ___     \n");
    printf("   /:/\\:\\  \\      |:|  |       /:/  /        /:/ /\\__\\     /:/\\:\\  \\     /\\__\\         /\\__\\    \n");
    printf("  /:/  \\:\\__\\   __|:|  |      /:/  /  ___   /:/ /:/ _/_   /:/  \\:\\__\\   /:/__/        /:/  /    \n");
    printf(" /:/__/ \\:|__| /\\ |:|__|____ /:/__/  /\\__\\ /:/_/:/ /\\__\\ /:/__/ \\:|__| /::\\  \\       /:/__/     \n");
    printf(" \\:\\  \\ /:/  / \\:\\/:::::/__/ \\:\\  \\ /:/  / \\:\\/:/ /:/  / \\:\\  \\ /:/  / \\/\\:\\  \\__   /::\\  \\     \n");
    printf("  \\:\\  /:/  /   \\::/~~/~      \\:\\  /:/  /   \\::/_/:/  /   \\:\\  /:/  /   ~~\\:\\/\\__\\ /:/\\:\\  \\    \n");
    printf("   \\:\\/:/  /     \\:\\~~\\        \\:\\/:/  /     \\:\\/:/  /     \\:\\/:/  /       \\::/  / \\/__\\:\\  \\   \n");
    printf("    \\::/  /       \\:\\__\\        \\::/  /       \\::/  /       \\::/  /        /:/  /       \\:\\__\\  \n");
    printf("     \\/__/         \\/__/         \\/__/         \\/__/         \\/__/         \\/__/         \\/__/  \n");
    printf("Version: ");
    printf("1.1\n");
    printf("Type a command to get started... (type help or h for information)\n");
    while(1) {
        char buffer[MAX_STRING];
        COMMAND input = get_cli_input(buffer, MAX_STRING);
        process_input(buffer, input);
    }
}

uint8_t check_nop_rax(uint8_t buffer[4]){
    return buffer[0] == 0x48 && buffer[1] == 0x0F && buffer[2] == 0x1F && buffer[3] == 0xC0;
}

uint8_t check_nop_rbx(uint8_t buffer[4]){
    return buffer[0] == 0x48 && buffer[1] == 0x0F && buffer[2] == 0x1F && buffer[3] == 0xC3;
}

uint8_t check_nop_rcx(uint8_t buffer[4]){
    return buffer[0] == 0x48 && buffer[1] == 0x0F && buffer[2] == 0x1F && buffer[3] == 0xC1;
}

void shuffle_nop_finder(uint8_t buffer[4], uint8_t new_val) {
    buffer[0] = buffer[1];
    buffer[1] = buffer[2];
    buffer[2] = buffer[3];
    buffer[3] = new_val;
}

void load_mod(FILE* game){
    FILE* variables;
    FILE* functions;
    FILE* mod;
    uint8_t cur_byte;
    uint32_t new_code_start;
    uint32_t cur_instruction_addr;
    uint32_t file_address;
    printf("Checking if mod files exist... ");
    if (variables = fopen("variables.txt", "r")){
    } else{
        printf("Missing or corrupted variables.txt file\n");
        return;
    }
    if (functions = fopen("functions.txt", "r")){
    } else{
        printf("Missing or corrupted functions.txt file\n");
        return;
    }
    if (mod = fopen("mod.bin", "rb+")){
    } else{
        printf("Missing or corrupted mod.bin file\n");
        return;
    }
    printf("ok\n");
    printf("Looking for spot in dkkstm.exe to place code...");
    fseek(game, MOD_SIZE_ADDR, SEEK_SET);
    uint32_t space_used;
    fread(&space_used, sizeof(uint32_t), 1, game);
    printf("phys: %x | virt %x\n", DEFAULT_MOD_START + space_used, DEFAULT_MOD_START_VIRT + space_used);
    file_address = DEFAULT_MOD_START + space_used;
    cur_instruction_addr = DEFAULT_MOD_START_VIRT + space_used;
    new_code_start = DEFAULT_MOD_START_VIRT + space_used;
    uint8_t nop_finder[4];
    printf("Writing mod code...");
    fseek(game, DEFAULT_MOD_START + space_used, SEEK_SET);
    while(fread(&cur_byte, 1, 1, mod) == 1) {
        shuffle_nop_finder(nop_finder, cur_byte);
        if(check_nop_rax(nop_finder)) {
            //Don't forget the last nop byte
            fwrite(&cur_byte, 1, 1, game);
            space_used++;
            cur_instruction_addr++;
            file_address++;
            //Put in call (0xE8) byte
            fread(&cur_byte, 1, 1, mod);
            fwrite(&cur_byte, 1, 1, game);
            space_used++;
            cur_instruction_addr++;
            file_address++;
            //Do call instruction
            char call_virt[255];
            if(fgets(call_virt, 255, functions) == NULL){
                printf("Incomplete (or corrupted) functions file. Mod failed to install (space was still used)\n");
                goto mod_end;
            }
            char *end;
            uint32_t target_call = (uint32_t)strtoul(call_virt, &end, 16);
            if (*end != '\0' && *end != '\n') {
                printf("Invalid formatting in functions file. Mod failed to install (space was still used)\n");
                goto mod_end;
            }
            int call_offset = target_call - (cur_instruction_addr + 4);
            fwrite(&call_offset, sizeof(int), 1, game);
            int discard;
            fread(&discard, sizeof(int), 1, mod);
            printf("Call offset generated: %x\n", call_offset);
            space_used += 4;
            cur_instruction_addr += 4;
            file_address += 4;
        } else if (check_nop_rbx(nop_finder)) {
            //Don't forget the last nop byte
            fwrite(&cur_byte, 1, 1, game);
            space_used++;
            cur_instruction_addr++;
            file_address++;
            //First three bytes of mov inst
            fread(&cur_byte, 1, 1, mod);
            fwrite(&cur_byte, 1, 1, game);
            space_used++;
            cur_instruction_addr++;
            file_address++;
            fread(&cur_byte, 1, 1, mod);
            fwrite(&cur_byte, 1, 1, game);
            space_used++;
            cur_instruction_addr++;
            file_address++;
            fread(&cur_byte, 1, 1, mod);
            fwrite(&cur_byte, 1, 1, game);
            space_used++;
            cur_instruction_addr++;
            file_address++;
            //Do variable
            char var_virt[255];
            if(fgets(var_virt, 255, variables) == NULL){
                printf("Incomplete (or corrupted) variables file. Mod failed to install (space was still used)\n");
                goto mod_end;
            }
            char *end;
            uint32_t target_var = (uint32_t)strtoul(var_virt, &end, 16);
            if (*end != '\0' && *end != '\n') {
                printf("Invalid formatting in variables file. Mod failed to install (space was still used)\n");
                goto mod_end;
            }
            int var_offset = target_var - (cur_instruction_addr + 4);
            fwrite(&var_offset, sizeof(int), 1, game);
            int discard;
            fread(&discard, sizeof(int), 1, mod);
            printf("Variable offset generated: %x\n", var_offset);
            space_used += 4;
            cur_instruction_addr += 4;
            file_address += 4;

        } else if(check_nop_rcx(nop_finder)) {
            fwrite(&cur_byte, 1, 1, game);
            space_used++;
            cur_instruction_addr++;//finish out nop
            file_address++;
            printf("Replacing old call pointer...");
            char physical_buffer[255];
            char virtual_buffer[255];
            if(fgets(physical_buffer, 255, functions) == NULL){
                printf("Incomplete (or corrupted) functions file. Mod failed to install (space was still used)\n");
                goto mod_end;
            }
            if(fgets(virtual_buffer, 255, functions) == NULL){
                printf("Incomplete (or corrupted) functions file. Mod failed to install (space was still used)\n");
                goto mod_end;
            }
            char *endptr;
            uint32_t target_address = (uint32_t)strtoul(virtual_buffer, &endptr, 16);
            if (*endptr != '\0' && *endptr != '\n') {
                printf("Invalid formatting in functions file. Mod failed to install (space was still used)\n");
                goto mod_end;
            }
            uint32_t physical_address = (uint32_t)strtoul(physical_buffer, &endptr, 16);
            if (*endptr != '\0' && *endptr != '\n') {
                printf("Invalid formatting in functions file. Mod failed to install (space was still used)\n");
                goto mod_end;
            }
            fseek(game, physical_address, SEEK_SET);
            int offset = cur_instruction_addr - target_address;
            fwrite(&offset, sizeof(int), 1, game);
            printf("%x\n", offset);
            fseek(game, file_address, SEEK_SET);
        } else{
            fwrite(&cur_byte, 1, 1, game);
            space_used++;
            cur_instruction_addr++;
            file_address++;
        }
    }
    printf("ok\n");
    mod_end:
    printf("Mod injected, closing files\n");
    fseek(game, MOD_SIZE_ADDR, SEEK_SET);
    fwrite(&space_used, sizeof(uint32_t), 1, game);
    fclose(mod);
    fclose(functions);
    fclose(variables);
    return;
}

unsigned char prompt_user(char* text) {
    reprompt:
    printf("%s", text);
    char input[MAX_STRING];
    fgets(input, 200, stdin);
    if(input[0] != 'y' && input[0] != 'n'){
            printf("Invalid option type y or n\n");
            goto reprompt;
    } else if(input[0] == 'y'){ 
        return 1;
    }
    return 0;
}

void loader_loop() {

    printf("Input the file path to %s\n", GAME_NAME);
    char input[MAX_STRING];
    fgets(input, 200, stdin);
    newline_remover(input);
    strcat(input, "\\");
    strcat(input, GAME_NAME);
    printf("%s\n", input);
    FILE *game;
    if (game = fopen(input, "rb+")){
    } else{
        printf("Invalid filename\n");
        return;
    }
    fseek(game, SECTION_OFFSET, SEEK_SET);
    short sections;
    fread(&sections, 2, 1, game);
    printf("Sections found = %d\n", sections);
    
    if(sections == DEFAULT_SECTIONS){
        default_prompt:
        printf("Default amount of sections in PE found. Would you like to run the mod initializer?\n(y / n)\n");
        fgets(input, 10, stdin);
        newline_remover(input);
        if(input[0] != 'y' && input[0] != 'n'){
            printf("Invalid option type y or n\n");
            goto default_prompt;
        }else if(input[0] == 'y'){
            mod_bootloader(game);
        }else{
            goto finish;
        }
    }
    char old_path[256];
    getcwd(old_path, 256);
    mod_loop:; //C syntax (╯°□°)╯︵ ┻━┻
    unsigned char more_prompt = prompt_user("This file has already been modded. Would you like to add more mods?\n");
    if(!more_prompt) {
        goto finish;
    }
    printf("Input the file path to the mod folder (path must be <200 characters)\n");
    fgets(input, 200, stdin);
    newline_remover(input);
    if(chdir(input) != 0) {
        printf("Invalid path. Include the full path\n");
        goto mod_loop;
    }
    load_mod(game);
    chdir(old_path);
    goto mod_loop;
finish:
    fclose(game);
    printf(":3\n");
}

void generator_loop() {
    char old_path[0x100];
    getcwd(old_path, 0x100);
    char mod_path[0x100];
    char* code_name = NULL;
    char** patch_names;
    int patch_count = 0;
    printf("Enter the filepath to the mod files\n");
    fgets(mod_path, 0x100, stdin);
    newline_remover(mod_path);
    chdir(mod_path);
    if(prompt_user("Does this mod need to compile code?\n")) {
        printf("Enter the mod name\n");
        code_name = calloc(1, 0x100);
        fgets(code_name, 0x100, stdin);
        newline_remover(code_name);
    }
    if(prompt_user("Does this mod have patches?\n")) {
        char patch_count_buf[0x100];
        invalid_number:
        printf("How many patches\n");
        fgets(patch_count_buf, 0x100, stdin);
        if (sscanf(patch_count_buf, "%d", &patch_count) != 1) {
            printf("Not a decimal number\n");
            goto invalid_number;
        }
        patch_names = malloc(patch_count * sizeof(char*));
        for(int i = 0; i < patch_count; i++) {
            char name_temp[0x100];
            printf("Input the name for patch %d\n", patch_count);
            fgets(name_temp, 0x100, stdin);
            char* new_name = malloc(sizeof(name_temp));
            strcpy(new_name, name_temp);
            patch_names[i] = new_name;
        }
    }
    CODE_POSITION exec_order;
    char order_buf[0x100];
    invalid_order:
    printf("When should the code run (f = First, m = Middle, l = last)");
    fgets(order_buf, 0x100, stdin);
    if(order_buf[0] == 'f') {
        exec_order = RUN_FIRST;
    } else if (order_buf[0] == 'm') {
        exec_order = RUN_MIDDLE;
    } else if (order_buf[0] == 'l') {
        exec_order = RUN_LAST;
    } else {
        goto invalid_order;
    }

    generate_binary(exec_order, patch_count, code_name, patch_names);
    free(code_name);
    if(patch_count == 0) {
        return;
    }

    for(int i = 0; i < patch_count; i++) {
        free(patch_names[i]);
    }
    free(patch_names);
}

COMMAND get_cli_input(char* buf, int size) {
    fgets(buf, size, stdin);
    newline_remover(buf);
    if(strcmp(buf, "h") == 0 || strcmp(buf, "help") == 0) {
        return HELP;
    }
    if(strcmp(buf, "l") == 0 || strcmp(buf, "loader") == 0) {
        return LOAD;
    }
    if(strcmp(buf, "g") == 0 || strcmp(buf, "generator") == 0) {
        return GENERATE;
    }
    if(strcmp(buf, "q") == 0 || strcmp(buf, "quit") == 0) {
        return EXIT;
    }
    return ERROR;
}

void process_input(char* buf, COMMAND type) {
    switch (type)
    {
    case EXIT:
        exit(0);
        break;
    case HELP:
        printf("h help - Displays this page\n");
        printf("q quit - Terminates DKCedit\n");
        printf("l loader - Launches the loader\n");
        printf("g generator - Launches the generator\n");
        break;
    case GENERATE:
        generator_loop();
        break;
    case LOAD:
        loader_loop();
        break;
    default:
        printf("Unknown command. Type h or help for information\n");
        break;
    }
}