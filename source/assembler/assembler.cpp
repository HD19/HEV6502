#include "assembler.h"

bool contains(string& toCheck, char target)
{
    return !(toCheck.find(target) == string::npos);
}

string StringToUpper(string strToConvert)
{
    std::transform(strToConvert.begin(), strToConvert.end(), strToConvert.begin(), ::toupper);

    return strToConvert;
}

vector<string> inline SplitString(const string& source, const char *delimiter = " ", bool keepEmpty = false)
{
    vector<string> results;

    size_t prev = 0;
    size_t next = 0;

    while ((next = source.find_first_of(delimiter, prev)) != string::npos)
    {
        if (keepEmpty || (next - prev != 0))
        {
            results.push_back(source.substr(prev, next - prev));
        }
        prev = next + 1;
    }

    if (prev < source.size())
    {
        results.push_back(source.substr(prev));
    }

    return results;
}

short extractValue(string toExtract)
{
    stringstream ss;
    short value = -1;
    
    if(toExtract[0] == '$') //hex value
    {
        ss << hex << toExtract.substr(1, toExtract.length()-1);
        ss >> value;
    }
    else
    {
       ss << toExtract;
       ss >> value;
    }
    return value;
    
}

sbyte Assembler::calculateBranch(short addr, short branchAddr)
{
    short tmpAddr = branchAddr;
    //branch is after us, need to jump forward
    if(tmpAddr - addr > 0xFF)
    {
        errorStack.push("Branch target is out of range!");
        return tmpAddr - addr;
    }
    if(addr < tmpAddr) // branch target is ahead of current PC
        return tmpAddr-addr;
    else
        return tmpAddr - (addr+2); //branch target is behind, need to account for instruction loading
}

short Assembler::getLabel(string toCheck)
{
    map<string, short>::iterator it;
    string tmp = StringToUpper(toCheck);
    if(isdigit(toCheck[0]))
        return -1;  //No numeric labels, damnit 
    
    it = labelMap.find(tmp);
    if(it != labelMap.end())
        return (it->second);
        
    return -1;
}

Assembler::Assembler()
{
    this->currentPC = 0;
    this->offset = 0;
    this->loadTable();
    byteCounts[IMP] = 1;
    byteCounts[IMM] = 2;
    byteCounts[ZP]  = 2;
    byteCounts[ZPX] = 2;
    byteCounts[ZPY] = 2;
    byteCounts[ABS] = 3;
    byteCounts[ABX] = 3;
    byteCounts[ABY] = 3;
    byteCounts[IND] = 3;
    byteCounts[IDX] = 3;
    byteCounts[IDY] = 3;
    byteCounts[REL] = 2;
}

void Assembler::setEntry(byte opCode, byte addrMode, string inst)
{
    map<string, byte*>::iterator it;

    it = opTable.find(inst);
    if(it != opTable.end())
    {
        (it->second)[addrMode] = opCode;
    }
    else
    {
        byte* tmp = (byte*)malloc(12 * sizeof(byte));
        bzero(tmp, 12 * sizeof(byte));
        tmp[addrMode] = opCode;
        (opTable[inst]) = tmp;
    }
    return;
}

bool Assembler::createLabel(string label)
{
    string tmp = StringToUpper(label);
    if(getLabel(tmp) > 0)
    {
        errorStack.push("Label " + label + " already exists, not redefining");
        return false;
    }
    labelMap[tmp] = currentPC; //Where the code should start, could be -1 if unresolved.
    return true; 
}

bool Assembler::isInstruction(string& toCheck)
{
    string tmp = StringToUpper(toCheck);
    
    map<string, byte*>::iterator it = opTable.find(tmp);
    if(it == opTable.end())
        return false;
    
    return true;
}

