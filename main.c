#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/iwdg.h>

int main(void) {
    iwdg_set_period_ms(1000);
    iwdg_reset();
    iwdg_start();

    return 0;
}
