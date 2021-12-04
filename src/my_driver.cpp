#include "mbed.h"
#include "my_driver.h"

L3GD20Gyroscope::L3GD20Gyroscope(SPI *spi_ptr, PinName ssel) {
    this->spi_ptr = spi_ptr;
    spi_ptr->frequency(10000000);
    spi_ptr->format(8,3);
    if(ssel == NC) {
        _spi_ssel_ptr = NULL;
    }else{
        _spi_ssel_ptr = new DigitalOut(ssel, 1);
    }
}

void L3GD20Gyroscope::write_register(uint8_t reg, uint8_t val) {
    reg = reg & 0x7F;
    if (_spi_ssel_ptr != NULL) {
        _spi_ssel_ptr->write(0);
    }
    spi_ptr->write(reg);
    spi_ptr->write(val);
    if (_spi_ssel_ptr != NULL) {
        _spi_ssel_ptr->write(1);
    }
}

uint8_t L3GD20Gyroscope::read_register(uint8_t reg) {
    uint8_t val;
    reg = reg | 0x80;
    if (_spi_ssel_ptr != NULL) {
        _spi_ssel_ptr->write(0);
    }
    spi_ptr->write(reg);
    val = spi_ptr->write(0x00);
    if (_spi_ssel_ptr != NULL) {
        _spi_ssel_ptr->write(1);
    }

    return val;
}

void L3GD20Gyroscope::update_register(uint8_t reg, uint8_t val, uint8_t mask) {
    uint8_t reg_val = read_register(reg);
    reg_val &= ~mask;
    val = val & mask;
    reg_val |= val;
    write_register(reg, reg_val);
}

int L3GD20Gyroscope::init() {
    uint8_t id;
    for (int i = 0; i < 3; i++) {
        id = read_register(WHO_AM_I_ADDR);
        if (id == Gyroscope_ID) {
            break;
        }
    }

    if(id != Gyroscope_ID){
        return -1;
    }
    update_register(CTRL_REG4_ADDR, 0x00, 0xC0);
    update_register(CTRL_REG5_ADDR, 0x03, 0x03);

    update_register(CTRL_REG3_ADDR, 0x00, 0x0F); //Disable interrupts

    update_register(CTRL_REG5_ADDR, 0x00, 0x40); //Disable FIFO
    update_register(FIFO_CTRL_REG_ADDR, 0x00, 0xE0); // configure FIFO bypass mode
    update_register(CTRL_REG4_ADDR, 0x00, 0x30); //Set Full Scale 250

    update_register(CTRL_REG5_ADDR, 0, 0x10); //High Pass filter disable

    update_register(CTRL_REG2_ADDR, 0, 0x0F); //Disable HPF mode config
    update_register(CTRL_REG1_ADDR, 0, 0x30); //Bandwidth Selection 00

    update_register(CTRL_REG1_ADDR, 0, 0xC0); //Output Data rate ODR :95hz i.e 0

    update_register(CTRL_REG1_ADDR, 0x0F, 0x0F); //Enable gyroscope ; power up

    return MBED_SUCCESS;
}

void L3GD20Gyroscope::read_registers(uint8_t reg, uint8_t* data, uint8_t length) {
        reg |= 0x60; // read multiple bytes
        reg |= 0x80; // read mode
        if (_spi_ssel_ptr != NULL) {
            _spi_ssel_ptr->write(0);
        }
        spi_ptr->write(reg); // send register address
        spi_ptr->write(NULL, 0, (char*)data, length); // read data
        if (_spi_ssel_ptr != NULL) {
            _spi_ssel_ptr->write(1);
        }
}

void L3GD20Gyroscope::read_data_16(int16_t data[3]) {
    uint8_t raw_data[6];
    read_registers(OUT_X_L_ADDR, raw_data, 6);
    data[0] = (int16_t)(raw_data[1] << 8) + raw_data[0];
    data[1] = (int16_t)(raw_data[3] << 8) + raw_data[2];
    data[2] = (int16_t)(raw_data[5] << 8) + raw_data[4];
}