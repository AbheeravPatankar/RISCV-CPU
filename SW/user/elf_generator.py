#!/usr/bin/env python3

import subprocess
import sys
import re
import os


def run_command(command):
    """Run a shell command and return its output."""
    try:
        result = subprocess.run(command, shell=True, capture_output=True, text=True, check=True)
        return result.stdout
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {command}")
        print(f"Error: {e.stderr}")
        return None


def get_section_info(elf_path, section_name):
    """Extract section address and size from ELF file."""
    command = f"riscv64-unknown-elf-objdump -h {elf_path} | grep ' {section_name} '"
    output = run_command(command)

    if output:
        parts = output.split()
        if len(parts) >= 4:
            size = int(parts[2], 16)
            vma = int(parts[3], 16)
            return {'name': section_name, 'address': vma, 'size': size}

    return None


def get_main_address(elf_path):
    """Extract the address of the main function."""
    command = f"riscv64-unknown-elf-nm {elf_path} | grep ' main$'"
    output = run_command(command)

    if output:
        match = re.search(r'^([0-9a-fA-F]+)', output.strip())
        if match:
            return int(match.group(1), 16)

    return None


def align_up(value, alignment):
    """Align a value up to the nearest alignment boundary."""
    return (value + alignment - 1) & ~(alignment - 1)


def analyze_data_sections(elf_path):
    """Analyze data sections and return combined info."""
    data_section_names = ['.rodata', '.srodata', '.sdata', '.data', '.sbss', '.bss']
    found_sections = []

    for section_name in data_section_names:
        info = get_section_info(elf_path, section_name)
        if info:
            found_sections.append(info)

    if not found_sections:
        return None, 0, [], None

    found_sections.sort(key=lambda x: x['address'])
    first_section = found_sections[0]
    first_address = first_section['address']

    last_section = found_sections[-1]
    last_end_address = last_section['address'] + last_section['size']
    last_end_aligned = align_up(last_end_address, 4)

    memory_range_size = last_end_aligned - first_address

    return first_address, memory_range_size, found_sections, last_end_aligned


def print_header_structure(elf_path, output_file="header.c"):
    """Print the header structure with all placeholders filled in."""

    # Analyze the ELF file
    text_info = get_section_info(elf_path, '.text')
    main_address = get_main_address(elf_path)
    first_data_addr, total_data_size, data_sections, last_data_addr = analyze_data_sections(elf_path)

    if not text_info:
        print("ERROR: .text section not found!")
        return False

    if main_address is None:
        print("ERROR: main function not found!")
        return False

    # Calculate sizes and offsets
    text_address = text_info['address']
    text_size = text_info['size']
    text_memsz = align_up(text_size, 4096)  # Round up to multiple of 4096

    # Data section info
    if first_data_addr is not None:
        data_address = first_data_addr
        data_filesz = total_data_size
        data_memsz = align_up(data_filesz, 4096)
    else:
        data_address = 0
        data_filesz = 0
        data_memsz = 0

    # Build the header structure content
    content = []
    content.append("/* This is a generated file */")
    content.append("")
    content.append("typedef unsigned int uint32;")
    content.append("")
    content.append("typedef struct elf_header")
    content.append("{")
    content.append("    uint32 magic;")
    content.append("    uint32 entry;")
    content.append("    int segment_count;")
    content.append("    uint32 segment_offset;")
    content.append("} ELF_HEADER;")
    content.append("")
    content.append("typedef struct segment_header")
    content.append("{")
    content.append("    int flags;")
    content.append("    uint32 offset;")
    content.append("    uint32 vaddr;")
    content.append("    uint32 filesz;")
    content.append("    uint32 memsz;")
    content.append("} SEGMENT_HEADER;")
    content.append("")
    content.append("__asm__(\".section .header,\\\"aw\\\",@progbits\");")
    content.append("__asm__(\".align 16\");")
    content.append("")

    # ELF_HEADER
    content.append(f"ELF_HEADER elf_header __attribute__((section(\".header\"), aligned(16))) = {{1234, 0x{main_address:08x}, 2, 16}};")
    content.append("")

    # SEGMENT_HEADER array
    content.append("SEGMENT_HEADER segment_header[2] __attribute__((section(\".header\"), aligned(16))) = {")
    content.append("    {")
    content.append(f"        4, 0x{text_address + 0x40:08x}, 0x{text_address:08x}, {text_size}, {text_memsz}")
    content.append("    },")
    content.append("    {")
    content.append(f"        4, 0x{data_address + 0x40:08x}, 0x{data_address:08x}, {data_filesz}, {data_memsz}")
    content.append("    }")
    content.append("};")
    content.append("")

    # Padding
    content.append("char padding[8] __attribute__((section(\".header\"))) = {0};")

    # Write to file
    with open(output_file, 'w') as f:
        f.write('\n'.join(content) + '\n')

    # Print to stdout
    for line in content:
        print(line)

    print(f"\nHeader structure written to: {output_file}")
    return True


