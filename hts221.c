#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/iwdg.h>

#include "usart.h"

#define SENSOR_ADDR 0xBE

uint16_t hts221_read_sensor(uint8_t reg)
{
    uint32_t i2c = I2C2;
    uint16_t temperature;

    /* Send START condition. */
    i2c_send_start(i2c);

    /* Waiting for START is send and switched to master mode. */
    while (!((I2C_SR1(i2c) & I2C_SR1_SB)
        & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

    /* Say to what address we want to talk to. */
    /* Yes, WRITE is correct - for selecting register in STTS75. */
    i2c_send_7bit_address(i2c, SENSOR_ADDR, I2C_WRITE);

    /* Waiting for address is transferred. */
    while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));

    /* Cleaning ADDR condition sequence. */
    volatile uint32_t reg32 __attribute__((unused));
    reg32 = I2C_SR2(i2c);

    i2c_send_data(i2c, reg); /* sensor register */
    while (!(I2C_SR1(i2c) & (I2C_SR1_BTF | I2C_SR1_TxE)));

    /*
     * Now we transferred that we want to ACCESS the temperature register.
     * Now we send another START condition (repeated START) and then
     * transfer the destination but with flag READ.
     */

    /* Send START condition. */
    i2c_send_start(i2c);

    /* Waiting for START is send and switched to master mode. */
    while (!((I2C_SR1(i2c) & I2C_SR1_SB)
        & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

    /* Say to what address we want to talk to. */
    i2c_send_7bit_address(i2c, SENSOR_ADDR, I2C_READ); 

    /* 2-byte receive is a special case. See datasheet POS bit. */
    I2C_CR1(i2c) |= (I2C_CR1_POS | I2C_CR1_ACK);

    /* Waiting for address is transferred. */
    while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));

    /* Cleaning ADDR condition sequence. */
    reg32 = I2C_SR2(i2c);

    /* Cleaning I2C_SR1_ACK. */
    I2C_CR1(i2c) &= ~I2C_CR1_ACK;

    /* Now the slave should begin to send us the first byte. Await BTF. */
    while (!(I2C_SR1(i2c) & I2C_SR1_BTF));
    temperature = (uint16_t)(I2C_DR(i2c) << 8); /* MSB */

    /*
     * Yes they mean it: we have to generate the STOP condition before
     * saving the 1st byte.
     */
    I2C_CR1(i2c) |= I2C_CR1_STOP;

    temperature |= I2C_DR(i2c); /* LSB */

    /* Original state. */
    I2C_CR1(i2c) &= ~I2C_CR1_POS;

    iwdg_reset();

    return temperature;
}

