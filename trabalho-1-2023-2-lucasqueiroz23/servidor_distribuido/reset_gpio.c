#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <bcm2835.h>

int main(int argc, char **argv) {
  if (!bcm2835_init()) {
    exit(1);
  }

  int pin[] = {2, 3, 4, 17, 27, 22, 10, 9, 11, 0, 5, 6, 13, 19, 26, 14, 15, 18, 23, 24, 25, 8, 7, 1, 12, 16, 20, 21};

  for (int i = 0; i < 28; i++) {
    bcm2835_gpio_fsel(pin[i], BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(pin[i], LOW);
  }

  bcm2835_close();
  return 0;
}
