#include <libopencm3/stm32/iwdg.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/i2c.h>

#include <libopencm3/cm3/systick.h>

#include "clock.h"
#include "usart.h"
#include "hts221.h"

#define TIMEOINF (~(uint32_t)-1)

#define ENABLE_SENSOR 0

int main(void)
{
    setup_clock();
    usart_setup();

    usart_send_str(USART1, "stm\r\n");

    /* Setup watchdog expiration after 1s w/o reset */
    iwdg_set_period_ms(1000);
    iwdg_reset();
    iwdg_start();

    hts221_setup();

    iwdg_reset();
#if ENABLE_SENSOR
    hts221_check_id();
#endif

    float temp = 0, humid = 0;
    uint16_t old_period = get_periods();
    while (1) {
        iwdg_reset();

        uint16_t p = get_periods();
        if (old_period == p)
            continue;

        old_period = p;

#if ENABLE_SENSOR
        int16_t t_out = hts221_read_sensor(I2C2, 0x2A);
        int16_t t0_out = hts221_read_sensor(I2C2, 0x3C);
        int16_t t1_out = hts221_read_sensor(I2C2, 0x3E);

        uint8_t t0_deg = hts221_read_byte(I2C2, 0x32, TIMEOINF);
        uint8_t t1_deg = hts221_read_byte(I2C2, 0x33, TIMEOINF);
        uint8_t t_msb = hts221_read_byte(I2C2, 0x35, TIMEOINF);

        temp = hts221_get_temperature(t0_out, t1_out, t_out, t0_deg, t1_deg, t_msb);
#else
        temp = 25.18;
#endif

#if ENABLE_SENSOR
        int16_t h_out = hts221_read_sensor(I2C2, 0x28);
        int16_t h0_out = hts221_read_sensor(I2C2, 0x36);
        int16_t h1_out = hts221_read_sensor(I2C2, 0x3A);

        uint8_t h0_rh = hts221_read_byte(I2C2, 0x30, TIMEOINF);
        uint8_t h1_rh = hts221_read_byte(I2C2, 0x31, TIMEOINF);

        humid = hts221_get_humidity(h0_out, h1_out, h_out, h0_rh, h1_rh, h_msb);
#else
        humid = 18.98;
#endif

        usart_print_float(USART1, temp);
        usart_print_float(USART1, humid);
        usart_send_str(USART1, "\r\n");
    }

    return 0;
}
