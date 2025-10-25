# ==============================
# Bare-metal RISC-V Makefile
# ==============================

ARCH = riscv32-unknown-elf
CC   = $(ARCH)-gcc
OBJDUMP = $(ARCH)-objdump
GDB  = $(ARCH)-gdb

CFLAGS = -g -Wall -Werror -ffreestanding -fno-pic -fno-builtin -march=rv32i -mabi=ilp32 -nostdlib
ASFLAGS = $(CFLAGS)

LDSCRIPT = kernel.ld

# Add all your C source files here
SRCS_C   = main.c paging.c utils.c proc.c
SRCS_ASM = entry.S trampoline.S

OBJS_C   = $(SRCS_C:.c=.o)
OBJS_ASM = $(SRCS_ASM:.S=.o)
OBJS     = $(OBJS_ASM) $(OBJS_C)

PROG     = kernel

QEMU = qemu-system-riscv32
QEMU_FLAGS = -machine virt -nographic -bios none -kernel $(PROG)

# ==============================
# Rules
# ==============================

all: $(PROG)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(ASFLAGS) -c $< -o $@

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -T $(LDSCRIPT) -o $@ $(OBJS)

disasm: $(PROG)
	$(OBJDUMP) -D -M numeric,no-aliases $(PROG) > $(PROG).lst

run: $(PROG)
	$(QEMU) $(QEMU_FLAGS)

debug: $(PROG)
	$(QEMU) $(QEMU_FLAGS) -S -gdb tcp::1234

clean:
	rm -f *.o $(PROG) *.lst

.PHONY: all disasm run debug clean
