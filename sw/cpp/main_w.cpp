#include <iostream>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdexcept>
#include <cstring>

#define I2C_DEVICE "/dev/i2c-17"
#define DEVICE_ADDRESS 0x60

#define CONTROLLER_FAULT_STATUS_REGISTER 0x05  // Replace with the actual address if different

void readRegister(int file, uint8_t deviceAddress, uint32_t registerAddress, uint8_t* data, size_t length, bool crcEnabled) {
    uint32_t controlWord = 0x800000 | (registerAddress << 8);
    uint8_t controlBytes[3];
    controlBytes[0] = (controlWord >> 16) & 0xFF;
    controlBytes[1] = (controlWord >> 8) & 0xFF;
    controlBytes[2] = controlWord & 0xFF;

    // Write control bytes to the device
    if (write(file, controlBytes, 3) != 3) {
        throw std::runtime_error("Failed to write control word to the device.");
    }

    // Read data from the device
    size_t readLength = length + (crcEnabled ? 1 : 0);
    if (read(file, data, readLength) != readLength) {
        throw std::runtime_error("Failed to read data from the device.");
    }

    if (crcEnabled) {
        // Handle CRC check if enabled
        uint8_t crc = data[readLength - 1];
        // Add CRC handling code here if necessary
    }
}

void readMultipleRegisters(uint8_t deviceAddress, const std::vector<uint32_t>& registerAddresses, size_t length, bool crcEnabled) {
    int file = open(I2C_DEVICE, O_RDWR);
    if (file < 0) {
        throw std::runtime_error("Failed to open the I2C bus.");
    }

    if (ioctl(file, I2C_SLAVE, deviceAddress) < 0) {
        close(file);
        throw std::runtime_error("Failed to set the I2C device address.");
    }

    for (const auto& regAddr : registerAddresses) {
        std::vector<uint8_t> data(length + (crcEnabled ? 1 : 0));
        readRegister(file, deviceAddress, regAddr, data.data(), length, crcEnabled);
        std::cout << "Register 0x" << std::hex << regAddr << ": ";
        for (size_t i = 0; i < length; ++i) {
            std::cout << "0x" << std::hex << static_cast<int>(data[i]) << " ";
        }
        std::cout << std::dec << std::endl;
    }

    close(file);
}


void readControllerFaultStatus(uint8_t deviceAddress, size_t length, bool crcEnabled) {
    int file = open(I2C_DEVICE, O_RDWR);
    if (file < 0) {
        throw std::runtime_error("Failed to open the I2C bus.");
    }

    if (ioctl(file, I2C_SLAVE, deviceAddress) < 0) {
        close(file);
        throw std::runtime_error("Failed to set the I2C device address.");
    }

    std::vector<uint8_t> data(length + (crcEnabled ? 1 : 0));
    readRegister(file, deviceAddress, CONTROLLER_FAULT_STATUS_REGISTER, data.data(), length, crcEnabled);
    std::cout << "CONTROLLER_FAULT_STATUS Register: ";
    for (size_t i = 0; i < length; ++i) {
        std::cout << "0x" << std::hex << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::dec << std::endl;

    close(file);
}

int main() {
    try {
        uint8_t deviceAddress = DEVICE_ADDRESS;
        std::vector<uint32_t> registerAddresses = {0x01, 0x02, 0x03};  // Example register addresses
        size_t length = 4;  // Number of bytes to read from each register
        bool crcEnabled = false;  // Set to true if CRC is enabled

        readControllerFaultStatus(deviceAddress, length, crcEnabled);
        readMultipleRegisters(deviceAddress, registerAddresses, length, crcEnabled);
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
