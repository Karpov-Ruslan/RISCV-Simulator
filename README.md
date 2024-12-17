To prepare .elf for loading in memory:

```
riscv32-unknown-elf-objcopy -O binary your.elf your.bin 
```

To show first bytes of binary file:
```
xxd -c 4 -b your.bin | less
```

To create 5Gb zeroed-ram with loaded code.bin:
```
dd if=/dev/zero of=ram.bin bs=1000000000 count=5
cd ram
python create_ram.py --code-file your.bin
```

To start simulator:
```
./RISCV_Simulator --pc 0x10094
```

To compare 2 traces:
```
 sdiff -l ./build/ref_trace.txt ./build/check_trace | cat -n | grep -v -e '($'  | less
```

Transition RAM from file to mmap:
5705 ms -> 1274 ms per 3.7 millions of instructions

Usefull links:

Nice and compact reference - there are opcode, semantics, instruction layout:
https://www.cs.sfu.ca/~ashriram/Courses/CS295/assets/notebooks/RISCV/RISCV_CARD.pdf

Prebuilt toolchain:
https://github.com/stnolting/riscv-gcc-prebuilt

If you get unknown instruction "100010101010101010".
It's convinient to put this bin code here and check if it's really an existing instruction:
https://luplab.gitlab.io/rvcodecjs/