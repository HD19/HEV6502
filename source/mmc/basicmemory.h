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
    unsigned short loadWord(unsigned short address);
    byte  loadByte(unsigned short address);
    void  writeWord(unsigned short address, unsigned short toWrite);
    void  writeByte(byte toWrite, unsigned short address);
    void  loadProgram(unsigned short address, byte* toLoad, int size);
    unsigned short getStartAddr();
    unsigned short lastAddr;
    //void (*memChange)(void);
private:
    vector<byte> memoryMap;
    unsigned short programStart;
};

#endif // BASICMEMORY_H
