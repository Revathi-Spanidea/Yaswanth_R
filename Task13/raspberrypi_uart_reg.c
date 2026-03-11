#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/select.h>

#define PERIPHERAL_BASE 0xFE000000
#define GPIO_BASE  (PERIPHERAL_BASE + 0x200000)
#define UART0_BASE (PERIPHERAL_BASE + 0x201000)

#define BLOCK_SIZE 4096

volatile uint32_t *gpio;
volatile uint32_t *uart;

/* GPIO registers */
#define GPFSEL1   *(gpio + 1)
#define GPPUD     *(gpio + 37)
#define GPPUDCLK0 *(gpio + 38)

/* UART registers */
#define UART_DR   *(uart + 0)
#define UART_FR   *(uart + 6)
#define UART_IBRD *(uart + 9)
#define UART_FBRD *(uart + 10)
#define UART_LCRH *(uart + 11)
#define UART_CR   *(uart + 12)
#define UART_ICR  *(uart + 17)

void gpio_uart_init()
{
    GPFSEL1 &= ~((7<<12) | (7<<15));
    GPFSEL1 |=  (4<<12) | (4<<15);

    GPPUD = 0;
    usleep(5);

    GPPUDCLK0 = (1<<14) | (1<<15);
    usleep(5);

    GPPUDCLK0 = 0;
}

void uart_init()
{
    UART_CR = 0x0;

    UART_ICR = 0x7FF;

    UART_IBRD = 312;
    UART_FBRD = 32;

    UART_LCRH = (3<<5) | (1<<4);

    UART_CR = (1<<0) | (1<<8) | (1<<9);
}

void uart_send(char c)
{
    while (UART_FR & (1<<5));
    UART_DR = c;
}

char uart_receive()
{
    while (UART_FR & (1<<4));
    return UART_DR & 0xFF;
}

int main()
{
    int mem_fd;
    void *gpio_map;
    void *uart_map;

    fd_set readfds;

    mem_fd = open("/dev/mem", O_RDWR | O_SYNC);

    gpio_map = mmap(NULL, BLOCK_SIZE,
                    PROT_READ|PROT_WRITE,
                    MAP_SHARED,
                    mem_fd,
                    GPIO_BASE);

    uart_map = mmap(NULL, BLOCK_SIZE,
                    PROT_READ|PROT_WRITE,
                    MAP_SHARED,
                    mem_fd,
                    UART0_BASE);

    gpio = (volatile uint32_t *)gpio_map;
    uart = (volatile uint32_t *)uart_map;

    gpio_uart_init();
    uart_init();

    printf("UART bidirectional communication started\n");

    while(1)
    {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 10000;

        select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

        /* Keyboard → UART */

        if(FD_ISSET(STDIN_FILENO, &readfds))
        {
            char c = getchar();
            uart_send(c);
        }

        /* UART → Terminal */

        if(!(UART_FR & (1<<4)))
        {
            char c = UART_DR & 0xFF;
            printf("%c", c);
            fflush(stdout);
        }
    }

    return 0;
}
