/**************************
 * HEV6502 CPU Emulator
 * CPU.H 
 * Defines CPU class for HEV6502
 **************************/
#include "mem.h"
#include "../common/common.h"
 
 class CPU
 {
   private:
   //We need registers.
       byte X; // X register
       byte Y; // Y register
       byte A; // Accumulator
       byte SP; // Stack pointer
       byte ST; //Status register
       short PC; // Program Counter
       Memory cpuMem; //CPU's memory
       byte   program[0x10000]; //Program buffer.
