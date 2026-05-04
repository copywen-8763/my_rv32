//透過##來達到左右內容合併的部分 如果直接concat(x,y) x ## y，編譯器會直接黏"x"和"y"，而不是將其當作變數展開
#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)
#define concat3(x, y, z) concat(concat(x, y), z)
#define concat4(x, y, z, w) concat3(concat(x, y), z, w)
#define concat5(x, y, z, v, w) concat4(concat(x, y), z, v, w)

// 匹配區塊的開頭與結尾
#define INSTPAT_START(name) { const void * __instpat_end = &&concat(__instpat_end_, name);
#define INSTPAT_END(name) concat(__instpat_end_, name): ; }

// INSTPAT 核心巨集
#define INSTPAT(pattern, ...) do { \
  uint64_t key, mask, shift; \
  pattern_decode(pattern, STRLEN(pattern), &key, &mask, &shift); \
  if ((((uint64_t)INSTPAT_INST(s) >> shift) & mask) == key) { \
    INSTPAT_MATCH(s, ##__VA_ARGS__); \
    goto *(__instpat_end); \
  } \
} while (0)


// 獲取當前需要解碼的指令值
#define INSTPAT_INST(s) ((s)->isa.inst.val)

// 當指令匹配成功時，執行的動作
#define INSTPAT_MATCH(s, name, type, ... /* 執行主體 */ ) { \
  int rd = 0; \
  word_t src1 = 0, src2 = 0, imm = 0; \
  /* decode_operand 會根據 type (例如 I, R, S 型) 自動切分並賦值給上面的變數 */ \
  decode_operand(s, &rd, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__; \
}