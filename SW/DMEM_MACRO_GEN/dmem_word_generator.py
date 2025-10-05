#!/usr/bin/env python3
import re

input_file = "/home/abheeravpatankar/RISCV-CPU/SW/DMEM_MACRO_GEN/hexdump_in_dmem.txt"
output_file = "/home/abheeravpatankar/RISCV-CPU/SW/DMEM_MACRO_GEN/dmem_words.txt"

# regex: match lines like "   0: 0a 00 00 00 ...."
line_re = re.compile(r'^\s*([0-9a-fA-F]+):\s+((?:[0-9a-fA-F]{2}\s+)+)')

dmem = {}

with open(input_file, "r") as f:
    for line in f:
        m = line_re.match(line)
        if m:
            addr_str, bytes_str = m.groups()
            addr = int(addr_str, 16)
            bytes_list = [int(b, 16) for b in bytes_str.strip().split()]

            # group into 32-bit words (little endian)
            for i in range(0, len(bytes_list), 4):
                word_bytes = bytes_list[i:i+4]
                if len(word_bytes) < 4:
                    # pad if incomplete word (rare)
                    word_bytes += [0] * (4 - len(word_bytes))
                word = word_bytes[0] | (word_bytes[1] << 8) | (word_bytes[2] << 16) | (word_bytes[3] << 24)
                dmem[addr + i] = word

# write output file
with open(output_file, "w") as f:
    for addr in sorted(dmem.keys()):
        f.write(f"0x{addr:08x} : 0x{dmem[addr]:08x}\n")

print(f"DMEM dump written to {output_file}")
