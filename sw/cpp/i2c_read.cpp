#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <cstdint>

// Funkce pro výpočet CRC-8 (používá CRC-8-ATM)
uint8_t calculate_crc(const std::vector<uint8_t>& data) {
    uint8_t crc = 0x00;
    for (size_t i = 0; i < data.size(); ++i) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; ++j) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

void read_smbus_data(int file, uint8_t addr, uint32_t register_addr, int data_length, std::vector<uint8_t>& data_out) {
    // Příprava příkazové sekvence
    uint8_t control_word_23_16 = (register_addr >> 16) & 0xFF;
    uint8_t control_word_15_8 = (register_addr >> 8) & 0xFF;
    uint8_t control_word_7_0 = register_addr & 0xFF;
    
    std::vector<uint8_t> write_command = {
        control_word_23_16,
        control_word_15_8,
        control_word_7_0
    };

    // Zápis příkazu s řídicími slovy
    if (write(file, write_command.data(), write_command.size()) != static_cast<int>(write_command.size())) {
        throw std::runtime_error("Failed to write to the i2c bus.");
    }

    // Čtení datových bajtů a CRC
    std::vector<uint8_t> read_data(data_length + 1); // +1 pro CRC
    if (read(file, read_data.data(), read_data.size()) != static_cast<int>(read_data.size())) {
        throw std::runtime_error("Failed to read from the i2c bus.");
    }

    // Extrakce dat a CRC
    data_out = std::vector<uint8_t>(read_data.begin(), read_data.begin() + data_length);
    uint8_t received_crc = read_data[data_length]; // Předpoklad, že poslední bajt je CRC

    // Výpočet CRC pro přijatá data
    std::vector<uint8_t> data_for_crc = { static_cast<uint8_t>(addr << 1) };
    data_for_crc.insert(data_for_crc.end(), write_command.begin(), write_command.end());
    data_for_crc.push_back(static_cast<uint8_t>((addr << 1) | 1));
    data_for_crc.insert(data_for_crc.end(), data_out.begin(), data_out.end());
    uint8_t calculated_crc = calculate_crc(data_for_crc);

    if (calculated_crc != received_crc) {
        throw std::runtime_error("CRC mismatch, data may be corrupted");
    }
}

int main() {
    const char* filename = "/dev/i2c-1"; // I2C bus
    int addr = 0x01; // I2C adresa zařízení

    int file = open(filename, O_RDWR);
    if (file < 0) {
        std::cerr << "Failed to open the bus." << std::endl;
        return 1;
    }

    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        std::cerr << "Failed to acquire bus access and/or talk to slave." << std::endl;
        return 1;
    }

    uint32_t register_addr = 0x000080; // Příklad registru
    int data_length = 4; // Počet bajtů k přečtení

    try {
        std::vector<uint8_t> data;
        read_smbus_data(file, addr, register_addr, data_length, data);
        std::cout << "Read data:";
        for (uint8_t byte : data) {
            std::cout << " 0x" << std::hex << static_cast<int>(byte);
        }
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    close(file);
    return 0;
}
