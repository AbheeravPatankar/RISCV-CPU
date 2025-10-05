#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Execute DMEM scripts
python3 /home/abheeravpatankar/RISCV-CPU/SW/DMEM_MACRO_GEN/dmem_word_generator.py
python3 /home/abheeravpatankar/RISCV-CPU/SW/DMEM_MACRO_GEN/dmem_macro_generator.py

# Execute IMEM scripts
python3 /home/abheeravpatankar/RISCV-CPU/SW/IMEM_MACRO_GEN/bin_extractor.py
python3 /home/abheeravpatankar/RISCV-CPU/SW/IMEM_MACRO_GEN/generate_macro.py

echo "All scripts executed successfully!"
