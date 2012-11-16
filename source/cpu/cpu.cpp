/**********************
 * HEV6502 CPU Emulator
 * CPU.CPP
 * CPU function definitions
 **********************/
#include "cpu.h"


#define byte char

short CPU::relative(byte* in)
{
    short addr = cpuMem.loadWord(*in);
    (addr < 0x80)? addr+= PC : addr += (PC - 256); 
    return addr;
} 
 
short CPU::zeroPageX(byte* in)
{
    short addr = (*in);
    addr = (( addr + X ) & 0xFF);
    return addr;
}

short CPU::zeroPageY(byte* in)
{
    short addr = (*in);
    addr = (( addr + Y ) & 0xFF);
    return addr;
}

short CPU::absolute(byte* in)
{
    //full address
    short addr = ((*++in) << 8); //high byte
    addr = (addr ^ (*in));       //low  byte
    //byte res = cpuMem.loadByte(addr);
    return addr;
}
short CPU::absoluteX(byte* in)
{
    //full address + X with wrapping..maybe
    byte low = (*in);
    (low + X > 255) ? low -= X : low += X;
    short addr = ((*++in) << 8); //high byte
    addr = (addr ^ low);
    //byte res = cpuMem.loadByte(addr);
    return addr;
}
short CPU::absoluteY(byte* in)
{
    //full address + Y with wrapping..maybe
    byte low = (*in);
    (low + Y > 255) ? low -= Y : low += Y;
    short addr = ((*++in) << 8); //high byte
    addr = (addr ^ low);
    //byte res = cpuMem.loadByte(addr);
    return addr;
}
short CPU::indirect(byte* in)
{
    //used for jump, load address from address.
    short addr = (((*++in) << 8) & 0xFF00);
    addr = (addr ^ (*in)); //sweet, now load this byte and the next one
    short target = (((cpuMem.loadByte(addr)) << 8) & 0xFF00);
    target = (target ^ cpuMem.loadByte(++addr));
    return target;

}
//returning the address to work with.
short CPU::indexedIndirect(byte* in)
{
    //we're loading the address at ($00(x + *in))
    short addr = (*in) + X;
    addr &= 0xFF00; //load the zero page address here and +1
    short target = cpuMem.loadByte(addr) << 8 & 0xFF00;
    target = target ^ cpuMem.loadByte(++addr);
    //now load the target
    return target; 
}
//returning the address to work with
short CPU::indirectIndexed(byte* in)
{
    //rol ($2A), Y
    //The value $03 in Y is added to the address $C235 at addresses $002A and $002B for a sum of $C238. 
    //The value $2F at $C238 is shifted right (yielding $17) and written back to $C238.
    short target = cpuMem.loadByte(*in) << 8 & 0xFF00;
    target = target ^ cpuMem.loadByte(++(*in));
    target += Y;
    return target;
}

void CPU::updateFlagReg()
{
    //compress flags into ST
    carryFlag = (carryFlag ? 1 : 0);
    if(carryFlag)
        ST &= FLAG_CARRY;
    zeroFlag = (zeroFlag ? 1 : 0);
    if(zeroFlag)
        ST &= FLAG_ZERO;
    intFlag = (intFlag ? 1 : 0);
    if(intFlag)
        ST &= FLAG_INT;
    decFlag = (decFlag ? 1 : 0);
    if(decFlag)
        ST &= FLAG_DEC;
    brkFlag = (brkFlag ? 1 : 0);
    if(brkFlag)
        ST &= FLAG_BRK;
    overFlag = (overFlag ? 1 : 0);
    if(overFlag)
        ST &= FLAG_OVER;
    signFlag = (signFlag ? 1 : 0);
    if(signFlag)
        ST &= FLAG_SIGN;

}

void CPU::updateStatusFlags()
{
    //extract ST into flag vars
    if(ST & FLAG_CARRY)
        carryFlag = 1;
    if(ST & FLAG_ZERO)
        zeroFlag = 1;
    if(ST & FLAG_INT)
        intFlag = 1;
    if(ST & FLAG_DEC)
        decFlag = 1;
    if(ST & FLAG_BRK)
        brkFlag = 1;
    if(ST & FLAG_OVER)
        overFlag = 1;
    if(ST & FLAG_SIGN)
        signFlag = 1;
}

