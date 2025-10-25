#!/usr/bin/env python3

import subprocess
import sys
import re


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
        # Format: Idx Name Size VMA LMA File off Algn
        # Example:  1 .text 00001234 00010000 00010000 00001000 2**2
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
        # Parse: address is the first column in the symbol table
        match = re.search(r'^([0-9a-fA-F]+)', output.strip())
        if match:
            return int(match.group(1), 16)

    return None


def align_up(value, alignment):
    """Align a value up to the nearest alignment boundary."""
    return (value + alignment - 1) & ~(alignment - 1)


def analyze_data_sections(elf_path):
    """
    Analyze data sections and return:
    1. Start address of the first data section that appears
    2. Combined size of all data sections (considering 4-byte alignment)
    """
    # Data sections in the order they might appear
    data_section_names = ['.rodata', '.srodata', '.sdata', '.data', '.sbss', '.bss']

    found_sections = []

    # Find all existing data sections
    for section_name in data_section_names:
        info = get_section_info(elf_path, section_name)
        if info:
            found_sections.append(info)

    if not found_sections:
        print("No data sections found!")
        return None, 0, [], None

    # Sort by address to find the first one
    found_sections.sort(key=lambda x: x['address'])

    # First data section address
    first_section = found_sections[0]
    first_address = first_section['address']

    # Calculate combined size considering 4-byte alignment between sections
    total_size = 0
    for i, section in enumerate(found_sections):
        # Add the section size
        section_size = section['size']
        aligned_size = align_up(section_size, 4)

        # Store both raw and aligned sizes
        section['aligned_size'] = aligned_size
        section['padding'] = aligned_size - section_size

        total_size += aligned_size

    # Alternative calculation: measure from first section start to last section end (aligned)
    last_section = found_sections[-1]
    last_end_address = last_section['address'] + last_section['size']
    last_end_aligned = align_up(last_end_address, 4)

    # Use memory range method if sections are contiguous
    memory_range_size = last_end_aligned - first_address

    return first_address, memory_range_size, found_sections, last_end_aligned


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <path_to_elf_file>")
        sys.exit(1)

    elf_path = sys.argv[1]

    print(f"Analyzing ELF file: {elf_path}\n")
    print("=" * 70)

    # Get .text section info
    text_info = get_section_info(elf_path, '.text')
    if text_info:
        text_address = text_info['address']
        text_size = text_info['size']
        text_end = text_address + text_size
    else:
        print("ERROR: .text section not found!")
        text_address = None
        text_size = None
        text_end = None

    # Get main function address
    main_address = get_main_address(elf_path)

    # Get data sections info
    first_data_addr, total_data_size, data_sections, last_data_addr = analyze_data_sections(elf_path)

    # Display TEXT section results
    print("\n.TEXT SECTION:")
    print("-" * 70)
    if text_info:
        print(f"  Start Address: 0x{text_address:08x} ({text_address})")
        print(f"  Size:          {text_size} bytes (0x{text_size:x})")
        print(f"  End Address:   0x{text_end:08x} ({text_end})")
    else:
        print("  NOT FOUND")

    # Display main function address
    print("\nMAIN FUNCTION:")
    print("-" * 70)
    if main_address is not None:
        print(f"  Address:       0x{main_address:08x} ({main_address})")
    else:
        print("  NOT FOUND")

    # Display data sections results
    print("\nDATA SECTIONS:")
    print("-" * 70)
    if first_data_addr is not None:
        print(f"  First Data Section Start: 0x{first_data_addr:08x} ({first_data_addr})")
        print(f"  Combined Size (4-byte aligned): {total_data_size} bytes (0x{total_data_size:x})")
        print(f"  Data Memory Range:        0x{first_data_addr:08x} - 0x{last_data_addr:08x}")
    else:
        print("  NO DATA SECTIONS FOUND")

    # Detailed breakdown
    if data_sections:
        print(f"\n{'=' * 70}")
        print("DATA SECTIONS BREAKDOWN (in memory order, with 4-byte alignment):")
        print(f"{'=' * 70}")
        print(f"{'Section':<12} {'Address':<12} {'Raw Size':<12} {'Aligned':<12} {'Padding':<8}")
        print(f"{'-' * 70}")

        for section in data_sections:
            end_addr = section['address'] + section['size']
            aligned = section.get('aligned_size', section['size'])
            padding = section.get('padding', 0)
            print(f"{section['name']:<12} 0x{section['address']:08x}   "
                  f"{section['size']:4d} bytes   {aligned:4d} bytes   {padding:2d} bytes")

    print(f"{'=' * 70}")

    # Summary
    print("\nSUMMARY:")
    print("-" * 70)
    print(f"  .text start:           0x{text_address:08x}" if text_address else "  .text start:           NOT FOUND")
    print(
        f"  .text size:            {text_size} bytes (0x{text_size:x})" if text_size else "  .text size:            NOT FOUND")
    print(f"  main() address:        0x{main_address:08x}" if main_address else "  main() address:        NOT FOUND")
    print(
        f"  first data section:    0x{first_data_addr:08x}" if first_data_addr else "  first data section:    NOT FOUND")
    print(
        f"  total data size:       {total_data_size} bytes (0x{total_data_size:x}) [4-byte aligned]" if first_data_addr else "  total data size:       NOT FOUND")
    print("=" * 70)

    # Return as dictionary for programmatic use
    result = {
        'text_address': text_address,
        'text_size': text_size,
        'main_address': main_address,
        'first_data_section_address': first_data_addr,
        'combined_data_size': total_data_size,
        'data_range_start': first_data_addr,
        'data_range_end': last_data_addr,
        'data_sections': data_sections
    }

    return result


if __name__ == "__main__":
    result = main()