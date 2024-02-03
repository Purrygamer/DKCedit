gcc -g -c main.c
gcc -g -c ../Generator/mod_generator.c
gcc main.o mod_generator.o -o DKCedit.exe