/*int Assembler::resolveLabels()
{
    map<string, short>::iterator it;
    string curKey;


    for(it = unresolvedLabelMap.begin(); it != unresolvedLabelMap.end(); ++it)
    {
        curKey = (it->first);
        short targetAddr = (it->second) - offset;
        short tmpAddr = getLabel(curKey);

        if(tmpAddr == -1)
        {
            errorStack.push("Couldn't resolve unknown label: " + curKey);
            return -1;
        }
        byte high = (tmpAddr >> 8) & 0xFF;
        byte low = (tmpAddr) & 0xFF;
        currentCode[++targetAddr] = low;   //IN PLACE CODE EDITING
        currentCode[++targetAddr] = high;
    }

    for(it = unresolvedBranchMap.begin(); it != unresolvedBranchMap.end(); ++it)
    {
        curKey = (it->first);
        short targetBranch = (it->second) - offset;
        short tmpAddr = getLabel(curKey);

        if(tmpAddr == -1)
        {
            errorStack.push("Couldn't resolve unknown label " + curKey + " for branch!");
            return -1;
        }
        byte branch = calculateBranch(targetBranch+1, tmpAddr-1);
        currentCode[++targetBranch] = branch;
    }
    return 0;
}*/

int Assembler::resolveLabels()
{
    //pop each item off the stack
    UnLabelEntry* entPtr;
    string curKey;
    short targetAddr = 0;
    short tmpAddr = 0;

    while(!unresolvedLabelStack.empty())
    {
        entPtr = unresolvedLabelStack.top();
        curKey = entPtr->labelName;
        targetAddr = entPtr->targetAddress - offset;
        tmpAddr = getLabel(curKey);
        if(tmpAddr == -1)
        {
            errorStack.push("Couldn't resolve unkown label " + curKey);
            return -1;
        }

        byte high = (tmpAddr >> 8) & 0xFF;
        byte low = (tmpAddr) & 0xFF;
        currentCode[++targetAddr] = low;
        currentCode[++targetAddr] = high;
        unresolvedLabelStack.pop();
    }

    while(!unresolvedBranchStack.empty())
    {
        entPtr = unresolvedBranchStack.top();
        curKey = entPtr->labelName;
        targetAddr = entPtr->targetAddress - offset;
        tmpAddr = getLabel(curKey);
        if(tmpAddr == -1)
        {
            errorStack.push("Couldn't resolve unkown label " + curKey + " for branch");
            return -1;
        }

        sbyte branch = calculateBranch((entPtr->targetAddress)+1, tmpAddr-1);
        currentCode[++targetAddr] = branch;
        unresolvedBranchStack.pop();
    }
    return 0;
}

