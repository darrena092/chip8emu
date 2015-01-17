# chip8emu
A chip 8 emulator (with slightly broken schip support) written in C++ by Darren Anderson (wornwinter).

##What Works
This project isn't complete yet, and so it lacks many features at the moment. I plan to implement these in the future.
Here's a brief list of what works:
* Chip8 CPU Emulation.
* Chip8 Graphics.
* Schip CPU Extensions.
  
##What Doesn't Work
Several things are currently broken or not implemented yet:
* User input (Not implemented, but will be easily sorted).
* Schip graphics (This is due to a problem with how I'm drawing two bytes in extended mode instead of one).
* Sound... if you can call it sound. I don't like beeping sounds. This isn't implemented.
  
##Building Source
You must have SDL2 installed.
Simply load the project up in Code::Blocks and build!

##How to Use
This emulator is command line only. Use as follows:
```$ chip8emu [Path to ROM]```
