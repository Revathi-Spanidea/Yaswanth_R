#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

int main()
{
    int fd;
    struct termios options;
    char tx_buffer[100];
    char rx_buffer[100];

    fd = open("/dev/serial0", O_RDWR | O_NOCTTY);

    if (fd < 0) {
        perror("UART open failed");
        return -1;
    }

    tcgetattr(fd, &options);

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    tcsetattr(fd, TCSANOW, &options);

    while(1)
    {
        printf("Enter message: ");
        fgets(tx_buffer, sizeof(tx_buffer), stdin);

        write(fd, tx_buffer, strlen(tx_buffer));

        int n = read(fd, rx_buffer, sizeof(rx_buffer)-1);

        if(n > 0)
        {
            rx_buffer[n] = '\0';
            printf("Received: %s\n", rx_buffer);
        }
    }

    close(fd);
    return 0;
}

