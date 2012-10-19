/**************************
 * HEV6502 CPU Emulator
 * CPU.H 
 * Defines CPU class for HEV6502
 **************************/
#ifndef CPU_H
#define CPU_H
#include "mem.h"
#include "../common/common.h"

#define FLAG_CARRY 1 << 0 //Cary flag, used if a borrowed is required in subtraction, also used in shift and rotates
#define FLAG_ZERO  1 << 1 //Zero flag, if op result is 0
#define FLAG_INT   1 << 2 //Interrupt flag, they're disabled if this is set
#define FLAG_DEC   1 << 3 //Shouldn't be used
#define FLAG_BRK   1 << 4 //Break flag, executing a software interrupt
#define FLAG_OVER  1 << 6 //Overflow flag, used when arithmetic op produces result too large to be represented in a byte. (> 255)
#define FLAG_SIGN  1 << 7 //Set if result of an operation is negative, clear if positive.
 
 class CPU
 {
   private:
   //We need registers.
       byte X; // X register
       byte Y; // Y register
       byte A; // Accumulator
       byte SP; // Stack pointer
       byte ST; // Status Register
       short PC; // Program Counter
       Memory cpuMem; //CPU's memory, only a block for now.
       byte   program[0x10000]; //Program buffer. To be loaded by wrapper.
       
       /* Addressing Modes */
       byte immediate(byte* in);
       byte accumulator(byte* in);
       byte zeroPage(byte* in);
       byte zeroPageX(byte* in);
       byte zeroPageY(byte* in);
       byte absolute(byte* in);
       byte absoluteX(byte* in);
       byte absoluteY(byte* in);
       byte indirect(byte* in);
       byte indexedIndirect(byte* in);
       byte indirectIndexed(byte* in);
              
       /* ADC */
       int adci(byte* in);
       int adcz(byte* in);
       int adczx(byte* in);
       int adca(byte* in);
       int adcax(byte* in);
       int adcay(byte* in);
       int adcix(byte* in);
       int adciy(byte* in);
       
       /* AND */
       int andop(byte toAnd);
       int andi(byte* in);
       int andz(byte* in);
       int andzx(byte* in);
       int anda(byte* in);
       int andax(byte* in);
       int anday(byte* in);
       int andix(byte* in);
       int andiy(byte* in);
       //Opcode Table
       //Format will be int opFunc(byte* in))
       //Return type is the number of cycles, in is input, out is output that might be needed. 
       static const opTable[0x100] = {
//     0x0   0x1    0x2  0x3   0x4   0x5   0x6   0x7   0x8   0x9   0xA   0xB   0xC   0xD   0xE   0xF
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0x00
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0x10
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0x20
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0x30
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0x40
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0x50
       NULL, &CPU::adcix, NULL, NULL, NULL, &CPU::adcz, NULL, NULL, NULL, &CPU::adci, NULL, &CPU::adca, NULL, NULL, NULL, NULL, //0x60
       NULL, &CPU::adciy, NULL, NULL, NULL, &CPU::adczx, NULL, NULL, NULL, NULL, NULL, NULL, &CPU::adcax, NULL, NULL, NULL, //0x70
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0x80
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0x90
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0xA0
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0xB0
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0xC0
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0xD0
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0xE0
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0xF0
       }

/*Not sure this is neccessary.       
       static const timeTable[0x100] = {
//     0x0   0x1    0x2  0x3   0x4   0x5   0x6   0x7   0x8   0x9   0xA   0xB   0xC   0xD   0xE   0xF       
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0x00
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0x10
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0x20
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0x30
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0x40
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0x50
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0x60
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0x70
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0x80
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0x90
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0xA0
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0xB0
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0xC0
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0xD0
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0xE0
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //0xF0
*/

#endif
