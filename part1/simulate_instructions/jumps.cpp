
// g++ jumps.cpp ../sim86/sim86_lib.cpp -I../sim86/shared -o jumps && ./jumps
#include "sim86_shared.h"
#include <cstdio>
#include <cstdlib>
#include <cstdio>

typedef char unsigned u8;
typedef short unsigned u16;
typedef int unsigned u32;
typedef long long unsigned u64;
typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

// following mapping:
// - 0 ax
// - 1 bx
// - 2 cx
// - 3 dx
// - 4 sp
// - 5 bp
// - 6 si
// - 7 di
s16 registers[8] = {0,0,0,0,0,0,0,0};

u8 S = 0; //signed flag
u8 Z = 0; //zero flag

void print_instruction(operation_type op_type,  register_access* dest,  instruction_operand* src){
  const char* op_mem = Sim86_MnemonicFromOperationType(op_type);
  const char* dest_mem = Sim86_RegisterNameFromOperand(dest);
  switch(src->Type){
    case(Operand_Immediate):
      printf("%s %s, %d\n", op_mem, dest_mem, src->Immediate.Value);
    break;
    case(Operand_Register):
    const char* src_mem = Sim86_RegisterNameFromOperand(&src->Register);
    printf("%s %s, %s\n", op_mem, dest_mem, src_mem);
  }
}

void print_reg_contents(){
  // go through all regs and print their values
  printf("\n");
  printf("Final Registers:        \n");
  for (u8 i = 0; i<=7;i++){
    // we know the index-regName mapping so just use that and the provided function to get register name
    register_access reg = {};
    reg.Index = i + 1;
    reg.Offset = 0;
    reg.Count = 2;
    printf("%s: %d \n", Sim86_RegisterNameFromOperand(&reg), registers[i]);
  }
  printf("\n");
  printf("Flags: \n");
  printf("S = %d \n", S);
  printf("Z = %d \n", Z);
}

void printFlags(){
  printf("\n");
  printf("Flags: \n");
  printf("S = %d \n", S);
  printf("Z = %d \n", Z);
}

void setFlags(s16 regVal){
    if (regVal == 0b0){
      Z = 1;
    }else{
    Z = 0;
    }
    if(regVal >> 15) {
      //if highest bit is set, set sign flag
      S = 1;
    }else{
    S = 0;
    }
}

void resetFlags(){
  Z = S = 0;
}
void exec_instruction(instruction instr){

  instruction_operand dest = instr.Operands[0];
  instruction_operand src = instr.Operands[1];

  // get the address of the dest register we want to write into
  u32 destIndex = dest.Register.Index;
  s16 tmp;
  s16 srcValue;

  switch(src.Type){
    case(Operand_Immediate):
      // just write the value the dest index
      srcValue = src.Immediate.Value;
      break;
    case(Operand_Register):
      srcValue = registers[src.Register.Index - 1];
      break;
  }

  switch(instr.Op){
    case(Op_None):
      break; //if no operand do nothing!
    case(Op_mov):
      registers[destIndex - 1] = srcValue;
      resetFlags();
      break;
    case(Op_add):
      registers[destIndex - 1] += srcValue;
      setFlags(registers[destIndex - 1]);
      break;
    case(Op_sub):
      registers[destIndex - 1] -= srcValue;
      setFlags(registers[destIndex - 1]);
      break;
    case(Op_cmp):
      tmp =  registers[destIndex - 1] - srcValue;
      setFlags(tmp);
      break;
  }
  print_instruction(instr.Op, &dest.Register, &src);
  printFlags();
}
int main() {
  // first we need to read in the bytes from "listing_0044_register_movs"
  FILE* file = fopen("listing_0048_ip_register", "rb");
  if(!file) return 1;
  fseek(file, 0, SEEK_END); //see where the end of the file is
  long fileSize = ftell(file);
  rewind(file); //go back to beginning of file
  u8* buffer = (u8*) malloc(fileSize); //allocate fileSize many bytes on heap
  fread(buffer, 1, fileSize, file); //1 is size in bytes of each element
  u16 offset = 0; // the current position/offset
  u16 remaining = fileSize;// we need remaining bytes from current position to end of file
  instruction instr; // an instruction var that caseys function will fill

  while(true){
    remaining = fileSize - offset;
    if(remaining <= 0){
      break;
    }
    Sim86_Decode8086Instruction(remaining, buffer+offset, &instr);
    offset += instr.Size;
    //get the type of operation the current instruction is
    exec_instruction(instr);
  }
  print_reg_contents();
  free(buffer);
}
