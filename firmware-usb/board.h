/* LEDs */

#define LED_A_PORT    PORTC
#define LED_A_DDR     DDRC
#define LED_A         PC3

#define LED_B_PORT    PORTD
#define LED_B_DDR     DDRD
#define LED_B         PD7

#define led_a_on()     LED_A_PORT &= ~_BV(LED_A);
#define led_a_off()    LED_A_PORT |=  _BV(LED_A);
#define led_a_toggle() LED_A_PORT ^=  _BV(LED_A);

#define led_b_on()     LED_B_PORT &= ~_BV(LED_B);
#define led_b_off()    LED_B_PORT |=  _BV(LED_B);
#define led_b_toggle() LED_B_PORT ^=  _BV(LED_B);

#define led_init()				\
	do {					\
		LED_A_DDR |= _BV(LED_A);	\
		LED_B_DDR |= _BV(LED_B);	\
	} while (0);
