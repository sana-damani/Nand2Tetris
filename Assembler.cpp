#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <string>
#include <map>

using namespace std;

// map string to value
std::map <string, string> encoder, destEncoder, symbolTable;
int currentAddress = 16;

typedef enum {
    ADDRESS,
    LABEL,
    MATH
} instType;

void InitEncoder()
{
    // dest encoder
    destEncoder[""] = "000";
    destEncoder["M"] = "001";
    destEncoder["D"] = "010";
    destEncoder["MD"] = "011";
    destEncoder["A"] = "100";
    destEncoder["AM"] = "101";
    destEncoder["AD"] = "110";
    destEncoder["AMD"] = "111";

    // jump encoder
    encoder[""] = "000";
    encoder["JGT"] = "001";
    encoder["JEQ"] = "010";
    encoder["JGE"] = "011";
    encoder["JLT"] = "100";
    encoder["JNE"] = "101";
    encoder["JLE"] = "110";
    encoder["JMP"] = "111";

    // a+comp encoder
    encoder["0"] = "0101010";
    encoder["1"] = "0111111";
    encoder["-1"] = "0111010";
    encoder["D"] = "0001100";
    encoder["A"] = "0110000";
    encoder["M"] = "1110000";
    encoder["!D"] = "0001101";
    encoder["!A"] = "0110001";
    encoder["!M"] = "1110001";
    encoder["-D"] = "0001111";
    encoder["-A"] = "0110011";
    encoder["-M"] = "1110011";
    encoder["D+1"] = "0011111";
    encoder["A+1"] = "0110111";
    encoder["M+1"] = "1110111";
    encoder["D-1"] = "0001110";
    encoder["A-1"] = "0110010";
    encoder["M-1"] = "1110010";
    encoder["D+A"] = "0000010";
    encoder["D+M"] = "1000010";
    encoder["D-A"] = "0010011";
    encoder["D-M"] = "1010011";
    encoder["A-D"] = "0000111";
    encoder["M-D"] = "1000111";
    encoder["D&A"] = "0000000";
    encoder["D&M"] = "1000000";
    encoder["D|A"] = "0010101";
    encoder["D|M"] = "1010101";
}

string StripWhiteSpace(string inputStr)
{
    string outputStr="";

    for (std::string::iterator it = inputStr.begin(); it != inputStr.end(); ++it)
    {
        if (*it == '/')
            break;
        if (*it == ' ' || *it == '\n' || *it == '\t')
            continue;
        outputStr += *it;
    }
    return outputStr;
}

string IntToBinaryString(int val)
{
    string binaryStr;

    if (val == 0)
        binaryStr += "0";

    while (val != 0) {
        int digit = val & 1;
        if (digit) {
            binaryStr = '1' + binaryStr;
        } else {
            binaryStr = '0' + binaryStr;
        }
        val = val >> 1;
    }

    // compute padding
    int padding = 15 - binaryStr.length();
    string paddingStr;
    while (padding) {
        paddingStr += "0";
        padding--;
    }

    return paddingStr + binaryStr;
}

bool IsSymbol(string sym) {
    // check if sym is an immediate address or a symbol
    for (std::string::iterator it = sym.begin(); it != sym.end(); ++it) {
        if (!isdigit(*it)) {
            return true;
        }
    }
    return false;
}

