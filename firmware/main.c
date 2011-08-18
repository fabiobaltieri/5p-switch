#include "defines.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <string.h>

#include "uart.h"
#include "spi.h"
#include "commands.h"

#define MAXARGS 32
static uint8_t argc;
static char * argv[MAXARGS];

/*
 * Strings to argv and argc
 */
static void str_to_argc_argv (char * buf, uint8_t * argc, char ** argv, uint8_t maxargs)
{
  *argc = 0;

  argv[*argc] = strtok(buf, " \t");

  if (argv[*argc] == NULL)
      return;

  for (*argc = 1; *argc < MAXARGS; (*argc)++) {

    argv[*argc] = strtok(NULL, " \t");

    if (argv[*argc] == NULL)
      return;
  }
}

static void start_mode_A (void)
{
  //####################
  //# 5 -> 1,2 vid=100 #
  //# 5 -> 3,4 vid=200 #
  //####################

  char * vlan_a = "vlan_set 0 1 0x13 0 0x0064"; // vlan0: valid=1 membership=1,2,5 fid=0 vid=100
  str_to_argc_argv(vlan_a, &argc, argv, MAXARGS);
  execute(argc, argv);

  char * vlan_b = "vlan_set 1 1 0x1c 0 0x00c8"; // vlan1: valid=1 membership=3,4,5 fid=0 vid=200
  str_to_argc_argv(vlan_b, &argc, argv, MAXARGS);
  execute(argc, argv);

  spi_write(0x13, 0x00);
  spi_write(0x14, 0x64); // port1: vid=100
  spi_write(0x23, 0x00);
  spi_write(0x24, 0x64); // port2: vid=100
  spi_write(0x33, 0x00);
  spi_write(0x34, 0xc8); // port3: vid=200
  spi_write(0x43, 0x00);
  spi_write(0x44, 0xc8); // port4: vid=200
  spi_write(0x53, 0x00);
  spi_write(0x54, 0x01); // port5: vid=1

  spi_write(0x10, 0x02); // port 1, enable removal
  spi_write(0x12, 0x66); //         enable vlan filter and pvid filter
  spi_write(0x20, 0x02); // port 2, enable removal
  spi_write(0x22, 0x66); //         enable vlan filter and pvid filter
  spi_write(0x30, 0x02); // port 3, enable removal
  spi_write(0x32, 0x66); //         enable vlan filter and pvid filter
  spi_write(0x40, 0x02); // port 4, enable removal
  spi_write(0x42, 0x66); //         enable vlan filter and pvid filter
  spi_write(0x50, 0x04); // port 5, enable tag insertion
  spi_write(0x52, 0x46); //         enable vlan filter, disable pvid filter

  spi_write(0x07, 0x4e); // null VID replacement
  spi_write(0x05, 0x80); // VLAN mode enable

  spi_write(0x01, 0x01); // start switch
}

/*
 * Parse one command
 */
#define CMDBUFSZ 128
static void interactive (void)
{
  uint8_t i;
  char buf[CMDBUFSZ];
  char c;

  //start_mode_A();
  spi_write(0x01, 0x01); // start switch

  printf("sw> ");

  /* read a command */
  i = 0;
  while ((c = getchar())) {
    switch (c) {

    case '\n':
    case '\r':
      break;

    case '\b':
      if (i > 0) {
	i--;
	printf("\b \b");
      }
      break;

    default:
      buf[i] = c;
      if (i < CMDBUFSZ)
	i++;
      
      putchar(c);

    }

    if (c == '\r' || c == '\n')
      break;

    led_a_toggle();
  }

  led_b_toggle();

  buf[i] = '\0';

  putchar('\n');

  str_to_argc_argv(buf, &argc, argv, MAXARGS);

  if (argc > 0)
    execute(argc, argv);
}

/*
 * Entry routine
 */
int main (void)
{
  stdout = stdin = stderr = &uart_stdout;

  uart_init();
  spi_init();

  /* leds, all output, all off */
  LED_PORT |= _BV(LED_A) | _BV(LED_B);
  LED_DDR  |= _BV(LED_A) | _BV(LED_B);

  sei();

  _delay_ms(10);

  printf("\n\nAVR 5p-switch Started\n\n");

  for (;;)
    interactive();

  cli();

  return 0;
}