//return bytes generated
int Assembler::decodeLine(string toDecode)
{
    //Getting a line to decode.
    vector<string> tokens = SplitString(toDecode);
    //did we get anything?
    string& curString = tokens[0];
    byte currentToken = 0;
    byte lastToken = tokens.size();
    //byte addrMode = -1;
    byte* opCodes = NULL;
    
    if(!tokens.size())
    {
        //we didn't get anything
        //errorStack.push("Couldn't tokenize given line!");
        return 0;
    }
    if(curString[0] == '\t')
    {
        //could be part of the string
        if(curString.length() > 1)
        {
            curString = curString.substr(1, curString.length() -1);
        }
        //get the next one
        else if((lastToken - (currentToken+1)))
        {
            currentToken++;
            curString = tokens[currentToken];
        }
        else
        {
            return 0; //no more tokens
        }
    }
    if(contains(curString, '\t'))
    {
        //we're past the first element
        curString = curString.substr(0, curString.find('\t'));
    }
    if(curString[0] == ';')
    {
        //we have a comment, ignore the rest.
        return 0;
    }

    if(curString[curString.length()-1] == ':')
    {
        //we're defining a label
        createLabel(curString.substr(0, curString.length()-1));
        if((lastToken - (currentToken+1)))
        {
            currentToken++;
            curString = tokens[currentToken];
            if(curString[0] == ';')
                return 0;
        }
        else
        {
            //Only this is in the line
            return 0;
        }
    }
    
    if(!isInstruction(curString))
    {
        errorStack.push("Didn't recognize " + curString + " as an instruction!");
        //we don't have an instruction
        //so panic
        return -1;
    }
    //figure out what the rest of the string is.
    string opStr = StringToUpper(curString);
    opCodes = opTable.find(opStr)->second;
    
    if((lastToken - (currentToken+1)))
    {
        currentToken++;
        curString = tokens[currentToken];

        if(contains(curString, '\t'))
        {
            //we're past the first element
            curString = curString.substr(0, curString.find('\t'));
        }
    }
    else
    {
        //implied instruction
        //addrMode = IMP;
        if(!opCodes[IMP] && curString != "BRK")
        {
            errorStack.push("Illegal address mode IMP for " + opStr);
            return -1;
        }
        currentCode.push_back(opCodes[IMP]); //push back the instruction
        return 1;
    }
    //we have an operand
    if(curString[0] == 'A' || curString[0] == ';')
    {
        //accumulator
        if(!opCodes[IMP] && opStr != "BRK")
        {
            errorStack.push("Illegal address mode IMP for " + opStr);
            return -1;
        }
        currentCode.push_back(opCodes[IMP]); 
        return 1;
    }
    if(curString[0] == '#' || isdigit(curString[0]))
    {
        if(curString[0] == '#')
            curString = curString.substr(1);
        //immediate
        byte value;
        
        if(!opCodes[IMM] && curString.length() > 1)
        {
            errorStack.push("Illegal address mode IMM for " + opStr);
            return -1;
        }
        
        //could be a label NOT SUPPORTED in this assembler!

        value = extractValue(curString);
        currentCode.push_back(opCodes[IMM]);
        currentCode.push_back((byte)(value & 0xFF));
        return 2;
    }
    if(contains(curString, ',') && (contains(curString, 'X') || contains(curString,'x')))
    {
        //could be ZPX, ABX or IDX
        if(contains(curString,'('))
        {
            //IDX
            string tmp = curString.substr(1, 3); //this is assuming $ will always have two digits
            if(!opCodes[IDX])
            {
                errorStack.push("Illegal address mode IDX for " + opStr);
                return -1;
            }
            byte value = (byte)extractValue(tmp) & 0xFF;
            currentCode.push_back(opCodes[IDX]);
            currentCode.push_back(value);
            return 2;
        }
        curString = curString.substr(0, curString.find(','));
        if(curString.length() > 3)
        {
            //ABX
            if(!opCodes[ABX])
            {
                errorStack.push("Illegal address mode ABX for " + opStr);
                return -1;
            }
            short value = extractValue(curString);
            byte high = (value >> 8) & 0xFF;
            byte low = (value) & 0xFF;
            currentCode.push_back(opCodes[ABX]);
            currentCode.push_back(low);
            currentCode.push_back(high);
            return 3;
        }
        else
        {
            //ZPX
            if(!opCodes[ZPX])
            {
                errorStack.push("Illegal address mode ZPX for " + opStr);
                return -1;
            }
            byte value = (byte)extractValue(curString) & 0xFF;
            currentCode.push_back(opCodes[ZPX]);
            currentCode.push_back(value);
            return 2;
        }
    }
    if(contains(curString, ',') && (contains(curString, 'Y') || contains(curString, 'y')))
    {
        //could be ZPY, ABY or IDY
        if(contains(curString,'('))
        {
            //IDY
            string tmp = curString.substr(1, 3); //this is assuming $ will always have two digits
            if(!opCodes[IDY])
            {
                errorStack.push("Illegal address mode IDY for " + opStr);
                return -1;
            }
            byte value = (byte)extractValue(tmp) & 0xFF;
            currentCode.push_back(opCodes[IDY]);
            currentCode.push_back(value);
            return 2;
        }
        curString = curString.substr(0, curString.find(','));
        if(curString.length() > 2)
        {
            //ABY
            if(!opCodes[ABY])
            {
                errorStack.push("Illegal address mode ABY for " + opStr);
                return -1;
            }
            short value = extractValue(curString);
            byte high = (value >> 8) & 0xFF;
            byte low = (value) & 0xFF;
            currentCode.push_back(opCodes[ABY]);
            currentCode.push_back(low);
            currentCode.push_back(high);
            return 3;
        }
        else
        {
            //ZPX
            if(!opCodes[ZPY])
            {
                errorStack.push("Illegal address mode ZPX for " + opStr);
                return -1;
            }
            byte value = (byte)extractValue(curString) & 0xFF;
            currentCode.push_back(opCodes[ZPY]);
            currentCode.push_back(value);
            return 2;
        }
    }
    //No X, nor Y. could be ZP IND ABS REL
    if(contains(curString, '(') && contains(curString, ')'))
    {
        //need them both! IND
        if(!opCodes[IND])
        {
            errorStack.push("Illegal address mdoe IND for " + opStr);
            return -1;
        }
        string tmp = curString.substr(1, curString.find(')') - 1);
        short value = -1;
        if(!(isdigit(tmp[0])) && !(tmp[0] == '$'))
        {
            //we have a label
            value = getLabel(tmp);
            if(value == -1)
            {
                //we don't have an address for that label yet!
                //unresolvedLabelMap[tmp] = currentPC; //store it's result for later resolution
                tmp = StringToUpper(tmp);
                unresolvedLabelStack.push(new UnLabelEntry(tmp, currentPC));
            }
        }
        else
        {
            value = extractValue(tmp);
        }
        byte high = (value >> 8) & 0xFF;
        byte low  = (value & 0xFF);
        currentCode.push_back(opCodes[IND]);
        currentCode.push_back(low);
        currentCode.push_back(high);
        return 3;
    }
    //Not IND
    if(curString.length() > 3 && curString[0] == '$')
    {
        //string is $xxxx
        //ABS
        if(!opCodes[ABS])
        {
            errorStack.push("Illegal address mode ABS for " + opStr);
            return -1;
        }

        short value = extractValue(curString);
        byte high = (value >> 8) & 0xFF;
        byte low = (value & 0xFF);
        currentCode.push_back(opCodes[ABS]);
        currentCode.push_back(low);
        currentCode.push_back(high);
        return 3;
    }
    if((curString.length() == 3 || curString.length() == 2 )&& curString[0] == '$')
    {
        //string is $xx or $x
        //ZP
        if(!opCodes[ZP])
        {
            errorStack.push("Illegal address mode ZP for " + opStr);
            return -1;
        }

        byte value = (byte)extractValue(curString) & 0xFF;
        currentCode.push_back(opCodes[ZP]);
        currentCode.push_back(value);
        return 2;
    }
    //Could only be a label, assuming relative or absolute!
    if(!opCodes[REL] && !opCodes[ABS])
    {
        errorStack.push("Illegal address mode REL for " + opStr);
        return -1;
    }
    short tmpAddr = getLabel(curString);

    if(opStr[0] == 'B' || opStr[0] == 'b')
    {
        //relative + branch

        if(tmpAddr == -1)
        {
            //label doesn't exist yet, need to resolve as branch later, so target will be at a later address.
            string tmp = StringToUpper(curString);
            //unresolvedBranchMap[tmp] = currentPC;
            unresolvedBranchStack.push(new UnLabelEntry(tmp, currentPC));
        }

        sbyte value = calculateBranch(currentPC, tmpAddr);
        currentCode.push_back(opCodes[REL]);
        currentCode.push_back(value);
        return 2;
    }
    else
    {
        //absolute, probably a jump
        if(tmpAddr == -1)
        {
            //label doesn't exist, put into unresolved labels for later.
            string tmp = StringToUpper(curString);
            //unresolvedLabelMap[tmp] = currentPC;
            unresolvedLabelStack.push(new UnLabelEntry(tmp, currentPC));
        }

        byte high = (tmpAddr >> 8) & 0xFF;
        byte low = (tmpAddr & 0xFF);
        currentCode.push_back(opCodes[ABS]);
        currentCode.push_back(low);
        currentCode.push_back(high);
        return 3;
    }

}

