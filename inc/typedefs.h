#ifndef TAU_TYPEDEFS_H
#define TAU_TYPEDEFS_H

/**
 * \brief Represents a symbol.
 * 
 * \details A symbol is an AST node with an identifier.
 * (e.g. module, struct, variable)
 */
typedef struct symbol_s symbol_t;

/**
 * \brief Represents a symbol table.
 * 
 * \details A symbol table is a hierarchical collection of symbols
 */
typedef struct symtable_s symtable_t;

/** Represents an analyzer. */
typedef struct analyzer_s analyzer_t;

/** Represents a register in the virtual machine. */
typedef enum register_e
{
  REG_A,   // 64-bit general purpose register A
  REG_AHD, // 32-bit general purpose register A high double word
  REG_ALD, // 32-bit general purpose register A low double word
  REG_AHW, // 16-bit general purpose register A high word
  REG_ALW, // 16-bit general purpose register A low word
  REG_AHB, // 8-bit general purpose register A high byte
  REG_ALB, // 8-bit general purpose register A low byte

  REG_B,   // 64-bit general purpose register B
  REG_BHD, // 32-bit general purpose register B high double word
  REG_BLD, // 32-bit general purpose register B low double word
  REG_BHW, // 16-bit general purpose register B high word
  REG_BLW, // 16-bit general purpose register B low word
  REG_BHB, // 8-bit general purpose register B high byte
  REG_BLB, // 8-bit general purpose register B low byte

  REG_C,   // 64-bit general purpose register C
  REG_CHD, // 32-bit general purpose register C high double word
  REG_CLD, // 32-bit general purpose register C low double word
  REG_CHW, // 16-bit general purpose register C high word
  REG_CLW, // 16-bit general purpose register C low word
  REG_CHB, // 8-bit general purpose register C high byte
  REG_CLB, // 8-bit general purpose register C low byte

  REG_D,   // 64-bit general purpose register D
  REG_DHD, // 32-bit general purpose register D high double word
  REG_DLD, // 32-bit general purpose register D low double word
  REG_DHW, // 16-bit general purpose register D high word
  REG_DLW, // 16-bit general purpose register D low word
  REG_DHB, // 8-bit general purpose register D high byte
  REG_DLB, // 8-bit general purpose register D low byte

  REG_E,   // 64-bit general purpose register E
  REG_EHD, // 32-bit general purpose register E high double word
  REG_ELD, // 32-bit general purpose register E low double word
  REG_EHW, // 16-bit general purpose register E high word
  REG_ELW, // 16-bit general purpose register E low word
  REG_EHB, // 8-bit general purpose register E high byte
  REG_ELB, // 8-bit general purpose register E low byte

  REG_F,   // 64-bit general purpose register F
  REG_FHD, // 32-bit general purpose register F high double word
  REG_FLD, // 32-bit general purpose register F low double word
  REG_FHW, // 16-bit general purpose register F high word
  REG_FLW, // 16-bit general purpose register F low word
  REG_FHB, // 8-bit general purpose register F high byte
  REG_FLB, // 8-bit general purpose register F low byte

  REG_SP,  // 64-bit stack pointer register
  REG_BP,  // 64-bit base pointer register
  REG_IP,  // 64-bit instruction pointer register

  REG_FLAGS, // 8-bit status flags register
} register_t;

