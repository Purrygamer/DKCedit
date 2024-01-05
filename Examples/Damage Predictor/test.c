char evil_trick[] __attribute__((section(".text"))) = {'D', 'K', 'C', 'E', 0x48, 0x0F, 0x1F, 0xC1};
char new_attack[] __attribute__((section(".text"))) = "Attack(####)";
char temp_rax[] __attribute__((section(".text"))) = "1234567";
char temp_ret[] __attribute__((section(".text"))) = "ABCDEFG";
void mod_main(){
	int* game_damage = 0;
	asm volatile(".intel_syntax noprefix\n\t"
	"nop %%rbx\n\t"
	"mov %0, [%%rip-0x2dc997]\n\t"
	: "+r"(game_damage)
	);
	game_damage += 0x1d04 / sizeof(int);
	int damage;
	damage = *game_damage;
	new_attack[7] = (damage / 1000) + 0x30;
	damage = damage % 1000;
	new_attack[8] = (damage / 100) + 0x30;
	damage = damage % 100;
	new_attack[9] = (damage / 10) + 0x30;
	damage = damage % 10;
	new_attack[10] = damage + 0x30;
}