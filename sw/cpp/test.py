import smbus2
import struct

def read_register(bus, device_address, register_address, length=4, crc_enabled=False):
    # Preparing control word: 0x800000 | (register_address << 8)
    # This sets the MSB (read bit) and specifies the register address
    control_word = 0x800000 | (register_address << 8)
    # Convert control word to bytes, ensuring MSB is sent first
    control_bytes = struct.pack('>I', control_word)[1:]  # Skip the first byte to make it 3 bytes

    # Prepare write (for control word) and read transactions
    write = smbus2.i2c_msg.write(device_address, [0x00] + list(control_bytes))  # Start byte + Control word
    read = smbus2.i2c_msg.read(device_address, length + (1 if crc_enabled else 0))  # Read data + optional CRC

    # Execute I2C operations
    bus.i2c_rdwr(write, read)

    # Extract data
    data = list(read)
    if crc_enabled:
        crc = data[-1]
        data = data[:-1]  # Remove CRC from data if present

    return data

def read_multiple_registers(bus_number, device_address, register_addresses, length=4, crc_enabled=False):
    with smbus2.SMBus(bus_number) as bus:
        results = {}
        for register_address in register_addresses:
            data = read_register(bus, device_address, register_address, length, crc_enabled)
            results[register_address] = data
        return results

# Example usage
bus_number = 17  # Corresponds to /dev/i2c-17
device_address = 0x60  # MCT8329A I2C address
register_addresses = [0x01, 0x02, 0x03]  # Example register addresses to read

data = read_multiple_registers(bus_number, device_address, register_addresses, length=4, crc_enabled=False)
for reg_addr, reg_data in data.items():
    print(f"Register {hex(reg_addr)}: {reg_data}")
