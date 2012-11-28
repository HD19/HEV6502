#include "basicmemory.h"

BasicMemory::BasicMemory()
{
    memoryMap.reserve(0x10000); //We're gonna have at most 64 k
    for(int i = 0x200; i < 0x600; i++)
    {
        memoryMap[i] = 0;
    }
}

void BasicMemory::loadProgram(unsigned short startAddress, byte* toLoad, int size)
{
    for(int i = 0; i < size; i++)
    {
        memoryMap[startAddress + i] = toLoad[i];
    }
    programStart = startAddress;
}

unsigned short BasicMemory::loadWord(unsigned short address)
{
    return (unsigned short)((memoryMap[address] << 8) + (memoryMap[++address]));
}

byte  BasicMemory::loadByte(unsigned short address)
{
    return memoryMap[address];
}

void  BasicMemory::writeWord(unsigned short address, unsigned short toWrite)
{
    //Not really implemented, but if so, think about byte order
    memoryMap[address] = (byte)(toWrite >> 8) & 0xFF;
    memoryMap[++address] = (byte)toWrite & 0xFF;
    lastAddr = address;
}

void  BasicMemory::writeByte(unsigned char toWrite, unsigned short address)
{
    memoryMap[address] = toWrite;
    lastAddr = address;
}

unsigned short BasicMemory::getStartAddr()
{
    return this->programStart;
}
