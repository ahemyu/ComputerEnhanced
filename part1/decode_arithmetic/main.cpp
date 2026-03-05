
// run like this: g++ main.cpp -o main
#include <fcntl.h> //file control/open constants and declarations
#include <string>
#include <unistd.h> // brings POSIX syscall declarations like read, write, close
#include <cstdio>
#include <cstring>
#include <cstdint>

std::string getMvOperation(unsigned char op){ //highest 4bits of first byte
    
#include <string>
  if(op == 0b1000){
    return "rm_to_rm";

  }else if(op == 0b1011){
    return "immediate_to_reg";
  }else if(op == 0b000000){
    return "r/m_with_reg_to_either";
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


std::string getEffectiveAddress(unsigned char rm){
    if(rm == 0b000){
      return "bx + si";
    }
    else if(rm == 0b001){

      return "bx + di";
    }
    else if(rm == 0b010){

      return "bp + si";
      }
    else if(rm == 0b011){

      return "bp + di";
    }
    else if(rm == 0b100){

      return "si";
    }
    else if(rm == 0b101){

      return "di";
    }
    else if(rm == 0b110){

      return "bp";
    }
    else if(rm == 0b111){

      return "bx";
    }
    return "invalid rm";
}

int regMemToRegMem(char* buf, int index){
    unsigned char d = (buf[index] >> 1) & 0b1; //if 1, reg is dest; otherwise src
    unsigned char w = buf[index] & 0b1; //if 1 we got a 16 bit operation; 8bit otherwise
    
    unsigned char secondByte = buf[index + 1];
    unsigned char mod = (secondByte >> 6) & 0b11; //decides which mode we are in
    unsigned char reg = (secondByte >> 3) & 0b111; //which register
    std::string regName = getRegisterName(reg, w);
    unsigned char rm = secondByte & 0b111; //eiher reg (if mod is 11) or used for effective address calculation
    
    std::string address = getEffectiveAddress(rm);
    std::string src;
    std::string dest;
    if(d){
      dest = regName;
    }else{
      src = regName;
    }
    if(mod == 0b11){
      //easy case, reg to reg
      std::string rmReg = getRegisterName(rm, w);
      if(dest == regName){
        src = rmReg;
      }else{
        dest = rmReg;
      }
      printf("mov %s, %s\n", dest.c_str(), src.c_str());
      return 2;
    }else if(mod == 0b00){
      //no displacement, EXCEPT rm is 110
        if(rm == 0b110){
          //there is an 16 bit displacement
          printf("IMPLEMENT ME");
          return 4;
        }else{
          if(dest == regName){
            src = "[" + address + "]";
          }else{
            dest = "[" + address + "]";
          }
          printf("mov %s, %s\n", dest.c_str(), src.c_str());
          return 2;
      }
    }else{
      int displacement;
      unsigned int bytes;
      if(mod == 0b01){
        // get the 3rd byte as the displacement value
        displacement = static_cast<int8_t>(static_cast<unsigned char>(buf[index+2]));
        bytes = 3;
      }else{
        displacement = (static_cast<uint8_t>(static_cast<unsigned char>(buf[index+3])) << 8) |
                                   static_cast<uint8_t>(static_cast<unsigned char>(buf[index + 2]));
        bytes = 4;
      }
      if (displacement > 0){
        address = "[" + address + " + " + std::to_string(displacement) + "]";
      }else if(displacement < 0){
        address = "[" + address + " - " + std::to_string(-displacement) + "]";
      }else{
        address = "[" + address + "]";
      }
      if(dest == regName){
        src = address;
      }else{
        dest = address;
      }
      printf("mv %s, %s\n", dest.c_str(), src.c_str());
      return bytes;

    }
}
int immediateToReg(char* buf, int index){
  // immed to reg starts with 1011 w reg; where w is 1 bit determining size (byte or word) and reg is 3 bits specifying register
  unsigned char firstByte = buf[index];
  //shift by 3 to the right such that w is at lowest bit; to & with 0b1 to get w
  unsigned char word = (firstByte >> 3 ) & 0b1;
  unsigned char regBits = firstByte & 0b111;
  std::string reg = getRegisterName(regBits, word);

  int val;
  unsigned int numberofBytes;

  if(word){
    // we need to read in two bytes, so shift the second byte left by 8 bits and then OR with the first byte
    numberofBytes = 3;
    uint16_t rawValue = (static_cast<uint8_t>(static_cast<unsigned char>(buf[index+2])) << 8) |
                        static_cast<uint8_t>(static_cast<unsigned char>(buf[index + 1]));
    val = static_cast<int16_t>(rawValue);
  }else{
    numberofBytes = 2;
    val = static_cast<int8_t>(static_cast<unsigned char>(buf[index + 1]));
  }
  printf("mv %s, %d\n", reg.c_str(), val);

  return numberofBytes;
}

int add(char* buf, size_t index){
  
  return 0;
}

int sub(char* buf, size_t index){
  
  return 0;
}

int cmp(char* buf, size_t index){
  
  return 0;
}


int main() {

  int fd = open("listing_0039_more_movs", O_RDONLY);

  char buf[200];
  ssize_t n = read(fd, buf, sizeof(buf));

  unsigned int i = 0;

  while(i < n - 1)
  {

    // so we we need to look at 4 highest bits to decide which operatio we are in
    unsigned char firstByte = buf[i];
    //first shift highest 4 bits to low and then do an and with 1111 to get 4 bits as the opcode
    unsigned char opcode = (firstByte >> 4) & 0b1111; 
    std::string operation = getMvOperation(opcode);
    int numberofBytes = 2; //where the next instruction starts

    if (operation == "rm_to_rm"){
      numberofBytes = regMemToRegMem(buf, i);
    }else if(operation == "immediate_to_reg"){
      numberofBytes = immediateToReg(buf, i);
    }
    i+= numberofBytes;
  }

  close(fd);
}
