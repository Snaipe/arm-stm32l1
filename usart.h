#ifndef _USART_H
# define _USART_H

void usart_clock_setup(void);

void usart_setup(void);

void gpio_setup(void);

void usart_print_u16(uint32_t usart, int16_t value);

#endif /* _USART_H */
