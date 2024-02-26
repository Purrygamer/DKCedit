#ifndef GENERATOR
#define GENERATOR

#define FILE_HEADER ".DKCe"
#define VARIABLE_HEADER ".VBL"
#define FUNCTION_HEADER ".FXN"
#define MOD_HEADER ".MOD"
#define PATCH_HEADER ".PTH"
#define END_OF_FILE_MARK ".EOF"

void generate_binary(CODE_POSITION run_position, int patch_count, char* mod_name, char** mod_list);

typedef enum CODE_POSITION{
    RUN_FIRST,
    RUN_MIDDLE,
    RUN_LAST
} CODE_POSITION;

#endif