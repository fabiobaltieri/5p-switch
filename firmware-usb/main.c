#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usbdrv.h"

#include "requests.h"

#include "board.h"
#include "spi.h"
#include "ksz8995ma.h"

static uint8_t buf[64];

static void reset_cpu(void)
{
	wdt_disable();

	wdt_enable(WDTO_15MS);

	for (;;);
}

usbMsgLen_t usbFunctionSetup(uint8_t data[8])
{
	struct usbRequest *rq = (void *)data;

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
