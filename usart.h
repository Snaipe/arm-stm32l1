#ifndef _USART_H
# define _USART_H

void usart_setup(void);

void usart_print_s32(uint32_t usart, int32_t value);

void usart_print_float(uint32_t usart, float value);

void usart_send_str(uint32_t uart, const char *str);

#endif /* _USART_H */