void CPU::push(byte toPush)
{
    //stack grows down
    SP--;
    if(!SP)
    {
        //STACK OVERFLOW!
    }
    cpuMem.writeByte(toPush, SP);
}

byte CPU::pull()
{
    //stack pops up
    byte retVal = cpuMem.loadByte(SP);
    SP++;
    return retVal;
}

byte CPU::addCOp(byte toAdd)
{
    short res = A + toAdd + carryFlag;
    overFlag = (!((A ^ toAdd) & 0x80) && ((A ^ res) & 0x80)) ? 1 : 0; //signed overflow
    carryFlag = res > 0xFF ? 1 : 0;  //unsigned overflow
    signFlag  = (res >> 7) & 1;      //last bit set to 1
    zeroFlag  = !(res);              //Did we get a zero?
    return (byte)res;
}

int CPU::adci(byte* in)
{
    //add with carry immediate
    A = (addCOp(*in) & 0xFF);
    return 2;
}

int CPU::adcz(byte* in)
{
    A = (addCOp(cpuMem.loadByte(*in)) & 0xFF);
    return 3;
}

int CPU::adczx(byte* in)
{
    byte val = cpuMem.loadByte(zeroPageX(in));
    A = (addCOp(val) & 0xFF);
    return 4;
}

int CPU::adca(byte* in)
{
    byte val = cpuMem.loadByte(absolute(in));
    A = (addCOp(val) & 0xFF);
    return 4;
}

int CPU::adcax(byte* in)
{
    byte val = cpuMem.loadByte(absoluteX(in));
    A = (addCOp(val) & 0xFF);
    return 4;   //could be +1
}

int CPU::adcay(byte* in)
{
    byte val = cpuMem.loadByte(absoluteY(in));
    A = (addCOp(val) & 0xFF);
    return 4;   //could be +1
}

int CPU::adcix(byte* in)
{
    byte val = cpuMem.loadByte(indexedIndirect(in));
    A = (addCOp(val) & 0xFF);
    return 6;
}


int CPU::adciy(byte* in)
{
    byte val = cpuMem.loadByte(indirectIndexed(in));
    A = (addCOp(val) & 0xFF);
    return 5;
}

byte CPU::andOp(byte toAnd)
{
    //Used to perform common AND operations
    byte res = A & toAnd;
    signFlag = (A >> 7 & 1);
    (A > 0) ? zeroFlag = 0 : zeroFlag = 1;
    
    return res;
    
}

int CPU::andi(byte* in)
{
    A = andOp((*in));
    return 2;
}

int CPU::andz(byte* in)
{
    A = andOp(cpuMem.loadByte((*in)));
    return 3;
}

int CPU::andzx(byte* in)
{
    byte res = cpuMem.loadByte(zeroPageX(in));
    A = andOp(res);
    return 4;
}

int CPU::anda(byte* in)
{
    byte res = cpuMem.loadByte(absolute(in));
    A = andOp(res);
    return 4;
}

int CPU::andax(byte* in)
{
    byte res = cpuMem.loadByte(absoluteX(in));
    A = andOp(res);
    return 4; //could be + 1
}

int CPU::anday(byte* in)
{
    byte res = cpuMem.loadByte(absoluteY(in));
    A = andOp(res);
    return 4; //could be + 1
}

int CPU::andix(byte* in)
{
    byte res = cpuMem.loadByte(indexedIndirect(in));
    A = andOp(res);
    return 6;    
}

int CPU::andiy(byte* in)
{
    byte res = cpuMem.loadByte(indirectIndexed(in));
    A = andOp(res);
    return 5; //could be + 1
}

byte CPU::aslOp(byte toShift)
{
    //arithmetic shift left
    byte res  = toShift;
    carryFlag = (res >> 7) & 1;
    toShift   = (res << 1) & 0xFF;
    signFlag  = (res >> 7) &1;
    zeroFlag  = !(res);
    return res;
}

int CPU::aslac(byte* in) //accumulator
{
    A = aslOp(A);
    return 2;
}

int CPU::aslz(byte* in)
{
    cpuMem.writeByte(aslOp(cpuMem.loadByte(*in)), *in);
    return 5;
}

int CPU::aslzx(byte* in)
{
    cpuMem.writeByte(aslOp(cpuMem.loadByte(zeroPageX(in))), zeroPageX(in));
    return 6;
}

