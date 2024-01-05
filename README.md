# DKCedit
Modding tool for the Steam release of Dokapon Kingdom: Connect
<br>
DKCedit comprises two pieces of software used to make code related mods for Dokapon Kingdom. These are located in Generator and Loader. If you only want to install mods, then you only have to use the loader application.

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
My method of finding them is to find the virtual one first (with Cheat Engine or Ghidra) and search for the string of bytes in the `DkkStm.exe` file. If you are changing a call instruction, give the address to the location of the next instruction (the one immediately after the `call` instruction).
