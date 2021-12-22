/**
  ******************************************************************************
  * @file    GYRO_SENSOR.h 
  * @author  Rituraj Singh, Aradhya Alamuru
  * @brief   Main File to interface with Gyroscope
  *          This file provides API's to configure and read/write gyroscope 
  ******************************************************************************
  **/
#include "mbed.h"

extern float speed;
extern float distanceTravelled;
// extern float gyroX;
// extern float gyroY;
// extern float gyroZ;
// extern float gyroX1;
extern float avg_speed;
extern bool debug_flag;
extern uint32_t secondsCount;

namespace l3gd20 {
class Gyroscope {

public:
    // Constructor to initialize SPI
    Gyroscope(SPI *spi_ptr, PinName ssel);

    // To initialize Gyroscope and configure CTR_REG1-5
    int init();

    enum Registers {
        WHO_AM_I_ADDR = 0x0F,
        CTRL_REG1_ADDR = 0x20,
        CTRL_REG2_ADDR = 0x21,
        CTRL_REG3_ADDR = 0x22,
        CTRL_REG4_ADDR = 0x23,
        CTRL_REG5_ADDR = 0x24,
        REFERENCE_REG_ADDR = 0x25,
        OUT_TEMP_ADDR = 0x26,
        STATUS_REG_ADDR = 0x27,
        OUT_X_L_ADDR = 0x28,
        OUT_X_H_ADDR = 0x29,
        OUT_Y_L_ADDR = 0x2A,
        OUT_Y_H_ADDR = 0x2B,
        OUT_Z_L_ADDR = 0x2C,
        OUT_Z_H_ADDR = 0x2D,
        FIFO_CTRL_REG_ADDR = 0x2E,
        FIFO_SRC_REG_ADDR = 0x2F,
        INT1_CFG_ADDR = 0x30,
        INT1_SRC_ADDR = 0x31,
        INT1_TSH_XH_ADDR = 0x32,
        INT1_TSH_XL_ADDR = 0x33,
        INT1_TSH_YH_ADDR = 0x34,
        INT1_TSH_YL_ADDR = 0x35,
        INT1_TSH_ZH_ADDR = 0x36,
        INT1_TSH_ZL_ADDR = 0x37,
        INT1_DURATION_ADDR = 0x38,
    };

    // This function will read value from the register address passed as param
    uint8_t read_register(uint8_t reg);

    // This function will Write value to the register address passed as param
    void write_register(uint8_t reg, uint8_t val);  

    // A wrapper function for write_register with a mask
    void update_register(uint8_t reg, uint8_t val, uint8_t mask);

    // This function will read 8-bit low data, left shit it by 8 and read and append
    // 8-bit high data. Result will be 16-bit data
    // Used to read OUT_X,OUT_Y and OUT_Z registers in dps
    void read_data_16(int16_t data[3]);

    // This function will read length bytes from register with address reg
    // Read values are stores in array passed as param
    void read_registers(uint8_t reg, uint8_t* data, uint8_t length);

    // Function to take care of STOP(i.e when user stops and walks again)
    // Also a wrapper function for putting samples in a circular queue
    float average_Velocity(float angular_x);

    // This function will push Samples into a Circular Queue
    // Once queue is full, old samples are de-queued and new samples
    // are en-queued at a run time
    void push(float angular_x);

    private:
        // Whoami Register
        static const uint8_t Gyroscope_ID = 0xD3;
        SPI *spi_ptr;    
        DigitalOut *_spi_ssel_ptr;

        // Circular queue size
        uint8_t size = 30;
        float queue[30];

        uint8_t front;
        uint8_t rear;
        bool is_queue_full;
        float average_angular_velocity;
        uint8_t queue_count;
        uint8_t zero_count;
    };
}

using l3gd20::Gyroscope;