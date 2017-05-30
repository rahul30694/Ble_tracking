#include "hal/delay.h"
#include "hal/time.h"
#include "boards.h"
#include "hal/uart.h"
#include "hal/gpio.h"

#include <stdio.h>
#include <stdint.h>

#define DELAY_MS_LED	100

volatile bool t = false;
void print()
{
	t = true;
}
int _main(void)
{
	gpio_output_cfg(LED_1);
	gpio_output_cfg(LED_2);
	gpio_output_cfg(LED_3);
	gpio_output_cfg(LED_4);
	
	uart_cfg(115200, false);
	gpio_write(LED_1, true);
	gpio_write(LED_2, true);
	gpio_write(LED_3, true);
	gpio_write(LED_4, true);

	timer_init();
	systick_init(100, print);

	while (true) {
		int led=-1;
		gpio_write(LED_1, false);

		if (t) {
			static uint32_t b = 0x00;
			printf("Time: %u\r\n", millis() - b);
			b = millis();
			t = false;
		}
		continue;
/*
    	printf("Enter LED number to Glow: ");
    	fflush(stdout);
    	scanf("%d", &led);
        gpio_write(LED_1, true);
        gpio_write(LED_2, true);
        gpio_write(LED_3, true);
        gpio_write(LED_4, true);

    	if (led == 1)
    		gpio_write(LED_1, false);
    	else if (led == 2)
    		gpio_write(LED_2, false);
    	else if (led == 3)
    		gpio_write(LED_3, false);
    	else if (led == 4)
    		gpio_write(LED_4, false);
    	else
    		printf("Wrong Input");
    	printf("%d\n", led);

       /*
        if (uart_is_available())
        	uart_putchar_unlocked(uart_getchar_unlocked());
       */
    }
}
