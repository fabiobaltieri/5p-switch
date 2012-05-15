#include "board.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usbdrv.h"

#include "requests.h"

static void reset_cpu(void)
{
	wdt_disable();

	wdt_enable(WDTO_15MS);

	for (;;);
}

usbMsgLen_t usbFunctionSetup(uint8_t data[8])
{
	struct usbRequest *rq = (void *)data;
	static uchar dataBuffer[4];

	led_b_toggle();

	if (rq->bRequest == CUSTOM_RQ_ECHO) {
		dataBuffer[0] = rq->wValue.bytes[0];
		dataBuffer[1] = rq->wValue.bytes[1];
		dataBuffer[2] = rq->wIndex.bytes[0];
		dataBuffer[3] = rq->wIndex.bytes[1];
		usbMsgPtr = dataBuffer;
		return 4;
	} else if (rq->bRequest == CUSTOM_RQ_SET_STATUS) {
		if (rq->wValue.bytes[0] & 0x01)
			led_a_on()
		else
			led_a_off()
	} else if (rq->bRequest == CUSTOM_RQ_GET_STATUS) {
		dataBuffer[0] = 0xca;
		usbMsgPtr = dataBuffer;
		return 1;
	} else if (rq->bRequest == CUSTOM_RQ_RESET) {
		reset_cpu();
	}
	return 0;
}

int __attribute__((noreturn)) main(void)
{
	uint8_t i;

	led_init();
	led_a_off();
	led_b_off();

	wdt_enable(WDTO_1S);

	usbInit();
	usbDeviceDisconnect();

	i = 0;
	while (--i) {
		wdt_reset();
		_delay_ms(1);
	}

	usbDeviceConnect();

	sei();
	for (;;) {
		wdt_reset();
		usbPoll();
	}
}
