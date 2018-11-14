#include <stdio.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h>

#define BCM2708_PERI_BASE       0x20000000
#define GPIO_BASE               (BCM2708_PERI_BASE + 0x200000)	// GPIO controller
#define USB_BASE                (BCM2708_PERI_BASE + 0x980000)  // usb controller

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x)
#define INP_GPIO(g)   *(gpio.addr + ((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g)   *(gpio.addr + ((g)/10)) |=  (1<<(((g)%10)*3))

#define GPIO_SET  *(gpio.addr + 7)
#define GPIO_CLR  *(gpio.addr + 10)

#define GPIO_READ(g)  *(gpio.addr + 13) &= (1<<(g))

// GPIO disable macros.
#define GPIO_REN0_DISABLE(g) *(gpio.addr + 19) &= (0<<(g))
#define GPIO_REN1_DISABLE(g) *(gpio.addr + 20) &= (0<<(g))

#define GPIO_FEN0_DISABLE(g) *(gpio.addr + 22) &= (0<<(g))
#define GPIO_FEN1_DISABLE(g) *(gpio.addr + 23) &= (0<<(g))

#define GPIO_HEN0_DISABLE(g) *(gpio.addr + 25) &= (0<<(g))
#define GPIO_HEN1_DISABLE(g) *(gpio.addr + 26) &= (0<<(g))

#define GPIO_LEN0_DISABLE(g) *(gpio.addr + 28) &= (0<<(g))
#define GPIO_LEN1_DISABLE(g) *(gpio.addr + 29) &= (0<<(g))
#define GPIO_AREN0_DISABLE(g) *(gpio.addr + 31) &= (0<<(g))
#define GPIO_AREN1_DISABLE(g) *(gpio.addr + 32) &= (0<<(g))

#define GPIO_AFEN0_DISABLE(g) *(gpio.addr + 34) &= (0<<(g))
#define GPIO_AFEN1_DISABLE(g) *(gpio.addr + 35) &= (0<<(g))

#define GPIO_PUD_DISABLE(g) *(gpio.addr + 37) &= (0<<(g))

//USB low power macros. USB MDIO CONTORL 20bit clear
#define USB_MDIOCTL_FREERUN_CLR *(usb.addr + 32) &= ~(1 << 20)
#define USB_MDIOCTL_MDCRATIO_READ *(usb.addr + 32) &= (15 << 16)
//(*(usb.addr + 32) & (15 << 16)) >> 16


#define BLOCK_SIZE 		(4*1024)

// IO Acces
struct bcm2835_peripheral {
    unsigned long addr_p;
    int mem_fd;
    void *map;
    volatile unsigned int *addr;
};

struct bcm2835_peripheral gpio = {GPIO_BASE};
struct bcm2835_peripheral usb = {USB_BASE};
extern struct bcm2835_peripheral gpio;
extern struct bcm2835_peripheral usb;
