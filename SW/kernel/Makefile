# ==============================
# Bare-metal RISC-V Makefile
# ==============================

ARCH = riscv32-unknown-elf
CC   = $(ARCH)-gcc
OBJDUMP = $(ARCH)-objdump
OBJCOPY = $(ARCH)-objcopy
GDB  = $(ARCH)-gdb

CFLAGS  = -g -w -ffreestanding -fno-pic -fno-builtin \
          -march=rv32i -mabi=ilp32 -nostdlib
ASFLAGS = $(CFLAGS)

LDSCRIPT = kernel.ld

# ------------------------------
# Kernel source files
# ------------------------------
SRCS_C   = main.c paging.c utils.c proc.c timer.c scheduler.c trap.c start.c kernel_vm.c uart.c plic.c
SRCS_ASM = entry.S swtch.S  timervec.S        # trampoline removed from kernel

OBJS_C   = $(SRCS_C:.c=.o)
OBJS_ASM = $(SRCS_ASM:.S=.o)
OBJS     = $(OBJS_ASM) $(OBJS_C)

PROG = kernel

# ------------------------------
# Trampoline (standalone)
# ------------------------------
TRAMPOLINE_SRC = trampoline.S
TRAMPOLINE_OBJ = trampoline.o
TRAMPOLINE_BIN = trampoline.bin

# ------------------------------
# User program
# ------------------------------
USER_BIN  = /home/abheerav/RISCV-CPU/SW/user/parent.bin
USER_ADDR = 0x80005000

QEMU = qemu-system-riscv32

# ------------------------------
# QEMU flags
# ------------------------------
QEMU_FLAGS = \
	-machine virt \
	-nographic \
	-bios none \
	-kernel $(PROG) \
	-device loader,file=$(TRAMPOLINE_BIN),addr=0x80004000 \
	-device loader,file=$(USER_BIN),addr=$(USER_ADDR) \
	-d int,mmu,guest_errors -D qemu.log

# ==============================
# Rules
# ==============================

all: $(PROG) $(TRAMPOLINE_BIN)

# ---- Compile rules ----
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(ASFLAGS) -c $< -o $@

# ---- Kernel link (then cleanup .o) ----
$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -T $(LDSCRIPT) -o $@ $(OBJS) -lgcc
	rm -f $(OBJS)

# ---- Trampoline build (raw binary) ----
$(TRAMPOLINE_BIN): $(TRAMPOLINE_SRC)
	$(CC) $(ASFLAGS) -c $< -o $(TRAMPOLINE_OBJ)
	$(OBJCOPY) -O binary $(TRAMPOLINE_OBJ) $@
	rm -f $(TRAMPOLINE_OBJ)

# ---- Utilities ----
disasm: $(PROG)
	$(OBJDUMP) -D -M numeric,no-aliases $(PROG) > $(PROG).lst

run: all
	$(QEMU) $(QEMU_FLAGS)

debug: all
	@echo "Generating kernel objdump..."
	$(OBJDUMP) -D -M numeric,no-aliases $(PROG) > $(PROG).dump
	@echo "Objdump saved to $(PROG).dump"
	$(QEMU) $(QEMU_FLAGS) -S -gdb tcp::1234

clean:
	rm -f *.o $(PROG) *.lst $(PROG).dump $(TRAMPOLINE_BIN)

.PHONY: all disasm run debug clean
