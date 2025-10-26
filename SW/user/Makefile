# Makefile for RISC-V User Program Build
# Compiles user program, generates header, and creates final binary

# Tools
CC = riscv64-unknown-elf-gcc
OBJCOPY = riscv64-unknown-elf-objcopy
PYTHON = python3

# Compiler flags
CFLAGS = -march=rv32i -mabi=ilp32 -g
LDFLAGS = -march=rv32i -mabi=ilp32 -nostartfiles -nostdlib -lgcc

# Source files
USER_SRC = RV32_test.c
LINKER_SCRIPT = user.ld
ELF_GENERATOR = elf_generator.py

# Intermediate files
USER_OBJ = RV32_test.o
PROG_ELF = prog.elf
PROG_BIN = prog.bin
HEADER_C = header.c
HEADER_OBJ = header.o
HEADER_BIN = header.bin
ELF_INFO = elf_info.txt

# Final output
FINAL_BIN = final.bin

# Sections to extract
SECTIONS = .text .rodata .data .bss .sdata .sbss .srodata

# Output binary named after source file
OUTPUT_BIN = $(basename $(USER_SRC)).bin

# Default target
.PHONY: all
all: $(FINAL_BIN)
	@echo "================================"
	@echo "Build complete: $(FINAL_BIN)"
	@echo "================================"
	@hexdump -C $(FINAL_BIN) | head -30
	@echo ""
	@echo "Cleaning intermediate files..."
	@rm -f $(USER_OBJ) $(PROG_ELF) $(PROG_BIN)
	@rm -f $(HEADER_C) $(HEADER_OBJ) $(HEADER_BIN)
	@rm -f $(ELF_INFO)
	@mv $(FINAL_BIN) $(OUTPUT_BIN)
	@echo "Final binary: $(OUTPUT_BIN)"
	@echo "================================"

# Build user program ELF
$(PROG_ELF): $(USER_SRC) $(LINKER_SCRIPT)
	@echo "Compiling user program..."
	$(CC) $(CFLAGS) -c $(USER_SRC) -o $(USER_OBJ)
	$(CC) $(LDFLAGS) $(USER_OBJ) -T $(LINKER_SCRIPT) -o $(PROG_ELF)
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

# Create final binary (header + program)
$(FINAL_BIN): $(HEADER_BIN) $(PROG_BIN)
	@echo "Creating final binary..."
	cat $(HEADER_BIN) $(PROG_BIN) > $(FINAL_BIN)
	@echo "Final binary created: $(FINAL_BIN)"

# Show ELF information
.PHONY: info
info: $(ELF_INFO)
	@cat $(ELF_INFO)

# Show final binary hex dump
.PHONY: hexdump
hexdump: $(OUTPUT_BIN)
	hexdump -C $(OUTPUT_BIN)

# Clean all generated files
.PHONY: clean
clean:
	@echo "Cleaning generated files..."
	rm -f $(USER_OBJ) $(PROG_ELF) $(PROG_BIN)
	rm -f $(HEADER_C) $(HEADER_OBJ) $(HEADER_BIN)
	rm -f $(ELF_INFO) $(FINAL_BIN) $(OUTPUT_BIN)
	@echo "Clean complete"

# Clean everything except source files
.PHONY: distclean
distclean: clean
	@echo "Deep clean complete"

# Help target
.PHONY: help
help:
	@echo "RISC-V User Program Build System"
	@echo "================================"
	@echo "Targets:"
	@echo "  all       - Build final binary (default)"
	@echo "  info      - Show ELF analysis information"
	@echo "  hexdump   - Display hex dump of final binary"
	@echo "  clean     - Remove all generated files"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Generated files:"
	@echo "  $(PROG_ELF)   - User program ELF"
	@echo "  $(PROG_BIN)   - Stripped program binary"
	@echo "  $(HEADER_C)   - Generated header C file"
	@echo "  $(HEADER_BIN) - Header binary"
	@echo "  $(OUTPUT_BIN) - Final output binary (named after source)"

# Phony targets
.PHONY: all clean distclean info hexdump help