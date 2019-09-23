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

  std::ifstream _sourceFile;
  int _registerMap[32];
  std::vector<std::string> _textSegment;
  std::vector<std::string> _dataSegment;

  //Mipps Functions
  void MippsAddiu (mippsRegister rs, mippsRegister rt , int immed);
  void MippsAddu (mippsRegister rd, mippsRegister rs, mippsRegister rt);
  void MippsAnd (mippsRegister rd, mippsRegister rs, mippsRegister rt);
  void MippsBeq (mippsRegister rs, mippsRegister rt, int offset);
  void MippsBne (mippsRegister rs, mippsRegister rt, int offset);
  void MippsDiv (mippsRegister rs, mippsRegister rt);
  void MippsJ (int targAddr);
  void MippsLw (mippsRegister rs, mippsRegister rt, int immed);
  void MippsMfhi (mippsRegister rd);
  void MippsMflo (mippsRegister rd);
  void MippsMult (mippsRegister rs, mippsRegister rt);
  void MippsOr (mippsRegister rd, mippsRegister rs, mippsRegister rt);
  void MippsSlt (mippsRegister rd, mippsRegister rs, mippsRegister rt);
  void MippsSubu (mippsRegister rd, mippsRegister rs, mippsRegister rt);
  void MippsSw (mippsRegister rs, mippsRegister rt, mippsRegister immed);
  void MippsSyscall();

  //Helper functions
  std::string HexToBinary (std::string hex);
  int BinaryToInt (std::string binary);
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
  for (size_t i = 0; i < _textSegment.size(); i++)
  {
    std::string machineInstruction = HexToBinary(_textSegment[i]);
    if (machineInstruction.substr(0,6) == "000000") // R or syscall
    {
      int rs = BinaryToInt(machineInstruction.substr(6,5));
      int rt = BinaryToInt(machineInstruction.substr(11,5));
      int rd = BinaryToInt(machineInstruction.substr(16,5));
      int funct = BinaryToInt(machineInstruction.substr(27,5));
      std::cout << rs << ", " << rt << ", " << rd << ", " << funct << "\n";
    }

    //TODO finish parsing R instructions

    //Get Opcode
      //if 0 get funct, rd, rs, rt, shamt = 0


      //else get rs, rt, immed, targaddr




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


}