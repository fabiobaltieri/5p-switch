/* general */

#define F_CPU 8000000UL

#define UART_SPEED 38400

/* LED functions */
#define LED_PORT PORTD
#define LED_DDR  DDRD
#define LED_A    PORTD6
#define LED_B    PORTD7

#define led_a_on()     LED_PORT |=  _BV(LED_A)
#define led_a_off()    LED_PORT &= ~_BV(LED_A)
#define led_a_toggle() LED_PORT ^=  _BV(LED_A)

#define led_b_on()     LED_PORT |=  _BV(LED_B)
#define led_b_off()    LED_PORT &= ~_BV(LED_B)
#define led_b_toggle() LED_PORT ^=  _BV(LED_B)
