/**********************
 * HEV6502 CPU Emulator
 * CPU.CPP
 * CPU function definitions
 **********************/
short relative(byte* in)
{
    short addr = cpuMem.loadWord(*in);
    (addr < 0x80)? addr+= PC : addr += (PC - 256); 
    return relative;
} 
 
short zeroPageX(byte* in)
{
    short addr = (*in);
    addr = (( addr + X ) & 0xFF);
    return addr;
}

short zeroPageY(byte* in)
{
    short addr = (*in);
    addr = (( addr + Y ) & 0xFF);
    return addr;
}

short absolute(byte* in)
{
    //full address
    short addr = ((*++in) << 8); //high byte
    addr = (addr ^ (*in));       //low  byte
    //byte res = cpuMem.loadByte(addr);
    return addr;
}
short absoluteX(byte* in)
{
    //full address + X with wrapping..maybe
    byte low = (*in);
    (low + X > 255) ? res -= X : res += X;
    short addr = ((*++in) << 8); //high byte
    addr = (addr ^ low);
    //byte res = cpuMem.loadByte(addr);
    return addr;
}
byte absoluteY(byte* in)
{
    //full address + Y with wrapping..maybe
    byte low = (*in);
    (low + Y > 255) ? res -= Y : res += Y;
    short addr = ((*++in) << 8); //high byte
    addr = (addr ^ low);
    //byte res = cpuMem.loadByte(addr);
    return addr;
}
short indirect(byte* in)
{
    //used for jump, load address from address.
    short addr = (((*++in) << 8) & 0xFF00);
    addr = (addr ^ (*in)); //sweet, now load this byte and the next one
    short target = (((cpuMem.loadByte(addr)) << 8) & 0xFF00);
    target = (target ^ cpuMem.loadByte(++addr));
    return target;

}
//returning the address to work with.
short indexedIndirect(byte* in)
{
    //we're loading the address at ($00(x + *in))
    short addr = (*in) + X;
    addr &= 0xFF00; //load the zero page address here and +1
    short target = cpuMem.loadByte(addr) << 8 & 0xFF00;
    target = target ^ cpuMem.loadByte(++addr));
    //now load the target
    return target; 
}
//returning the address to work with
short indirectIndexed(byte* in)
{
    //LSR ($2A), Y
    //The value $03 in Y is added to the address $C235 at addresses $002A and $002B for a sum of $C238. 
    //The value $2F at $C238 is shifted right (yielding $17) and written back to $C238.
    short target = cpuMem.loadByte(*in) << 8 & 0xFF00;
    target = target ^ cpuMem.loadByte(++(*in));
    target += Y;
    return target;
}

byte CPU::addCOp(byte toAdd)
{
    byte res = A + toAdd + carryFlag;
    overFlag = (!((A ^ toAdd) & 0x80) && ((A ^ res) & 0x80)); //signed overflow
    carryFlag = res > 0xFF ? 1 : 0;  //unsigned overflow
    signFlag  = (res >> 7) & 1;      //last bit set to 1
    zeroFlag  = !(res);              //Did we get a zero?
    return res; 
}

