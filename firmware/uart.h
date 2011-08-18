void uart_init (void);
int uart_putchar (char ch);
int uart_getchar (void);
int uart_puts (char * s);
int uart_poll (void);

extern FILE uart_stdout;