uint8_t hts221_read_byte(uint8_t reg, uint32_t timeout)
{
    uint32_t i2c = I2C2;

    /* Send START condition. */
    i2c_send_start(i2c);

    /* Waiting for START is send and switched to master mode. */
    while (!((I2C_SR1(i2c) & I2C_SR1_SB)
        & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY)))) {
        if (--timeout == 0)
            return 0xff;
    }

    /* Say to what address we want to talk to. */
    /* Yes, WRITE is correct - for selecting register in STTS75. */
    i2c_send_7bit_address(i2c, SENSOR_ADDR, I2C_WRITE);

    /* Waiting for address is transferred. */
    while (!(I2C_SR1(i2c) & I2C_SR1_ADDR))
        if (--timeout == 0)
            return 0xff;

    /* Cleaning ADDR condition sequence. */
    volatile uint32_t reg32 __attribute__((unused));
    reg32 = I2C_SR2(i2c);

    i2c_send_data(i2c, reg); /* sensor register */
    while (!(I2C_SR1(i2c) & (I2C_SR1_BTF | I2C_SR1_TxE)))
        if (--timeout == 0)
            return 0xff;

    /*
     * Now we transferred that we want to ACCESS the temperature register.
     * Now we send another START condition (repeated START) and then
     * transfer the destination but with flag READ.
     */

    /* Send START condition. */
    i2c_send_start(i2c);

    /* Waiting for START is send and switched to master mode. */
    while (!((I2C_SR1(i2c) & I2C_SR1_SB)
        & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))))
        if (--timeout == 0)
            return 0xff;

    /* Say to what address we want to talk to. */
    i2c_send_7bit_address(i2c, SENSOR_ADDR, I2C_READ); 

    /* 2-byte receive is a special case. See datasheet POS bit. */
    I2C_CR1(i2c) |= (I2C_CR1_POS | I2C_CR1_ACK);

    /* Waiting for address is transferred. */
    while (!(I2C_SR1(i2c) & I2C_SR1_ADDR))
        if (--timeout == 0)
            return 0xff;

    /* Cleaning ADDR condition sequence. */
    reg32 = I2C_SR2(i2c);

    /* Cleaning I2C_SR1_ACK. */
    I2C_CR1(i2c) &= ~I2C_CR1_ACK;

    /* Now the slave should begin to send us the first byte. Await BTF. */
    while (!(I2C_SR1(i2c) & I2C_SR1_BTF))
        if (--timeout == 0)
            return 0xff;

    uint8_t byte = I2C_DR(i2c) & 0xff;

    I2C_CR1(i2c) |= I2C_CR1_STOP;

    /* Original state. */
    I2C_CR1(i2c) &= ~I2C_CR1_POS;

    i2c_send_stop(i2c);

    iwdg_reset();

    return byte;
}

float hts221_get_humidity(int16_t H0_T0_OUT,
				   int16_t H1_T0_OUT,
				   int16_t H_OUT,
				   uint8_t H0_rH,
				   uint8_t H1_rH)
{
	float num = H0_rH + (H_OUT - H0_T0_OUT) * (H1_rH - H0_rH);
	float dem = H1_T0_OUT - H0_T0_OUT;
	float H_percent = num / dem;

	return H_percent;
}

float hts221_get_temperature(int16_t T0_OUT,
                      int16_t T1_OUT,
                      int16_t T_OUT,
                      uint8_t T0_degC_x8,
                      uint8_t T1_degC_x8,
                      uint8_t T1_T0msb)
{
    /*
     * Value is in 3rd and fourth bit,
     * so we only need to shift this value 6 more bits
     */
    float T0_degC = (T0_degC_x8 + (1 << 8) * (T1_T0msb & 0x03)) / 8.0;
    float T1_degC = (T1_degC_x8 + (1 << 6) * (T1_T0msb & 0x0C)) / 8.0;
    float T_degC = (T0_degC + (T_OUT - T0_OUT) *
             (T1_degC - T0_degC) / (T1_OUT - T0_OUT));

    return T_degC;
}

void hts221_check_id(void)
{
    uint8_t id = hts221_read_byte(0xf, 1000000);
    if (id != 0xBC) {
        usart_send_str(USART1, "Unknown sensor ID ");
        usart_print_s32(USART1, id);
        usart_send_str(USART1, "!\r\n");
        for (;;);
    }
}

void hts221_setup(void)
{
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_I2C2);

    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10 | GPIO11);
    gpio_set_af(GPIOB, GPIO_AF4, GPIO10 | GPIO11);

    i2c_reset(I2C2);
    i2c_peripheral_disable(I2C2);
    i2c_set_fast_mode(I2C2);
    i2c_set_clock_frequency(I2C2, I2C_CR2_FREQ_32MHZ);

    /* Tlow = 1300ns inc Trise, Tcycle ~= 31ns => Tlow / Tcycle = 0x29 */
    i2c_set_ccr(I2C2, 0x33);

    /* Trise = 300ns, Tcycle ~= 31ns => Trise / Tcycle = 0xA, add 1 */
    i2c_set_trise(I2C2, 0xB);
    i2c_peripheral_enable(I2C2);
}