/** Represents an operation code for the virtual machine. */
typedef enum opcode_e
{
  OPCODE_NOP,  // NOP

  OPCODE_MOV,  // MOV <reg|mem>, <reg|mem|imm>
  OPCODE_PSH,  // PSH <reg|mem|imm>
  OPCODE_POP,  // POP <reg|mem>
  OPCODE_LEA,  // LEA <reg>, <mem>

  OPCODE_ADD, // ADD <reg|mem>, <reg|mem|imm>
  OPCODE_SUB, // SUB <reg|mem>, <reg|mem|imm>
  OPCODE_MUL, // MUL <reg|mem>, <reg|mem|imm>
  OPCODE_DIV, // DIV <reg|mem>, <reg|mem|imm>
  OPCODE_MOD, // MOD <reg|mem>, <reg|mem|imm>
  OPCODE_INC, // INC <reg|mem>
  OPCODE_DEC, // DEC <reg|mem>

  OPCODE_IADD, // IADD <reg|mem>, <reg|mem|imm>
  OPCODE_ISUB, // ISUB <reg|mem>, <reg|mem|imm>
  OPCODE_IMUL, // IMUL <reg|mem>, <reg|mem|imm>
  OPCODE_IDIV, // IDIV <reg|mem>, <reg|mem|imm>
  OPCODE_IMOD, // IMOD <reg|mem>, <reg|mem|imm>
  OPCODE_IINC, // IINC <reg|mem>
  OPCODE_IDEC, // IDEC <reg|mem>
  OPCODE_INEG, // INEG <reg|mem>

  OPCODE_FADD, // FADD <reg|mem>, <reg|mem|imm>
  OPCODE_FSUB, // FSUB <reg|mem>, <reg|mem|imm>
  OPCODE_FMUL, // FMUL <reg|mem>, <reg|mem|imm>
  OPCODE_FDIV, // FDIV <reg|mem>, <reg|mem|imm>
  OPCODE_FMOD, // FMOD <reg|mem>, <reg|mem|imm>
  OPCODE_FNEG, // FNEG <reg|mem>

  OPCODE_AND,  // AND <reg|mem>, <reg|mem|imm>
  OPCODE_OR,   // OR <reg|mem>, <reg|mem|imm>
  OPCODE_XOR,  // XOR <reg|mem>, <reg|mem|imm>
  OPCODE_NOT,  // NOT <reg|mem>
  OPCODE_SHL,  // SHL <reg|mem>, <reg|mem|imm>
  OPCODE_SHR,  // SHL <reg|mem>, <reg|mem|imm>

  OPCODE_JMP,  // JMP <label>

  OPCODE_JE,   // JE <label>
  OPCODE_JNE,  // JNE <label>
  OPCODE_JL,   // JL <label>
  OPCODE_JLE,  // JLE <label>
  OPCODE_JG,   // JG <label>
  OPCODE_JGE,  // JGE <label>
  OPCODE_JZ,   // JZ <label>
  OPCODE_JNZ,  // JNZ <label>
  OPCODE_JN,   // JN <label>
  OPCODE_JNN,  // JNN <label>
  OPCODE_JO,   // JO <label>
  OPCODE_JNO,  // JNO <label>
  OPCODE_JC,   // JC <label>
  OPCODE_JNC,  // JNC <label>
  OPCODE_JP,   // JP <label>
  OPCODE_JNP,  // JNP <label>

  OPCODE_CMP,  // CMP <reg|mem>, <reg|mem|imm>
  OPCODE_ICMP,  // CMP <reg|mem>, <reg|mem|imm>

  OPCODE_CLF,  // CLF

  OPCODE_CALL, // CALL <label>
  OPCODE_RET,  // RET

  OPCODE_HLT,  // HLT
} opcode_t;

/** Operation code parameter types. */
typedef enum opcode_param_e
{
  OPCODE_PARAM_NONE,    // OP
  OPCODE_PARAM_REG,     // OP <reg>
  OPCODE_PARAM_MEM,     // OP <mem>
  OPCODE_PARAM_IMM,     // OP <imm>
  OPCODE_PARAM_LABEL,   // OP <label>
  OPCODE_PARAM_REG_REG, // OP <reg>, <reg>
  OPCODE_PARAM_MEM_REG, // OP <mem>, <reg>
  OPCODE_PARAM_REG_MEM, // OP <reg>, <mem>
  OPCODE_PARAM_MEM_MEM, // OP <mem>, <mem>
  OPCODE_PARAM_REG_IMM, // OP <reg>, <imm>
  OPCODE_PARAM_MEM_IMM, // OP <mem>, <imm>
} opcode_param_t;

/** Operation code parameter widths. */
typedef enum opcode_width_e
{
  OPCODE_WIDTH_NONE,
  OPCODE_WIDTH_8BIT,
  OPCODE_WIDTH_16BIT,
  OPCODE_WIDTH_32BIT,
  OPCODE_WIDTH_64BIT,
} opcode_width_t;

/** Memory addressing mode. */
typedef enum addr_mode_e
{
  ADDR_MODE_OFFSET,                  // <offset>
  ADDR_MODE_BASE,                    // <base>
  ADDR_MODE_BASE_OFFSET,             // <base> + <offset>
  ADDR_MODE_BASE_INDEX,              // <base> + <index>
  ADDR_MODE_BASE_INDEX_OFFSET,       // <base> + <index> + <offset>
  ADDR_MODE_BASE_INDEX_SCALE,        // <base> + <index> * <scale>
  ADDR_MODE_INDEX_SCALE_OFFSET,      // <index> * <scale> + <offset>
  ADDR_MODE_BASE_INDEX_SCALE_OFFSET, // <base> + <index> * <scale> + <offset>
} addr_mode_t;

/** Represents a virtual machine instance. */
typedef struct vm_s vm_t;

#endif