#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

void scan_i2c_bus() {
    int file;
    const char* filename = "/dev/i2c-1"; // I2C bus
    if ((file = open(filename, O_RDWR)) < 0) {
        std::cerr << "Failed to open the i2c bus." << std::endl;
        return;
    }

    for (int addr = 0x03; addr <= 0x77; addr++) {
        if (ioctl(file, I2C_SLAVE, addr) < 0) {
            std::cerr << "Failed to acquire bus access for address 0x" << std::hex << addr << std::endl;
            continue;
        }

        uint8_t buffer[1] = { 0x00 };
        if (write(file, buffer, 1) == 1) {
            std::cout << "Device found at address 0x" << std::hex << addr << std::endl;
        }
    }

    close(file);
}

int main() {
    scan_i2c_bus();
    return 0;
}
