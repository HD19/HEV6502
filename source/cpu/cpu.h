/**************************
 * HEV6502 CPU Emulator
 * CPU.H 
 * Defines CPU class for HEV6502
 **************************/
#ifndef CPU_H
#define CPU_H

#define byte unsigned char

#define FLAG_CARRY 1 << 0 //Cary flag, used if a borrowed is required in subtraction, also used in shift and rotates
#define FLAG_ZERO  1 << 1 //Zero flag, if op result is 0
#define FLAG_INT   1 << 2 //Interrupt flag, they're disabled if this is set
#define FLAG_DEC   1 << 3 //Shouldn't be used
#define FLAG_BRK   1 << 4 //Break flag, executing a software interrupt
#define FLAG_OVER  1 << 6 //Overflow flag, used when arithmetic op produces result too large to be represented in a byte. (> 255)
#define FLAG_SIGN  1 << 7 //Set if result of an operation is negative, clear if positive.

class MemoryController
{
public:
    //virtual ~MemoryController();
    virtual unsigned short loadWord(unsigned short address) = 0;
    virtual byte  loadByte(unsigned short address) = 0;
    virtual void  writeWord(unsigned short toStore, unsigned short address) = 0;
    virtual void  writeByte(byte toStore, unsigned short address) = 0;
    virtual unsigned short getStartAddr() = 0;
    virtual void  loadProgram(unsigned short startAddress, byte* toLoad, int size) = 0;
};


class CPU
{
  public:
    CPU(MemoryController *memory);
    ~CPU();
    void loadJumpTable();
       byte X; // X register
       byte Y; // Y register
       byte A; // Accumulator
       byte SP; // Stack pointer should range between 0x100 and 0x1FF, but only holds 8 bits.
       byte ST; // Status Register
       byte carryFlag;
       byte zeroFlag;
       byte intFlag;
       byte decFlag;
       byte brkFlag;
       byte overFlag;
       byte signFlag;
       int currentClocks;
       unsigned short PC; // Program Counter
       unsigned short codeEnd;
       unsigned short codeBegin;
       MemoryController* cpuMem; //CPU's memory, note it's abstract

       /* Status flag updates */
       void updateFlagReg();
       void updateStatusFlags();

       /* push and pull */
       void push(byte toPush);
       byte pull();

       /* Addressing Modes */
       unsigned short relative();
       unsigned short zeroPageX();
       unsigned short zeroPageY();
       unsigned short absolute();
       unsigned short absoluteX();
       unsigned short absoluteY();
       unsigned short indirect();
       unsigned short indexedIndirect();
       unsigned short indirectIndexed();

       /* ADC */
       byte addCOp(byte toAdD);
       int adci();
       int adcz();
       int adczx();
       int adca();
       int adcax();
       int adcay();
       int adcix();
       int adciy();
       
       /* AND */
       byte andOp(byte toAnd);
       int andi();
       int andz();
       int andzx();
       int anda();
       int andax();
       int anday();
       int andix();
       int andiy();
       
       /* ASL */
       byte aslOp(byte toShift);
       int aslac();
       int aslz();
       int aslzx();
       int asla();
       int aslax();

       /* Bxx */
       int bcc();
       int bcs();
       int beq();
       int bitz();
       int bita();
       int bmi();
       int bne();
       int bpl();
       int brk();
       int bvc();
       int bvs();

       /* CLx */
       int clc();
       int cld();
       int cli();
       int clv();

       /* CMP */
       byte cmpOp(byte toComp);
       int cmpi();
       int cmpz();
       int cmpzx();
       int cmpa();
       int cmpax();
       int cmpay();
       int cmpix();
       int cmpiy();

       /* CPX */
       byte cpxOp(byte toComp);
       int cpxi();
       int cpxz();
       int cpxa();

       /* CPY */
       byte cpyOp(byte toComp);
       int cpyi();
       int cpyz();
       int cpya();

       /* DEC */
       byte decOp(byte toDec);
       int decz();
       int deczx();
       int deca();
       int decax();

       int dex();
       int dey();

       /* EOR */
       byte eorOp(byte toOr);
       int eori();
       int eorz();
       int eorzx();
       int eora();
       int eorax();
       int eoray();
       int eorix();
       int eoriy();

       /* INC */
       byte incOp(byte toinc);
       int incz();
       int inczx();
       int inca();
       int incax();

       int inx();
       int iny();

       /* JMP */
       int jmpa();
       int jmpi();

       int jsr();

       /* LDA */
       byte ldaOp(byte toOr);
       int ldai();
       int ldaz();
       int ldazx();
       int ldaa();
       int ldaax();
       int ldaay();
       int ldaix();
       int ldaiy();

       /* LDX */
       byte ldxOp(byte toOr);
       int ldxi();
       int ldxz();
       int ldxzy();
       int ldxa();
       int ldxax();
       int ldxay();

       /* LDY */
       byte ldyOp(byte toOr);
       int ldyi();
       int ldyz();
       int ldyzx();
       int ldya();
       int ldyax();

       /* LSR */
       byte lsrOp(byte toOr);
       int lsrac();
       int lsrz();
       int lsrzx();
       int lsra();
       int lsrax();

       /* NOP */
       int nop();

       /* ORA */
       byte oraOp(byte toOra);
       int orai();
       int oraz();
       int orazx();
       int oraa();
       int oraax();
       int oraay();
       int oraix();
       int oraiy();

       /* PHA */
       int pha();

       /* PHP */
       int php();

       /* PLA */
       int pla();

       /* PLP */
       int plp();

       /* ROL */
       byte rolOp(byte toRol);
       int rolac();
       int rolz();
       int rolzx();
       int rola();
       int rolax();

       /* ROR */
       byte rorOp(byte toRor);
       int rorac();
       int rorz();
       int rorzx();
       int rora();
       int rorax();

       /* RTI */
       int rti();

       /* RTS */
       int rts();

       /* SBC */
       byte sbcOp(byte toAdD);
       int sbci();
       int sbcz();
       int sbczx();
       int sbca();
       int sbcax();
       int sbcay();
       int sbcix();
       int sbciy();

       /* SEC */
       int sec();

       /* SED */
       int sed();

       /* SEI */
       int sei();

       /* STA */
       int staz();
       int stazx();
       int staa();
       int staax();
       int staay();
       int staix();
       int staiy();

       /* STX */
       int stxz();
       int stxzy();
       int stxa();

       /* STY */
       int styz();
       int styzx();
       int stya();

       int tax();
       int tay();
       int tsx();
       int txa();
       int txs();
       int tya();

       //Opcode Table
       //Format will be int opFunc())
       //Return type is the number of cycles, in is input, out is output that might be needed. 
       typedef int (CPU::*FuncPtr)();
       FuncPtr opTable[256];

       int execute();
       int step();
       void clearFlags();
       void clearRegs();

 };
#endif
