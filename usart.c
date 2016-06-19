#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include "usart.h"

void usart_setup(void)
{
	/* We are running on MSI after boot. */
	/* Enable GPIOD clock for USART */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART1);

	/* Setup GPIO pins for USART1 transmit. */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);

	/* Setup USART1 TX pin as alternate function. */
	gpio_set_af(GPIOA, GPIO_AF7, GPIO9);

	/* Setup USART1 parameters. */
	usart_set_baudrate(USART1, 38400);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	usart_enable(USART1);
}

void usart_print_s32(uint32_t usart, int32_t value)
{
	int8_t i;
	int8_t nr_digits = 0;
	char buffer[11];

	if (value < 0) {
		usart_send_blocking(usart, '-');
		value = value * -1;
	}

	if (value == 0)
		usart_send_blocking(usart, '0');

	while (value > 0) {
		buffer[nr_digits++] = "0123456789"[value % 10];
		value /= 10;
	}

	for (i = nr_digits-1; i >= 0; i--)
		usart_send_blocking(usart, buffer[i]);

	usart_send_blocking(usart, '\r');
	usart_send_blocking(usart, '\n');
}

void usart_print_float(uint32_t usart, float value)
{
	uint8_t *ptr = (uint8_t *)&value;

	usart_send_blocking(usart, ptr[0]);
	usart_send_blocking(usart, ptr[1]);
	usart_send_blocking(usart, ptr[2]);
	usart_send_blocking(usart, ptr[3]);
}

void usart_send_str(uint32_t uart, const char *str)
{
    for (; *str; ++str)
        usart_send_blocking(uart, *str);
}