int CPU::asla(byte* in)
{
    cpuMem.writeByte(aslOp(cpuMem.loadByte(absolute(in))), absolute(in));
    return 6;
}

int CPU::aslax(byte* in)
{
    cpuMem.writeByte(aslOp(cpuMem.loadByte(absoluteX(in))), absoluteX(in));
    return 7;
}

int CPU::bcc(byte *in)
{
    //if carry is clear, branch
    if(!carryFlag)
        PC = relative(in);
    return 2; //could be + 1 or 2
}

int CPU::bcs(byte *in)
{
    //if carry is set, branch
    if(carryFlag)
        PC = relative(in);
    return 2;
}

int CPU::beq(byte *in)
{
    //branch if equal
    if(zeroFlag)
        PC = relative(in);
    return 2;
}


int CPU::bitz(byte *in)
{
    byte val = cpuMem.loadByte(*in);
    signFlag = (val >> 7) & 1;
    overFlag = (val >> 6) & 1;
    zeroFlag = A & val;
    return 3;
}

int CPU::bita(byte *in)
{
    byte val = cpuMem.loadByte(absolute(in));
    signFlag = (val >> 7) & 1;
    overFlag = (val >> 6) & 1;
    zeroFlag = A & val;
    return 4;
}

int CPU::bmi(byte* in)
{
    if(signFlag)
        PC = relative(in);
    return 2;
}

int CPU::bne(byte* in)
{
    if(!zeroFlag)
        PC = relative(in);
    return 2;
}

int CPU::bpl(byte* in)
{
    if(!signFlag)
        PC = relative(in);
    return 2;
}

int CPU::brk(byte* in)
{
    //forces interrupt
    PC += 2; //increment counter
    push(((PC >> 8) & 0xFF));
    push((PC & 0xFF));
    brkFlag = 1;
    updateFlagReg();
    push(ST);
    
    intFlag = 1;
    PC = cpuMem.loadWord(0xFFFE);
    PC--;
    return 7;
}

int CPU::bvc(byte* in)
{
    //branch if overflow clear
    if(!overFlag)
        PC = relative(in);
    return 2;
}

int CPU::bvs(byte* in)
{
    if(overFlag)
        PC = relative(in);
    return 2;
} 

int CPU::clc(byte* in)
{
    carryFlag = 0;
    return 2;
}

int CPU::cld(byte* in)
{
    decFlag = 0;
    return 2;
}

int CPU::cli(byte* in)
{
    intFlag = 0;
    return 2;
}

int CPU::clv(byte* in)
{
    overFlag = 0;
    return 2;
}

byte CPU::cmpOp(byte toComp)
{
    byte res = A - toComp;
    carryFlag = (res > 0? 1 : 0);
    signFlag = (res >> 7) &1;
    zeroFlag = res & 0xFF;
    return res;
}

int CPU::cmpi(byte* in)
{
    //add with carry immediate
    cmpOp(*in);
    return 2;
}

int CPU::cmpz(byte* in)
{
    cmpOp(cpuMem.loadByte(*in));
    return 3;
}

int CPU::cmpzx(byte* in)
{
    cmpOp(cpuMem.loadByte(zeroPageX(in)));
    return 4;
}

int CPU::cmpa(byte* in)
{
    cpuMem.loadByte(absolute(in));
    return 4;
}

int CPU::cmpax(byte* in)
{
    cpuMem.loadByte(absoluteX(in));
    return 4;   //could be +1
}

int CPU::cmpay(byte* in)
{
    cpuMem.loadByte(absoluteY(in));
    return 4;   //could be +1
}

int CPU::cmpix(byte* in)
{
    cpuMem.loadByte(indexedIndirect(in));
    return 6;
}


int CPU::cmpiy(byte* in)
{
    cpuMem.loadByte(indirectIndexed(in));
    return 5;
}

byte CPU::cpxOp(byte toComp)
{
    byte res = X - toComp;
    carryFlag = (res >= 0? 1: 0);
    signFlag = (res >> 7)&1;
    zeroFlag = res&0xFF;
    return res;
}

int CPU::cpxi(byte* in)
{
    cpxOp(*in);
    return 2;
}

int CPU::cpxz(byte* in)
{
    cpxOp(cpuMem.loadByte(*in));
    return 3;
}

