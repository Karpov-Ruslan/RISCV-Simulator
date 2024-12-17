To prepare .elf for loading in memory:

```
riscv32-unknown-elf-objcopy -O binary your.elf your.bin 
```

To create zeroed-ram with loaded code.bin:
```
dd if=/dev/zero of=ram.bin bs=1000000000 count=5
cd ram
python create_ram.py --code-file your.bin
```

To start simulator:
```
./RISCV_Simulator --pc 0x10094
```
