#include "RPI.h"

//struct bcm2835_peripheral gpio = {GPIO_BASE};

// Exposes the physical address defined in the passed structure using mmap on /dev/mem
int map_peripheral(struct bcm2835_peripheral *p)
{
   // Open /dev/mem
   if ((p->mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("Failed to open /dev/mem, try checking permissions.\n");
      return -1;
   }

   p->map = mmap(
      NULL,
      BLOCK_SIZE,
      PROT_READ|PROT_WRITE,
      MAP_SHARED,
      p->mem_fd,      // File descriptor to physical memory virtual file '/dev/mem'
      p->addr_p       // Address in physical map that we want this memory block to expose
   );

   if (p->map == MAP_FAILED) {
        perror("mmap");
        return -1;
   }

   p->addr = (volatile unsigned int *)p->map;

   return 0;
}

void unmap_peripheral(struct bcm2835_peripheral *p) {

    munmap(p->map, BLOCK_SIZE);
    close(p->mem_fd);
}

void disable_gpio(int gpio_pin){
  GPIO_REN0_DISABLE(gpio_pin);
  GPIO_REN1_DISABLE(gpio_pin);
  GPIO_FEN0_DISABLE(gpio_pin);
  GPIO_FEN1_DISABLE(gpio_pin);
  GPIO_HEN0_DISABLE(gpio_pin);
  GPIO_HEN1_DISABLE(gpio_pin);
  GPIO_LEN0_DISABLE(gpio_pin);
  GPIO_LEN1_DISABLE(gpio_pin);
  GPIO_AREN0_DISABLE(gpio_pin);
  GPIO_AREN1_DISABLE(gpio_pin);
  GPIO_AFEN0_DISABLE(gpio_pin);
  GPIO_AFEN1_DISABLE(gpio_pin);
  GPIO_PUD_DISABLE(gpio_pin);
  return;
}

//HDMI disabling code
void hdmi_disable(){
/*
  pid_t pid;
  pid = fork();

  if(pid == 0){
    execl("/opt/vc/bin/tvservice -o", "-o", NULL);
  }
  if(pid == -1){
    perror("port error : ");
    exit(1);
  }*/
  system("tvservice -o");

  sleep(3);

  system("tvservice -p");
  system("fbset -depth 8 && fbset -depth 16");
  
  return;
}

//txt file input.... it is fail
void wifi_and_bluetooth_disable(){
  system("brcmfmac");
  system("brcmutil");

  return;
}

void usb_optimize(){
  USB_MDIOCTL_FREERUN_CLR;
}

int check_usb_MDC_clock_freq(){
  return USB_MDIOCTL_MDCRATIO_READ;
}


int main()
{
//mapping GPIO
  if(map_peripheral(&gpio) == -1){
    printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
    return -1;
  }

//mapping USB
  if(map_peripheral(&usb) == -1){
    printf("Failed to map the physical usb registers into the virtual memory space\n");
    return -1;
  }

  int gpio_pin_number = 4;
  disable_gpio(gpio_pin_number);
  
  usb_optimize();

  int usb_freq = check_usb_MDC_clock_freq();
  printf("usb freq = %d\n", usb_freq);
//  wifi_and_bluetooth_disable();

//  hdmi_disable();

/* Blink led..
  // Define pin 7 as output
  INP_GPIO(4);
  OUT_GPIO(4);

  while(1)
  {
    // Toggle pin 7 (blink a led!)
    GPIO_SET = 1 << 4;
    sleep(1);

    GPIO_CLR = 1 << 4;
    sleep(1);
  }
*/
  return 0;
}