int CPU::cpxa(byte* in)
{
    cpxOp(cpuMem.loadByte(absolute(in)));
    return 4;
}

byte CPU::cpyOp(byte toComp)
{
    byte res = Y - toComp;
    carryFlag = (res >= 0? 1: 0);
    signFlag = (res >> 7)&1;
    zeroFlag = res&0xFF;
    return res;
}

int CPU::cpyi(byte* in)
{
    cpyOp(*in);
    return 2;
}

int CPU::cpyz(byte* in)
{
    cpyOp(cpuMem.loadByte(*in));
    return 3;
}

int CPU::cpya(byte* in)
{
    cpyOp(cpuMem.loadByte(absolute(in)));
    return 4;
}

byte CPU::decOp(byte toDec)
{
    byte res = (toDec -1)&0xFF;
    signFlag = (res >> 7)&1;
    zeroFlag = res;
    return res;
}

int CPU::decz(byte* in)
{
    cpuMem.writeByte(cpuMem.loadByte(*in), *in);
    return 5;
}

int CPU::deczx(byte* in)
{
    cpuMem.writeByte(cpuMem.loadByte(zeroPageX(in)), zeroPageX(in));
    return 6;
}

int CPU::deca(byte* in)
{
    cpuMem.writeByte(cpuMem.loadByte(absolute(in)), absolute(in));
    return 6;
}

int CPU::decax(byte* in)
{
    cpuMem.writeByte(cpuMem.loadByte(absoluteX(in)), absoluteX(in));
    return 7;
}

int CPU::dex(byte* in)
{
    X -= 1;
    zeroFlag = X;
    signFlag = (X >> 7&1);
    return 2;
}

int CPU::dey(byte* in)
{
    Y -= 1;
    zeroFlag = Y;
    signFlag = (Y >> 7&1);
    return 2;
}


byte CPU::eorOp(byte toeor)
{
    //Used to perform common eor operations
    byte res = A ^ toeor;
    signFlag = (A >> 7 & 1);
    (A > 0) ? zeroFlag = 0 : zeroFlag = 1;

    return res;

}

int CPU::eori(byte* in)
{
    A = eorOp((*in));
    return 2;
}

int CPU::eorz(byte* in)
{
    A = eorOp(cpuMem.loadByte((*in)));
    return 3;
}

int CPU::eorzx(byte* in)
{
    byte res = cpuMem.loadByte(zeroPageX(in));
    A = eorOp(res);
    return 4;
}

int CPU::eora(byte* in)
{
    byte res = cpuMem.loadByte(absolute(in));
    A = eorOp(res);
    return 4;
}

int CPU::eorax(byte* in)
{
    byte res = cpuMem.loadByte(absoluteX(in));
    A = eorOp(res);
    return 4; //could be + 1
}

int CPU::eoray(byte* in)
{
    byte res = cpuMem.loadByte(absoluteY(in));
    A = eorOp(res);
    return 4; //could be + 1
}

int CPU::eorix(byte* in)
{
    byte res = cpuMem.loadByte(indexedIndirect(in));
    A = eorOp(res);
    return 6;
}

int CPU::eoriy(byte* in)
{
    byte res = cpuMem.loadByte(indirectIndexed(in));
    A = eorOp(res);
    return 5; //could be + 1
}

byte CPU::incOp(byte toinc)
{
    byte res = (toinc + 1)&0xFF;
    signFlag = (res >> 7)&1;
    zeroFlag = res;
    return res;
}

int CPU::incz(byte* in)
{
    cpuMem.writeByte(cpuMem.loadByte(*in), *in);
    return 5;
}

int CPU::inczx(byte* in)
{
    cpuMem.writeByte(cpuMem.loadByte(zeroPageX(in)), zeroPageX(in));
    return 6;
}

int CPU::inca(byte* in)
{
    cpuMem.writeByte(cpuMem.loadByte(absolute(in)), absolute(in));
    return 6;
}

int CPU::incax(byte* in)
{
    cpuMem.writeByte(cpuMem.loadByte(absoluteX(in)), absoluteX(in));
    return 7;
}

int CPU::inx(byte* in)
{
    X += 1;
    zeroFlag = X;
    signFlag = (X >> 7&1);
    return 2;
}

int CPU::iny(byte* in)
{
    Y += 1;
    zeroFlag = Y;
    signFlag = (Y >> 7&1);
    return 2;
}

