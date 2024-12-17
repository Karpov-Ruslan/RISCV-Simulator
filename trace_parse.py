def process_instructions(instructions):
    # print(f"Input instructions: {instructions}")
    
    result = []
    
    lines = instructions.strip().split('\n')
    for line in lines:
        if line.startswith('insn:'):
            parts = line.split()
            opcode = parts[2][1:]
            result.append(opcode)
            # print(f"Found opcode: {opcode}")
    
    # print(f"Result after processing: {result}")
    return result

# Example usage
instructions = """insn:     0x0100d8                       -auipc gp, 16384;  // gp = pc + 16384
insn:     0x0100d8                       -auipc gp, 16384;  // gp = pc + 16384
insn:     0x0100dc                       -addi gp, gp, 0xfffffa68;  // gp = gp + 0xfffffa68
insn:     0x0100e0                       -addi a0, gp, 0xfffffd48;  // a0 = gp + 0xfffffd48
insn:     0x0100e4                       -addi a2, gp, 0x78;  // a2 = gp + 0x78
insn:     0x0100e8                       -sub a2, a2, a0;  // a2 = a2 - a0
insn:     0x0100ec                       -addi a1, zero, 0;  // a1 = zero + 0
insn:     0x0100f0                       -jal ra, 3240;
insn:     0x010d98                       -addi t1, zero, 0xf;  // t1 = zero + 0xf
insn:     0x010d9c                       -addi a4, a0, 0;  // a4 = a0 + 0
insn:     0x010da0                       -bgeu t1, a2, 0x3c;  // if (t1 >= a2) goto 0x3c
insn:     0x010da4                       -andi a5, a4, 15;  // a5 = a4 & 0xf
insn:     0x010da8                       -bne a5, zero, 0xa0;  // if (a5 != zero) goto 0xa0
insn:     0x010e48                       -slli a3, a5, 2;  // a3 = a5 << 0x2
insn:     0x010e4c                       -auipc t0, 0;  // t0 = pc + 0
insn:     0x010e50                       -add a3, a3, t0;  // a3 = a3 + t0
insn:     0x010e54                       -addi t0, ra, 0;  // t0 = ra + 0
insn:     0x010e58                       -jalr ra, a3, -96;
insn:     0x010e0c                       -sb a1, 7(a4);
insn:     0x010e10                       -sb a1, 6(a4);
insn:     0x010e14                       -sb a1, 5(a4);
insn:     0x010e18                       -sb a1, 4(a4);
insn:     0x010e1c                       -sb a1, 3(a4);
insn:     0x010e20                       -sb a1, 2(a4);
insn:     0x010e24                       -sb a1, 1(a4);
insn:     0x010e28                       -sb a1, 0(a4);
insn:     0x010e2c                       -jalr zero, ra, 0;
..."""

text = []
with open('trace.txt', 'r') as f:
    text = f.readlines()

text = ''.join(text)

formatted_instructions = process_instructions(text)

with open('simple_trace.txt', 'w') as f:
    f.write('\n'.join(formatted_instructions))