def generate_header_c_file(elf_path, output_c_path):
    """Generate a proper C file with the header structure."""
    return print_header_structure(elf_path, output_c_path)


def compile_and_extract_header(c_file, output_bin):
    """Compile the C file and extract the binary header."""
    obj_file = c_file.replace('.c', '.o')

    # Compile to object file
    print(f"Compiling {c_file}...")
    compile_cmd = f"riscv64-unknown-elf-gcc -c {c_file} -o {obj_file}"
    result = run_command(compile_cmd)
    if result is None:
        return False

    # Extract binary data
    print(f"Extracting binary header to {output_bin}...")
    extract_cmd = f"riscv64-unknown-elf-objcopy -O binary {obj_file} {output_bin}"
    result = run_command(extract_cmd)
    if result is None:
        return False

    # Clean up object file
    if os.path.exists(obj_file):
        os.remove(obj_file)

    return True


def attach_header_to_binary(header_bin, original_bin, output_bin):
    """Attach header to the original binary."""
    print(f"Attaching header to binary...")

    # Read header
    with open(header_bin, 'rb') as f:
        header_data = f.read()

    # Read original binary
    with open(original_bin, 'rb') as f:
        binary_data = f.read()

    # Write combined binary
    with open(output_bin, 'wb') as f:
        f.write(header_data)
        f.write(binary_data)

    print(f"\nSuccessfully created: {output_bin}")
    print(f"  Header size:  {len(header_data)} bytes")
    print(f"  Binary size:  {len(binary_data)} bytes")
    print(f"  Total size:   {len(header_data) + len(binary_data)} bytes")

    return True


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <elf_file> [binary_file] [output_file]")
        sys.exit(1)

    elf_path = sys.argv[1]

    # Mode 1: Just print the header structure
    if len(sys.argv) == 2:
        print(f"Analyzing ELF file: {elf_path}\n")
        print("=" * 70)
        print_header_structure(elf_path)
        print("=" * 70)
        return

    # Mode 2: Generate C file and attach to binary
    binary_path = sys.argv[2]
    output_path = sys.argv[3] if len(sys.argv) > 3 else "output.bin"

    # Temporary files
    header_c = "header_generated.c"
    header_bin = "header.bin"

    print(f"Processing ELF file: {elf_path}")
    print(f"Binary to attach header to: {binary_path}")
    print(f"Output file: {output_path}\n")
    print("=" * 70)

    # Print the header structure
    print_header_structure(elf_path)
    print("=" * 70)
    print()

    # Generate C file
    if not generate_header_c_file(elf_path, header_c):
        print("Failed to generate header C file")
        sys.exit(1)

    # Compile and extract header binary
    if not compile_and_extract_header(header_c, header_bin):
        print("Failed to compile header")
        sys.exit(1)

    # Attach header to binary
    if not attach_header_to_binary(header_bin, binary_path, output_path):
        print("Failed to attach header")
        sys.exit(1)

    print("\n" + "=" * 70)
    print("SUCCESS! Header attached successfully.")
    print("=" * 70)


if __name__ == "__main__":
    main()
