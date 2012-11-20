#include "assembler.h"

Assembler::Assembler()
{
    this->currentPC = 0;
    this->offset = 0;
    this->loadTable();
    byteCounts[IMP] = 1;
    byteCounts[IMM] = 2;
    byteCounts[ZP]  = 2;
    byteCounts[ZPX] = 2;
    byteCounts[ZPY] = 2;
    byteCounts[ABS] = 3;
    byteCounts[ABX] = 3;
    byteCounts[ABY] = 3;
    byteCounts[IND] = 3;
    byteCounts[IDX] = 3;
    byteCounts[IDY] = 3;
    byteCounts[REL] = 2;
}

void Assembler::setEntry(byte opCode, byte addrMode, string inst)
{
    map<string, byte*>::iterator it;

    it = opTable.find(inst);
    if(it != opTable.end())
    {
        (it->second)[addrMode] = opCode;
    }
    else
    {
        byte* tmp = (byte*)malloc(12 * sizeof(byte));
        tmp[addrMode] = opCode;
        (opTable[inst]) = tmp;
    }
    return;
}

void Assembler::loadTable()
{
    //Load table of instructions.

    /* ADC */
    setEntry(0x69, IMM, "ADC");
    setEntry(0x65, ZP,  "ADC");
    setEntry(0x75, ZPX, "ADC");
    setEntry(0x6D, ABS, "ADC");
    setEntry(0x7D, ABX, "ADC");
    setEntry(0x79, ABY, "ADC");
    setEntry(0x61, IDX, "ADC");
    setEntry(0x71, IDY, "ADC");

    /* AND */
    setEntry(0x29, IMM, "AND");
    setEntry(0x25, ZP,  "AND");
    setEntry(0x35, ZPX, "AND");
    setEntry(0x2D, ABS, "AND");
    setEntry(0x3D, ABX, "AND");
    setEntry(0x39, ABY, "AND");
    setEntry(0x21, IDX, "AND");
    setEntry(0x31, IDY, "AND");

    /* ASL */
    setEntry(0x0A, IMP, "ASL");
    setEntry(0x06, ZP,  "ASL");
    setEntry(0x16, ZPX, "ASL");
    setEntry(0x0E, ABS, "ASL");
    setEntry(0x1E, ABX, "ASL");

    /* Bxx */
    setEntry(0x90, REL, "BCC");
    setEntry(0xB0, REL, "BCS");
    setEntry(0xF0, REL, "BEQ");
    setEntry(0x30, REL, "BMI");
    setEntry(0xD0, REL, "BNE");
    setEntry(0x10, REL, "BPL");
    setEntry(0x50, REL, "BVC");
    setEntry(0x70, REL, "BVS");

    setEntry(0x24, ZP, "BIT");
    setEntry(0x2C, ABS,"BIT");

    setEntry(0x00, IMP, "BRK");

    /* Cxx */
    setEntry(0x18, IMP, "CLC");
    setEntry(0xD8, IMP, "CLD");
    setEntry(0x58, IMP, "CLI");
    setEntry(0xB8, IMP, "CLV");

    /* CMP */
    setEntry(0xC9, IMM, "CMP");
    setEntry(0xC5, ZP,  "CMP");
    setEntry(0xD5, ZPX, "CMP");
    setEntry(0xCD, ABS, "CMP");
    setEntry(0xDD, ABX, "CMP");
    setEntry(0xD9, ABY, "CMP");
    setEntry(0xC1, IDX, "CMP");
    setEntry(0xD1, IDY, "CMP");

    /* CPX */
    setEntry(0xE0, IMM, "CPX");
    setEntry(0xE4, ZP,  "CPX");
    setEntry(0xEC, ABS, "CPX");

    /* CPY */
    setEntry(0xC0, IMM, "CPY");
    setEntry(0xC4, ZP,  "CPY");
    setEntry(0xCC, ABS, "CPY");

    /* DEC */
    setEntry(0xC6, ZP,  "DEC");
    setEntry(0xD6, ZPX, "DEC");
    setEntry(0xCE, ABS, "DEC");
    setEntry(0xDE, ABX, "DEC");

    setEntry(0xCA, IMP, "DEX");
    setEntry(0x88, IMP, "DEY");

    /* EOR */
    setEntry(0x49, IMM, "EOR");
    setEntry(0x45, ZP,  "EOR");
    setEntry(0x55, ZPX, "EOR");
    setEntry(0x4D, ABS, "EOR");
    setEntry(0x5D, ABX, "EOR");
    setEntry(0x59, ABY, "EOR");
    setEntry(0x41, IDX, "EOR");
    setEntry(0x51, IDY, "EOR");

    /* INC */
    setEntry(0xE6, ZP,  "INC");
    setEntry(0xF6, ZPX, "INC");
    setEntry(0xEE, ABS, "INC");
    setEntry(0xFE, ABX, "INC");

    setEntry(0xE8, IMP, "INX");
    setEntry(0xC8, IMP, "INY");


}
