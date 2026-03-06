// g++ add_sub_cmp.cpp ../sim86/sim86_lib.cpp -I../sim86/shared -o add_sub_cmp && ./add_sub_cmp

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
s32 registers[8] = {0,0,0,0,0,0,0,0};

u8 S = 0; //signed flag
u8 Z = 0; //zero flag

void print_instruction(operation_type op_type,  register_access* dest,  register_access* src){
  const char* op_mem = Sim86_MnemonicFromOperationType(op_type);
  const char* dest_mem = Sim86_RegisterNameFromOperand(dest);
  const char* src_mem = Sim86_RegisterNameFromOperand(src);

  printf("%s %s, %s\n", op_mem, dest_mem, src_mem);

}

void exec_mov(instruction instr){
  // simulate instruction and print all of the registers current values
  instruction_operand dest = instr.Operands[0];
  instruction_operand src = instr.Operands[1];

  // based on the index, offset and count we can read/write the bytes of the registers (a at 1, b at 2, ...,di at 8 )
  // TODO: as for this exercise the Count will always be 2 (16 bit movs) and offset will be 0, we will ignore it for now
 
  // get the address of the dest register we want to write into
  u32 destIndex = dest.Register.Index;

  switch(src.Type){
    case(Operand_None):
      break;
    case(Operand_Immediate):
      // just write the value the dest index
      registers[destIndex - 1] = src.Immediate.Value;
      break;
    case(Operand_Register):
      registers[destIndex - 1] = registers[src.Register.Index - 1];
      break;
  }
  print_instruction(Op_mov, &dest.Register, &src.Register);
}


void exec_add(instruction instr){
  //TODO: add a print of the current instruction
  // as far as I understood, for the add instruction the flags are not relevant, so we'll just add them
  instruction_operand dest = instr.Operands[0];
  instruction_operand src = instr.Operands[1];
  // get the address of the dest register we want to write into
  u32 destIndex = dest.Register.Index;

  switch(src.Type){
    case(Operand_None):
      break;
    case(Operand_Immediate):
      // just write the value the dest index
      registers[destIndex - 1] += src.Immediate.Value;
      break;
    case(Operand_Register):
      registers[destIndex - 1] += registers[src.Register.Index - 1];
      break;
  }
  print_instruction(Op_add, &dest.Register, &src.Register);
}

void exec_sub(instruction instr){

  instruction_operand dest = instr.Operands[0];
  instruction_operand src = instr.Operands[1];
  // get the address of the dest register we want to write into
  u32 destIndex = dest.Register.Index;

  switch(src.Type){
    case(Operand_None):
      break;
    case(Operand_Immediate):
      // just write the value the dest index
      registers[destIndex - 1] -= src.Immediate.Value;
      break;
    case(Operand_Register):
      registers[destIndex - 1] -= registers[src.Register.Index - 1];
      break;
  }
  print_instruction(Op_sub, &dest.Register, &src.Register);
}


void exec_cmp(instruction instr){
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
}

int main() {
  
  // first we need to read in the bytes from "listing_0044_register_movs"
  FILE* file = fopen("listing_0046_add_sub_cmp", "rb");
  if(!file) return 1;
  fseek(file, 0, SEEK_END); //see where the end of the file is
  long fileSize = ftell(file);
  rewind(file); //go back to beginning of file
  u8* buffer = (u8*) malloc(fileSize); //allocate fileSize many bytes on heap
  fread(buffer, 1, fileSize, file); //1 is size in bytes of each element
  u16 offset = 0; // the current position/offset
  u16 remaining = fileSize;// we need remaining bytes from current position to end of file
  instruction instr; // an instruction var that caseys function will fill

  while(remaining > 0){
    remaining = fileSize - offset;
    Sim86_Decode8086Instruction(remaining, buffer+offset, &instr);
    offset += instr.Size;
    //get the type of operation the current instruction is
    switch(instr.Op){
      case(Op_None):
        break; //if no operand do nothing!
      case(Op_mov):
        exec_mov(instr);
      break;
      case(Op_add):
        exec_add(instr);
      break;
      case(Op_sub):
        exec_sub(instr);
      break;
      case(Op_cmp):
        exec_cmp(instr);
      break;
    }
  }
  
  print_reg_contents();
  free(buffer);
}
