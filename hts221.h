#ifndef HTS221_H_
# define HTS221_H_

void hts221_setup(void);
void hts221_check_id(void);

uint16_t hts221_read_sensor(uint32_t i2c, uint8_t reg);
uint8_t hts221_read_byte(uint8_t reg, uint32_t timeout);

float hts221_get_humidity(int16_t H0_T0_OUT,
				   int16_t H1_T0_OUT,
				   int16_t H_OUT,
				   uint8_t H0_rH,
				   uint8_t H1_rH);

float hts221_get_temperature(int16_t T0_OUT,
                      int16_t T1_OUT,
                      int16_t T_OUT,
                      uint8_t T0_degC_x8,
                      uint8_t T1_degC_x8,
                      uint8_t T1_T0msb);

#endif /* !HTS221_H_ */
