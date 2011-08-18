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
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "uart.h"
#include "spi.h"
#include "commands.h"

#include "ksz8995ma.h"

static char sbuf[128];
#define printf_P(fmt, args...) strcpy_P(sbuf, PSTR(fmt)); printf(sbuf, ##args)

/*
 *** structures ***
 */

enum arg_type { NARGS_EQ, NARGS_MIN };

#define N_CMDS ( sizeof(commands) / sizeof(struct command) )

struct command {
  char * name;
  uint8_t nargs;
  enum arg_type nargs_type;
  uint8_t (* callback) (uint8_t, char **);
};

struct config {
};

/*
 *** raw commands ***
 */

static uint8_t raw_wr (uint8_t argc, char ** argv)
{
  uint8_t i;
  uint8_t addr;
  uint8_t val;

  addr = strtol(argv[1], NULL, 0);

  for (i = 2; i < argc; i++) {
    val = strtol(argv[i], NULL, 0);

    printf_P("0x%02x <- 0x%02x\n", addr, val);
    spi_write(addr, val);

    addr++;
  }

  return 0;
}

static uint8_t raw_rd (uint8_t argc, char ** argv)
{
  uint8_t i;
  uint8_t addr;
  uint8_t val;

  addr = strtol(argv[1], NULL, 0);

  for (i = strtol(argv[2], NULL, 0); i > 0; i--) {

    val = spi_read(addr);

    printf_P("0x%02x: 0x%02x - %c%c%c%c %c%c%c%c\n", addr, val,
	     (val & _BV(7)) ? '1' : '0',
	     (val & _BV(6)) ? '1' : '0',
	     (val & _BV(5)) ? '1' : '0',
	     (val & _BV(4)) ? '1' : '0',
	     (val & _BV(3)) ? '1' : '0',
	     (val & _BV(2)) ? '1' : '0',
	     (val & _BV(1)) ? '1' : '0',
	     (val & _BV(0)) ? '1' : '0'
	     );

    addr++;
  }
  return 0;
}

/*
 *** indirect tables ***
 */

static uint8_t sm_show (uint8_t argc, char ** argv)
{
  uint8_t data[8];
  uint16_t addr;
  uint16_t i;

  addr = strtol(argv[1], NULL, 0);

  for (i = strtol(argv[2], NULL, 0); i > 0; i--) {

    spi_write(IAC0, IAC_RD | IAC_SM | ((addr >> 8) & 0x03));
    spi_write(IAC1, (addr & 0xff));

    data[7] = spi_read(IDR7);
    data[6] = spi_read(IDR6);
    data[5] = spi_read(IDR5);
    data[4] = spi_read(IDR4);
    data[3] = spi_read(IDR3);
    data[2] = spi_read(IDR2);
    data[1] = spi_read(IDR1);
    data[0] = spi_read(IDR0);

    printf_P("SM Entry %4d: %02x %02x %02x %02x %02x %02x %02x %02x\n", addr,
	     data[7], data[6], data[5], data[4], data[3], data[2], data[1], data[0]);

    addr++;
  }

  return 0;
}

static uint8_t dm_show (uint8_t argc, char ** argv)
{
  uint8_t data[8];
  uint16_t count;
  uint16_t addr;

  /* get number of entries */

  addr = 0;

  spi_write(IAC0, IAC_RD | IAC_DM | ((addr >> 8) & 0x03));
  spi_write(IAC1, (addr & 0xff));
  
  data[1] = spi_read(IDR8);
  data[0] = spi_read(IDR7);

  if (data[1] & 0x10)
    return 0;

  count = ((((data[1] << 8) | data[0]) >> 2) & 0x3ff) + 1;
    
  printf_P("Total: %d entries\n", count);

  /* get all entries */

  for (; count > 0; count--) {

    spi_write(IAC0, IAC_RD | IAC_DM | ((addr >> 8) & 0x03));
    spi_write(IAC1, (addr & 0xff));

    do {
      data[6] = spi_read(IDR6);
    } while (data[6] & 0x80);
    data[7] = spi_read(IDR7);
    data[5] = spi_read(IDR5);
    data[4] = spi_read(IDR4);
    data[3] = spi_read(IDR3);
    data[2] = spi_read(IDR2);
    data[1] = spi_read(IDR1);
    data[0] = spi_read(IDR0);

    printf_P("%4d: age=%01x port=%01d fid=%02x mac=%02x:%02x:%02x:%02x:%02x:%02x\n", addr,
	     (data[7] >> 2) & 0x03,
	     ((data[6] >> 4) & 0x07) + 1,
	     (data[6] >> 0) & 0x0f,
	     data[5], data[4], data[3], data[2], data[1], data[0]);

    addr++;
  }

  return 0;
}

static uint8_t vlan_show (uint8_t argc, char ** argv)
{
  uint8_t data[3];
  uint16_t addr;
  uint16_t i;

  addr = 0;

  for (i = 16; i > 0; i--) {

    spi_write(IAC0, IAC_RD | IAC_VLAN | ((addr >> 8) & 0x03));
    spi_write(IAC1, (addr & 0xff));

    data[2] = spi_read(IDR2);
    data[1] = spi_read(IDR1);
    data[0] = spi_read(IDR0);

    printf_P("VLAN %2d: valid=%01x membership=%02x fid=%01x vid=%03x\n", addr,
	     (data[2] >> 5) & 0x01,
	     (data[2] >> 0) & 0x1f,
	     (data[1] >> 4) & 0x0f,
	     (data[1] << 8 | data[0]) & 0x0fff);

    addr++;
  }

  return 0;
}

