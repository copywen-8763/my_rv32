#ifndef __DECODE_H__
#define __DECODE_H__

#include <stdint.h>
#include "cpu.h" // 為了使用 CPU_state 和你的 BITS 巨集

//透過##來達到左右內容合併的部分 如果直接concat(x,y) x ## y，編譯器會直接黏"x"和"y"，而不是將其當作變數展開
#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)
#define concat3(x, y, z) concat(concat(x, y), z)
#define concat4(x, y, z, w) concat3(concat(x, y), z, w)
#define concat5(x, y, z, v, w) concat4(concat(x, y), z, v, w)


typedef struct {
    uint32_t pc;
    uint32_t snpc; // 下一條指令的 PC (通常是 pc + 4)
    uint32_t inst; // 取出的 32-bit 指令
} Decode;


//此處的入參s是Decode *s
//透過這個macro取出s->inst
#define INSTPAT_INST(s) ((s)->inst)

//always_inline:
/*

*/
__attribute__((always_inline))
static inline void pattern_decode(const char *str, int len,
    uint64_t *key, uint64_t *mask, uint64_t *shift) {
    uint64_t __key = 0, __mask = 0, __shift = 0;

/*
ex.任務是辨認ADDI，特徵是opcode為0b01100 11，fucnt3為0b000，這裡忽略空白字元
則其str會是"??????? ????? ????? 000 ????? 01100 11"
for macro(0):讀取第0位，key只記錄1，mask只記錄非?項，shift紀錄出現了幾次?項
key "0000000 00000 00000 000 00000 01100 11"
mask "0000000 00000 00000 111 00000 11111 11"
shift "22"
*/
#define macro(i) \
    if ((i) >= len) goto finish; \
    else { \
    char c = str[i]; \
    if (c != ' ') { \
      __key  = (__key  << 1) | (c == '1' ? 1 : 0); \
      __mask = (__mask << 1) | (c == '?' ? 0 : 1); \
      __shift = (c == '?' ? __shift + 1 : 0); \
        } \
    }

//輸入marco2(0)會得到 macro(0) macro(1)
//輸入macro4(0)會得到 marco2(0) macro2(2)，展開得到
//macro(0) macro(1) macro(2) macro(3)....
#define macro2(i)  macro(i);   macro((i) + 1)
#define macro4(i)  macro2(i);  macro2((i) + 2)
#define macro8(i)  macro4(i);  macro4((i) + 4)
#define macro16(i) macro8(i);  macro8((i) + 8)
#define macro32(i) macro16(i); macro16((i) + 16)
#define macro64(i) macro32(i); macro32((i) + 32)
    macro64(0);
//   panic("pattern too long");
#undef macro
finish:
    *key = __key >> __shift;
    *mask = __mask >> __shift;
    *shift = __shift;
}

/*

*/
#define INSTPAT_START(name) { const void * __instpat_end = &&concat(__instpat_end_, name);
#define INSTPAT_END(name)   concat(__instpat_end_, name): ; }



/*

*/
#define INSTPAT(pattern, ...) do { \
    uint64_t key, mask, shift; \
    pattern_decode(pattern, sizeof(pattern) - 1, &key, &mask, &shift); \
    if ((((uint64_t)INSTPAT_INST(s) >> shift) & mask) == key) { \
        INSTPAT_MATCH(s, ##__VA_ARGS__); \
        goto *(__instpat_end); \
    } \
} while (0)


/*

*/
#define INSTPAT_MATCH(s, name, type, ... ) { \
    int rd = 0; \
    uint32_t src1 = 0, src2 = 0; \
    int32_t imm = 0; \
    /* 呼叫狹義解碼，把提取出來的值塞進上面的變數 */ \
    decode_operand(s, &rd, &src1, &src2, &imm, concat(TYPE_, type)); \
    /* 執行真正的指令動作 */ \
    __VA_ARGS__; \
}



enum { TYPE_I, TYPE_R, TYPE_S, TYPE_B, TYPE_U, TYPE_J, TYPE_N };

/*
opcode、funct3、rs1、rs2、funct7 在每個 type 內皆為固定位置
但imm需要分 type 處理
*/
static inline void decode_operand(Decode *s, int *rd, uint32_t *src1, uint32_t *src2, int32_t *imm, int type) {
    uint32_t i = s->inst;
    int rs1 = RS1(i);
    int rs2 = RS2(i);
    *rd = RD(i);
    (void)rs2;
    (void)src2;
    // 根據 type 進行不同的處理 (利用你在 cpu.h 寫好的巨集)
    switch (type) {
        case TYPE_I:
        *src1 = cpu.gpr[rs1]; // 直接把暫存器的「值」取出來
        *imm = sext(IMM_I(i), 12);
        break;
    // 未來如果你要實作 R 型指令 (如 add)，就在這裡加上 case TYPE_R: ...
    }
}

#endif