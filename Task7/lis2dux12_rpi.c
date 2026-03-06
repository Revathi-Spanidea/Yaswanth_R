#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define I2C_DEVICE "/dev/i2c-1"
#define SLAVE_ADDR 0x18

#define CTRL1 0x10
#define CTRL3 0x12
#define CTRL4 0x13
#define CTRL5 0x14
#define AXIS_STR_ADDR 0x28

int main()
{
    int file;
    uint8_t buffer[6];
    int16_t x,y,z;

    file = open(I2C_DEVICE, O_RDWR);

    if(file < 0){
        perror("Failed to open I2C device");
        return -1;
    }

    if(ioctl(file, I2C_SLAVE, SLAVE_ADDR) < 0){
        perror("Failed to connect to sensor");
        return -1;
    }

    uint8_t config[2];

    // CTRL1 configuration
    config[0] = CTRL1;
    config[1] = 0x37;
    write(file, config, 2);

    // CTRL3
    config[0] = CTRL3;
    config[1] = 0x20;
    write(file, config, 2);

    // CTRL4
    config[0] = CTRL4;
    config[1] = 0x08;
    write(file, config, 2);

    // CTRL5
    config[0] = CTRL5;
    config[1] = 0x80;
    write(file, config, 2);

    usleep(50000);

    while(1)
    {
        uint8_t reg = AXIS_STR_ADDR | 0x80;

        write(file, &reg, 1);
        read(file, buffer, 6);

        x = (int16_t)(buffer[1]<<8 | buffer[0]);
        y = (int16_t)(buffer[3]<<8 | buffer[2]);
        z = (int16_t)(buffer[5]<<8 | buffer[4]);

        float ax = x * 0.061f / 1000.0f;
        float ay = y * 0.061f / 1000.0f;
        float az = z * 0.061f / 1000.0f;

        printf("X: %.3f g  Y: %.3f g  Z: %.3f g\n", ax, ay, az);

        sleep(1);
    }

    close(file);
    return 0;
}
