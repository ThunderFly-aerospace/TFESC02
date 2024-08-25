#include <iostream>
#include <vector>
#include <bitset>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdexcept>
#include <cstring>
#include "mct8329a_registers.h"

#define I2C_DEVICE "/dev/i2c-17"




void readRegister(int file, uint8_t deviceAddress, uint32_t registerAddress, uint8_t* data, size_t length, bool crcEnabled) {
    
    std::cout << "........." << std::endl << "Reading register 0x" << std::hex << registerAddress << std::dec << std::endl;

    uint8_t OP_R_W = 1; // Read operation
    uint8_t CRC_EN = crcEnabled ? 1 : 0;
    uint8_t DLEN = 0b01; // 32-bit data length
    uint8_t MEM_SEC = (registerAddress >> 16) & 0xF;
    uint8_t MEM_PAGE = (registerAddress >> 12) & 0xF;
    uint16_t MEM_ADDR = registerAddress & 0xFFF;

    uint32_t controlWord = (OP_R_W << 23) | (CRC_EN << 22) | (DLEN << 20) | (MEM_SEC << 16) | (MEM_PAGE << 12) | MEM_ADDR;
    
    uint8_t controlBytes[3];
    controlBytes[0] = (controlWord >> 16) & 0xFF;
    controlBytes[1] = (controlWord >> 8) & 0xFF;
    controlBytes[2] = controlWord & 0xFF;

    std::cout << "Control bytes: ";
    for (int i = 0; i < 3; ++i) {
        std::cout << "0x" << std::hex << static_cast<int>(controlBytes[i]) << " ";
    }
    std::cout << std::dec << std::endl;

    // Write control bytes to the device
    if (write(file, controlBytes, 3) != 3) {
        throw std::runtime_error("Failed to write control word to the device.");
    }

    // Read data from the device
    size_t readLength = length + (crcEnabled ? 1 : 0);
    ssize_t bytesRead = read(file, data, readLength);
    if (bytesRead != readLength) {
        std::cerr << "Expected to read " << readLength << " bytes but got " << bytesRead << " bytes." << std::endl;
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

void printBits(uint8_t byte) {
    std::bitset<8> bits(byte);
    std::cout << bits << std::endl;
}

void readAndPrintStatusRegister(uint8_t deviceAddress, uint32_t registerAddress, const std::string& registerName) {
    int file = open(I2C_DEVICE, O_RDWR);
    if (file < 0) {
        throw std::runtime_error("Failed to open the I2C bus.");
    }

    if (ioctl(file, I2C_SLAVE, deviceAddress) < 0) {
        close(file);
        throw std::runtime_error("Failed to set the I2C device address.");
    }

    size_t length = 4;  // Assuming each status register is 4 bytes long
    bool crcEnabled = false;  // Set to true if CRC is enabled
    std::vector<uint8_t> data(length + (crcEnabled ? 1 : 0));
    readRegister(file, deviceAddress, registerAddress, data.data(), length, crcEnabled);

    std::cout << registerName << " Register: ";
    for (size_t i = 0; i < length; ++i) {
        std::cout << "0x" << std::hex << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::dec << std::endl;

    for (size_t i = 0; i < length; ++i) {
        std::cout << registerName << " byte " << i << ": ";
        printBits(data[i]);
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
    readRegister(file, deviceAddress, MCT8329A_CONTROLLER_FAULT_STATUS, data.data(), length, crcEnabled);
    std::cout << "CONTROLLER_FAULT_STATUS Register: ";
    for (size_t i = 0; i < length; ++i) {
        std::cout << "0x" << std::hex << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::dec << std::endl;

    close(file);
}













ISDConfig get_isd_config(uint8_t deviceAddress) {
    int file = open(I2C_DEVICE, O_RDWR);
    if (file < 0) {
        throw std::runtime_error("Failed to open the I2C bus.");
    }

    if (ioctl(file, I2C_SLAVE, deviceAddress) < 0) {
        close(file);
        throw std::runtime_error("Failed to set the I2C device address.");
    }

    uint8_t data[4];
    readRegister(file, deviceAddress, MCT8329A_ISD_CONFIG, data, 4, false);

    close(file);

    std::cout << "ISD_CONFIG Data in binary: ";
    for (size_t i = 0; i < 4; ++i) {
        std::bitset<8> bits(data[i]);
        std::cout << bits << " ";
    }
    std::cout << std::endl;

    ISDConfig config;
    uint32_t rawData = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];

    config.parity = (rawData >> 31) & 0x01;
    config.isdEn = (rawData >> 30) & 0x01;
    config.brakeEn = (rawData >> 29) & 0x01;
    config.hizEn = (rawData >> 28) & 0x01;
    config.rvsDrEn = (rawData >> 27) & 0x01;
    config.resyncEn = (rawData >> 26) & 0x01;
    config.statBrkEn = (rawData >> 25) & 0x01;
    config.statDetectThr = (rawData >> 22) & 0x07;
    config.brkMode = (rawData >> 21) & 0x01;
    config.brkTime = (rawData >> 16) & 0x0F;
    config.hizTime = (rawData >> 9) & 0x0F;
    config.startupBrkTime = (rawData >> 6) & 0x07;
    config.resyncMinThreshold = (rawData >> 3) & 0x07;
    config.mtrStartup = (rawData >> 1) & 0x03;

    return config;
}

void set_isd_config(uint8_t deviceAddress, const ISDConfig& config) {
    int file = open(I2C_DEVICE, O_RDWR);
    if (file < 0) {
        throw std::runtime_error("Failed to open the I2C bus.");
    }

    if (ioctl(file, I2C_SLAVE, deviceAddress) < 0) {
        close(file);
        throw std::runtime_error("Failed to set the I2C device address.");
    }

    uint32_t rawData = 0;
    rawData |= (config.parity & 0x01) << 31;
    rawData |= (config.isdEn & 0x01) << 30;
    rawData |= (config.brakeEn & 0x01) << 29;
    rawData |= (config.hizEn & 0x01) << 28;
    rawData |= (config.rvsDrEn & 0x01) << 27;
    rawData |= (config.resyncEn & 0x01) << 26;
    rawData |= (config.statBrkEn & 0x01) << 25;
    rawData |= (config.statDetectThr & 0x07) << 22;
    rawData |= (config.brkMode & 0x01) << 21;
    rawData |= (config.brkTime & 0x0F) << 16;
    rawData |= (config.hizTime & 0x0F) << 9;
    rawData |= (config.startupBrkTime & 0x07) << 6;
    rawData |= (config.resyncMinThreshold & 0x07) << 3;
    rawData |= (config.mtrStartup & 0x03) << 1;

    uint8_t controlBytes[3];
    controlBytes[0] = (rawData >> 16) & 0xFF;
    controlBytes[1] = (rawData >> 8) & 0xFF;
    controlBytes[2] = rawData & 0xFF;

    // Send the control word and data to the device
    if (write(file, controlBytes, 3) != 3) {
        close(file);
        throw std::runtime_error("Failed to write control word to the device.");
    }

    close(file);
}











SysStatus1 get_sys_status1(uint8_t deviceAddress) {
    int file = open(I2C_DEVICE, O_RDWR);
    if (file < 0) {
        throw std::runtime_error("Failed to open the I2C bus.");
    }

    if (ioctl(file, I2C_SLAVE, deviceAddress) < 0) {
        close(file);
        throw std::runtime_error("Failed to set the I2C device address.");
    }

    uint8_t data[4];
    readRegister(file, deviceAddress, MCT8329A_SYSTEM_STATUS1, data, 4, false);

    close(file);

    std::cout << "Data in binary: ";
    for (size_t i = 0; i < 4; ++i) {
        std::bitset<8> bits(data[i]);
        std::cout << bits << " ";
    }
    std::cout << std::endl;

    SysStatus1 status;
    uint32_t rawData = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];
    status.voltMag = ((rawData >> 16) & 0xFFFF)/10.0;
    status.speedCmd = ((rawData >> 1) & 0x7FFF) / 32767.0 * 100;
    status.i2cEntryStatus = rawData & 0x01;

    return status;
}


SysStatus2 get_sys_status2(uint8_t deviceAddress) {
    int file = open(I2C_DEVICE, O_RDWR);
    if (file < 0) {
        throw std::runtime_error("Failed to open the I2C bus.");
    }

    if (ioctl(file, I2C_SLAVE, deviceAddress) < 0) {
        close(file);
        throw std::runtime_error("Failed to set the I2C device address.");
    }

    uint8_t data[4];
    readRegister(file, deviceAddress, MCT8329A_SYSTEM_STATUS2, data, 4, false);

    close(file);

    std::cout << "SYS_STATUS2 Data in binary: ";
    for (size_t i = 0; i < 4; ++i) {
        std::bitset<8> bits(data[i]);
        std::cout << bits << " ";
    }
    std::cout << std::endl;

    SysStatus2 status;
    uint32_t rawData = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];

    status.state = (rawData >> 28) & 0x0F;
    status.stlFault = (rawData >> 17) & 0x01;
    status.motorSpeed = (rawData & 0xFFFF) / 10.0;

    return status;
}

SysStatus3 get_sys_status3(uint8_t deviceAddress) {
    int file = open(I2C_DEVICE, O_RDWR);
    if (file < 0) {
        throw std::runtime_error("Failed to open the I2C bus.");
    }

    if (ioctl(file, I2C_SLAVE, deviceAddress) < 0) {
        close(file);
        throw std::runtime_error("Failed to set the I2C device address.");
    }

    uint8_t data[4];
    readRegister(file, deviceAddress, MCT8329A_SYSTEM_STATUS3, data, 4, false);

    close(file);

    std::cout << "SYS_STATUS3 Data in binary: ";
    for (size_t i = 0; i < 4; ++i) {
        std::bitset<8> bits(data[i]);
        std::cout << bits << " ";
    }
    std::cout << std::endl;

    SysStatus3 status;
    uint32_t rawData = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];

    status.dcBusCurr = ((rawData >> 16) & 0xFFFF) / 256.0;
    status.dcBattPow = (rawData & 0xFFFF) / 64.0;

    return status;
}

ControllerFaultStatus get_controller_fault_status(uint8_t deviceAddress) {
    int file = open(I2C_DEVICE, O_RDWR);
    if (file < 0) {
        throw std::runtime_error("Failed to open the I2C bus.");
    }

    if (ioctl(file, I2C_SLAVE, deviceAddress) < 0) {
        close(file);
        throw std::runtime_error("Failed to set the I2C device address.");
    }

    uint8_t data[4];
    readRegister(file, deviceAddress, MCT8329A_CONTROLLER_FAULT_STATUS, data, 4, false);

    close(file);

    std::cout << "CONTROLLER_FAULT_STATUS Data in binary: ";
    for (size_t i = 0; i < 4; ++i) {
        std::bitset<8> bits(data[i]);
        std::cout << bits << " ";
    }
    std::cout << std::endl;

    ControllerFaultStatus status;
    uint32_t rawData = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];

    status.controllerFault = (rawData >> 31) & 0x01;
    status.ipdFreqFault = (rawData >> 29) & 0x01;
    status.ipdT1Fault = (rawData >> 28) & 0x01;
    status.abnSpeed = (rawData >> 22) & 0x01;
    status.lossOfSync = (rawData >> 21) & 0x01;
    status.noMtr = (rawData >> 20) & 0x01;
    status.mtrLck = (rawData >> 19) & 0x01;
    status.cbcIlimit = (rawData >> 18) & 0x01;
    status.lockIlimit = (rawData >> 17) & 0x01;
    status.mtrUnderVoltage = (rawData >> 16) & 0x01;
    status.mtrOverVoltage = (rawData >> 15) & 0x01;
    status.stlEn = (rawData >> 2) & 0x01;
    status.stlStatus = (rawData >> 1) & 0x01;
    status.appReset = rawData & 0x01;

    return status;
}