int CPU::jmpa(byte* in)
{
    //set PC to absolute address
    PC = absolute(in) -1;
    return 3;
}

int CPU::jmpi(byte* in)
{
    PC = indirect(in) -1;
    return 5;
}

int CPU::jsr(byte* in)
{
    push((PC >> 8) & 0xFF);
    push((PC & 0xFF));
    PC = absolute(in)-1;
    return 6;
}

int CPU::ldai(byte* in)
{
    A = *in;
    signFlag = (A >> 7)&1;
    zeroFlag = A;
    return 2;
}

int CPU::ldaz(byte* in)
{
    A = cpuMem.loadByte(*in);
    signFlag = (A >> 7) &1;
    zeroFlag = A;
    return 3;
}

int CPU::ldazx(byte* in)
{
    A = cpuMem.loadByte(zeroPageX(in));
    signFlag = (A >> 7) &1;
    zeroFlag = A;
    return 4;
}

int CPU::ldaa(byte* in)
{
    A = cpuMem.loadByte(absolute(in));
    signFlag = (A >> 7) &1;
    zeroFlag = A;
    return 4;
}

int CPU::ldaax(byte* in)
{
    A = cpuMem.loadByte(absoluteX(in));
    signFlag = (A >> 7) &1;
    zeroFlag = A;
    return 4; //could be +1
}

int CPU::ldaay(byte* in)
{
    A = cpuMem.loadByte(absoluteY(in));
    signFlag = (A >> 7) &1;
    zeroFlag = A;
    return 4; //could be + 1
}

int CPU::ldaix(byte* in)
{
    A = cpuMem.loadByte(indexedIndirect(in));
    signFlag = (A >> 7) &1;
    zeroFlag = A;
    return 6;
}

int CPU::ldaiy(byte* in)
{
    A = cpuMem.loadByte(indirectIndexed(in));
    signFlag = (A >> 7) &1;
    zeroFlag = A;
    return 5; //could be + 1
}

int CPU::ldxi(byte* in)
{
    A = *in;
    signFlag = (A >> 7)&1;
    zeroFlag = A;
    return 2;
}

int CPU::ldxz(byte* in)
{
    A = cpuMem.loadByte(*in);
    signFlag = (A >> 7) &1;
    zeroFlag = A;
    return 3;
}

int CPU::ldxzy(byte* in)
{
    A = cpuMem.loadByte(zeroPageY(in));
    signFlag = (A >> 7) &1;
    zeroFlag = A;
    return 4;
}

int CPU::ldxa(byte* in)
{
    A = cpuMem.loadByte(absolute(in));
    signFlag = (A >> 7) &1;
    zeroFlag = A;
    return 4;
}

int CPU::ldxay(byte* in)
{
    A = cpuMem.loadByte(absoluteY(in));
    signFlag = (A >> 7) &1;
    zeroFlag = A;
    return 4; //could be + 1
}

int CPU::ldyi(byte* in)
{
    A = *in;
    signFlag = (A >> 7)&1;
    zeroFlag = A;
    return 2;
}

int CPU::ldyz(byte* in)
{
    A = cpuMem.loadByte(*in);
    signFlag = (A >> 7) &1;
    zeroFlag = A;
    return 3;
}

int CPU::ldyzx(byte* in)
{
    A = cpuMem.loadByte(zeroPageX(in));
    signFlag = (A >> 7) &1;
    zeroFlag = A;
    return 4;
}

int CPU::ldya(byte* in)
{
    A = cpuMem.loadByte(absolute(in));
    signFlag = (A >> 7) &1;
    zeroFlag = A;
    return 4;
}

int CPU::ldyax(byte* in)
{
    A = cpuMem.loadByte(absoluteX(in));
    signFlag = (A >> 7) &1;
    zeroFlag = A;
    return 4; //could be + 1
}

byte CPU::rolOp(byte toShift)
{
    //arithmetic shift left
    byte res  = toShift;
    carryFlag = (res >> 7) & 1;
    toShift   = (res >> 1) & 0xFF;
    signFlag  = (res >> 7) &1;
    zeroFlag  = !(res);
    return res;
}

int CPU::rolac(byte* in) //accumulator
{
    A = rolOp(A);
    return 2;
}

