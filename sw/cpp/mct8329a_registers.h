#ifndef MCT8329A_REGISTERS_H
#define MCT8329A_REGISTERS_H

// Define the I2C address of the MCT8329A device
#define MCT8329A_I2C_ADDRESS 0x60

// Define register addresses
#define MCT8329A_CONTROL_REGISTER_1       0x00
#define MCT8329A_CONTROL_REGISTER_2       0x01
#define MCT8329A_STATUS_REGISTER          0x02
#define MCT8329A_CONTROLLER_FAULT_STATUS  0xE2
#define MCT8329A_SPEED_COMMAND_REGISTER   0x10
#define MCT8329A_TORQUE_COMMAND_REGISTER  0x11
#define MCT8329A_VOLTAGE_COMMAND_REGISTER 0x12
#define MCT8329A_CURRENT_COMMAND_REGISTER 0x13
#define MCT8329A_CONFIG_REGISTER_1        0x20
#define MCT8329A_CONFIG_REGISTER_2        0x21
#define MCT8329A_DIAGNOSTIC_REGISTER      0x30

// EEPROM Registers
#define MCT8329A_ISD_CONFIG               0x80
#define MCT8329A_MOTOR_STARTUP1           0x82
#define MCT8329A_MOTOR_STARTUP2           0x84
#define MCT8329A_CLOSED_LOOP1             0x86
#define MCT8329A_CLOSED_LOOP2             0x88
#define MCT8329A_CLOSED_LOOP3             0x8A
#define MCT8329A_CLOSED_LOOP4             0x8C
#define MCT8329A_CONST_SPEED              0x8E
#define MCT8329A_CONST_PWR                0x90
#define MCT8329A_FAULT_CONFIG1            0x92
#define MCT8329A_FAULT_CONFIG2            0x94
#define MCT8329A_150_DEG_TWO_PH_PROFILE   0x96
#define MCT8329A_150_DEG_THREE_PH_PROFILE 0x98
#define MCT8329A_REF_PROFILES1            0x9A
#define MCT8329A_REF_PROFILES2            0x9C
#define MCT8329A_REF_PROFILES3            0x9E
#define MCT8329A_REF_PROFILES4            0xA0
#define MCT8329A_REF_PROFILES5            0xA2
#define MCT8329A_REF_PROFILES6            0xA4
#define MCT8329A_PIN_CONFIG1              0xA6
#define MCT8329A_PIN_CONFIG2              0xA8
#define MCT8329A_DEVICE_CONFIG            0xAA
#define MCT8329A_GD_CONFIG1               0xAC
#define MCT8329A_GD_CONFIG2               0xAE

// RAM Registers
#define MCT8329A_GATE_DRIVER_FAULT_STATUS 0xE0
#define MCT8329A_SYSTEM_STATUS1            0xE4
#define MCT8329A_SYSTEM_STATUS2            0xEA
#define MCT8329A_SYSTEM_STATUS3            0xEC
#define MCT8329A_ALGO_CTRL1               0xE6
#define MCT8329A_DEVICE_CTRL              0xE8
#define MCT8329A_INPUT_DUTY               0x40C
#define MCT8329A_CURRENT_DUTY             0x512
#define MCT8329A_SET_DUTY                 0x522
#define MCT8329A_MOTOR_SPEED_PU           0x5CE
#define MCT8329A_DC_BUS_POWER_PU          0x714






struct SysStatus1 {
    float voltMag;        // Applied DC input voltage (VOLT_MAG / 10)
    float speedCmd;       // Decoded speed command (%)
    bool i2cEntryStatus;     // I2C entry status
};


struct SysStatus2 {
    uint8_t state;           // Current status of the state machine
    bool stlFault;           // STL fault status
    float motorSpeed;        // Speed output (electrical Hz)
};

struct SysStatus3 {
    float dcBusCurr;         // DC bus current (A)
    float dcBattPow;         // Battery (input) power (W)
};

struct ControllerFaultStatus {
    bool controllerFault;
    bool ipdFreqFault;
    bool ipdT1Fault;
    bool abnSpeed;
    bool lossOfSync;
    bool noMtr;
    bool mtrLck;
    bool cbcIlimit;
    bool lockIlimit;
    bool mtrUnderVoltage;
    bool mtrOverVoltage;
    bool stlEn;
    bool stlStatus;
    bool appReset;
};

struct GateDriverFaultStatus {
    bool driverFault;
    bool pwrOn;
    bool ocpVdsFault;
    bool ocpSnsFault;
    bool bstUvFault;
    bool gvddUvFlt;
    bool drvOff;
};


struct ISDConfig {
    bool parity;                // Parity bit
    bool isdEn;                 // ISD enable
    bool brakeEn;               // Brake enable
    bool hizEn;                 // Hi-Z enable
    bool rvsDrEn;               // Reverse Resynchronization enable
    bool resyncEn;              // Forward Resynchronization enable
    bool statBrkEn;             // Enable or disable brake during stationary
    uint8_t statDetectThr;      // Stationary BEMF detect threshold
    bool brkMode;               // Brake mode
    uint8_t brkTime;            // Brake time
    uint8_t hizTime;            // Hi-Z time
    uint8_t startupBrkTime;     // Brake time when motor is stationary
    uint8_t resyncMinThreshold; // Minimum phase BEMF below which the motor is coasted instead of resync
    uint8_t mtrStartup;         // Motor start-up method
};




#endif // MCT8329A_REGISTERS_H
