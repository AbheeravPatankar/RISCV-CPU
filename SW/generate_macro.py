input_file = "/home/abheeravpatankar/RISCV-CPU/SW/hexdump_bin.txt"
output_file = "/home/abheeravpatankar/RISCV-CPU/SW/hexdump_out.txt"

with open(input_file) as f:
    bin_instrs = [line.strip() for line in f if line.strip()]

# Convert all binaries to 32-bit literal format
tlv_instrs = [f"32'b{b}" for b in bin_instrs]

# Prepare the macro as a SystemVerilog array initialization
lines = []
lines.append(f"`define READONLY_MEM(ADDR, DATA) \\\n" )
lines.append(f"  logic [31:0] instrs [0:{len(tlv_instrs)-1}]; \\\n")
lines.append(f"  initial begin \\\n")
for i, instr in enumerate(tlv_instrs):
    lines.append(f"    instrs[{i}] = {instr}; \\\n")
lines.append(f"  end \\\n")
lines.append(f"  assign DATA = instrs[ADDR >> 2];")

with open(output_file, "w") as f:
    f.writelines(lines)

print("TLV macro generated in hexdump_out.txt")
