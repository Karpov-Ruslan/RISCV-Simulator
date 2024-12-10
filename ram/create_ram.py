import os

# Define file paths
ram_file_path = 'ram.bin'
code_file_path = 'code.bin'

# Get the size of code.bin in bytes
with open(code_file_path, 'rb') as f:
    code_size = os.fstat(f.fileno()).st_size

# Calculate the remaining size for ram.bin
remaining_size = 8 * 1024 * 1024 * 1024 - code_size

# Create ram.bin with the remaining size
with open(ram_file_path, 'wb') as f:
    # Write code.bin content first
    with open(code_file_path, 'rb') as cf:
        f.write(cf.read())
    
    # Fill the rest with zeros
    f.seek(code_size)
    f.write(b'\x00' * remaining_size)

print(f"Generated {ram_file_path} with size: {os.path.getsize(ram_file_path)} bytes")