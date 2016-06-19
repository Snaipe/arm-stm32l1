#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/iwdg.h>
#include <libopencm3/stm32/usart.h>

#include "usart.h"

static int16_t hts221_get_temperature(int16_t T0_OUT,
				      int16_t T1_OUT,
				      int16_t T_OUT,
				      uint8_t T0_degC_x8,
				      uint8_t T1_degC_x8,
				      uint8_t T1_T0msb)
{
	/*
	 * Value is in 3rd and fourth bit,
	 *so we only need to shift this value 6 more bits
	 */
	double T0_degC = (T0_degC_x8 + (1 << 8) * (T1_T0msb & 0x03)) / 8.0;
	double T1_degC = (T1_degC_x8 + (1 << 6) * (T1_T0msb & 0x0C)) / 8.0;
	double T_degC = (T0_degC + (T_OUT - T0_OUT) *
			 (T1_degC - T0_degC) / (T1_OUT - T0_OUT));

	return (int16_t)T_degC;
}

static float hts221_get_humidity(int16_t H0_T0_OUT,
				   int16_t H1_T0_OUT,
				   int16_t H_OUT,
				   uint8_t H0_rH,
				   uint8_t H1_rH)
{
	double num = H0_rH + (H_OUT - H0_T0_OUT) * (H1_rH - H0_rH);
	double dem = H1_T0_OUT - H0_T0_OUT;
	double H_percent = num / dem;

	return (float)H_percent;
}

int main(void)
{
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
