#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/iwdg.h>

#define SAMPLING_TIME 1

static volatile uint32_t counter;
static volatile uint16_t periods;

void sys_tick_handler(void)
{
    counter = (counter + 1) % (1000 * SAMPLING_TIME);
    if (counter == 0)
        ++periods;
}

void reset_periods(void)
{
    periods = 0;
}

uint16_t get_periods(void)
{
    return periods;
}

void setup_clock(void)
{
    /* Setup clock source @ 32MHz */
    rcc_clock_setup_pll(&rcc_clock_config[1]);

    /* Setup sysclock to trigger every 1ms */
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
    systick_set_reload(3999);
    systick_interrupt_enable();
    systick_counter_enable();
}
