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
        # grep returns 1 when no match — not fatal
        if e.returncode == 1:
            return None
        print(f"Error running command: {command}")
        print(e.stderr)
        return None


def align_up(value, alignment):
    return (value + alignment - 1) & ~(alignment - 1)


def get_section_info(elf_path, section_name):
    cmd = f"riscv64-unknown-elf-objdump -h {elf_path} | grep ' {section_name} '"
    out = run_command(cmd)

    if not out:
        return None

    parts = out.split()
    if len(parts) >= 4:
        return {
            "name": section_name,
            "size": int(parts[2], 16),
            "address": int(parts[3], 16),
        }

    return None


def get_main_address(elf_path):
    cmd = f"riscv64-unknown-elf-nm {elf_path} | grep ' main$'"
    out = run_command(cmd)

    if not out:
        return None

    m = re.search(r'^([0-9a-fA-F]+)', out.strip())
    if m:
        return int(m.group(1), 16)

    return None


def analyze_data_sections(elf_path):
    names = ['.rodata', '.srodata', '.sdata', '.data', '.sbss', '.bss']
    sections = []

    for name in names:
        info = get_section_info(elf_path, name)
        if info and info["size"] > 0:
            sections.append(info)

    if not sections:
        return None, 0, []

    sections.sort(key=lambda s: s['address'])

    first = sections[0]
    last = sections[-1]

    first_addr = first['address']
    last_end = align_up(last['address'] + last['size'], 4)

    total_size = last_end - first_addr
    return first_addr, total_size, sections


def compute_header_size(segment_count):
    ELF_HEADER_SIZE = 16
    SEGMENT_HEADER_SIZE = 20

    actual_size = ELF_HEADER_SIZE + (segment_count * SEGMENT_HEADER_SIZE)

    return align_up(actual_size, 16)


def print_header_structure(elf_path, output_file="header.c"):
    text = get_section_info(elf_path, '.text')
    entry = get_main_address(elf_path)
    data_addr, data_size, data_sections = analyze_data_sections(elf_path)

    if entry is None:
        print("ERROR: main not found")
        return False

    segments = []

    # TEXT segment
    if text and text["size"] > 0:
        segments.append({
            "flags": 4,
            "vaddr": text['address'],
            "filesz": text['size'],
            "memsz": align_up(text['size'], 4096)
        })

    # DATA segment
    if data_sections and data_size > 0:
        segments.append({
            "flags": 4,
            "vaddr": data_addr,
            "filesz": data_size,
            "memsz": align_up(data_size, 4096)
        })

    segment_count = len(segments)

    HEADER_SIZE = compute_header_size(segment_count)

    # compute file offsets
    for seg in segments:
        seg["offset"] = HEADER_SIZE + (seg["vaddr"] - PROGRAM_BASE_VA)

    actual_struct_size = 16 + (segment_count * 20)
    pad_size = HEADER_SIZE - actual_struct_size

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

    # ELF header
    content.append(
        f"ELF_HEADER elf_header "
        f"__attribute__((section(\".header\"), aligned(16))) = "
        f"{{1234, 0x{entry:08x}, {segment_count}, 16}};"
    )
    content.append("")

    # Segment headers
    if segment_count > 0:
        content.append(
            f"SEGMENT_HEADER segment_header[{segment_count}] "
            "__attribute__((section(\".header\"), aligned(16))) = {"
        )

        for i, seg in enumerate(segments):
            content.append("    {")
            content.append(
                f"        {seg['flags']}, "
                f"0x{seg['offset']:08x}, "
                f"0x{seg['vaddr']:08x}, "
                f"{seg['filesz']}, "
                f"{seg['memsz']}"
            )
            content.append("    }" + ("," if i != segment_count - 1 else ""))

        content.append("};")
        content.append("")

    # Dynamic padding to 16-byte alignment
    if pad_size > 0:
        content.append(
            f"char padding[{pad_size}] "
            "__attribute__((section(\".header\"))) = {{0}};"
        )

    with open(output_file, "w") as f:
        f.write("\n".join(content) + "\n")

    print(f"\nActual header struct size : {actual_struct_size} bytes")
    print(f"Aligned header size       : {HEADER_SIZE} bytes")
    print(f"Padding emitted           : {pad_size} bytes")
    print(f"Header written to         : {output_file}")

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

    if not generate_header_c_file(elf, header_c):
        sys.exit(1)

    if not compile_and_extract_header(header_c, header_bin):
        sys.exit(1)

    if not attach_header_to_binary(header_bin, bin_in, out):
        sys.exit(1)


if __name__ == "__main__":
    main()