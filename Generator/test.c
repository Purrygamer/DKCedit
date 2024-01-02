char new_attack[] __attribute__((section(".text"))) = "Attack(####)";
char temp_rax[] __attribute__((section(".text"))) = "1234567";
char temp_ret[] __attribute__((section(".text"))) = "ABCDEFG";
void mod_main(){
	int damage = 0;
	asm volatile(".intel_syntax noprefix\n\t"
	"nop %%rbx\n\t"
	"mov %0, [%%rip-0x2dc997]\n\t"
	"mov %0, [%0+0x1d04]"
	: "+r"(damage)
	);
	new_attack[7] = (damage / 1000) + 0x30;
	damage = damage % 1000;
	new_attack[8] = (damage / 100) + 0x30;
	damage = damage % 100;
	new_attack[9] = (damage / 10) + 0x30;
	damage = damage % 10;
	new_attack[10] = damage + 0x30;
}