int CPU::rolz(byte* in)
{
    cpuMem.writeByte(rolOp(cpuMem.loadByte(*in)), *in);
    return 5;
}

int CPU::rolzx(byte* in)
{
    cpuMem.writeByte(rolOp(cpuMem.loadByte(zeroPageX(in))), zeroPageX(in));
    return 6;
}

int CPU::rola(byte* in)
{
    cpuMem.writeByte(rolOp(cpuMem.loadByte(absolute(in))), absolute(in));
    return 6;
}

int CPU::rolax(byte* in)
{
    cpuMem.writeByte(rolOp(cpuMem.loadByte(absoluteX(in))), absoluteX(in));
    return 7;
}

int CPU::nop(byte* in)
{
    //Oh, it's a nop
    return 2;
}

byte CPU::oraOp(byte toOra)
{
    //Used to perform common ora operations
    byte res = A | toOra;
    signFlag = (A >> 7 & 1);
    (A > 0) ? zeroFlag = 0 : zeroFlag = 1;

    return res;

}

int CPU::orai(byte* in)
{
    A = oraOp((*in));
    return 2;
}

int CPU::oraz(byte* in)
{
    A = oraOp(cpuMem.loadByte((*in)));
    return 3;
}

int CPU::orazx(byte* in)
{
    byte res = cpuMem.loadByte(zeroPageX(in));
    A = oraOp(res);
    return 4;
}

int CPU::oraa(byte* in)
{
    byte res = cpuMem.loadByte(absolute(in));
    A = oraOp(res);
    return 4;
}

int CPU::oraax(byte* in)
{
    byte res = cpuMem.loadByte(absoluteX(in));
    A = oraOp(res);
    return 4; //could be + 1
}

int CPU::oraay(byte* in)
{
    byte res = cpuMem.loadByte(absoluteY(in));
    A = oraOp(res);
    return 4; //could be + 1
}

int CPU::oraix(byte* in)
{
    byte res = cpuMem.loadByte(indexedIndirect(in));
    A = oraOp(res);
    return 6;
}

int CPU::oraiy(byte* in)
{
    byte res = cpuMem.loadByte(indirectIndexed(in));
    A = oraOp(res);
    return 5; //could be + 1
}

int CPU::pha(byte* in)
{
    push(A);
    return 3;
}

int CPU::php(byte* in)
{
    updateFlagReg();
    push(ST);
    return 3;
}

int CPU::pla(byte* in)
{
    A = pull();
    signFlag = (A >> 7)&1;
    zeroFlag = A;
    return 4;
}

int CPU::plp(byte* in)
{
    updateFlagReg();
    ST = pull();
    updateStatusFlags();
    return 4;

}

byte CPU::rorOp(byte toShift)
{
    //arithmetic shift left
    byte res  = toShift;
    byte tmp = carryFlag << 7;
    carryFlag = toShift & 1;
    toShift   = ((res >> 1) & 0xFF) + tmp;
    signFlag = (toShift >>7)&1;
    zeroFlag  = !(res);
    return res;
}

int CPU::rorac(byte* in) //accumulator
{
    A = rorOp(A);
    return 2;
}

int CPU::rorz(byte* in)
{
    cpuMem.writeByte(rorOp(cpuMem.loadByte(*in)), *in);
    return 5;
}

int CPU::rorzx(byte* in)
{
    cpuMem.writeByte(rorOp(cpuMem.loadByte(zeroPageX(in))), zeroPageX(in));
    return 6;
}

int CPU::rora(byte* in)
{
    cpuMem.writeByte(rorOp(cpuMem.loadByte(absolute(in))), absolute(in));
    return 6;
}

int CPU::rorax(byte* in)
{
    cpuMem.writeByte(rorOp(cpuMem.loadByte(absoluteX(in))), absoluteX(in));
    return 7;
}

int CPU::rti(byte* in)
{
    //return from function
    byte tmp = pull();
    ST = tmp;
    updateStatusFlags();

    PC = pull(); //get PC back
    PC += (pull() << 8);
    if(PC == 0xFFFF)
        return -1;
    PC--;
    return 6;
}

byte CPU::sbcOp(byte toAdd)
{
    short res = A - toAdd - (1 - (carryFlag ? 1 : 0));
    overFlag = (!((A ^ toAdd) & 0x80) && ((A ^ res) & 0x80)) ? 1 : 0; //signed overflow
    carryFlag = res > 0xFF ? 1 : 0;  //unsigned overflow
    signFlag  = (res >> 7) & 1;      //last bit set to 1
    zeroFlag  = !(res);              //Did we get a zero?
    return (byte)res;
}

