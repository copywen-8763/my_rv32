#ifndef __CPU_H__
#define __CPU_H__

#include<stdint.h> //for uint32_t, int32_t

enum { NEMU_RUNNING, NEMU_STOP, NEMU_END };

//要記得一些數字 word : 32 bits = 4 bytes ， 256 = 2^8 也就是 i<<8，代表兩位0x
typedef uint32_t word_t; 

//gpr 和 pc 指向的是地址，所以是 uint32_t
typedef struct{
  word_t gpr[32]; //因為打算製作rv32，general purpose register
  word_t pc;      //program counter
  int state;      
} CPU_state;




//將 32 bits x 取出 hi ~ lo 的內容，其中右側為遮罩，左側為對齊
#define BITS(x, hi, lo) (((x) >> (lo)) & ((1U << ((hi) - (lo) + 1)) - 1))


/*
opcode、funct3、rs1、rs2、funct7 在每個 type 內皆為固定位置
但imm需要分 type 處理
*/
#define OPCODE(i) BITS(i, 6, 0) 
#define RD(i)     BITS(i, 11, 7)
#define FUNC3(i)  BITS(i, 14, 12)
#define RS1(i)    BITS(i, 19, 15)
#define RS2(i)    BITS(i, 24, 20)
#define FUNC7(i)  BITS(i, 31, 25)
#define IMM_I(i)  BITS(i, 31, 20)

//signed extension 因為 cpu 運行需要用到邏輯右移以及數值右移
//針對 gcc 這個編譯器，unsigned 數值進行右移會填充 0，而 signed 則會依照最高為進行填充
static inline int32_t sext(uint32_t x, int len){
  int32_t res = (int32_t)x;//先強制轉型
  return (res << (32 - len)) >> (32 - len);//將要處理的數段左移到底使高位元overflow
  //再透過上述提到的填充邏輯填充達到數值右移
}

extern CPU_state cpu;
#endif