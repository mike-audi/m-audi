#ifndef USART_DRIVER_H
#define USART_DRIVER_H

#include "avr_compiler.h"

#define USART USARTD0

void initUSART(void);
void usartTX(char data);

#endif
