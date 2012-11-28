#include "basicmemory.h"

BasicMemory::BasicMemory()
{
    memoryMap.reserve(0x10000); //We're gonna have at most 64 k
    for(int i = 0x200; i < 0x600; i++)
    {
        memoryMap[i] = 0;
    }
}

void BasicMemory::loadProgram(short startAddress, byte* toLoad, int size)
{
    for(int i = 0; i < size; i++)
    {
        memoryMap[startAddress + i] = toLoad[i];
    }
    programStart = startAddress;
}

short BasicMemory::loadWord(short address)
{
    return (short)((memoryMap[address] << 8) + (memoryMap[++address]));
}

byte  BasicMemory::loadByte(short address)
{
    return memoryMap[address];
}

void  BasicMemory::writeWord(short address, short toWrite)
{
    //Not really implemented, but if so, think about byte order
    memoryMap[address] = (byte)(toWrite >> 8) & 0xFF;
    memoryMap[++address] = (byte)toWrite & 0xFF;
}

void  BasicMemory::writeByte(unsigned char toWrite, short address)
{
    memoryMap[address] = toWrite;
}

short BasicMemory::getStartAddr()
{
    return this->programStart;
}