int Assembler::assemble()
{
    int tmpRes = 0;
    currentPC = 0 + offset;
    outputBlock = 0;
    currentCode.clear();
    labelMap.clear();
    //unresolvedLabelStack.clear();
    //unresolvedBranchStack.clear();


    //main logic goes here
    if(inputBuffer == "")
    {
        errorStack.push("No code to assemble!");
        return -1;
    }

    vector<string> lines = SplitString(inputBuffer, "\n");
    for(unsigned int i = 0; i < lines.size(); i++)
    {
        tmpRes = decodeLine(lines[i]);
        if(tmpRes == -1)
            return -1; //Failed to assemble, check the errorStack for error list
        else
            currentPC += tmpRes; //tmp res is how many bytes were written
    }
    //When that's done, hopefully without error, resolve branches.
    tmpRes = resolveLabels();
    if(tmpRes == -1)    //we failed!
        return -1;
    outputBlock = currentCode.data();
    return currentCode.size();

}

stack<string>* Assembler::getErrors()
{
    return &errorStack;
}

void Assembler::setText(string text)
{
    inputBuffer = text;   
}

byte* Assembler::getBinary()
{
    return outputBlock;
}

void Assembler::setOffset(short offset)
{
    this->offset = offset;
}

void Assembler::outputToFile(string fileName)
{
    return;
}

