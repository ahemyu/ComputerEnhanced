// run like this: g++ decode.cpp -o decode

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

  int fd = open("listing_0037_single_register_mov", O_RDONLY);

  char buf[1024];
  ssize_t n = read(fd, buf, sizeof(buf));
  
  //Ok so in buf we got the raw bytes that we read
  //now we need to start the decode process
  //First, it is important to note that we are ONLY gonna do register to register moves here, memory is ignored for now
  //that means that DISP-LO and DISP-HI will not be present in the instructions we need to decode
  //so we only have two bytes that we care about (per instruction)
  // so the decoded instructions should look like: mov destination,source
  // first byte will always be 100010(opcode for mv) d w, d is direction (so 1 means REG in 2nd byte is destination, 0 means it is source), w determines if word/byte operation (0=byte, 1=word)
  // second byte is : mod reg r/m , where mod determines if register or memory(for us it will always be 11, bc we only do reg to reg), reg together with w determines the 1s treg of the operation, r/m determines second register
  // for our case here reg and r/m do the exact same thing and have the same lookup tables (bc mode will always be 11)
  
  // so the possible options for the first byte are
  // 10001000 (reg ist src; byte operation)
  // 10001001 (reg ist src; word operation)
  // 10001010 (reg ist dest; byte operation)
  // 10001011 (reg ist dest; word operation)
  //   byte = 8 bits (1 byte)
  //  word = 16 bits (2 bytes)
  unsigned char first = buf[0];
  unsigned char isWord = first & 1;
  unsigned char regIsDest = first & 0b10;

  //get name of opcode
  // or that shift first byte to bits to the right, and with 0b111111
  unsigned char firstShifted = first >> 2;
  unsigned char opcode = firstShifted & 0b111111;
  std::string opcodeName = getOpcodeName(opcode);

  //now that we got the opcode, we need to determine the two registers
  unsigned char second = buf[1];
  // first two bits we can ignore, so we need the bits 234 and 567 to encode the registers (together with isWord)
  //to get reg bits (234) we need to shift right by 3 bits, such that reg is on lowest 3 bits
   unsigned char regLow = second >> 3;
  // Now and that with 0b111, to only keep the reg bits
  unsigned char onlyReg = regLow & 0b111;
  // get 2nd register name
  unsigned char rmRegister = second & 0b11;
  std::string secondReg= getRegisterName(rmRegister, isWord);
  std::string dest;
  std::string src;
  
  //depending on regIsDest, firstReg is source secondReg is dest or vice versa
  if(regIsDest){
    dest = getRegisterName(onlyReg, isWord);
    src = getRegisterName(rmRegister, isWord);
  }else{

    src = getRegisterName(onlyReg, isWord);
    dest = getRegisterName(rmRegister, isWord);
  }

  std::cout << opcodeName << " " << dest << "," << src << '\n';


  close(fd);
}
