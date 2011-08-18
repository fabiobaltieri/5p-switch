/*
 * Copyright 2011 Fabio Baltieri (fabio.baltieri@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 */

#include "defines.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "uart.h"

#define BAUD UART_SPEED
#include <util/setbaud.h>

#if defined (__AVR_ATmega168__)
#define UBRRL UBRR0L
#define UBRRH UBRR0H
#define RXEN RXEN0
#define TXEN TXEN0
#define UCSRA UCSR0A
#define UCSRB UCSR0B
#define RXC RXC0
#define UDRE UDRE0
#define UDR UDR0
#define U2X U2X0
#endif

FILE uart_stdout = FDEV_SETUP_STREAM((int (*)(char, FILE *))uart_putchar,
				     (int (*)(FILE *))uart_getchar,
				     _FDEV_SETUP_RW);

/* uart */

void uart_init (void)
{
  UBRRL = UBRRL_VALUE;
  UBRRH = UBRRH_VALUE;
  UCSRB = _BV(RXEN) | _BV(TXEN);
#if USE_2X
  UCSRA |= _BV(U2X);
#else
  UCSRA &= ~_BV(U2X);
#endif
}

int uart_poll (void)
{
  if (UCSRA & _BV(RXC))
    return 1;
  else
    return 0;
}

int uart_putchar (char ch)
{
  if (ch == '\n')
    uart_putchar('\r');
  loop_until_bit_is_set(UCSRA, UDRE);
  UDR = ch;
  return ch;
}

int uart_getchar (void)
{
  while (!(UCSRA & _BV(RXC)));
  return UDR;
}

int uart_puts (char * s)
{
  while (*s) {
    uart_putchar(*s++);
  }
  return 0;
}
