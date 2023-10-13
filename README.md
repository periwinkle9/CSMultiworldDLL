# Cave Story Multiworld Connector Thing
This is a DLL mod that does 3 things:
1. Implements a parallel TSC event loop, allowing a second event to be ran
simultaneously alongside the game's "main" TSC processor;
2. Runs a TCP server allowing other programs to send events to that secondary
TSC parser (among other things);
3. Spawns a debug console allowing one to interact with both of those things.
For debug builds this is always enabled, but on release builds you can enable it
by creating a file called `debug` (with no extension) in the same directory.

## Debug console usage
If the debug console is enabled, press Alt+Enter from the game window to bring up
a prompt. You can type any TSC in this prompt and it will be executed in the
parallel processor. You can also enter line breaks by ending a line with a `\`
character.

In addition to executing arbitrary TSC, the debug console also supports the following
commands:
* `/get_flag n` retrieves the value of flag `n` (note, `n` must be an integer, so
OOB TSC values must be converted into their integer value)
* `/log_level [n]` sets the current log level to `n` (if provided), or retrieves the
current log level if called with no argument. Log message with a level greater
than the current log level will no longer be displayed. (Valid log levels are 0-4,
inclusive.)
* `/log_timestamps <0|1>` disables or enables timestamp logging. (Timestamps are shown
in UTC time.)
* `/kill_server` force-kills the TCP server. Currently, it cannot be restarted aside
from restarting the game.

## Build Instructions
You will need a C++ compiler that supports the C++20 language standard to build
this project.

### Dependencies
This project requires the [Asio](https://think-async.com/Asio/) library (not included
with this repo). You may need to edit the project settings/Makefile to add the
header files for this library to your compiler's `#include` search directories.

### Microsoft Visual Studio (recommended)
Project files for Visual Studio 2019 are included in the repo.

### MSYS2 (MinGW 32-bit)
You must use the MinGW 32-bit environment of MSYS2 to build this project.
(The 64-bit environment gives linker errors when building, for some reason.)
You will also need to install GCC and make if they are not already installed.
You can install both of them at once by typing `pacman -S mingw-w64-i686-gcc make`
in your terminal.

Once those are installed, simply type `make` from the top-level directory and it
should produce a `dinput.dll` output file in the `bin` folder.
You can also type `make debug` to compile with debug symbols enabled, or
`make clean` to delete all files generated by the compiler.
