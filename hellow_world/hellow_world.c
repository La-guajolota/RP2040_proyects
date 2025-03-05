#include <stdio.h>
#include "pico/stdlib.h"


int main()
{
    stdio_init_all();

    while (true) {
        printf("Holaaa esto es la guajolota desde mi terminal de linux enviados desde mi Raspberry pi pico\n");
        sleep_ms(1000);
    }
}
