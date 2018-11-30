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

void disable_gpio(int gpio_pin){
  GPIO_REN0_ENABLE(gpio_pin);
  GPIO_REN1_ENABLE(gpio_pin);
  GPIO_FEN0_ENABLE(gpio_pin);
  GPIO_FEN1_ENABLE(gpio_pin);
  GPIO_HEN0_ENABLE(gpio_pin);
  GPIO_HEN1_ENABLE(gpio_pin);
  GPIO_LEN0_ENABLE(gpio_pin);
  GPIO_LEN1_ENABLE(gpio_pin);
  GPIO_AREN0_ENABLE(gpio_pin);
  GPIO_AREN1_ENABLE(gpio_pin);
  GPIO_AFEN0_ENABLE(gpio_pin);
  GPIO_AFEN1_ENABLE(gpio_pin);
  GPIO_PUD_ENABLE(gpio_pin);
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

  sleep(10);

  system("tvservice -p");
  system("fbset -depth 8 && fbset -depth 16");
  
  return;
}

// wifi&bluetooth disable
void wifi_and_bluetooth_disable(){
  int fd;
  
  char *disable_bt1 = "blacklist btbcm\n";
  char *disable_bt2 = "blacklist hci_uart\n";

  char *disable_wifi1 = "blacklist brcmfmac\n";
  char *disable_wifi2 = "blacklist brcmutil\n";

  fd = open("/etc/modprobe.d/raspi-blacklist.conf", O_RDWR|O_APPEND);

  if( fd == -1 ){
    perror("failed open ");
    return;
  }
  else{
    write(fd, disable_bt1, strlen(disable_bt1));
    write(fd, disable_bt2, strlen(disable_bt2));

    
    write(fd, disable_wifi1, strlen(disable_wifi1));
    write(fd, disable_wifi2, strlen(disable_wifi2)); 
    

    close(fd);
  }
  return;
}

void wifi_and_bluetooth_enable(){
  int fd;

  fd = open("/etc/modprobe.d/raspi-blacklist.conf", O_TRUNC);
  if( fd == -1 ){
    perror("failed open ");
    return;
  }
  printf("config initialization is done\n");
  return;
}

void usb_optimize(){
  USB_MDIOCTL_FREERUN_CLR;
}
void usb_unoptimize(){
  USB_MDIOCTL_FREERUN_SET;
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

//gpio disable test
//  int gpio_pin_number = 4;
//  for(int i = 1; i <= 54; i ++){
//    disable_gpio(i);
//  }
//  disable_gpio(gpio_pin_number);

// usb test  
//  usb_optimize();
//  usb_unoptimize();

// usb mdio freq read  
//  int usb_freq = check_usb_MDC_clock_freq();
//  printf("usb freq = %d\n", usb_freq);

// wifi and bluetooth test  
  wifi_and_bluetooth_disable();
//  wifi_and_bluetooth_enable();

// hdmi test
//  hdmi_disable();

  return 0;
}
