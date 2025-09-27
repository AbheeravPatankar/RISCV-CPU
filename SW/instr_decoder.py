def decode_riscv_full(bin_str: str):
    """Decode a 32-bit RISC-V instruction into its fields and identify instruction type."""
    if len(bin_str) != 32 or not set(bin_str) <= {"0", "1"}:
        raise ValueError("Input must be a 32-bit binary string")

    instr = int(bin_str, 2)

    # Extract basic fields
    opcode = instr & 0x7F
    rd = (instr >> 7) & 0x1F
    funct3 = (instr >> 12) & 0x7
    rs1 = (instr >> 15) & 0x1F
    rs2 = (instr >> 20) & 0x1F
    funct7 = (instr >> 25) & 0x7F
    instr30 = (instr >> 30) & 0x1  # bit 30 for R/I shift instructions

    # Determine instruction type (U, J, I, S, B, R)
    opcode_5b = (opcode >> 2) & 0x1F
    if opcode_5b in [0b00101, 0b01101]:
        instr_type = "U"
    elif opcode_5b in [0b01100, 0b01011, 0b01110, 0b10100]:
        instr_type = "R"
    elif opcode_5b in [0b00000, 0b00001, 0b00100, 0b00110, 0b11001]:
        instr_type = "I"
    elif opcode_5b in [0b01000, 0b01001]:
        instr_type = "S"
    elif opcode_5b == 0b11000:
        instr_type = "B"
    elif opcode_5b == 0b11011:
        instr_type = "J"
    else:
        instr_type = "Unknown"

    # Immediate extraction
    imm = 0
    if instr_type == "I":
        imm = (instr >> 20) & 0xFFF
        if imm & 0x800: imm |= -1 << 12
    elif instr_type == "S":
        imm = ((instr >> 7) & 0x1F) | (((instr >> 25) & 0x7F) << 5)
        if imm & 0x800: imm |= -1 << 12
    elif instr_type == "B":
        imm = (((instr >> 8) & 0xF) << 1) | (((instr >> 25) & 0x3F) << 5) | (((instr >> 7) & 0x1) << 11) | (((instr >> 31) & 0x1) << 12)
        if imm & 0x1000: imm |= -1 << 13
    elif instr_type == "U":
        imm = instr & 0xFFFFF000
    elif instr_type == "J":
        imm = (((instr >> 21) & 0x3FF) << 1) | (((instr >> 20) & 0x1) << 11) | (((instr >> 12) & 0xFF) << 12) | (((instr >> 31) & 0x1) << 20)
        if imm & 0x100000: imm |= -1 << 21

    # Instruction identification logic
    is_lui   = opcode == 0b0110111
    is_auipc = opcode == 0b0010111
    is_jal   = opcode == 0b1101111
    is_jalr  = funct3 == 0b000 and opcode == 0b1100111

    is_beq  = funct3 == 0b000 and opcode == 0b1100011
    is_bne  = funct3 == 0b001 and opcode == 0b1100011
    is_blt  = funct3 == 0b100 and opcode == 0b1100011
    is_bge  = funct3 == 0b101 and opcode == 0b1100011
    is_bltu = funct3 == 0b110 and opcode == 0b1100011
    is_bgeu = funct3 == 0b111 and opcode == 0b1100011

    is_addi  = funct3 == 0b000 and opcode == 0b0010011
    is_slti  = funct3 == 0b010 and opcode == 0b0010011
    is_sltiu = funct3 == 0b011 and opcode == 0b0010011
    is_xori  = funct3 == 0b100 and opcode == 0b0010011
    is_ori   = funct3 == 0b110 and opcode == 0b0010011
    is_andi  = funct3 == 0b111 and opcode == 0b0010011

    is_slli  = funct3 == 0b001 and opcode == 0b0010011 and instr30 == 0
    is_srli  = funct3 == 0b101 and opcode == 0b0010011 and instr30 == 0
    is_srai  = funct3 == 0b101 and opcode == 0b0010011 and instr30 == 1

    is_add = funct3 == 0b000 and opcode == 0b0110011 and instr30 == 0
    is_sub = funct3 == 0b000 and opcode == 0b0110011 and instr30 == 1
    is_sll = funct3 == 0b001 and opcode == 0b0110011 and instr30 == 0
    is_slt = funct3 == 0b010 and opcode == 0b0110011 and instr30 == 0
    is_sltu = funct3 == 0b011 and opcode == 0b0110011 and instr30 == 0
    is_xor = funct3 == 0b100 and opcode == 0b0110011 and instr30 == 0
    is_srl = funct3 == 0b101 and opcode == 0b0110011 and instr30 == 0
    is_sra = funct3 == 0b101 and opcode == 0b0110011 and instr30 == 1
    is_or  = funct3 == 0b110 and opcode == 0b0110011 and instr30 == 0
    is_and = funct3 == 0b111 and opcode == 0b0110011 and instr30 == 0

    is_lb  = funct3 == 0b000 and opcode == 0b0000011
    is_lh  = funct3 == 0b001 and opcode == 0b0000011
    is_lw  = funct3 == 0b010 and opcode == 0b0000011
    is_lbu = funct3 == 0b100 and opcode == 0b0000011
    is_lhu = funct3 == 0b101 and opcode == 0b0000011

    is_sb  = funct3 == 0b000 and opcode == 0b0100011
    is_sh  = funct3 == 0b001 and opcode == 0b0100011
    is_sw  = funct3 == 0b010 and opcode == 0b0100011

    is_load  = is_lb or is_lh or is_lw or is_lbu or is_lhu
    is_store = is_sb or is_sh or is_sw

    # Aggregate detected instruction
    detected_instrs = []
    for name, flag in locals().items():
        if name.startswith("is_") and flag:
            detected_instrs.append(name)

    # Print components and instruction
    print(f"Instruction Type: {instr_type}")
    print(f"Opcode: {opcode:07b} ({opcode})")
    print(f"rd: {rd:05b} ({rd})")
    print(f"funct3: {funct3:03b} ({funct3})")
    print(f"rs1: {rs1:05b} ({rs1})")
    print(f"rs2: {rs2:05b} ({rs2})")
    print(f"funct7: {funct7:07b} ({funct7})")
    print(f"imm: {imm & 0xFFFFFFFF:032b} ({imm})")
    print(f"Detected Instructions: {detected_instrs}")

def decode_riscv_from_hex(hex_str: str):
    """Convert a hex instruction to 32-bit binary and decode."""
    # Remove '0x' if present
    hex_str = hex_str.lower().replace("0x", "")
    # Convert to integer
    instr_int = int(hex_str, 16)
    # Convert to 32-bit binary string
    bin_str = format(instr_int, "032b")
    # Decode
    decode_riscv_full(bin_str)

# Example usage
#decode_riscv_full("00000000100000010010110000100011")
decode_riscv_from_hex("fef407a3")
