HEV6502
=======

6502 as a class emulator. The idea here is to implement a 6502's instruction and functionality generically in a C++ class. Then implementation can be done however you like. There is an abstract class within cpu.h that exposes an interface for memory usage. This way, external devices can be emulated however is necessary, the CPU class only needs that interface done. There is a demonstration application called 'Visual 6502' which can be looked at as an example application for use of the HEV6502. There's also an assembler written for the 6502 assembly language included. An example of it's use is also included in the Visual 6502.

Building
========

The HEV6502 is only implemented as a class, so building it is up to the context of whatever project you want to use it for. To build the Visual 6502, you need to have Qt4. I'm no Qt expert, so I've only built this using Qt Creator, which you can download for a Linux or Windows platform. Using qt creator, you can open the .pro project file and build the project this way.

Usage
=====

---------
HEV6502 |
---------

To use the emulated CPU, implement a memory interface for it. An example is included as the BasicMemory class. The interface for memory classes are defined in the CPU.h header. Here you can implement memory however you want. Note though that the CPU is designed to block during a memory operation, so it will wait until memory becomes available.

The HEV6502 will initialize itself once it is constructed. Note though, that it needs a pointer to a memory interface object before it will be constructed. The CPU will ask the memory where the program starts, and use that address to begin execution when execution is requested. 

The CPU has an execute() method that will run through the code, but it also has a step() method that will execute one instruction per call. The Visual 6502 uses step() in a loop for its execution in order to update the interface as necessary.

Each of the opcodes in the HEV6502 is implemented as a function and these functions are called though a function pointer table the CPU has. Each opcode will index into the function pointer table, calling the desired method and doing whatever work needs doing. Each of these opcode functions takes no input parameters, but returns the number of cycles used to complete the instruction.

-------------
Visual 6502 |
-------------
To use the Visual 6502, build it and run it, no arguments are required.

The top left text box is the assembly input box. To assemble code, click the 'Assemble' button located in the lower left hand corner. There are plenty references and examples available for 6502 assembly.

The top right hand box is the 'monitor'. This is basically a view of the memory located between 0x200 and 0x5ff. The monitor will display a color for any value in these locations. There are 16 different colors in no particular order. Placing any values in these locations will change the displayed color, these values are masked with 0xF, so values don't go out of bounds. 

Clicking the 'Assemble' button will assemble the code and load it into memory. The visual 6502 will also spit out the hex of the binary it just assembled in the bottom left box. This is used to see if the assembler is working correctly, and doing some debugging. 

After the program is loaded, you can step through execution using the 'step' button which will execute one instruction at a time. The values of memory and registers will update accordingly after each step. Alternatively, you can click the 'Execute' button which will run the CPU until it halts or encounters a break.

Issues
======

Assembler - 

The assembler used in Visual6502 is not perfect, and can easily be fooled. Comments and labels are supported, but you can trick the assembler by not having spaces after labels, or using tabs, or putting spaces after commas in some instructions, i.e:

lda $200, x

Note the comma here will confuse the assembler and generate an error. The assembler was quickly put together statically, and doesn't do a whole lot of tokenizing dynamically. Building a sturdy assembelr is beyond the scope of the project.

CPU -

This has been tested to the extent possible, but there are probably errors in it. If you find any, which are typically related to flags not being updated correctly for instructions, let me know and I will make an effort to fix it. 

Visual 6502 -

The application is neat, but it's slow. The reason for the lack of speed is that the GUI is updated after each instruction. You can see this in the execute loop implemented in the GUI. All of these excessive calls are intensive, but is a quick way to keep things responsive and updated while the CPU is still executing. The CPU should probably be created and ran in its own thread in order to keep the GUI unblocked and updating normally, but this just isnt implemented yet.