static uint8_t vlan_set (uint8_t argc, char ** argv)
{
  uint8_t data[3];
  uint16_t addr;

  /* params: addr, valid, membership, fid, vid */
  addr = strtol(argv[1], NULL, 0);

  data[2] = ( (strtol(argv[2], NULL, 0) ? 0x20 : 0x00) |
	      (strtol(argv[3], NULL, 0) & 0x1f) );
  data[1] = ( ((strtol(argv[4], NULL, 0) << 4) & 0xf0) |
	      ((strtol(argv[5], NULL, 0) >> 8) & 0x0f) );
  data[0] = ( strtol(argv[5], NULL, 0) & 0xff );

  spi_write(IDR2, data[2]);
  spi_write(IDR1, data[1]);
  spi_write(IDR0, data[0]);

  spi_write(IAC0, IAC_WR | IAC_VLAN | ((addr >> 8) & 0x03));
  spi_write(IAC1, (addr & 0xff));

  return 0;
}

static uint8_t mib_show (uint8_t argc, char ** argv)
{
  uint8_t data[4];
  uint16_t addr;
  uint16_t i;

  addr = strtol(argv[1], NULL, 0);

  for (i = strtol(argv[2], NULL, 0); i > 0; i--) {

    spi_write(IAC0, IAC_RD | IAC_MIB | ((addr >> 8) & 0x03));
    spi_write(IAC1, (addr & 0xff));

    do {
      data[3] = spi_read(IDR3);
    } while (!(data[3] & 0x40));
    data[2] = spi_read(IDR2);
    data[1] = spi_read(IDR1);
    data[0] = spi_read(IDR0);

    printf_P("MIB %3x: ovf=%01x val=%02x%02x%02x%02x\n", addr,
	     (data[3] & 0x80) ? 1 : 0,
	     data[3] & 0x3f, data[2], data[1], data[0] );

    addr++;
  }

  return 0;
}

static uint8_t status (uint8_t argc, char ** argv)
{
  uint8_t i;
  uint8_t tmp[2];

  printf_P("Switch status:\n");

  tmp[0] = spi_read(ID0);
  tmp[1] = spi_read(ID1);
  printf_P("  chip family/id/rev: %02x/%01x/%01x\n",
	   tmp[0],
	   (tmp[1] >> 4) & 0x0f,
	   (tmp[1] >> 1) & 0x07);

  for (i = 0; i < 5; i++) {
    printf_P("Port %d: tag=%01x vlan_filter=%01x flags=%01x vid=%03x conf=%02x,%02x,%02x status=%02x\n",
	     i + 1,
	     (spi_read(P1C0 + 0x10 * i) >> 1) & 0x03,
	     (spi_read(P1C2 + 0x10 * i) >> 5) & 0x03,
	     (spi_read(P1C2 + 0x10 * i) >> 0) & 0x07,
	     spi_read(P1C4 + 0x10 * i) | spi_read(P1C3 + 0x10 * i) << 8,
	     spi_read(P1C12 + 0x10 * i),
	     spi_read(P1C13 + 0x10 * i),
	     spi_read(P1C14 + 0x10 * i),
	     spi_read(P1S0 + 0x10 * i)
	     );
  }

  return 0;
}

/*
 *** others ***
 */

#if TEST
static uint8_t test (uint8_t argc, char ** argv)
{
  uint8_t i;
  for (i = 0; i < argc; i++) {
    printf("[%2d] %s ", i, argv[i]);
    printf("0x%02lx", strtol(argv[i], NULL, 0));
    printf("\n");
  }

  return 0;
}
#endif

/*
 *** service ***
 */

static uint8_t help (uint8_t argc, char ** argv);

struct command commands[] = {
  {"help", 0, NARGS_EQ,  help},
#if TEST
  {"test", 2, NARGS_MIN, test},
#endif

  {"wr", 2, NARGS_MIN, raw_wr},
  {"rd", 2, NARGS_EQ,  raw_rd},

  {"status",    0, NARGS_EQ,  status},
  {"sm_show",   2, NARGS_EQ,  sm_show},
  {"dm_show",   0, NARGS_EQ,  dm_show},
  {"vlan_show", 0, NARGS_EQ,  vlan_show},
  {"vlan_set",  5, NARGS_EQ,  vlan_set},
  {"mib_show",  2, NARGS_EQ,  mib_show},
};

static uint8_t help (uint8_t argc, char ** argv)
{
  uint8_t i;

  printf_P("\nCommand list:\n");

  for (i = 0; i < N_CMDS; i++) {
    printf_P("  %s\n", commands[i].name);
  }

  printf_P("\n");

  return 0;
}

int execute (uint8_t argc, char ** argv)
{
  uint8_t i;

  for (i = 0; i < N_CMDS; i++) {

    if (!strcmp(argv[0], commands[i].name)) {

      if (commands[i].nargs_type == NARGS_EQ && argc - 1 != commands[i].nargs) {
	printf_P("error: command %s requres exactly %d arguments\n",
		 commands[i].name,
		 commands[i].nargs);
	return -2;
      } else if (commands[i].nargs_type == NARGS_MIN && argc - 1 < commands[i].nargs) {
	printf_P("error: command %s requres at least %d arguments\n",
		 commands[i].name,
		 commands[i].nargs);
	return -2;
      } else {
	return commands[i].callback(argc, argv);
      }

    }

  }
  
  printf_P("%s: command not found\n", argv[0]);

  return -1;
}
