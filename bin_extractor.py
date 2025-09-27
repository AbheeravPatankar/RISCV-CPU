# File paths
input_file = "/home/abheeravpatankar/RISCV-CPU/hexdump_in.txt"
output_file = "/home/abheeravpatankar/RISCV-CPU/hexdump_bin.txt"

# Open input and output files
with open(input_file, "r") as infile, open(output_file, "w") as outfile:
    for line in infile:
        # Split line into parts
        parts = line.split()

        # Check if the line has a hex instruction (usually the second column)
        if len(parts) >= 2:
            hex_instr = parts[1]

            # Validate that it's 8-character hex (32-bit)
            if len(hex_instr) == 8:
                # Print parsed hex instruction
                print(hex_instr)

                # Convert hex to 32-bit binary string
                bin_instr = format(int(hex_instr, 16), "032b")
                outfile.write(bin_instr + "\n")
