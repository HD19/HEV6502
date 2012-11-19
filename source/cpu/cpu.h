/**************************
 * HEV6502 CPU Emulator
 * CPU.H 
 * Defines CPU class for HEV6502
 **************************/
#ifndef CPU_H
#define CPU_H

#define byte char

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
    virtual ~MemoryController();
    virtual short loadWord(short address);
    virtual byte  loadByte(short address);
    virtual bool  writeWord(short toStore, short address);
    virtual bool  writeByte(byte toStore, short address);
    virtual short getStartAddr();
};


class CPU
{
  private:
    CPU(MemoryController *memory);
    ~CPU();
    bool loadJumpTable();
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
       MemoryController* cpuMem; //CPU's memory, note it's abstract

       /* Status flag updates */
       void updateFlagReg();
       void updateStatusFlags();

       /* push and pull */
       void push(byte toPush);
       byte pull();

       /* Addressing Modes */
       short relative(byte* in);
       short zeroPageX(byte* in);
       short zeroPageY(byte* in);
       short absolute(byte* in);
       short absoluteX(byte* in);
       short absoluteY(byte* in);
       short indirect(byte* in);
       short indexedIndirect(byte* in);
       short indirectIndexed(byte* in);

       /* ADC */
       byte addCOp(byte toAdD);
       int adci(byte* in);
       int adcz(byte* in);
       int adczx(byte* in);
       int adca(byte* in);
       int adcax(byte* in);
       int adcay(byte* in);
       int adcix(byte* in);
       int adciy(byte* in);
       
       /* AND */
       byte andOp(byte toAnd);
       int andi(byte* in);
       int andz(byte* in);
       int andzx(byte* in);
       int anda(byte* in);
       int andax(byte* in);
       int anday(byte* in);
       int andix(byte* in);
       int andiy(byte* in);
       
       /* ASL */
       byte aslOp(byte toShift);
       int aslac(byte* in);
       int aslz(byte* in);
       int aslzx(byte* in);
       int asla(byte* in);
       int aslax(byte* in);

       /* Bxx */
       int bcc(byte *in);
       int bcs(byte *in);
       int beq(byte *in);
       int bitz(byte *in);
       int bita(byte *in);
       int bmi(byte* in);
       int bne(byte* in);
       int bpl(byte* in);
       int brk(byte* in);
       int bvc(byte* in);
       int bvs(byte* in);

       /* CLx */
       int clc(byte* in);
       int cld(byte* in);
       int cli(byte* in);
       int clv(byte* in);

       /* CMP */
       byte cmpOp(byte toComp);
       int cmpi(byte* in);
       int cmpz(byte* in);
       int cmpzx(byte* in);
       int cmpa(byte* in);
       int cmpax(byte* in);
       int cmpay(byte* in);
       int cmpix(byte* in);
       int cmpiy(byte* in);

       /* CPX */
       byte cpxOp(byte toComp);
       int cpxi(byte* in);
       int cpxz(byte* in);
       int cpxa(byte* in);

       /* CPY */
       byte cpyOp(byte toComp);
       int cpyi(byte* in);
       int cpyz(byte* in);
       int cpya(byte* in);

       /* DEC */
       byte decOp(byte toDec);
       int decz(byte* in);
       int deczx(byte* in);
       int deca(byte* in);
       int decax(byte* in);

       int dex(byte* in);
       int dey(byte* in);

       /* EOR */
       byte eorOp(byte toOr);
       int eori(byte* in);
       int eorz(byte* in);
       int eorzx(byte* in);
       int eora(byte* in);
       int eorax(byte* in);
       int eoray(byte* in);
       int eorix(byte* in);
       int eoriy(byte* in);

       /* INC */
       byte incOp(byte toinc);
       int incz(byte* in);
       int inczx(byte* in);
       int inca(byte* in);
       int incax(byte* in);

       int inx(byte* in);
       int iny(byte* in);

       /* JMP */
       int jmpa(byte* in);
       int jmpi(byte* in);

       int jsr(byte* in);

       /* LDA */
       byte ldaOp(byte toOr);
       int ldai(byte* in);
       int ldaz(byte* in);
       int ldazx(byte* in);
       int ldaa(byte* in);
       int ldaax(byte* in);
       int ldaay(byte* in);
       int ldaix(byte* in);
       int ldaiy(byte* in);

       /* LDX */
       byte ldxOp(byte toOr);
       int ldxi(byte* in);
       int ldxz(byte* in);
       int ldxzy(byte* in);
       int ldxa(byte* in);
       int ldxax(byte* in);
       int ldxay(byte* in);

       /* LDY */
       byte ldyOp(byte toOr);
       int ldyi(byte* in);
       int ldyz(byte* in);
       int ldyzx(byte* in);
       int ldya(byte* in);
       int ldyax(byte* in);

       /* LSR */
       byte lsrOp(byte toOr);
       int lsrac(byte* in);
       int lsrz(byte* in);
       int lsrzx(byte* in);
       int lsra(byte* in);
       int lsrax(byte* in);

       /* NOP */
       int nop(byte* in);

       /* ORA */
       byte oraOp(byte toOra);
       int orai(byte* in);
       int oraz(byte* in);
       int orazx(byte* in);
       int oraa(byte* in);
       int oraax(byte* in);
       int oraay(byte* in);
       int oraix(byte* in);
       int oraiy(byte* in);

       /* PHA */
       int pha(byte* in);

       /* PHP */
       int php(byte* in);

       /* PLA */
       int pla(byte* in);

       /* PLP */
       int plp(byte *in);

       /* ROL */
       byte rolOp(byte toRol);
       int rolac(byte* in);
       int rolz(byte* in);
       int rolzx(byte* in);
       int rola(byte* in);
       int rolax(byte* in);

       /* ROR */
       byte rorOp(byte toRor);
       int rorac(byte* in);
       int rorz(byte* in);
       int rorzx(byte* in);
       int rora(byte* in);
       int rorax(byte* in);

       /* RTI */
       int rti(byte* in);

       /* RTS */
       int rts(byte* in);

       /* SBC */
       byte sbcOp(byte toAdD);
       int sbci(byte* in);
       int sbcz(byte* in);
       int sbczx(byte* in);
       int sbca(byte* in);
       int sbcax(byte* in);
       int sbcay(byte* in);
       int sbcix(byte* in);
       int sbciy(byte* in);

       /* SEC */
       int sec(byte* in);

       /* SED */
       int sed(byte* in);

       /* SEI */
       int sei(byte* in);

       /* STA */
       int staz(byte* in);
       int stazx(byte* in);
       int staa(byte* in);
       int staax(byte* in);
       int staay(byte* in);
       int staix(byte* in);
       int staiy(byte* in);

       /* STX */
       int stxz(byte* in);
       int stxzy(byte* in);
       int stxa(byte* in);

       /* STY */
       int styz(byte* in);
       int styzx(byte* in);
       int stya(byte* in);

       int tax(byte* in);
       int tay(byte* in);
       int tsx(byte* in);
       int txa(byte* in);
       int txs(byte* in);
       int tya(byte* in);

       //Opcode Table
       //Format will be int opFunc(byte* in))
       //Return type is the number of cycles, in is input, out is output that might be needed. 
       typedef int (CPU::*FuncPtr)(byte*);
       FuncPtr opTable[256];

       int execute();

 };
#endif