int CPU::sbci(byte* in)
{
    //add with carry immediate
    A = (addCOp(*in) & 0xFF);
    return 2;
}

int CPU::sbcz(byte* in)
{
    A = (addCOp(cpuMem.loadByte(*in)) & 0xFF);
    return 3;
}

int CPU::sbczx(byte* in)
{
    byte val = cpuMem.loadByte(zeroPageX(in));
    A = (addCOp(val) & 0xFF);
    return 4;
}

int CPU::sbca(byte* in)
{
    byte val = cpuMem.loadByte(absolute(in));
    A = (addCOp(val) & 0xFF);
    return 4;
}

int CPU::sbcax(byte* in)
{
    byte val = cpuMem.loadByte(absoluteX(in));
    A = (addCOp(val) & 0xFF);
    return 4;   //could be +1
}

int CPU::sbcay(byte* in)
{
    byte val = cpuMem.loadByte(absoluteY(in));
    A = (addCOp(val) & 0xFF);
    return 4;   //could be +1
}

int CPU::sbcix(byte* in)
{
    byte val = cpuMem.loadByte(indexedIndirect(in));
    A = (addCOp(val) & 0xFF);
    return 6;
}


int CPU::sbciy(byte* in)
{
    byte val = cpuMem.loadByte(indirectIndexed(in));
    A = (addCOp(val) & 0xFF);
    return 5; //could be + 1
}

int CPU::sec(byte* in)
{
    carryFlag = 1;
    return 2;
}

int CPU::sed(byte* in)
{
    decFlag = 1;
    return 2;
}

int CPU::sei(byte* in)
{
    intFlag = 1;
    return 2;
}

int CPU::staz(byte* in)
{
    cpuMem.writeByte(A, *in);
    return 3;
}

int CPU::stazx(byte* in)
{
    cpuMem.writeByte(A, zeroPageX(in));
    return 4;
}

int CPU::staa(byte* in)
{
    cpuMem.writeByte(A, absolute(in));
    return 4;
}

int CPU::staax(byte* in)
{
    cpuMem.writeByte(A, absoluteX(in));
    return 4;
}

int CPU::staay(byte* in)
{
    cpuMem.writeByte(A, absoluteY(in));
    return 4;
}

int CPU::staix(byte* in)
{
    cpuMem.writeByte(A, cpuMem.loadWord(indexedIndirect(in)));
    return 6;
}

int CPU::staiy(byte* in)
{
    cpuMem.writeByte(A, cpuMem.loadWord(indirectIndexed(in)));
    return 6;
}

int CPU::stxz(byte* in)
{
    cpuMem.writeByte(X, *in);
    return 3;
}

int CPU::stxa(byte* in)
{
    cpuMem.writeByte(X, absolute(in));
    return 4;
}

int CPU::styz(byte* in)
{
    cpuMem.writeByte(Y, *in);
    return 3;
}

int CPU::styzx(byte* in)
{
    cpuMem.writeByte(Y, zeroPageX(in));
    return 4;
}

int CPU::stya(byte* in)
{
    cpuMem.writeByte(Y, absolute(in));
    return 4;
}

int CPU::tax(byte* in)
{
    X = A;
    signFlag = (A >> 7) & 1;
    zeroFlag =  A;
    return 2;
}

int CPU::tay(byte* in)
{
    Y = A;
    signFlag = (A >> 7) & 1;
    zeroFlag =  A;
    return 2;
}

int CPU::tsx(byte* in)
{
    X = SP; //stack pointer register
    signFlag = (SP >> 7) & 1;
    zeroFlag =  X;
    return 2;
}

int CPU::txa(byte* in)
{
    A = X;
    signFlag = (X >> 7) & 1;
    zeroFlag =  X;
    return 2;
}

int CPU::txs(byte* in)
{
    SP = X;
    signFlag = (X >> 7) & 1;
    zeroFlag =  SP;
    return 2;
}

int CPU::tya(byte* in)
{
    A = Y;
    signFlag = (Y >> 7) & 1;
    zeroFlag =  Y;
    return 2;
}
