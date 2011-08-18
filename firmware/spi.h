void spi_init (void);
uint8_t spi_io (uint8_t data);
void spi_write_block (uint8_t addr, uint8_t * buf, uint8_t count);
void spi_read_block (uint8_t addr, uint8_t * buf, uint8_t count);
void spi_write (uint8_t addr, uint8_t data);
uint8_t spi_read (uint8_t addr);
