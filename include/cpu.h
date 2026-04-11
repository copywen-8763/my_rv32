#ifndef __CPU_H__
#define __CPU_H__

#include<stdint.h> //for

typedef uint32_t word_t; //word : 32 bits = 4 bytes

typedef struct {
  word_t gpr[32]; //因為是32byte
  word_t pc;
} CPU_state;


#endif