#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/iwdg.h>
#include <libopencm3/stm32/usart.h>

#include "usart.h"

int main(void) {
	iwdg_set_period_ms(1000);
	iwdg_reset();
	iwdg_start();

	usart_clock_setup();
	usart_setup();
	gpio_setup();

	while (1) {
		usart_print_u16(USART1, -2000);
	}

	return 0;
}
