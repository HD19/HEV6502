#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <map>
#include <vector>
#include <stack>
#include <algorithm>
#include <string>
#include <stdlib.h>

#define IMM 0
#define ZP  1
#define ZPX 2
#define ZPY 3
#define ABS 4
#define ABX 5
#define ABY 6
#define IND 7
#define IDX 8
#define IDY 9
#define IMP 10
#define REL 11

#define byte unsigned char
#define word unsigned short
using namespace std;
//6502 assembler

class Assembler
{
public:
    Assembler();            //offset address for labels
    ~Assembler();
    int       assemble();                    //do the magic.
    void      setText(string* toSet);        //set our input buffer
    byte*     getBinary();                   //return code block
    void      setOffset();                   //setting the offset for labels.
    void      outputToFile(string fileName); //Output the binary as hex
private:
    int       decodeLine(byte* buffer);      //Assembles a single line, buffer poitns to current input buffer.
    void      loadTable();                   //Assign cmds to instruction table.
    void      setEntry(byte opCode, byte, string);

    short     currentPC;                    //current program counter.
    short     offset;                       //offset should we need it.
    string*   inputBuffer;                  //Assembly code
    byte*     outputBlock;                  //Binary output;
    byte      byteCounts[12];               //byte count per address mode.
    map<string, byte*> opTable;             //Opcode table
    map<string, short> labelMap;            //Map maintaining labels.
    map<string, short> unresolvedLabel;     //Labels we don't know yet.
    vector<byte> currentCode;               //Vector of current code
    stack<string> errorStack;               //stack of errors we've encountered.

};

#endif // ASSEMBLER_H
