[![progress-banner](https://backend.codecrafters.io/progress/shell/0fce3a66-dda0-4e30-8715-5636fcf6bb14)](https://app.codecrafters.io/users/codecrafters-bot?r=2qF)

## Overview

This is a POSIX-compliant shell very similar to Bash that I wrote in C. 
It's capable of interpreting shell commands, running external programs and builtin commands (`type`, `cd`, `pwd`, `echo`, `history`, and `exit`). 

In addition, the shell is capable of autocomplete for builtin commands as well as custom executables found in the user's PATH.                
Stdout and stderr redirection (`>` or `1>`, `2>`, `>>` or `1>>`, `2>>`) as well as pipelines (`|`) are supported.

## Usage
To compile, open a command line, `cd` into the `src` folder, and  run ```gcc -o main main.c```. 
If you see an error related to undefined symbols for arm64 architecture, such as in the screenshot below, then instead run ```gcc -o main main.c -lreadline```.

<img width="559" alt="Screenshot 2025-05-24 at 11 36 26 AM" src="https://github.com/user-attachments/assets/f38ba3b5-e814-46ca-899c-1d181c2aebe1" />

If you're curious, the `readline` library enables autocomplete by allowing the program to read inputs as they are typed, rather than waiting for a newline character.

### macOS/Linux
Once main.c has been compiled, you'll see an executable called `main` in the same directory as `main.c`. Simply run it with ```./main```.

### Windows Powershell
Once `main.c` has been compiled (which may require something like `WSL` or `MinGW` if you are using `gcc`), you'll have an executable file called `main.exe` in the same directory as `main.c`. 
Simply run it with ```.\main.exe```.

### Now what?
You'll know everything worked when you see a prompt (`$`) show up.

From there, treat it like a normal shell!
Try out some builtin commands such as `echo`, `type`, `cd`, `pwd`, and `history`. 
You can also use external commands such as `cat`, `ls`, `git` and more, as long as they are in your `PATH`.

If you want to close the shell at any point, type `exit` at the prompt and press `Enter`.
