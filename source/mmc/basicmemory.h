#ifndef BASICMEMORY_H
#define BASICMEMORY_H
#include <vector>

#include "../cpu/cpu.h"
#include "../common/common.h"
#define byte unsigned char

using namespace std;

class BasicMemory : public MemoryController
{
public:
    BasicMemory();
    short loadWord(short address);
    byte  loadByte(short address);
    void  writeWord(short address, short toWrite);
    void  writeByte(byte toWrite, short address);
    void  loadProgram(short address, byte* toLoad, int size);
    short getStartAddr();
private:
    vector<byte> memoryMap;
    short programStart;
};

#endif // BASICMEMORY_H
