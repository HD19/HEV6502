/**********************
 * HEV6502 CPU Emulator
 * CPU.CPP
 * CPU function definitions
 **********************/
byte immediate(byte* in)
{
    return (*in)
}
byte accumulator(byte* in)
{
    return A;
}
byte zeroPage(byte* in)
{
    return cpuMem.load(*in);
}
byte zeroPageX(byte* in)
{
    byte res = cpuMem.load((*in));
    (res + X > 255) ? res -= X : res += X;
    return res;
}
byte zeroPageY(byte* in)
{
    byte res = cpuMem.load((*in));
    (res + Y > 255) ? res -= Y : res += Y;
    return res;
}

byte absolute(byte* in)
{
    //full address
    short addr = ((*++in) << 8); //high byte
    addr = (addr ^ (*in));       //low  byte
    byte res = cpuMem.load(addr);
    return res;
}
byte absoluteX(byte* in)
{
    //full address + X with wrapping..maybe
    byte low = (*in);
    (low + X > 255) ? res -= X : res += X;
    short addr = ((*++in) << 8); //high byte
    addr = (addr ^ low);
    byte res = cpuMem.load(addr);
    return res;
}
byte absoluteY(byte* in)
{
    //full address + Y with wrapping..maybe
    byte low = (*in);
    (low + Y > 255) ? res -= Y : res += Y;
    short addr = ((*++in) << 8); //high byte
    addr = (addr ^ low);
    byte res = cpuMem.load(addr);
    return res;
}
short indirect(byte* in)
{
    //used for jump, load address from address.
    short addr = (((*++in) << 8) & 0xFF00);
    addr = (addr ^ (*in)); //sweet, now load this byte and the next one
    short target = (((cpuMem.load(addr)) << 8) & 0xFF00);
    target = (target ^ cpuMem.load(++addr));
    return target;

}
byte indexedIndirect(byte* in)
{
    //indirect load + x from zero page?
    byte res = 
    ((*in) + X > 0xFF) ? 
}
byte indirectIndexed(byte* in)

int CPU::adci(byte* in)
{
    //add with carry immediate
    
    
    return 2;
}
int CPU::adcz(byte* in)
}

}
int CPU::adczx(byte* in)
{

}
int CPU::adca(byte* in)
{

}
int CPU::adcax(byte* in)
{

}
int CPU::adcay(byte* in)
{

}
int CPU::adcix(byte* in)
{

}
int CPU::adciy(byte* in)
{

}

int CPU::andop(byte toAnd)
{
    //Used to perform common AND operations
    byte res = A & toAnd;
    negativeFlag = (A >> 7 & 1);
    (A > 0) ? zeroFlag = 1 : zeroFlag = 0;
    
    return res;
    
}
int CPU::andi(byte* in)
{
    A = andop((*in));
    
    return 2;
    
}
int CPU::andz(byte* in)
{
    A = andop(cpuMem.load((*in)));
    return 3;
}
int CPU::andzx(byte* in)
{
    byte res = cpuMem.load((*in));
    (res + X > 255) ? res -= X : res += X;
    A = andop(res);
    return 4;
}
int CPU::anda(byte* in)
{
    A = andop(res);
}
int CPU::andax(byte* in)
{

}
int CPU::anday(byte* in)
{

}
int CPU::andix(byte* in)
{

}
int CPU::andiy(byte* in)
{

}