void Assembler::loadTable()
{
    //Load table of instructions.

    /* ADC */
    setEntry(0x69, IMM, "ADC");
    setEntry(0x65, ZP,  "ADC");
    setEntry(0x75, ZPX, "ADC");
    setEntry(0x6D, ABS, "ADC");
    setEntry(0x7D, ABX, "ADC");
    setEntry(0x79, ABY, "ADC");
    setEntry(0x61, IDX, "ADC");
    setEntry(0x71, IDY, "ADC");

    /* AND */
    setEntry(0x29, IMM, "AND");
    setEntry(0x25, ZP,  "AND");
    setEntry(0x35, ZPX, "AND");
    setEntry(0x2D, ABS, "AND");
    setEntry(0x3D, ABX, "AND");
    setEntry(0x39, ABY, "AND");
    setEntry(0x21, IDX, "AND");
    setEntry(0x31, IDY, "AND");

    /* ASL */
    setEntry(0x0A, IMP, "ASL");
    setEntry(0x06, ZP,  "ASL");
    setEntry(0x16, ZPX, "ASL");
    setEntry(0x0E, ABS, "ASL");
    setEntry(0x1E, ABX, "ASL");

    /* Bxx */
    setEntry(0x90, REL, "BCC");
    setEntry(0xB0, REL, "BCS");
    setEntry(0xF0, REL, "BEQ");
    setEntry(0x30, REL, "BMI");
    setEntry(0xD0, REL, "BNE");
    setEntry(0x10, REL, "BPL");
    setEntry(0x50, REL, "BVC");
    setEntry(0x70, REL, "BVS");

    setEntry(0x24, ZP, "BIT");
    setEntry(0x2C, ABS,"BIT");

    setEntry(0x00, IMP, "BRK");

    /* Cxx */
    setEntry(0x18, IMP, "CLC");
    setEntry(0xD8, IMP, "CLD");
    setEntry(0x58, IMP, "CLI");
    setEntry(0xB8, IMP, "CLV");

    /* CMP */
    setEntry(0xC9, IMM, "CMP");
    setEntry(0xC5, ZP,  "CMP");
    setEntry(0xD5, ZPX, "CMP");
    setEntry(0xCD, ABS, "CMP");
    setEntry(0xDD, ABX, "CMP");
    setEntry(0xD9, ABY, "CMP");
    setEntry(0xC1, IDX, "CMP");
    setEntry(0xD1, IDY, "CMP");

    /* CPX */
    setEntry(0xE0, IMM, "CPX");
    setEntry(0xE4, ZP,  "CPX");
    setEntry(0xEC, ABS, "CPX");

    /* CPY */
    setEntry(0xC0, IMM, "CPY");
    setEntry(0xC4, ZP,  "CPY");
    setEntry(0xCC, ABS, "CPY");

    /* DEC */
    setEntry(0xC6, ZP,  "DEC");
    setEntry(0xD6, ZPX, "DEC");
    setEntry(0xCE, ABS, "DEC");
    setEntry(0xDE, ABX, "DEC");

    setEntry(0xCA, IMP, "DEX");
    setEntry(0x88, IMP, "DEY");

    /* EOR */
    setEntry(0x49, IMM, "EOR");
    setEntry(0x45, ZP,  "EOR");
    setEntry(0x55, ZPX, "EOR");
    setEntry(0x4D, ABS, "EOR");
    setEntry(0x5D, ABX, "EOR");
    setEntry(0x59, ABY, "EOR");
    setEntry(0x41, IDX, "EOR");
    setEntry(0x51, IDY, "EOR");

    /* INC */
    setEntry(0xE6, ZP,  "INC");
    setEntry(0xF6, ZPX, "INC");
    setEntry(0xEE, ABS, "INC");
    setEntry(0xFE, ABX, "INC");

    setEntry(0xE8, IMP, "INX");
    setEntry(0xC8, IMP, "INY");

    /* JMP */
    setEntry(0x4C, ABS, "JMP");
    setEntry(0x6C, IND, "JMP");
    
    setEntry(0x20, ABS, "JSR");
    
    /* LDA */
    setEntry(0xA9, IMM, "LDA");
    setEntry(0xA5, ZP,  "LDA");
    setEntry(0xB5, ZPX, "LDA");
    setEntry(0xAD, ABS, "LDA");
    setEntry(0xBD, ABX, "LDA");
    setEntry(0xB9, ABY, "LDA");
    setEntry(0xA1, IDX, "LDA");
    setEntry(0xB1, IDY, "LDA");
    
    /* LDX */
    setEntry(0xA2, IMM, "LDX");
    setEntry(0xA6, ZP,  "LDX");
    setEntry(0xB6, ZPY, "LDX");
    setEntry(0xAE, ABS, "LDX");
    setEntry(0xBE, ABY, "LDX");
    
    /* LDY */
    setEntry(0xA0, IMM, "LDY");
    setEntry(0xA4, ZP,  "LDY");
    setEntry(0xB4, ZPX, "LDY");
    setEntry(0xAC, ABS, "LDY");
    setEntry(0xBC, ABX, "LDY");
    
    /* LSR */
    setEntry(0x4A, IMP, "LSR");
    setEntry(0x46, ZP,  "LSR");
    setEntry(0x56, ZPX, "LSR");
    setEntry(0x4E, ABS, "LSR");
    setEntry(0x5E, ABX, "LSR");
    
    /* NOP */
    setEntry(0xEA, IMP, "NOP");
    
    /* ORA */
    setEntry(0x09, IMM, "ORA");
    setEntry(0x05, ZP,  "ORA");
    setEntry(0x15, ZPX, "ORA");
    setEntry(0x0D, ABS, "ORA");
    setEntry(0x1D, ABX, "ORA");
    setEntry(0x19, ABY, "ORA");
    setEntry(0x01, IDX, "ORA");
    setEntry(0x11, IDY, "ORA");
    
    /* pushing and pulling */
    setEntry(0x48, IMP, "PHA");
    setEntry(0x08, IMP, "PHP");
    setEntry(0x68, IMP, "PLA");
    setEntry(0x28, IMP, "PLP");
    
    /* ROL */
    setEntry(0x2A, IMP, "ROL");
    setEntry(0x26, ZP,  "ROL");
    setEntry(0x36, ZPX, "ROL");
    setEntry(0x2E, ABS, "ROL");
    setEntry(0x3E, ABX, "ROL");
    
    /* ROR */
    setEntry(0x6A, IMP, "ROR");
    setEntry(0x66, ZP,  "ROR");
    setEntry(0x76, ZPX, "ROR");
    setEntry(0x6E, ABS, "ROR");
    setEntry(0x7E, ABX, "ROR");
    
    setEntry(0x40, IMP, "RTI");
    
    setEntry(0x60, IMP, "RTS");
    
    /* SBC */
    setEntry(0xE9, IMM, "SBC");
    setEntry(0xE5, ZP,  "SBC");
    setEntry(0xF5, ZPX, "SBC");
    setEntry(0xED, ABS, "SBC");
    setEntry(0xFD, ABX, "SBC");
    setEntry(0xF9, ABY, "SBC");
    setEntry(0xE1, IDX, "SBC");
    setEntry(0xF1, IDY, "SBC");  
    
    setEntry(0x38, IMP, "SEC");
    setEntry(0xF8, IMP, "SED");
    setEntry(0x78, IMP, "SEI");
    
    /* STA */
    setEntry(0x85, ZP,  "STA");
    setEntry(0x95, ZPX, "STA");
    setEntry(0x8D, ABS, "STA");
    setEntry(0x9D, ABX, "STA");
    setEntry(0x99, ABY, "STA");
    setEntry(0x81, IDX, "STA");
    setEntry(0x91, IDY, "STA");
    
    /* STX */
    setEntry(0x86, ZP,  "STX");
    setEntry(0x96, ZPY, "STX");
    setEntry(0x8E, ABS, "STX");
    
    /* STY */
    setEntry(0x84, ZP,  "STY");
    setEntry(0x94, ZPX, "STY");
    setEntry(0x8C, ABS, "STY");
    
    /* Txx */
    setEntry(0xAA, IMP, "TAX");
    setEntry(0xA8, IMP, "TAY");
    setEntry(0xBA, IMP, "TSX");
    setEntry(0x8A, IMP, "TXA");
    setEntry(0x9A, IMP, "TXS");
    setEntry(0x98, IMP, "TYA");
    
    //Table should be all set.      
}

UnLabelEntry::UnLabelEntry(string name, short tAddr) : labelName(name), targetAddress(tAddr)
{

}
