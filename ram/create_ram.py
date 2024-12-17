import os
import sys
import time
from datetime import datetime
import logging
import argparse

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

# Define default values
default_base_address = 0x10094
default_ram_file_path = 'ram.bin'
default_code_file_path = 'code.bin'

def setup_logging():
    log_dir = 'logs'
    os.makedirs(log_dir, exist_ok=True)
    log_file = os.path.join(log_dir, f'{datetime.now().strftime("%Y%m%d_%H%M%S")}_create_ram.log')
    
    handler = logging.FileHandler(log_file)
    handler.setLevel(logging.DEBUG)
    
    formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
    handler.setFormatter(formatter)
    
    logger.addHandler(handler)

def create_ram(base_address, ram_file_path, code_file_path):
    try:
        # Get the size of code.bin in bytes
        with open(code_file_path, 'rb') as f:
            code_size = os.fstat(f.fileno()).st_size
        
        # Calculate the remaining size for ram.bin
        all_size = 8 * 1024 * 1024 * 1024
        
        # Create ram.bin with the remaining size
        with open(ram_file_path, 'wb') as f:
            f.seek(0)
            f.write(b'\x00' * base_address)
            
            # Write code.bin content starting at the specified base address
            with open(code_file_path, 'rb') as cf:
                f.seek(base_address)
                f.write(cf.read())
            
            # Fill the rest with zeros
            f.seek(base_address + code_size)
            f.write(b'\x00' * (all_size - (base_address + code_size)))
        
        logger.info(f"Successfully generated {ram_file_path} with size: {os.path.getsize(ram_file_path)} bytes")
        logger.info(f"Base address used: {hex(base_address)}")
        logger.info(f"Code size: {code_size} bytes")
        
        return True
    
    except Exception as e:
        logger.error(f"An error occurred while creating RAM: {str(e)}")
        return False

def main():
    setup_logging()
    logger.info("Starting RAM creation process...")
    
    parser = argparse.ArgumentParser(description="Create RAM image with configurable base address")
    parser.add_argument('--base-address', type=lambda x: int(x, 0), default=default_base_address,
                        help=f"Base address (in hex). Default is {hex(default_base_address)}")
    parser.add_argument('--output-file', default=default_ram_file_path,
                        help=f"Output file name. Default is '{default_ram_file_path}'")
    parser.add_argument('--code-file', default=default_code_file_path,
                        help=f"Code file to use. Default is '{default_code_file_path}'")
    
    args = parser.parse_args()
    
    base_address = args.base_address
    ram_file_path = args.output_file
    code_file_path = args.code_file
    
    logger.info(f"Using base address: {hex(base_address)}, Output file: {ram_file_path}, Code file: {code_file_path}")
    
    start_time = time.time()
    success = create_ram(base_address, ram_file_path, code_file_path)
    
    elapsed_time = time.time() - start_time
    logger.info(f"Process completed in {elapsed_time:.2f} seconds")
    
    if not success:
        logger.error("RAM creation failed. Please check logs for more details.")
        sys.exit(1)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        logger.info("Interrupted by user. Exiting...")
    except Exception as e:
        logger.exception("Unexpected error:")
        sys.exit(1)
