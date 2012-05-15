/* LEDs */

#define LED_A_PORT    PORTD
#define LED_A_DDR     DDRD
#define LED_A         PD6

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

/* SPI */

#define SPI_DDR  DDRB
#define SPI_PORT PORTB
#define SPI_SCK  PB5
#define SPI_MISO PB4
#define SPI_MOSI PB3
#define SPI_CS   PB2

#define spi_cs_h() SPI_PORT |=  _BV(SPI_CS)
#define spi_cs_l() SPI_PORT &= ~_BV(SPI_CS)
