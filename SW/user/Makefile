# Tools
CC = riscv64-unknown-elf-gcc
OBJCOPY = riscv64-unknown-elf-objcopy
OBJDUMP = riscv64-unknown-elf-objdump
PYTHON = python3

# Compiler flags
CFLAGS = -march=rv32i -mabi=ilp32 -g
LDFLAGS = -march=rv32i -mabi=ilp32 -nostartfiles -nostdlib -lgcc

# Source files
USER_SRC = parent.c heap.c
ASM_SRC  = syscall.S
LINKER_SCRIPT = user.ld
ELF_GENERATOR = elf_generator.py

# Object files
USER_OBJ = $(USER_SRC:.c=.o)
ASM_OBJ  = syscall.o
OBJS = $(USER_OBJ) $(ASM_OBJ)

# Intermediate files
PROG_ELF = prog.elf
PROG_BIN = prog.bin
HEADER_C = header.c
HEADER_OBJ = header.o
HEADER_BIN = header.bin
ELF_INFO = elf_info.txt
DUMP_FILE = prog.dump

# Final output
FINAL_BIN = final.bin

# Sections to extract
SECTIONS = .text .rodata .data .bss .sdata .sbss .srodata

# Output binary named after first source file
OUTPUT_BIN = $(basename $(firstword $(USER_SRC))).bin

# Default target
.PHONY: all
all: $(FINAL_BIN) $(DUMP_FILE)
	@echo "================================"
	@echo "Build complete: $(FINAL_BIN)"
	@echo "Objdump created: $(DUMP_FILE)"
	@echo "================================"
	@hexdump -C $(FINAL_BIN) | head -30
	@echo ""
	@echo "Cleaning intermediate files..."
	@rm -f $(OBJS) $(PROG_ELF) $(PROG_BIN)
	@rm -f $(HEADER_OBJ) $(HEADER_BIN)
	@rm -f $(ELF_INFO)
	@mv $(FINAL_BIN) $(OUTPUT_BIN)
	@echo "Final binary: $(OUTPUT_BIN)"
	@echo "Kept files: $(HEADER_C)"
	@echo "================================"



# Debug target - same as all but keeps ELF and program binary
.PHONY: debug
debug: $(FINAL_BIN) $(DUMP_FILE)
	@echo "================================"
	@echo "Debug build complete: $(FINAL_BIN)"
	@echo "Objdump created: $(DUMP_FILE)"
	@echo "================================"
	@hexdump -C $(FINAL_BIN) | head -30
	@echo ""
	@echo "Cleaning intermediate files (keeping ELF)..."
	@rm -f $(OBJS)
	@rm -f $(HEADER_OBJ) $(HEADER_BIN)
	@rm -f $(ELF_INFO)
	@mv $(FINAL_BIN) $(OUTPUT_BIN)
	@echo "Final binary: $(OUTPUT_BIN)"
	@echo "Kept files: $(HEADER_C) $(PROG_ELF) $(PROG_BIN)"
	@echo "================================"


# Pattern rule for compiling C files
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Build user program ELF
$(PROG_ELF): $(USER_OBJ) $(ASM_OBJ) $(LINKER_SCRIPT)
	@echo "Compiling syscall assembly..."
	$(CC) $(CFLAGS) -c $(ASM_SRC) -o $(ASM_OBJ)

	@echo "Linking ELF..."
	$(CC) $(LDFLAGS) $(OBJS) -T $(LINKER_SCRIPT) -o $(PROG_ELF) -lgcc

	@echo "User program ELF created: $(PROG_ELF)"

# Generate header.c from ELF
$(HEADER_C): $(PROG_ELF) $(ELF_GENERATOR)
	@echo "Generating header file..."
	$(PYTHON) $(ELF_GENERATOR) $(PROG_ELF) | tee $(ELF_INFO)
	@echo "Header C file generated: $(HEADER_C)"

# Strip ELF to create program binary
$(PROG_BIN): $(PROG_ELF)
	@echo "Stripping ELF to binary..."
	$(OBJCOPY) -O binary \
		$(addprefix -j ,$(SECTIONS)) \
		$(PROG_ELF) $(PROG_BIN)
	@echo "Program binary created: $(PROG_BIN)"

# Build header binary
$(HEADER_BIN): $(HEADER_C)
	@echo "Building header binary..."
	$(CC) $(CFLAGS) -c $(HEADER_C) -o $(HEADER_OBJ)
	$(OBJCOPY) -O binary --only-section=.header $(HEADER_OBJ) $(HEADER_BIN)
	@echo "Header binary created: $(HEADER_BIN)"
	@echo "Header contents:"
	@hexdump -C $(HEADER_BIN)

# Create objdump disassembly
$(DUMP_FILE): $(PROG_ELF)
	@echo "Generating objdump..."
	$(OBJDUMP) -D $(PROG_ELF) > $(DUMP_FILE)
	@echo "Objdump created: $(DUMP_FILE)"

# Create final binary (header + program)
$(FINAL_BIN): $(HEADER_BIN) $(PROG_BIN)
	@echo "Creating final binary..."
	cat $(HEADER_BIN) $(PROG_BIN) > $(FINAL_BIN)
	@echo "Final binary created: $(FINAL_BIN)"

# Clean target
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	@rm -f $(USER_OBJ) $(ASM_OBJ) $(HEADER_OBJ)
	@rm -f $(PROG_ELF) $(PROG_BIN)
	@rm -f $(HEADER_BIN) $(FINAL_BIN)
	@rm -f $(ELF_INFO) $(DUMP_FILE)
	@rm -f $(OUTPUT_BIN)
	@echo "Clean complete."
