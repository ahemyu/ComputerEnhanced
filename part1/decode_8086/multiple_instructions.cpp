// run like this: g++ multiple_instructions.cpp -o multi

#include <fcntl.h> //file control/open constants and declarations
#include <string>
#include <unistd.h> // brings POSIX syscall declarations like read, write, close
#include   <iostream>

std::string getOpcodeName(unsigned char op){
    
  if(op == 0b100010){
    return "mv";
  }
  return "opcode not supported yet";
}

std::string getRegisterName(unsigned char regBits, unsigned char isWord ){

  if(isWord){
    if(regBits == 0b000){
      return "ax";
    }
    else if(regBits == 0b001){

      return  "cx";
    }
    else if(regBits == 0b010){

      return "dx";
    }
    else if(regBits == 0b011){

      return "bx";
    }
    else if(regBits == 0b100){

      return  "sp";
    }
    else if(regBits == 0b101){

      return "bp";
    }
    else if(regBits == 0b110){

      return "si";
    }
    else if(regBits == 0b111){

      return "di";
    }
  }else{

    if(regBits == 0b000){
      return "al";
    }
    else if(regBits == 0b001){

      return "cl";
    }
    else if(regBits == 0b010){

      return "dl";
    }
    else if(regBits == 0b011){

      return "bl";
    }
    else if(regBits == 0b100){

      return "ah";
    }
    else if(regBits == 0b101){

      return "ch";
    }
    else if(regBits == 0b110){

      return "dh";
    }
    else if(regBits == 0b111){

      return "bh";
    }
  }
  
    return "Not a valid register";
}

int main() {

  // Just put all of it in a loop until we finish
  int fd = open("listing_0038_many_register_mov", O_RDONLY);

  char buf[1024];
  ssize_t n = read(fd, buf, sizeof(buf));

  unsigned int i = 0;

  while(i < n - 1)
  {
    unsigned char first = buf[i];
    unsigned char isWord = first & 1;
    unsigned char regIsDest = first & 0b10;

    unsigned char firstShifted = first >> 2;
    unsigned char opcode = firstShifted & 0b111111;
    std::string opcodeName = getOpcodeName(opcode);

    unsigned char second = buf[i+1];
    unsigned char regLow = second >> 3;
    unsigned char onlyReg = regLow & 0b111;
    unsigned char rmRegister = second & 0b111;
    std::string secondReg= getRegisterName(rmRegister, isWord);
    std::string dest;
    std::string src;

    if(regIsDest){
      dest = getRegisterName(onlyReg, isWord);
      src = getRegisterName(rmRegister, isWord);
    }else{

      src = getRegisterName(onlyReg, isWord);
      dest = getRegisterName(rmRegister, isWord);
    }

    std::cout << opcodeName << " " << dest << "," << src << '\n';
    i+=2;
    }

  close(fd);
}