string Parse(string instruction)
{
    string element, binaryInst;
    instType type;

    // get instruction type
    if (instruction[0] == '@')
    {
        type = ADDRESS;
    } else if (instruction[0] == '(') {
        type = LABEL;
    } else {
        type = MATH;
    }

    if (type == ADDRESS) {
        binaryInst += "0";
        string symbol = instruction.substr(1, instruction.length() - 1);
        if (IsSymbol(symbol)) {
            // add symbol to symbol table if not already available
            if (symbolTable.count(symbol) == 0) {
                symbolTable[symbol] = IntToBinaryString(currentAddress);
                currentAddress++;
            }
            // fetch address of symbol
            binaryInst += symbolTable[symbol];
        } else {
            // immediate address
            int addrInt = stoi(instruction.substr(1, instruction.length() - 1));
            binaryInst += IntToBinaryString(addrInt);
        }
        binaryInst += '\n';
    } else if (type == MATH) {
        instruction += '\n';
        binaryInst += "111";
        string element="", dest="", jump="", comp="";
        bool HasJump = false;
        bool HasDest = false;
        for (std::string::iterator it = instruction.begin(); it != instruction.end(); ++it)
        {
            if (*it == '=') {
                dest = destEncoder[element];
                element = "";
            } else if (*it == ';') {
                HasJump = true;
                comp = encoder[element];
                element = "";
            } else if (*it == '\n') {
                if (dest.empty())
                    dest = destEncoder[""];
                if (comp.empty()) {
                    comp = encoder[element];
                } else {
                    jump = encoder[element];
                }
                if (jump.empty())
                    jump = encoder[""];
                binaryInst += comp + dest + jump;
            } else {
                element += *it;
            }
        }
        binaryInst += '\n';
    }
    return binaryInst;
}

void InitSymbolTable()
{
    // virtual registers
    symbolTable["R0"] = IntToBinaryString(0);
    symbolTable["R1"] = IntToBinaryString(1);
    symbolTable["R2"] = IntToBinaryString(2);
    symbolTable["R3"] = IntToBinaryString(3);
    symbolTable["R4"] = IntToBinaryString(4);
    symbolTable["R5"] = IntToBinaryString(5);
    symbolTable["R6"] = IntToBinaryString(6);
    symbolTable["R7"] = IntToBinaryString(7);
    symbolTable["R8"] = IntToBinaryString(8);
    symbolTable["R9"] = IntToBinaryString(9);
    symbolTable["R10"] = IntToBinaryString(10);
    symbolTable["R11"] = IntToBinaryString(11);
    symbolTable["R12"] = IntToBinaryString(12);
    symbolTable["R13"] = IntToBinaryString(13);
    symbolTable["R14"] = IntToBinaryString(14);
    symbolTable["R15"] = IntToBinaryString(15);

    // predefined pointers
    symbolTable["SP"] = IntToBinaryString(0);
    symbolTable["LCL"] = IntToBinaryString(1);
    symbolTable["ARG"] = IntToBinaryString(2);
    symbolTable["THIS"] = IntToBinaryString(3);
    symbolTable["THAT"] = IntToBinaryString(4);

    // I/O pointers
    symbolTable["SCREEN"] = IntToBinaryString(16384);
    symbolTable["KBD"] = IntToBinaryString(24576);
}

bool AddLabelToSymbolTable(string assemblyInst, int lineNo)
{
    if (assemblyInst[0] == '(') {
        // add label:lineNo to symbol table
        string label = assemblyInst.substr(1, assemblyInst.length() - 2);
        symbolTable[label] = IntToBinaryString(lineNo);
        return true;
    }
    return false;
}

void AssembleProgram(string inputName)
{
    ofstream binaryFile;
    ifstream assemblyProgram;
    string assemblyInst, binaryProgram;
    int lineNo = 0;

    InitSymbolTable();

    // phase 1: collect labels to handle forward references
    assemblyProgram.open(inputName);
    while (getline(assemblyProgram, assemblyInst))
    {
        // ignore whitespace
        assemblyInst = StripWhiteSpace(assemblyInst);
        if (assemblyInst != "") {
            if (!AddLabelToSymbolTable(assemblyInst, lineNo))
                lineNo++;
        }
    }

    // phase 2: assemble program
    assemblyProgram.clear();
    assemblyProgram.seekg(0, ios::beg);

    while (getline(assemblyProgram, assemblyInst))
    {
        // ignore whitespace
        assemblyInst = StripWhiteSpace(assemblyInst);
        if (assemblyInst != "") {
            binaryProgram += Parse(assemblyInst);
        }
    }
    assemblyProgram.close();

    binaryFile.open("out.hack");
    binaryFile << binaryProgram;
    binaryFile.close();
}

int main(int argc, char *argv[])
{
    InitEncoder();

    AssembleProgram(argv[1]);

    return 0;
}

