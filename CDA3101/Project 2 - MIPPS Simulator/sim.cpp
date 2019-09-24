/*
sim.cpp
Jonathan Soszka
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>



class Simulator
{
  typedef int mippsRegister;


public:

  Simulator ();
  bool Open(char* sourceFileName);
  bool Run(char* logFileName);


private:
  void ParseFile();
  void LoadData();

  std::ifstream _sourceFile;
  int _registerMap[32][32] = {};
  std::string hi;
  std::string lo;
  std::vector<std::string> _textSegment;
  std::vector<std::string> _dataSegment;

  //Mipps Functions

  //R
  void MippsAddu (mippsRegister rd, mippsRegister rs, mippsRegister rt);
  void MippsAnd (mippsRegister rd, mippsRegister rs, mippsRegister rt);
  void MippsDiv (mippsRegister rs, mippsRegister rt);
  void MippsMfhi (mippsRegister rd);
  void MippsMflo (mippsRegister rd);
  void MippsMult (mippsRegister rs, mippsRegister rt);
  void MippsOr (mippsRegister rd, mippsRegister rs, mippsRegister rt);
  void MippsSlt (mippsRegister rd, mippsRegister rs, mippsRegister rt);
  void MippsSubu (mippsRegister rd, mippsRegister rs, mippsRegister rt);
  void MippsSyscall();

  //I
  void MippsAddiu (mippsRegister rs, mippsRegister rt , int immed);
  void MippsBeq (mippsRegister rs, mippsRegister rt, int offset, size_t& pc);
  void MippsBne (mippsRegister rs, mippsRegister rt, int offset, size_t& pc);
  void MippsLw (mippsRegister rs, mippsRegister rt, int immed);
  void MippsSw (mippsRegister rs, mippsRegister rt, mippsRegister immed);

  //J
  void MippsJ (int targAddr, size_t& pc);





  //Helper functions
  std::string HexToBinary (std::string hex);
  std::string IntToBinary(int val, size_t padding);
  int BinaryToInt (std::string binary);
  int BinaryToIntU (std::string binary);


};

//default constructor
Simulator::Simulator()
{}

//opens designated file for processing
bool Simulator::Open(char* sourceFileName)
{
  _sourceFile.open(sourceFileName);
  if (_sourceFile.fail() == 1)
    return 0;

  return 1;
}

//Call helper methods to run file.
bool Simulator::Run(char* logFileName)
{
  ParseFile();
  LoadData();
  for (size_t i = 0; i < _textSegment.size(); i++)
  {
    std::string machineInstruction = HexToBinary(_textSegment[i]);
    int opcode = BinaryToIntU(machineInstruction.substr(0,6));
    if (machineInstruction.substr(0,6) == "000000") // R or syscall
    {
      int rs = BinaryToIntU(machineInstruction.substr(6,5));
      int rt = BinaryToIntU(machineInstruction.substr(11,5));
      int rd = BinaryToIntU(machineInstruction.substr(16,5));
      int funct = BinaryToIntU(machineInstruction.substr(26,6));
      //std::cout << machineInstruction << " | " << opcode << ", " << rs << ", " << rt << ", " << rd << ", " << funct << "\n";

      switch (funct)
      {
        case 33: MippsAddu(rd,rs,rt);
        break;

        case 36:MippsAnd(rd,rs,rt);
        break;

        case 26: MippsDiv(rs,rt);
        break;

        case 16: MippsMfhi(rd);
        break;

        case 18: MippsMflo(rd);
        break;

        case 24: MippsMult(rs,rt);
        break;

        case 37: MippsOr(rd,rs,rt);
        break;

        case 42: MippsSlt(rd,rs,rt);
        break;

        case 35: MippsSubu(rd,rs,rt);
        break;

        case 12: MippsSyscall();
        break;
      }
    }

    else // I or J
    {
      int rs = BinaryToIntU(machineInstruction.substr(6,5));
      int rt = BinaryToIntU(machineInstruction.substr(11,5));
      int immediate = BinaryToInt(machineInstruction.substr(16,16));
      int targAddr = BinaryToIntU(machineInstruction.substr(6,27));
      //std::cout << machineInstruction << " | " << opcode << ", " << rs << ", " << rt << ", " << immediate << ", " << targAddr << "\n";

      switch (opcode)
      {
        case 9: MippsAddiu(rs,rt,immediate);
        break;
        case 4: MippsBeq(rs,rt,immediate,i);
        break;
        case 5: MippsBne(rs,rt,immediate,i);
        break;
        case 35: MippsLw(rs,rt,immediate);
        break;
        case 43: MippsSw(rs,rt,immediate);
        break;

        case 2: MippsJ(targAddr,i);
        break;
      }
    }
  }



  return 0;
}

void Simulator::ParseFile()
{
  size_t instructionCount;
  size_t dataCount;

  char line[50];
  _sourceFile.getline(line,50);
  std::string instruction = line;

  instructionCount = stoi(instruction.substr(0,instruction.find(' ')));
  instruction.erase(0,instruction.find(' '));
  dataCount = stoi(instruction.substr(0,instruction.find('\n')));

  for (size_t i = 0; i < instructionCount; i++)
  {
    _sourceFile.getline(line,50);
    std::string instruction = line;
    _textSegment.push_back(instruction);
  }

  for (size_t i = 0; i < dataCount; i++)
  {
    _sourceFile.getline(line,50);
    std::string instruction = line;
    _dataSegment.push_back(instruction);
  }
}

void Simulator::LoadData()
{
  for (size_t i = 0; i < _dataSegment.size(); i++)
  {
    _registerMap[28][i + 1] = BinaryToInt(HexToBinary(_dataSegment[i]));
  }
}

//====== Mipps Function Implementations ======

//R
void Simulator::MippsAddu(Simulator::mippsRegister rd, Simulator::mippsRegister rs, Simulator::mippsRegister rt)
{
  _registerMap[rd][0] = _registerMap[rs][0] + _registerMap[rt][0];
  //std::cout << "new RD = " << _registerMap[rd][0] << "\n";
}

void Simulator::MippsSubu(Simulator::mippsRegister rd, Simulator::mippsRegister rs, Simulator::mippsRegister rt)
{
  _registerMap[rd][0] = _registerMap[rs][0] - _registerMap[rt][0];
}


void Simulator::MippsMult(Simulator::mippsRegister rs, Simulator::mippsRegister rt)
{
  int product = _registerMap[rs][0] * _registerMap[rt][0];
  std::string result = IntToBinary(product,64);
  hi = result.substr(0,31);
  lo = result.substr(32,61);
  //std:: cout << _registerMap[rs][0] << " * " << _registerMap[rt][0] << " =  ";
  //std::cout << lo << "  |  " << hi << '\n';
}

void Simulator::MippsDiv(Simulator::mippsRegister rs, Simulator::mippsRegister rt)
{
  int quotient = _registerMap[rs][0] / _registerMap[rt][0];
  int remainder = _registerMap[rs][0] % _registerMap[rt][0];

  hi = IntToBinary(remainder,32);
  lo = IntToBinary(quotient,32);
}


void Simulator::MippsMfhi(Simulator::mippsRegister rd)
{
 _registerMap[rd][0] = BinaryToInt(hi);
}

void Simulator::MippsMflo(Simulator::mippsRegister rd)
{
  _registerMap[rd][0] = BinaryToInt(lo);
  //std::cout << BinaryToInt(lo) << " mflo " << '\n';
}


void Simulator::MippsAnd(Simulator::mippsRegister rd, Simulator::mippsRegister rs, Simulator::mippsRegister rt)
{
  _registerMap[rd][0] = _registerMap[rs][0] & _registerMap[rt][0];
}

void Simulator::MippsOr(Simulator::mippsRegister rd, Simulator::mippsRegister rs, Simulator::mippsRegister rt)
{
  _registerMap[rd][0] = _registerMap[rs][0] | _registerMap[rt][0];
}


void Simulator::MippsSlt(Simulator::mippsRegister rd, Simulator::mippsRegister rs, Simulator::mippsRegister rt)
{
  if (_registerMap[rs][0] < _registerMap[rt][0])
  {
    _registerMap[rd][0] = 1;
    //std::cout << "Set" << "\n";
  }
  else
    {
    _registerMap [rd][0] = 0;
    //std::cout << "not set\n";
    }
}




void Simulator::MippsSyscall()
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
    case 10: exit(0);
             break;
  }
}



//I
void Simulator::MippsAddiu(Simulator::mippsRegister rs, Simulator::mippsRegister rt, int immed)
{
  _registerMap[rt][0] = _registerMap[rs][0] + immed;
  //std::cout << "new RT = " << _registerMap[rt][0] << "\n";
}

void Simulator::MippsSw(Simulator::mippsRegister rs, Simulator::mippsRegister rt, Simulator::mippsRegister immed)
{
  int memLoc = immed + 1;
  _registerMap[rs][memLoc] = _registerMap[rt][0];
  //std::cout << "Stored: " << _registerMap[rt][0] << " in " << "(" << memLoc << ")" << rs << "\n";
}

void Simulator::MippsLw(Simulator::mippsRegister rs, Simulator::mippsRegister rt, int immed)
{
  int memLoc = immed + 1;
  _registerMap[rt][0] = _registerMap[rs][memLoc];
  //std::cout << "Loaded: " << _registerMap[rt][0] << " from " << "(" << memLoc << ")" << rs << "\n";
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


//J
void Simulator::MippsJ(int targAddr, size_t& pc)
{
  //std::cout << targAddr << '\n';
  pc = targAddr;
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
}

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
}

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
}



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
}




int main(int argc, char** argv)
{
  if (argc < 3)
  {
    std::cerr << "Please provide 2 arguments: input file and output file" << '\n';
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