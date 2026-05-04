#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "memory.h"


/*
在 host (PC) 分配一個 8 bits  * 128 MB 的連續空間
選擇 8bits/1byte 原因是記憶體內最小單位是 1byte (記憶 : 資料型態的char = 1 byte)
isa 指令長度為 32bits，也就是32 bits/8 bits (1byte) = 4bytes ，這就是為何 pc通常是 +4
*/
static uint8_t pmem[MEM_SIZE];

/*
在 C 語言中，pmem 這個名稱會被當作 pmem[0] 的地址，
所以 guest_to_host 就是將 emulator ( guest ) 的內存空間在 host 空間實際的位置計算出來
並透過資料量大小的指針的形式指向該地址，透過指定指針為uint8_t，可以使指針的步幅為 8 bits，保持指向資料的頭部
假設 host 提供的陣列地址開頭為 A ，addr 是 guest 內的某個地址表示為 MEM_BASE+offset ; 輸出為 A + (MEM_BASE + offset) - MEM_BASE = A + offset
*/
uint8_t* guest_to_host(uint32_t addr){
  return pmem + (addr - MEM_BASE);
}

/*
paddr_read 是輸入 guest 的地址，透過 guest_to_host 來得到 host 地址，透過 << i*8 的方式將資料推入 ret 內，ret 就是該地址下的資料內容
假設地址下的資料為 0x12345678 ，存取時以 1byte 為單位 (兩位 0x )，
先取 0x78，此時 i=0，0x78<<0，與 ret 進行 or 運算，得到 0x00000078，
接下來存取 0x56，此時 i=1，0x78<<8，也就是左移 8 為 b (兩位 0x )
，與 ret 進行 or 運算得到 0x00005678，....，最後得到 0x12345678
Little-Endian read，也就是低位字節存於低地址，78 為低位 byte，儲存於低地址 p[0]
*/
uint32_t paddr_read(uint32_t addr, int len){
  uint8_t *p = guest_to_host(addr);
  uint32_t ret = 0;
  for (int i = 0; i < len; i++){
    ret |= (uint32_t)p[i] << (i * 8);
  }
  return ret;
}


void init_mem(){
  uint32_t *p = (uint32_t *)guest_to_host(MEM_BASE);
// 1. addi x1, 10 (addi x1, x0, 10) -> x1 = 10
p[0] = 0x00a00093;
// 2. addi x2, x1, -7 -> x2 = x1 + (-7) = 3
p[1] = 0xff908113;
// 3. ebreak
p[2] = 0x00100073;
  printf("Memory initialized at 0x%08x with ADDI test image.\n", MEM_BASE);
}