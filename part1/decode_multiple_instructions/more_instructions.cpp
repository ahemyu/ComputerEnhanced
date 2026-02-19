
// run like this: g++ more_instructions.cpp -o multi_iso

#include <fcntl.h> //file control/open constants and declarations
#include <string>
#include <unistd.h> // brings POSIX syscall declarations like read, write, close
#include <cstdio>
#include <cstring>

std::string getOperation(unsigned char op){ //highest 4bits of first byte
    
#include <string>
  if(op == 0b1000){
    return "rm_to_rm";

  }else if(op == 0b1011){
    return "immediate_to_reg";
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

void regMemToRegMem(char* buf, int index){
    
  printf("REG TO MEM \n");
  
}
int immediateToReg(char* buf, int index){
  // immed to reg starts with 1011 w reg; where w is 1 bit determining size (byte or word) and reg is 3 bits specifying register
  unsigned char firstByte = buf[index];
  //shift by 3 to the right such that w is at lowest bit; to & with 0b1 to get w
  unsigned char word = (firstByte >> 3 ) & 0b1;
  unsigned char regBits = firstByte & 0b111;
  std::string reg = getRegisterName(regBits, word);

  unsigned int val;
  unsigned int numberofBytes;

  if(word){
    // we need to read in two bytes, so shift the second byte left by 8 bits and then OR with the first byte
    numberofBytes = 3;
    val = static_cast<unsigned int>(buf[index+2] << 8) | buf[index + 1];
  }else{
    numberofBytes = 2;
    val = static_cast<unsigned int>(buf[index+1]);
  }
  printf("mv %s, %d\n", reg.c_str(), val);

  return numberofBytes;
}


int main() {

  int fd = open("listing_0039_more_movs", O_RDONLY);

  char buf[200];
  ssize_t n = read(fd, buf, sizeof(buf));

  unsigned int i = 0;

  while(i < n - 1)
  {

    //TODO: read in the correct opcode and go into corresponding instruction flow; we have Register/memory to/from register and Immediate to register
    // reg/mem to reg/mem starts with 100010dw
    // so we we need to look at 4 highest bits to decide which operatio we are in
    unsigned char firstByte = buf[i];
    //first shift highest 4 bits to low and then do an and with 1111 to get 4 bits as the opcode
    unsigned char opcode = (firstByte >> 4) & 0b1111; 
    std::string operation = getOperation(opcode);
    int numberofBytes = 2;

    if (operation == "rm_to_rm"){
      regMemToRegMem(buf, i);
    }else if(operation == "immediate_to_reg"){
      numberofBytes = immediateToReg(buf, i);
    }

    i+= numberofBytes;
  }

  close(fd);
}