GateDriverFaultStatus get_gate_driver_fault_status(uint8_t deviceAddress) {
    int file = open(I2C_DEVICE, O_RDWR);
    if (file < 0) {
        throw std::runtime_error("Failed to open the I2C bus.");
    }

    if (ioctl(file, I2C_SLAVE, deviceAddress) < 0) {
        close(file);
        throw std::runtime_error("Failed to set the I2C device address.");
    }

    uint8_t data[4];
    readRegister(file, deviceAddress, MCT8329A_GATE_DRIVER_FAULT_STATUS, data, 4, false);

    close(file);

    std::cout << "GATE_DRIVER_FAULT_STATUS Data in binary: ";
    for (size_t i = 0; i < 4; ++i) {
        std::bitset<8> bits(data[i]);
        std::cout << bits << " ";
    }
    std::cout << std::endl;

    GateDriverFaultStatus status;
    uint32_t rawData = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];

    status.driverFault = (rawData >> 31) & 0x01;
    status.pwrOn = (rawData >> 30) & 0x01;
    status.ocpVdsFault = (rawData >> 28) & 0x01;
    status.ocpSnsFault = (rawData >> 27) & 0x01;
    status.bstUvFault = (rawData >> 26) & 0x01;
    status.gvddUvFlt = (rawData >> 25) & 0x01;
    status.drvOff = (rawData >> 24) & 0x01;

    return status;
}




