/*
sim.cpp
Jonathan Soszka


 Compile Command:

 $g++ -std=c++11 -Wall -Wextra -o sim.exe sim.o
 $g++ -std=c++11 -Wall -Wextra -I. -c sim.cpp

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <sstream>


class Simulator
{
  typedef int mippsRegister;


public:

  Simulator ();
  ~Simulator();
  bool Open(char* sourceFileName);     //Opens files for IO processes
  bool Run(char* logFileName);         //Main driver for simulation, Calls helper methods.


private:

  // ==== Private Data Members ====
  static const size_t registerCount = 31;
  std::ifstream _sourceFile;
  std::ofstream _logFile;
  int _registerMap[registerCount+1][32] = {};  //2d array for registers and memory
  std::string hi;                 //used in 64bit operations
  std::string lo;                 //used in 64bit operations
  std::vector<std::string> _textSegment; //Holds Machine Instructions
  std::vector<std::string> _dataSegment; //Holds Data instructions
  //Logfile support
  std::vector<std::string> _mippsInstructions; //Assembley language representation.


  // ==== Private Methods ====
  void ParseFile();  //Split source file into two segments, Data and Text(instructions)
  void LoadData();   //Load the data from the data segment into $gp

  //Mipps Functions

  //R
  void MippsAddu (mippsRegister rd, mippsRegister rs, mippsRegister rt, size_t& pc);
  void MippsAnd (mippsRegister rd, mippsRegister rs, mippsRegister rt, size_t& pc);
  void MippsDiv (mippsRegister rs, mippsRegister rt, size_t& pc);
  void MippsMfhi (mippsRegister rd, size_t& pc);
  void MippsMflo (mippsRegister rd, size_t& pc);
  void MippsMult (mippsRegister rs, mippsRegister rt, size_t& pc);
  void MippsOr (mippsRegister rd, mippsRegister rs, mippsRegister rt, size_t& pc);
  void MippsSlt (mippsRegister rd, mippsRegister rs, mippsRegister rt, size_t& pc);
  void MippsSubu (mippsRegister rd, mippsRegister rs, mippsRegister rt, size_t& pc);
  void MippsSyscall(size_t& pc);

  //I
  void MippsAddiu (mippsRegister rs, mippsRegister rt , int immed, size_t& pc);
  void MippsBeq (mippsRegister rs, mippsRegister rt, int offset, size_t& pc);
  void MippsBne (mippsRegister rs, mippsRegister rt, int offset, size_t& pc);
  void MippsLw (mippsRegister rs, mippsRegister rt, int immed, size_t& pc);
  void MippsSw (mippsRegister rs, mippsRegister rt, mippsRegister immed, size_t& pc);

  //J
  void MippsJ (int targAddr, size_t& pc);


  //Helper functions
  std::string HexToBinary (std::string hex);
  std::string IntToBinary(int val, size_t padding);
  int BinaryToInt (std::string binary);
  int BinaryToIntU (std::string binary);
  std::string GetAssemblyInstruction (int opcode, int rs, int rt, int rd, int funct, int immediate, int targAddr);
  std::string GetSimulatorStatus();
  std::string _registerNames[registerCount + 1];
  void MippsExit(size_t exitCode, std::string exitMessage = "");




};

//default constructor
Simulator::Simulator()
{
  _registerNames[0] = "$zero";
  _registerNames[1] = "$at";
  _registerNames[2] = "$v0";
  _registerNames[3] = "$v1";
  _registerNames[4] = "$a0";
  _registerNames[5] = "$a1";
  _registerNames[6] = "$a2";
  _registerNames[7] = "$a3";
  _registerNames[8] = "$t0";
  _registerNames[9] = "$t1";
  _registerNames[10] = "$t2";
  _registerNames[11] = "$t3";
  _registerNames[12] = "$t4";
  _registerNames[13] = "$t5";
  _registerNames[14] = "$t6";
  _registerNames[15] = "$t7";
  _registerNames[16] = "$s0";
  _registerNames[17] = "$s1";
  _registerNames[18] = "$s2";
  _registerNames[19] = "$s3";
  _registerNames[20] = "$s4";
  _registerNames[21] = "$s5";
  _registerNames[22] = "$s6";
  _registerNames[23] = "$s7";
  _registerNames[24] = "$t8";
  _registerNames[25] = "$t9";
  _registerNames[26] = "$k0";
  _registerNames[27] = "$k1";
  _registerNames[28] = "$gp";
  _registerNames[29] = "$sp";
  _registerNames[30] = "$fp";
  _registerNames[31] = "$ra";
}

Simulator::~Simulator()
{
}

//opens designated file for processing
bool Simulator::Open(char* sourceFileName)
{
  _sourceFile.open(sourceFileName);
  if (_sourceFile.fail() == 1)
    return 0;


  _logFile.open("log.txt");
  if (!_logFile.is_open())
  {
    std::cerr << "Log file could not be opened\n";
    return 0;
  }
    return 1;
}

//Call helper methods to run file.
bool Simulator::Run(char* logFileName)
{
  ParseFile();
  LoadData();
  _registerMap[28][0] = _textSegment.size();

  for (size_t i = 0; i < _textSegment.size(); i++)
  {
    std::string machineInstruction = HexToBinary(_textSegment[i]);
    int opcode = BinaryToIntU(machineInstruction.substr(0,6));    //Get Opcode
    int rs = BinaryToIntU(machineInstruction.substr(6,5));
    int rt = BinaryToIntU(machineInstruction.substr(11,5));
    int rd = BinaryToIntU(machineInstruction.substr(16,5));
    int funct = BinaryToIntU(machineInstruction.substr(26,6));
    int immediate = BinaryToInt(machineInstruction.substr(16,16));
    int targAddr = BinaryToIntU(machineInstruction.substr(6,27));

    if (_registerMap[0][0] != -0)
      _registerMap[0][0] = 0;

    _logFile << "PC: "  << i << '\n';
    _logFile << "inst: " << GetAssemblyInstruction(opcode,rs,rt,rd,funct,immediate,targAddr) << "\n";

    //R Instruction
    if (opcode == 0)
    {
      switch (funct)
      {
        case 33: MippsAddu(rd,rs,rt,i);
        break;
        case 36:MippsAnd(rd,rs,rt,i);
        break;
        case 26: MippsDiv(rs,rt,i);
        break;
        case 16: MippsMfhi(rd,i);
        break;
        case 18: MippsMflo(rd,i);
        break;
        case 24: MippsMult(rs,rt,i);
        break;
        case 37: MippsOr(rd,rs,rt,i);
        break;
        case 42: MippsSlt(rd,rs,rt,i);
        break;
        case 35: MippsSubu(rd,rs,rt,i);
        break;
        case 12: MippsSyscall(i);
        break;
        default: MippsExit(1, "could not find inst with opcode 0 and funct " + std::to_string(funct));
        break;
      }
    }

    // I or J Instruction
    else
    {
      switch (opcode)
      {
        case 9: MippsAddiu(rs,rt,immediate,i);
        break;
        case 4: MippsBeq(rs,rt,immediate,i);
        break;
        case 5: MippsBne(rs,rt,immediate,i);
        break;
        case 35: MippsLw(rs,rt,immediate,i);
        break;
        case 43: MippsSw(rs,rt,immediate,i);
        break;
        case 2: MippsJ(targAddr,i);
        break;
        default: MippsExit(1, "could not find inst with opcode " + std::to_string(opcode));
        break;
      }
    }

    if (i >= _textSegment.size())
    {
      MippsExit(2,"PC is accessing illegal memory address " + std::to_string(i+1));
    }

    _logFile << '\n' << GetSimulatorStatus();
  }



  //_logFile << GetSimulatorStatus();
  return 0;
}

void Simulator::ParseFile()
{
  size_t instructionCount;
  size_t dataCount;

  char line[50];
  _sourceFile.getline(line,50);
  std::string instruction = line;

  //Get instruction and data count from first line
  instructionCount = stoi(instruction.substr(0,instruction.find(' ')));
  instruction.erase(0,instruction.find(' '));
  dataCount = stoi(instruction.substr(0,instruction.find('\n')));

  _logFile << "insts:\n";
  for (size_t i = 0; i < instructionCount; i++)
  {

    _sourceFile.getline(line,50);
    std::string instruction = line;
    _textSegment.push_back(instruction);

    //Logging
    std::string machineInstruction = HexToBinary(line);
    int opcode = BinaryToIntU(machineInstruction.substr(0,6));
    int rs = BinaryToIntU(machineInstruction.substr(6,5));
    int rt = BinaryToIntU(machineInstruction.substr(11,5));
    int rd = BinaryToIntU(machineInstruction.substr(16,5));
    int funct = BinaryToIntU(machineInstruction.substr(26,6));
    int immediate = BinaryToInt(machineInstruction.substr(16,16));
    int targAddr = BinaryToIntU(machineInstruction.substr(6,27));
    _logFile << std::setw(4) << i << ": " << GetAssemblyInstruction(opcode,rs,rt,rd,funct,immediate,targAddr) << '\n';
  }

  _logFile << '\n';
  _logFile << "data:\n";
  for (size_t i = 0; i < dataCount; i++)
  {
    _sourceFile.getline(line,50);
    std::string instruction = line;
    _dataSegment.push_back(instruction);
    _logFile << "  " << i+instructionCount << ": " << BinaryToInt(HexToBinary(line)) << '\n';
  }
  _logFile << "\n";
}

void Simulator::LoadData()
{
  for (size_t i = 0; i < _dataSegment.size(); i++)
  {
    _registerMap[28][i + 1] = BinaryToInt(HexToBinary(_dataSegment[i]));
  }
}

//====== Mipps Function Implementations =======

// === R ===
void Simulator::MippsAddu(Simulator::mippsRegister rd, Simulator::mippsRegister rs, Simulator::mippsRegister rt, size_t& pc)
{
  _registerMap[rd][0] = _registerMap[rs][0] + _registerMap[rt][0];
}

void Simulator::MippsSubu(Simulator::mippsRegister rd, Simulator::mippsRegister rs, Simulator::mippsRegister rt, size_t& pc)
{
  _registerMap[rd][0] = _registerMap[rs][0] - _registerMap[rt][0];
}

void Simulator::MippsMult(Simulator::mippsRegister rs, Simulator::mippsRegister rt, size_t& pc)
{
  int product = _registerMap[rs][0] * _registerMap[rt][0];
  std::string result = IntToBinary(product,64);
  hi = result.substr(0,31);
  lo = result.substr(32,61);
}

void Simulator::MippsDiv(Simulator::mippsRegister rs, Simulator::mippsRegister rt, size_t& pc)
{
  if (rt == 0)
  {
    MippsExit(4,"divide by zero for instruction at " + std::to_string(pc));
  }
  int quotient = _registerMap[rs][0] / _registerMap[rt][0];
  int remainder = _registerMap[rs][0] % _registerMap[rt][0];
  hi = IntToBinary(remainder,32);
  lo = IntToBinary(quotient,32);
}

void Simulator::MippsMfhi(Simulator::mippsRegister rd, size_t& pc)
{
 _registerMap[rd][0] = BinaryToInt(hi);
}

void Simulator::MippsMflo(Simulator::mippsRegister rd, size_t& pc)
{
  _registerMap[rd][0] = BinaryToInt(lo);
}

void Simulator::MippsAnd(Simulator::mippsRegister rd, Simulator::mippsRegister rs, Simulator::mippsRegister rt, size_t& pc)
{
  _registerMap[rd][0] = _registerMap[rs][0] & _registerMap[rt][0];
}

void Simulator::MippsOr(Simulator::mippsRegister rd, Simulator::mippsRegister rs, Simulator::mippsRegister rt, size_t& pc)
{
  _registerMap[rd][0] = _registerMap[rs][0] | _registerMap[rt][0];
}

void Simulator::MippsSlt(Simulator::mippsRegister rd, Simulator::mippsRegister rs, Simulator::mippsRegister rt, size_t& pc)
{
  if (_registerMap[rs][0] < _registerMap[rt][0])
  {
    _registerMap[rd][0] = 1;
  }
  else
    {
      _registerMap [rd][0] = 0;
    }
}

void Simulator::MippsSyscall(size_t& pc)
{
  int arg = _registerMap[2][0]; //$v0
  int input;
  switch (arg)
  {
    case 1: std::cout << _registerMap[4][0] << '\n'; // $a0
             break;
    case 5: std::cin >> input;
            _registerMap[2][0] = input;
            break;
    case 10: MippsExit(0);
             break;
  }
}


// === I ===
void Simulator::MippsAddiu(Simulator::mippsRegister rs, Simulator::mippsRegister rt, int immed, size_t& pc)
{
  _registerMap[rt][0] = _registerMap[rs][0] + immed;
  //std::cout << "new RT = " << _registerMap[rt][0] << "\n";
}

void Simulator::MippsSw(Simulator::mippsRegister rs, Simulator::mippsRegister rt, Simulator::mippsRegister immed, size_t& pc)
{
  if (immed+1 > _dataSegment.size())
  {
    MippsExit(3, "store outside of data memory at address " + std::to_string(_textSegment.size() + immed));
  }
  int memLoc = immed + 1;
  _registerMap[rs][memLoc] = _registerMap[rt][0];
}

void Simulator::MippsLw(Simulator::mippsRegister rs, Simulator::mippsRegister rt, int immed, size_t& pc)
{
  if (immed + 1 > _dataSegment.size())
  {
    MippsExit(3, "load outside of data memory at address " + std::to_string(_textSegment.size() + immed));
  }
  int memLoc          = immed + 1;
  _registerMap[rt][0] = _registerMap[rs][memLoc];
}

void Simulator::MippsBeq(Simulator::mippsRegister rs, Simulator::mippsRegister rt, int offset, size_t& pc)
{
  //std::cout << "Offset: " << offset << "\n";
  if (_registerMap[rs][0] == _registerMap[rt][0])
  {
    pc = pc + (offset - 1);
    return;
  }
}

void Simulator::MippsBne(Simulator::mippsRegister rs, Simulator::mippsRegister rt, int offset, size_t& pc)
{
  if (_registerMap[rs][0] != _registerMap[rt][0])
  {
    pc = pc + (offset - 1);
    return;
  }
}

// === J ===
void Simulator::MippsJ(int targAddr, size_t& pc)
{
  //std::cout << targAddr << '\n';
  pc = targAddr-1;
}

void Simulator::MippsExit(size_t exitCode, std::string exitMessage)
{
  if (exitCode == 0)
  {
    _logFile << "exiting simulator\n";
    _logFile.close();
    _sourceFile.close();
    exit(0);
  }

  else
  {
    std::cerr << exitMessage << '\n';
    _logFile.close();
    _sourceFile.close();
    exit(0);
  }


}


//Logfile Support

std::string Simulator::GetAssemblyInstruction (int opcode, int rs, int rt, int rd, int funct, int immediate, int targAddr)
{
  std::stringstream ins;
  std::string rdString = _registerNames[rd];
  std::string rsString = _registerNames[rs];
  std::string rtString = _registerNames[rt];

  size_t col1 = 8;

  if (opcode == 0)
  {
    switch (funct)
    {
      case 33: ins << "addu\t" << rdString << "," <<  rsString << ',' <<  rtString ;
        break;
      case 36: ins << "and\t" << rdString << "," <<  rsString << ',' <<  rtString ;
        break;
      case 26: ins << "div\t" << rsString << ',' <<  rtString;
        break;
      case 16: ins << "mfhi\t" << rdString;
        break;
      case 18: ins << "mflo\t" << rdString;
        break;
      case 24: ins << "mult\t" << rsString << ',' <<  rtString;
        break;
      case 37: ins << "and\t" << rdString << "," <<  rsString << ',' <<  rtString ;
        break;
      case 42: ins << "slt\t" << rdString << "," <<  rsString << ',' <<  rtString ;
        break;
      case 35: ins << "subu\t" << rdString << "," <<  rsString << ',' <<  rtString ;
        break;
      case 12: ins << "syscall";
        break;
      default: ins << "Function not supported\t";
        break;
    }
    return ins.str();
  }

  // I or J Instruction
  else
  {
    switch (opcode)
    {
      case 9: ins << "addiu\t" << rtString << "," <<  rsString << ',' <<  immediate;
        break;
      case 4: ins << "beq\t" << rsString << "," <<  rtString << ',' <<  immediate;
        break;
      case 5: ins << "bne\t" << rsString << "," <<  rtString << ',' <<  immediate;
        break;
      case 35: ins << "lw\t" << rtString << "," << immediate <<'(' << rsString << ')';
        break;
      case 43: ins << "sw\t" << rtString << "," << immediate <<'(' << rsString << ')';
        break;
      case 2: ins << "j\t" << targAddr;
        break;
      default: ins << "Instruction Not supported.";
        break;
    }
    return ins.str();
  }
}

std::string Simulator::GetSimulatorStatus()
{
  std::stringstream status;
  status << "regs:\n";
  for (size_t i = 0; i < registerCount; i++)
  {
    status << std::setw(8) << _registerNames[i] << " =" << std::setw(6) << _registerMap[i][0];
    if ((i + 1) % 4 == 0)
      status << '\n';
  }

  status << std::setw(8) << "$ra" << " =" << std::setw(6) << 0 << '\n';
  status << std::setw(8) << "$lo" << " =" << std::setw(6) << BinaryToInt(lo);
  status << std::setw(8) << "$hi" << " =" << std::setw(6) << BinaryToInt(hi);

  status << "\n\n";

  status << "data memory:\n";
  for (size_t i = 0; i < _dataSegment.size(); i++)
  {
    status << std::setw(7) << "data" << "[" << std::setw(3) << i << "]" << " =" << std::setw(6) << _registerMap[28][i+1];
    if ((i+1) % 3 == 0)
    {
      status << '\n';
    }
  }

  status << "\n\n\n";

  return status.str();
}




//Helper function implementations
std::string Simulator::HexToBinary(std::string hex)
{
  std::string binary = "";
  for (size_t i = 0; i < hex.size(); i++)
  {
    switch (hex[i])
    {
      case '0': binary += "0000";
        break;
      case '1': binary += "0001";
        break;
      case '2': binary += "0010";
        break;
      case '3': binary += "0011";
        break;
      case '4': binary += "0100";
        break;
      case '5': binary += "0101";
        break;
      case '6': binary += "0110";
        break;
      case '7': binary += "0111";
        break;
      case '8': binary += "1000";
        break;
      case '9': binary += "1001";
        break;
      case 'a': binary += "1010";
        break;
      case 'b': binary += "1011";
        break;
      case 'c': binary += "1100";
        break;
      case 'd': binary += "1101";
        break;
      case 'e': binary += "1110";
        break;
      case 'f': binary += "1111";
        break;
    }
  }
  return binary;
}//HexToBinary

int Simulator::BinaryToInt(std::string binary)
{
  bool negative = 0;
  //Two's Compliment
  if (binary[0] == '1')
  {
    negative = 1;
    for (size_t i = 0; i < binary.size(); i++)
    {
      if (binary[i] == '1')
        binary[i] = '0';
      else
        binary[i] = '1';
    }
  }


  int result = 0;
  for (size_t i = binary.size() - 1; i <= binary.size(); i--)
  {
    if (binary[i] == '1')
    {
      result += pow(2,((binary.size()-1) - i));
    }
  }

  if (negative == 1)
  {
    result++;
    result = result * -1;
  }

  return result;
}//BinaryToInt

int Simulator::BinaryToIntU(std::string binary)
{
  int result = 0;
  for (size_t i = binary.size() - 1; i <= binary.size(); i--)
  {
    if (binary[i] == '1')
    {
      result += pow(2,((binary.size()-1) - i));
    }
  }

  return result;
}//BinaryToIntU

std::string Simulator::IntToBinary(int x, size_t padding)
{
  bool negative = 0;

  //twos compliment
  if (x < 0)
  {
    x*=-1;
    --x;
    negative = 1;
  }

  std::string binary="";
  int exp = 1;
  while (pow(2,exp) <= x)
    exp++;
  exp--;
  while (exp >= 0)
  {
    if (pow(2,exp) <= x)
    {
      binary.push_back('1');
      x -= (pow(2,exp));
    }
    else
      binary.push_back('0');
    --exp;
  }
  while (binary.size() < padding)
  {
    binary = "0" + binary;
  }

  //twos compliment
  if (negative == 1)
  {
    for (size_t i = 0; i < binary.size(); i++)
    {
      if (binary[i] == '1')
        binary[i] = '0';
      else
        binary[i] = '1';
    }

  }
  return binary;
}//IntToBinary




int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "Please provide 1 arguments: input file" << '\n';
    return 1;
  }

  Simulator sim;   //Create simulator object

  if (sim.Open(argv[1]) == 0)
  {
    std::cerr << "Source file could not be opened.\n";
    return 1;
  }

  sim.Run(argv[2]);  //Run simulator use destination file as log.

  std::cout << '\n';
  return 0;
}