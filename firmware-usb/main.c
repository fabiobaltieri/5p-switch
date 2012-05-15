#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usbdrv.h"

#include "requests.h"

#include "board.h"
#include "spi.h"
#include "ksz8995ma.h"

struct port_config {
	uint8_t ctrl[15];
	uint8_t status;
};

static uint8_t buf[512];

static void reset_cpu(void)
{
	wdt_disable();

	wdt_enable(WDTO_15MS);

	for (;;);
}

static uint16_t dump_vlan(uint8_t *buf, uint16_t size)
{
	uint16_t i;

	if (size < 16 * 4)
		return 0;

	for (i = 0; i < 16; i++) {
		spi_write(IAC_RD | IAC_VLAN | ((i >> 8) & 0x03), IAC0);
		spi_write(i & 0xff, IAC1);

		buf[3] = 0x00;
		buf[2] = spi_read(IDR2);
		buf[1] = spi_read(IDR1);
		buf[0] = spi_read(IDR0);

		buf += 4;
	}

	return 16 * 4;
}

static uint16_t dump_dm(uint16_t off, uint8_t *buf, uint16_t size)
{
	uint16_t count;
	uint16_t i;
	uint8_t data[2];

	/* get number of entries */
	spi_write(IAC_RD | IAC_DM | ((off >> 8) & 0x03), IAC0);
	spi_write((off & 0xff), IAC1);

	data[1] = spi_read(IDR8);
	data[0] = spi_read(IDR7);

	/* table empty */
	if (data[1] & 0x10)
		return 0;

	count = ((((data[1] << 8) | data[0]) >> 2) & 0x3ff) + 1;

	/* dump table */
	for (i = 0; i < size / 8; i++) {
		spi_write(IAC_RD | IAC_DM | ((off >> 8) & 0x03), IAC0);
		spi_write(off & 0xff, IAC1);

		do {
			buf[6] = spi_read(IDR6);
		} while (buf[6] & 0x80);

		buf[7] = spi_read(IDR7);
		buf[5] = spi_read(IDR5);
		buf[4] = spi_read(IDR4);
		buf[3] = spi_read(IDR3);
		buf[2] = spi_read(IDR2);
		buf[1] = spi_read(IDR1);
		buf[0] = spi_read(IDR0);

		buf += 8;
		off++;

		if (off > count)
			break;
	}

	return i * 8;
}

static uint8_t dump_port_status(uint8_t *buf, uint16_t size)
{
	struct port_config *pc;
	uint8_t i;

	if (size < sizeof(*pc) * 5)
		return 0;

	for (i = 0; i < 5; i++) {
		pc = (struct port_config *)(buf + sizeof(*pc) * i);

		pc->ctrl[0]  = spi_read(PxC0(i));
		pc->ctrl[1]  = spi_read(PxC1(i));
		pc->ctrl[2]  = spi_read(PxC2(i));
		pc->ctrl[3]  = spi_read(PxC3(i));
		pc->ctrl[4]  = spi_read(PxC4(i));
		pc->ctrl[5]  = spi_read(PxC5(i));
		pc->ctrl[6]  = spi_read(PxC6(i));
		pc->ctrl[7]  = spi_read(PxC7(i));
		pc->ctrl[8]  = spi_read(PxC8(i));
		pc->ctrl[9]  = spi_read(PxC9(i));
		pc->ctrl[10] = spi_read(PxC10(i));
		pc->ctrl[11] = spi_read(PxC11(i));
		pc->ctrl[12] = spi_read(PxC12(i));
		pc->ctrl[13] = spi_read(PxC13(i));
		pc->ctrl[14] = spi_read(PxC14(i));

		pc->status = spi_read(PxS0(i));
	}

	return sizeof(*pc) * 5;
}

usbMsgLen_t usbFunctionSetup(uint8_t data[8])
{
	struct usbRequest *rq = (void *)data;
	uint16_t ret;

	led_b_toggle();

	if ((rq->bmRequestType & USBRQ_RCPT_MASK) != USBRQ_RCPT_DEVICE &&
	    (rq->bmRequestType & USBRQ_TYPE_MASK) != USBRQ_TYPE_VENDOR)
		return 0;

	switch (rq->bRequest) {
	case CUSTOM_RQ_READ_REG:
		buf[0] = spi_read(rq->wIndex.bytes[0]);
		usbMsgPtr = buf;
		return 1;
	case CUSTOM_RQ_WRITE_REG:
		spi_write(rq->wValue.bytes[0], rq->wIndex.bytes[0]);
		return 0;
	case CUSTOM_RQ_GET_PORT_STATUS:
		ret = dump_port_status(buf, sizeof(buf));
		usbMsgPtr = buf;
		return ret;
	case CUSTOM_RQ_GET_DM:
		ret = dump_dm(rq->wIndex.word, buf, sizeof(buf));
		usbMsgPtr = buf;
		return ret;
	case CUSTOM_RQ_GET_VLAN:
		ret = dump_vlan(buf, sizeof(buf));
		usbMsgPtr = buf;
		return ret;
	case CUSTOM_RQ_RESET:
		reset_cpu();
		break;
	}

	return 0;
}

int __attribute__((noreturn)) main(void)
{
	uint8_t i;

	spi_init();

	led_init();
	led_a_off();
	led_b_off();

	wdt_enable(WDTO_1S);

	usbInit();
	usbDeviceDisconnect();

	for (i = 0; i < 6; i++) {
		wdt_reset();

		led_a_toggle();
		led_b_toggle();

		_delay_ms(50);
	}

	/* start switch */
	spi_write(0x01, ID1);

	usbDeviceConnect();

	sei();
	for (;;) {
		wdt_reset();
		usbPoll();
	}
}
