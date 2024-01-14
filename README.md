# DKCedit
Modding tool for the Steam release of Dokapon Kingdom: Connect
<br>
DKCedit comprises two pieces of software used to make code related mods for Dokapon Kingdom. These are located in Generator and Loader. If you only want to install mods, then you only have to use the loader application.

## Requirements
If you are only loading mods and not writing them, you just need to use the `DKCedit.exe` file and not `mod_generator.exe`. <br>
***
If you plan on writing mods, you will need both executables. <br>
🔹 gcc version 8.1.0 x86_64-posix-sjlj (x86_64-posix-seh may also work, but if the tools are failing compilation use the sjlj compiler)<br> (Yes, you need this specific version) <br>[Download](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/8.1.0/)<br>
[Installation Instructions](https://whitgit.whitworth.edu/tutorials/installing_mingw_64)
***
Recommended Software
* [Hxd](https://mh-nexus.de/en/hxd/)
* [Ghidra](https://ghidra-sre.org/)
* [Cheat Engine](https://www.cheatengine.org/)
* [Windows Calculator Programmer Mode](https://www.howtogeek.com/392134/how-to-use-the-windows-10-calculator/)

***
Required Knowledge <br>
Currently, writing code related mods are not straight-forward. So, here are C concepts that I think you should know and understand that will make programming/debugging your mods easier. <br>
Experience with terminals
* [C Programming Language](https://www.w3schools.com/c/c_intro.php)
* [Hexadecimal](https://byjus.com/maths/hexadecimal-number-system/#:~:text=The%20hexadecimal%20number%20system%20is,digit%20represents%20a%20decimal%20value.)
* [Bit Manipulation](https://en.wikipedia.org/wiki/Bitwise_operation)
* [pointers](https://www.w3schools.com/c/c_pointers.php)
* [Stack and Heap Memory](https://craftofcoding.wordpress.com/2015/12/07/memory-in-c-the-stack-the-heap-and-static/)
* [Physical and Virtual Memory](https://people.cs.pitt.edu/~xianeizhang/notes/vm.html)
* [x86 Registers](https://cs.brown.edu/courses/cs033/docs/guides/x64_cheatsheet.pdf)

## ⚠️Programming Restrictions
Below are things that either do not work (to my knowledge) or are untested with DKCedit. <br>

* Linking new DLL filed (Does not work)
* Using the C standard library (not tested, likely does not work)
* Floating point operations (Untested, some parts might work while others will not work)
* DKCedit currently only adds 4kB to `DkkStm.exe` (This will likely increase in the future)

## ♻️ Deleting mods
The only way to remove mods is to delete the modded `DkkStm.exe` and replace it with an unmodded one. You can either keep a backup of the unmodded executable (I recommend this) or [download a fresh copy from steam.](https://help.steampowered.com/en/faqs/view/0C48-FCBD-DA71-93EB)

## Loader
I recommend opening the Loader application in a command prompt window rather than just opening the executable. If the program terminates without installing the mod properly, you will be able to read the log (double clicking the application will close the command prompt window then the executable terminates). <br>
Provide the path to `DkkStm.exe`. Do not include `DkkStm.exe` in the path (ex `C:\path\to\steam` rather than `C:\path\to\steam\DkkStm.exe`).
<br>
DKCedit will also ask for a path for a mod which should go to the directory and all the way to the `mod.bin` file.
<br>
For both paths, relative paths are also an option (ex if you're mod is in a directory called `new_mod` and DKCedit is in the previous directory, you can use the argument `./new_mod` rather than `C:\path\to\new_mod`)
<br>
If the `DkkStm.exe` file has not been patched by DKCedit, then it will ask if you want to patch `DkkStm.exe`. If `DkkStm.exe` has already been patched, then DKCedit will ask if you want to add more mods to it.

## Generator
I also recommend opening the Generator file in a command prompt window for the same reasons as the Loader. Generator takes in one argument when running, which is the path to a C source file. When providing the path, **INCLUDE** the name of the C source file with the path. If the generator is successful, it will output a file called `mod.bin`. This contains all of the assembled source code for your mod. There are also two more files to have a complete mod, which are `functions.txt` and `variables.txt`.

### variables.txt
This file contains the pointers to a variable that Dokapon Kingdom uses during gameplay. If your mod access one of these variables, you need to put the address to that variable into this file. Put it in the `variables.txt` file in hex without a `0x` prefix.
<br>
The way to get this address is to find where the variable is located relative to the start of the application. If you use Cheat Engine to find the variable at `0xABCDEF`, it will be displayed as `DkkStm.exe+ABCDEF`. In Ghidra it will look like `140ABCDEF`, however only the last 6 digits are needed.

### functions.txt
This file contains modifications to function pointers. There are two cases of what need to be put in this file.
<br>
Case 1: Calling a game function
Put the virtual address to that function in the file (same process for finding it as for a variable)
Case 2: Modifying a pointer the game uses
You will need both where in the `DkkStm.exe` the pointer is located and where the pointer is located in memory (these are **NOT** the same value).
My method of finding them is to find the virtual one first (with Cheat Engine or Ghidra) and search for the string of bytes in the `DkkStm.exe` file. If you are changing a call instruction, give the address to the location of the next instruction (the one immediately after the `call` instruction). <br>
⚠️ The order of the addresses matter! In cases where you need both the physical and virtual address for the same variable/function, the **physical** address goes first. <br>
⚠️ You need to put the virtual address to the original function you overwrote at the end of your `functions.txt` file. Although it looks like your code stops after `return;`, the generator will add some more code to call the origial function which requires the virtual location of the original instruction.
### Code file
This file will contain your code (athough you may use multiple files with some restrictions).
<br>
There are some requirements that your code must follow.
#### Code requirements
You need two variables before your code starts (they do not need to be the first two; they just need to be present). They are:
<br>
```
char temp_rax[] __attribute__((section(".text"))) = "1234567";
char temp_ret[] __attribute__((section(".text"))) = "ABCDEFG";
```
The values `1234567` and `ABCDEFG` do not need to be these values specifically, but if you modify them they must be 8-bytes each (the compiler will add one more `\0` at the end of any `char*` or `char[]`). <br>
***
The first function that the game calls must be named `mod_main()`, which will be a void function that takes no arguments.<br>
***
There are two types of variables in your code: variables you create for your code and variables that `DkkStm.exe` uses. When you make a variable for your code, no extra steps are needed to make it work properly. If you want to use a variable from `DkkStm.exe` (or access the memory in some form), here are the steps to do so.
1. Create a pointer variable with the type of the variable you want. You can put a value there but I recommend to is `0x00` or the `Null` value.
2. Create an `asm` block. The first line will always be `asm volatile(".intel_syntax noprefix\n\t"`.
3. Determine what kind of variable you want to access. There are two types of variables that you can access.
   * The variable is in a register. In this case you would need a line of `"mov %0, %%rbx\n\t"` (assuming the variable is in `rbx`, change that to the register you need).
   * The variable is somewhere in the global memory space. In this case you need two lines. The first is required and will always be the same, and it will be `"nop %%rbx\n\t"`. The next line will be `"mov %0, [%%rip-0x2dc997]"`. The hex value (`0x2dc997`) does not matter as the loader will change it later, but if you change it you must choose a value that the assembler will still produce the correct instruction.
4. Finish the `asm` block with the following two lines: `: "r+"(variable_name)` and `);` where `variable_name` is the name of your variable. <br>
The completed `asm` block will look like
```
	int* variable_name = 0;
	asm volatile(".intel_syntax noprefix\n\t"
	"nop %%rbx\n\t"
	"mov %0, [%%rip-0x2dc997]\n\t"
	: "+r"(variable_name)
	);
```
for a variable in the gloabal memory space. Variables accessed this way need the virtual address location for the variable to be added to `variables.txt`. For a variable in a register the code block will look like:
```
	int* variable_name = 0;
	asm volatile(".intel_syntax noprefix\n\t"
	"mov %0, %%rbx\n\t"
	: "+r"(variable_name)
	);
```
assuming the variable is in `rbx`. <br>
After you make the `asm` block, you can interact with the variable as you normally would in a C program. <br>
***
If you want to make a global variable for your code, then you need to format your variable as `char new_attack[] __attribute__((section(".text"))) = "Attack(####)";`. Change the type and value to what you need, but the `__attribute__` section of the variable needs to be the same as the example provided or it will not compile correctly. <br>
⚠️ These variables will have their value **persist** between function calls. If you do not want this behavior then declare the variable in `mod_main()` or its function rather than outside of all of the code. <br>
⚠️ If you want your global to replace an existing global, then make a new variable **DIRECTLY** above your variable defined as `char evil_trick[] __attribute__((section(".text"))) = {'D', 'K', 'C', 'E', 0x48, 0x0F, 0x1F, 0xC1};` (the variable name does not matter. the first four bytes do not matter either but the last four bytes need to be `0x480F1FC1` for this to work correctly). This will require you to add both the pysical and virtual address to the `functions.txt` file (put the physical address first).