int main() {
    try {
        uint8_t deviceAddress = MCT8329A_I2C_ADDRESS;

        std::vector<uint32_t> registerAddresses = {0x01, 0x02, 0x03};  // Example register addresses
        size_t length = 4;  // Number of bytes to read from each register
        bool crcEnabled = false;  // Set to true if CRC is enabled

        readControllerFaultStatus(deviceAddress, length, crcEnabled);
        readMultipleRegisters(deviceAddress, registerAddresses, length, crcEnabled);
        // Read and print the CONTROLLER_FAULT_STATUS register
        readAndPrintStatusRegister(deviceAddress, MCT8329A_CONTROLLER_FAULT_STATUS, "CONTROLLER_FAULT_STATUS");

        // Read and print additional status registers
        readAndPrintStatusRegister(deviceAddress, MCT8329A_GATE_DRIVER_FAULT_STATUS, "GATE_DRIVER_FAULT_STATUS");
        readAndPrintStatusRegister(deviceAddress, MCT8329A_SYSTEM_STATUS1, "SYSTEM_STATUS1");
        readAndPrintStatusRegister(deviceAddress, MCT8329A_SYSTEM_STATUS2, "SYSTEM_STATUS2");
        readAndPrintStatusRegister(deviceAddress, MCT8329A_SYSTEM_STATUS3, "SYSTEM_STATUS3");

        SysStatus1 status1 = get_sys_status1(deviceAddress);
        std::cout << "voltMag: " << status1.voltMag << std::endl;
        std::cout << "speedCmd: " << status1.speedCmd << std::endl;
        std::cout << "i2cEntryStatus: " << status1.i2cEntryStatus << std::endl;

        SysStatus2 sysStatus2 = get_sys_status2(deviceAddress);
        std::cout << "SYS_STATUS2 - STATE: " << static_cast<int>(sysStatus2.state) << std::endl;
        std::cout << "SYS_STATUS2 - STL_FAULT: " << (sysStatus2.stlFault ? "Fail" : "Pass") << std::endl;
        std::cout << "SYS_STATUS2 - MOTOR_SPEED: " << sysStatus2.motorSpeed << " Hz" << std::endl;

        SysStatus3 sysStatus3 = get_sys_status3(deviceAddress);
        std::cout << "SYS_STATUS3 - DC_BUS_CURR: " << sysStatus3.dcBusCurr << " A" << std::endl;
        std::cout << "SYS_STATUS3 - DC_BATT_POW: " << sysStatus3.dcBattPow << " W" << std::endl;


        ControllerFaultStatus controllerFaultStatus = get_controller_fault_status(deviceAddress);
        std::cout << "CONTROLLER_FAULT_STATUS - CONTROLLER_FAULT: " << (controllerFaultStatus.controllerFault ? "Detected" : "Not Detected") << std::endl;
        std::cout << "CONTROLLER_FAULT_STATUS - IPD_FREQ_FAULT: " << (controllerFaultStatus.ipdFreqFault ? "Detected" : "Not Detected") << std::endl;
        std::cout << "CONTROLLER_FAULT_STATUS - IPD_T1_FAULT: " << (controllerFaultStatus.ipdT1Fault ? "Detected" : "Not Detected") << std::endl;
        std::cout << "CONTROLLER_FAULT_STATUS - ABN_SPEED: " << (controllerFaultStatus.abnSpeed ? "Detected" : "Not Detected") << std::endl;
        std::cout << "CONTROLLER_FAULT_STATUS - LOSS_OF_SYNC: " << (controllerFaultStatus.lossOfSync ? "Detected" : "Not Detected") << std::endl;
        std::cout << "CONTROLLER_FAULT_STATUS - NO_MTR: " << (controllerFaultStatus.noMtr ? "Detected" : "Not Detected") << std::endl;
        std::cout << "CONTROLLER_FAULT_STATUS - MTR_LCK: " << (controllerFaultStatus.mtrLck ? "Detected" : "Not Detected") << std::endl;
        std::cout << "CONTROLLER_FAULT_STATUS - CBC_ILIMIT: " << (controllerFaultStatus.cbcIlimit ? "Detected" : "Not Detected") << std::endl;
        std::cout << "CONTROLLER_FAULT_STATUS - LOCK_ILIMIT: " << (controllerFaultStatus.lockIlimit ? "Detected" : "Not Detected") << std::endl;
        std::cout << "CONTROLLER_FAULT_STATUS - MTR_UNDER_VOLTAGE: " << (controllerFaultStatus.mtrUnderVoltage ? "Detected" : "Not Detected") << std::endl;
        std::cout << "CONTROLLER_FAULT_STATUS - MTR_OVER_VOLTAGE: " << (controllerFaultStatus.mtrOverVoltage ? "Detected" : "Not Detected") << std::endl;
        std::cout << "CONTROLLER_FAULT_STATUS - STL_EN: " << (controllerFaultStatus.stlEn ? "Enabled" : "Disabled") << std::endl;
        std::cout << "CONTROLLER_FAULT_STATUS - STL_STATUS: " << (controllerFaultStatus.stlStatus ? "Pass" : "Fail") << std::endl;
        std::cout << "CONTROLLER_FAULT_STATUS - APP_RESET: " << (controllerFaultStatus.appReset ? "Fail" : "Successful") << std::endl;


        GateDriverFaultStatus gateDriverFaultStatus = get_gate_driver_fault_status(deviceAddress);
        std::cout << "GATE_DRIVER_FAULT_STATUS - DRIVER_FAULT: " << (gateDriverFaultStatus.driverFault ? "Detected" : "Not Detected") << std::endl;
        std::cout << "GATE_DRIVER_FAULT_STATUS - PWR_ON: " << (gateDriverFaultStatus.pwrOn ? "Cleared" : "Detected") << std::endl;
        std::cout << "GATE_DRIVER_FAULT_STATUS - OCP_VDS_FAULT: " << (gateDriverFaultStatus.ocpVdsFault ? "Detected" : "Not Detected") << std::endl;
        std::cout << "GATE_DRIVER_FAULT_STATUS - OCP_SNS_FAULT: " << (gateDriverFaultStatus.ocpSnsFault ? "Detected" : "Not Detected") << std::endl;
        std::cout << "GATE_DRIVER_FAULT_STATUS - BST_UV_FAULT: " << (gateDriverFaultStatus.bstUvFault ? "Detected" : "Not Detected") << std::endl;
        std::cout << "GATE_DRIVER_FAULT_STATUS - GVDD_UV_FLT: " << (gateDriverFaultStatus.gvddUvFlt ? "Detected" : "Not Detected") << std::endl;
        std::cout << "GATE_DRIVER_FAULT_STATUS - DRV_OFF: " << (gateDriverFaultStatus.drvOff ? "Detected" : "Not Detected") << std::endl;



    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
