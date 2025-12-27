#!/usr/bin/env python3

import subprocess
import sys
import re
import os

PROGRAM_BASE_VA = 0x2000


def run_command(command):
    try:
        result = subprocess.run(
            command, shell=True, capture_output=True, text=True, check=True
        )
        return result.stdout
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {command}")
        print(e.stderr)
        return None


def align_up(value, alignment):
    return (value + alignment - 1) & ~(alignment - 1)


def get_section_info(elf_path, section_name):
    """
    Extract section size and VMA.
    ELF file offsets are NOT used because we emit a flat binary.
    """
    cmd = f"riscv64-unknown-elf-objdump -h {elf_path} | grep ' {section_name} '"
    out = run_command(cmd)

    if out:
        parts = out.split()
        if len(parts) >= 4:
            return {
                "name": section_name,
                "size": int(parts[2], 16),
                "address": int(parts[3], 16),  # VMA
            }
    return None


def get_main_address(elf_path):
    cmd = f"riscv64-unknown-elf-nm {elf_path} | grep ' main$'"
    out = run_command(cmd)

    if out:
        m = re.search(r'^([0-9a-fA-F]+)', out.strip())
        if m:
            return int(m.group(1), 16)
    return None


def analyze_data_sections(elf_path):
    names = ['.rodata', '.srodata', '.sdata', '.data', '.sbss', '.bss']
    sections = []

    for name in names:
        info = get_section_info(elf_path, name)
        if info:
            sections.append(info)

    if not sections:
        return None, 0, []

    sections.sort(key=lambda s: s['address'])

    first = sections[0]
    last = sections[-1]

    first_addr = first['address']
    last_end = last['address'] + last['size']
    last_end = align_up(last_end, 4)

    total_size = last_end - first_addr
    return first_addr, total_size, sections


def compute_header_size():
    """
    ELF_HEADER        = 16 bytes
    2 SEGMENT_HEADERs = 2 * 20 = 40 bytes
    padding           = 8 bytes
    -------------------------------
    total             = 64 bytes (0x40)
    """
    return align_up(16 + (2 * 20) + 8, 16)


def print_header_structure(elf_path, output_file="header.c"):
    text = get_section_info(elf_path, '.text')
    entry = get_main_address(elf_path)
    data_addr, data_size, data_sections = analyze_data_sections(elf_path)

    if not text:
        print("ERROR: .text not found")
        return False

    if entry is None:
        print("ERROR: main not found")
        return False

    HEADER_SIZE = compute_header_size()

    if text['address'] != PROGRAM_BASE_VA:
        print(
            f"WARNING: .text VMA is 0x{text['address']:x}, "
            f"expected 0x{PROGRAM_BASE_VA:x}"
        )

    # ---- TEXT SEGMENT ----
    text_vaddr = text['address']
    text_size = text['size']
    text_memsz = align_up(text_size, 4096)
    text_offset = HEADER_SIZE + (text_vaddr - PROGRAM_BASE_VA)

    # ---- DATA SEGMENT ----
    if data_sections:
        data_vaddr = data_addr
        data_filesz = data_size
        data_memsz = align_up(data_filesz, 4096)
        data_offset = HEADER_SIZE + (data_vaddr - PROGRAM_BASE_VA)
    else:
        data_vaddr = 0
        data_filesz = 0
        data_memsz = 0
        data_offset = 0

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

    content.append(
        f"ELF_HEADER elf_header "
        f"__attribute__((section(\".header\"), aligned(16))) = "
        f"{{1234, 0x{entry:08x}, 2, 16}};"
    )
    content.append("")

    content.append(
        "SEGMENT_HEADER segment_header[2] "
        "__attribute__((section(\".header\"), aligned(16))) = {"
    )
    content.append("    {")
    content.append(
        f"        4, 0x{text_offset:08x}, 0x{text_vaddr:08x}, "
        f"{text_size}, {text_memsz}"
    )
    content.append("    },")
    content.append("    {")
    content.append(
        f"        4, 0x{data_offset:08x}, 0x{data_vaddr:08x}, "
        f"{data_filesz}, {data_memsz}"
    )
    content.append("    }")
    content.append("};")
    content.append("")
    content.append("char padding[8] __attribute__((section(\".header\"))) = {0};")

    with open(output_file, "w") as f:
        f.write("\n".join(content) + "\n")

    for line in content:
        print(line)

    print(f"\nHeader written to: {output_file}")
    return True


def generate_header_c_file(elf_path, output_c_path):
    return print_header_structure(elf_path, output_c_path)


def compile_and_extract_header(c_file, output_bin):
    obj = c_file.replace(".c", ".o")

    if run_command(f"riscv64-unknown-elf-gcc -c {c_file} -o {obj}") is None:
        return False

    if run_command(f"riscv64-unknown-elf-objcopy -O binary {obj} {output_bin}") is None:
        return False

    if os.path.exists(obj):
        os.remove(obj)

    return True


def attach_header_to_binary(header_bin, original_bin, output_bin):
    with open(header_bin, "rb") as f:
        h = f.read()
    with open(original_bin, "rb") as f:
        b = f.read()

    with open(output_bin, "wb") as f:
        f.write(h)
        f.write(b)

    print(f"\nCreated {output_bin}")
    print(f"  Header size: {len(h)} bytes")
    print(f"  Binary size: {len(b)} bytes")
    print(f"  Total size : {len(h) + len(b)} bytes")

    return True


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <elf> [bin] [out]")
        sys.exit(1)

    elf = sys.argv[1]

    if len(sys.argv) == 2:
        print_header_structure(elf)
        return

    bin_in = sys.argv[2]
    out = sys.argv[3] if len(sys.argv) > 3 else "final.bin"

    header_c = "header_generated.c"
    header_bin = "header.bin"

    print_header_structure(elf)

    if not generate_header_c_file(elf, header_c):
        sys.exit(1)

    if not compile_and_extract_header(header_c, header_bin):
        sys.exit(1)

    if not attach_header_to_binary(header_bin, bin_in, out):
        sys.exit(1)


if __name__ == "__main__":
    main()