int CPU::adci(byte* in)
{
    //add with carry immediate
    A = (addCOp(*in); & 0xFF);
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

int CPU::andOp(byte toAnd)
{
    //Used to perform common AND operations
    byte res = A & toAnd;
    signFlag = (A >> 7 & 1);
    (A > 0) ? zeroFlag = 0 : zeroFlag = 1;
    
    return res;
    
}

int CPU::andi(byte* in)
{
    A = andop((*in));
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
    byte res = cpuMem.loadByte(absoulteY(in));
    A = andOp(res);
    return 4; //could be + 1
}

int CPU::andix(byte* in)
{
    byte res = cpuMem.load(indexedIndirect(in));
    A = andOp(res);
    return 6;    
}

int CPU::andiy(byte* in)
{
    byte res = cpuMem.load(indrectIndexed(in));
    A = andOp(res);
    return 5; //could be + 1
}

int aslOp(byte toShift)
{
    //arithmetic shift left
    byte res  = toShift;
    carryFlag = (res >> 7) & 1;
    toShift   = (res << 1) & 0xFF;
    signFlag  = (res >> 7) &1;
    zeroFlag  = !(res);
    return res;
}

int aslac(byte* in) //accumulator
{
    A = aslOp(A);
    return 2;
}

int aslz(byte* in)
{
    cpuMem.writeByte(aslOp(cpuMem.loadByte(*in)), *in);
    return 5;
}

int aslzx(byte* in)
{
    cpuMem.writeByte(aslOp(cpuMem.loadByte(zeroPageX(in)), zeroPageX(in));
    return 6;
}

int asla(byte* in)
{
    cpuMem.writeByte(aslOp(cpuMem.loadByte(absolute(in)), absolute(in));
    return 6;
}

int aslax(byte* in)
{
    cpuMem.writeByte(aslOp(cpuMem.loadByte(absoluteX(in)), absoluteX(in));
    return 7;
}

int bcc(byte *in)
{
    //if carry is clear, branch
    if(!carryFlag)
        PC = relative(in);
    return 2; //could be + 1 or 2
}

int bcs(byte *in)
{
    //if carry is set, branch
    if(carryFlag)
        PC = relative(in);
    return 2;
}

int beq(byte *in)
{
    //branch if equal
    if(zeroFlag)
        PC = relative(in);
    return 2;
}


int bitz(byte *in)
{
    byte val = cpuMem.loadByte(*in);
    signFlag = (val >> 7) & 1;
    overFlag = (val >> 6) & 1;
    zeroFlag = A & val;
    return 3;
}

int bita(byte *in)
{
    byte val = cpuMem.loadByte(absolute(in));
    signFlag = (val >> 7) & 1;
    overFlag = (val >> 6) & 1;
    zeroFlag = A & val;
    return 4;
}

int bmi(byte* in)
{
    if(signFlag)
        PC = relative(in);
    return 2;
}

int bne(byte* in)
{
    if(!zeroFlag)
        PC = relative(in);
    return 2;
}

int bpl(byte* in)
{
    if(!signFlag)
        PC = relative(in);
    return 2;
}

int brk(byte* in)
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

int bvc(byte* in)
{
    //branch if overflow clear
    if(!overFlag)
        PC = relative(in);
    return 2;
}

int bvs(byte* in)
{
    if(overFlag)
        PC = relative(in);
    return 2;
} 

int clc(byte* in)
{
    clearFlag = 0;
    return 2;
}

int cld(byte* in)
{
    decFlag = 0;
    return 2;
}

int cli(byte* in)
{
    intFlag = 0;
    return 2;
}

int clv(byte* in)
{
    overFlag = 0;
    return 2;
}

int cmpOp(byte toComp)
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
    A = (cmpOp(*in); & 0xFF);
    return 2;
}

int CPU::cmpz(byte* in)
{
    A = (cmpOp(cpuMem.loadByte(*in)) & 0xFF);
    return 3;
}

int CPU::cmpzx(byte* in)
{
    byte val = cpuMem.loadByte(zeroPageX(in));
    A = (cmpOp(val) & 0xFF);
    return 4;
}

int CPU::cmpa(byte* in)
{
    byte val = cpuMem.loadByte(absolute(in));
    A = (cmpOp(val) & 0xFF);
    return 4;
}

int CPU::cmpax(byte* in)
{
    byte val = cpuMem.loadByte(absoluteX(in));
    A = (cmpOp(val) & 0xFF);
    return 4;   //could be +1
}

int CPU::cmpay(byte* in)
{
    byte val = cpuMem.loadByte(absoluteY(in));
    A = (cmpOp(val) & 0xFF);
    return 4;   //could be +1
}

int CPU::cmpix(byte* in)
{
    byte val = cpuMem.loadByte(indexedIndirect(in));
    A = (cmpOp(val) & 0xFF);
    return 6;
}


int CPU::cmpiy(byte* in)
{
    byte val = cpuMem.loadByte(indirectIndexed(in));
    A = (cmpOp(val) & 0xFF);
    return 5;
}




