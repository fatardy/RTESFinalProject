#include "mbed.h"
#include "GYRO_SENSOR.h"

Gyroscope::Gyroscope(SPI *spi_ptr, PinName ssel) {
    this->spi_ptr = spi_ptr;
    spi_ptr->frequency(10000000);
    spi_ptr->format(8,3);
    if(ssel == NC) {
        _spi_ssel_ptr = NULL;
    }else{
        _spi_ssel_ptr = new DigitalOut(ssel, 1);
    }
}

void Gyroscope::write_register(uint8_t reg, uint8_t val) {
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

uint8_t Gyroscope::read_register(uint8_t reg) {
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

void Gyroscope::update_register(uint8_t reg, uint8_t val, uint8_t mask) {
    uint8_t reg_val = read_register(reg);
    reg_val &= ~mask;
    val = val & mask;
    reg_val |= val;
    write_register(reg, reg_val);
}

int Gyroscope::init() {
    front = -1, rear = -1;
    is_queue_full = 0;
    average_angular_velocity = 0;
    queue_count = 0;
    zero_count = 0;

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

void Gyroscope::read_registers(uint8_t reg, uint8_t* data, uint8_t length) {
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

void Gyroscope::read_data_16(int16_t data[3]) {
    uint8_t raw_data[6];
    read_registers(OUT_X_L_ADDR, raw_data, 6);
    data[0] = (int16_t)(raw_data[1] << 8) + raw_data[0];
    data[1] = (int16_t)(raw_data[3] << 8) + raw_data[2];
    data[2] = (int16_t)(raw_data[5] << 8) + raw_data[4];
}

void Gyroscope::push(float angular_x)
{
    if(angular_x > 0){
        if(queue_count >= size && is_queue_full == 0)
        {
            is_queue_full = 1;
            front = 0;
        }
        if(!is_queue_full)
        {
            rear = rear + 1;
            queue[rear] = angular_x;
            average_angular_velocity = ((average_angular_velocity * queue_count) + angular_x)/(queue_count+1);
            queue_count++;  
            printf("1 : vel:%f qcnt:%d\n",average_angular_velocity,queue_count);
        }
        else{
            average_angular_velocity = ((average_angular_velocity * queue_count) - queue[front])/(queue_count-1);
            rear = front;
            front = (front + 1) % size;
            queue[rear] = angular_x;
            average_angular_velocity = ((average_angular_velocity * (queue_count-1)) + angular_x)/(queue_count);
            printf("2 : vel:%f qcnt:%d\n",average_angular_velocity,queue_count);
        }    
    }

}

float Gyroscope::average_Velocity(float angular_x)
{
    if(uint8_t(angular_x) == 0){
        zero_count++;
        if(zero_count >= 30)
            return 0;
        return average_angular_velocity;  
    }
    zero_count = 0;
    push(angular_x);
    return average_angular_velocity;
}