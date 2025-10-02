###Command to build Kernel

alias buildriscv='
riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -g -c RV32_test.c -o RV32_test.o && \
riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -c entry.S -o entry.o && \
riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -nostartfiles -nostdlib entry.o RV32_test.o -T kernel.ld -lgcc -o kernel.elf && \
riscv64-unknown-elf-objdump -d -j .text kernel.elf |& tee /home/abheeravpatankar/RISCV-CPU/SW/hexdump_in.txt
'
