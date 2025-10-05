#!/usr/bin/env python3

input_file = "/home/abheeravpatankar/RISCV-CPU/SW/DMEM_MACRO_GEN/dmem_words.txt"
output_file = "/home/abheeravpatankar/RISCV-CPU/SW/DMEM_MACRO_GEN/dmem_macro.v"

dmem_values = []

# Read the dmem words file
with open(input_file, "r") as f:
    for line in f:
        line = line.strip()
        if not line or ':' not in line:
            continue
        # Split "0xADDR : 0xDATA"
        addr_str, data_str = line.split(':')
        addr_str = addr_str.strip()
        data_str = data_str.strip()
        
        # Convert data to integer and store
        data = int(data_str, 16)
        dmem_values.append(data)

# Write the Verilog macro
with open(output_file, "w") as f:
    f.write("initial begin\n")
    for idx, word in enumerate(dmem_values):
        f.write(f"    /dmem[{idx}]$value = 32'h{word:08X};\n")
    f.write("end\n")

print(f"Verilog macro written to {output_file}")
