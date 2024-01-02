#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#define GAME_NAME "DkkStm.exe"
#define BUILD_VER "DKCedit: Ver 0.8 .%04d.%02d/%02d %s"
#define BUILD_DATE "Dec 22 2023"
#define BUILD_TIME "01:14:24"

#define OLD_DATE_OFFSET 0x4B47D8
#define OLD_TIME_OFFSET 0x4B47F8
#define OLD_STRING_OFFSET 0x279C7
#define NEW_STRING_OFFSET 0x00ADCA35

#define NEW_ATTACK_TEXT "Attack ("
#define NEW_ATTACK_CODE_PHYSICAL 0x5DEA28
#define NEW_ATTACK_CODE_VIRTUAL 0xB05028
#define NEW_ATTACK_TEXT_PHYSICAL 0x5DEAC5
#define NEW_ATTACK_VIRTUAL 0xB050C5
#define PHYSICAL_PREDICTION_OFFSET 0x172B0E
#define VIRTUAL_PREDICTION_OFFSET 0x173712
#define ORIG_PREDICTION_CALL_PHYSICAL 0x17923C
#define ORIG_PREDICTION_CALL_VIRTUAL 0x179E40
#define PREDICT_MENU_CODE 0x173340
#define CODE_JUMP_BYTES NEW_ATTACK_CODE_VIRTUAL - ORIG_PREDICTION_CALL_VIRTUAL
#define JUMP_BYTES NEW_ATTACK_VIRTUAL - VIRTUAL_PREDICTION_OFFSET

#define MOD_SIZE_ADDR 0x5DEA24

const unsigned char damage_mod[] =
    { 0x50, 0x53, 0x51, 0x52, 0x48, 0x8B, 0x05, 0x69, 0x36, 0xD2, 0xFF, 0x90, 0x8B, 0x80, 0x04, 0x1D,
    0x00, 0x00, 0x90, 0x48, 0x8D, 0x1D, 0x8F, 0x00, 0x00, 0x00, 0x48, 0x31, 0xD2, 0x48, 0xC7, 0xC1,
    0xE8, 0x03, 0x00, 0x00, 0x48, 0xF7, 0xF1, 0x48, 0x83, 0xC0, 0x30, 0x88, 0x03, 0x48, 0xFF, 0xC3,
    0x48, 0x89, 0xD0, 0x48, 0x31, 0xD2, 0x48, 0xC7, 0xC1, 0x64, 0x00, 0x00, 0x00, 0x48, 0xF7, 0xF1,
    0x48, 0x83, 0xC0, 0x30, 0x88, 0x03, 0x48, 0xFF, 0xC3, 0x48, 0x89, 0xD0, 0x48, 0x31, 0xD2, 0x48,
    0xC7, 0xC1, 0x0A, 0x00, 0x00, 0x00, 0x48, 0xF7, 0xF1, 0x48, 0x83, 0xC0, 0x30, 0x48, 0x83, 0xC2,
    0x30, 0x88, 0x03, 0x88, 0x53, 0x01, 0xB2, 0x29, 0x88, 0x53, 0x02, 0x5A, 0x59, 0x5B, 0x58, 0x48,
    0x89, 0x05, 0x3D, 0x00, 0x00, 0x00, 0x58, 0x48, 0x89, 0x05, 0x3D, 0x00, 0x00, 0x00, 0x48, 0x8B,
    0x05, 0x2E, 0x00, 0x00, 0x00, 0xE8, 0x92, 0xE2, 0x66, 0xFF, 0x48, 0x89, 0x05, 0x22, 0x00, 0x00,
    0x00, 0x48, 0x8B, 0x05, 0x23, 0x00, 0x00, 0x00, 0x50, 0x48, 0x8B, 0x05, 0x13, 0x00, 0x00, 0x00, 0xC3 };



#define MAX_STRING 255
#define COUNTER_MAGIC 0x520F60
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

unsigned char prompt_user(char* text);
void attack_calculator_mod(FILE* game);
void load_mod(FILE* game);
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

void main(){
    //system("COLOR 05");
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
    printf("0.8\n");
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
        exit(-1);
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
        }
    } else {
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
    }
    /*
    unsigned char prompted = prompt_user("Would you like the attack calculator mod? (y/n)");
    if(!prompted){
        goto finish;
    }
    attack_calculator_mod(game);
    */
finish:
    fclose(game);
    printf(":3\n");

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
    
    cur_instruction_addr = DEFAULT_MOD_START_VIRT + space_used;
    new_code_start = DEFAULT_MOD_START_VIRT + space_used;
    uint8_t nop_finder[4];
    printf("Writing mod code...");
    fseek(game, DEFAULT_MOD_START + space_used, SEEK_SET);
    while(fread(&cur_byte, 1, 1, mod) == 1) {
        shuffle_nop_finder(nop_finder, cur_byte);
        if(check_nop_rax(nop_finder)) {
            //Put in call (0xE8) byte
            fwrite(&cur_byte, 1, 1, game);
            space_used++;
            cur_instruction_addr++;
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
        } else if (check_nop_rbx(nop_finder)) {
            //First two bytes of mov inst
            fwrite(&cur_byte, 1, 1, game);
            space_used++;
            cur_instruction_addr++;
            fread(&cur_byte, 1, 1, mod);
            fwrite(&cur_byte, 1, 1, game);
            space_used++;
            cur_instruction_addr++;
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
        } else if(check_nop_rcx(nop_finder)) {
            fwrite(&cur_byte, 1, 1, game);
            space_used++;
            cur_instruction_addr++;//finish out nop
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
            fseek(game, cur_instruction_addr, SEEK_SET);
        } else{
            fwrite(&cur_byte, 1, 1, game);
            space_used++;
            cur_instruction_addr++;
        }
    }
    printf("ok\n");
    mod_end:
    printf("Mod injected, closing files\n");
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

void attack_calculator_mod(FILE* game){
    printf("Beginning attack calculator mod\n");
    fseek(game, NEW_ATTACK_TEXT_PHYSICAL, SEEK_SET);
    char new_attack[] = NEW_ATTACK_TEXT;
    fwrite(new_attack, sizeof(new_attack), 1, game);
    printf("Overwriting original string pointer\n");
    fseek(game, PHYSICAL_PREDICTION_OFFSET, SEEK_SET);
    uint32_t new_jump = JUMP_BYTES;
    fwrite(&new_jump, sizeof(uint32_t), 1, game);
    printf("Writing new code\n");
    fseek(game, NEW_ATTACK_CODE_PHYSICAL, SEEK_SET);
    fwrite(damage_mod, sizeof(damage_mod), 1, game);
    printf("Overwriting prediction menu call");
    fseek(game, ORIG_PREDICTION_CALL_PHYSICAL, SEEK_SET);
    uint32_t new_call = CODE_JUMP_BYTES;
    fwrite(&new_call, sizeof(uint32_t), 1, game);
}