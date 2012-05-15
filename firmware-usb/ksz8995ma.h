/*
 * Register definitions for Micrel KSZ8995MA
 */

/* Chip ID Registers */
#define ID0 0x00 /* 7-0 family id */
#define ID1 0x01 /* 7-4 chip id, 3-1 revision id, 0 start switch */

/* Global Control Registers */
#define GC0 0x02
#define GC1 0x03 /* 2 aging enable, 1 fast age enable */
#define GC2 0x04 /* 6 broadcast storm protection, 1 legal max packet size check disable */
#define GC3 0x05 /* 7 VLAN enable, 1 tag mask, 0, sniff mode */
#define GC4 0x06 /* 3 null VID replacement */
#define GC5 0x07 /* broadcast storm protection rate */
#define GC9 0x0b /* 1 LED mode, 0 special TPID */

/* Port x Control Registers */
#define PxC0(x)  (0x10 + x * 0x10)
#define PxC1(x)  (0x11 + x * 0x10)
#define PxC2(x)  (0x12 + x * 0x10)
#define PxC3(x)  (0x13 + x * 0x10)
#define PxC4(x)  (0x14 + x * 0x10)
#define PxC5(x)  (0x15 + x * 0x10)
#define PxC6(x)  (0x16 + x * 0x10)
#define PxC7(x)  (0x17 + x * 0x10)
#define PxC8(x)  (0x18 + x * 0x10)
#define PxC9(x)  (0x19 + x * 0x10)
#define PxC10(x) (0x1a + x * 0x10)
#define PxC11(x) (0x1b + x * 0x10)
#define PxC12(x) (0x1c + x * 0x10)
#define PxC13(x) (0x1d + x * 0x10)
#define PxC14(x) (0x1f + x * 0x10)

/* Port x Status Registers */
#define PxS0(x) (0x1e + x * 0x10)

/* TOS Priority Control Registers */

/* MAC Address Registers */
#define MACA0 0x68 /* MAC[47-40] */
#define MACA1 0x69 /* MAC[39-32] */
#define MACA2 0x6a /* MAC[31-24] */
#define MACA3 0x6b /* MAC[23-16] */
#define MACA4 0x6c /* MAC[15-8] */
#define MACA5 0x6d /* MAC[7-0] */

/* Indirect Access Control Registers */
#define IAC0 0x6e /* 4 read/write cycle, 3-2 table select, 1-0 addr MSB */
                  /* tables: 00 static mac, 01 VLAN, 10 dynamic address, 11 MIB counters */
#define IAC1 0x6f /* addr LSB */

#define IAC_RD   (0x1 << 4) /* read cycle */
#define IAC_WR   (0x0 << 4) /* write cycle */

#define IAC_SM   (0x0 << 2) /* static MAC table */
#define IAC_VLAN (0x1 << 2) /* VLAN table */
#define IAC_DM   (0x2 << 2) /* dynamic MAC table */
#define IAC_MIB  (0x3 << 2) /* MIB table */

/* Indirect Data Registers */
#define IDR8 0x70 /* data [68-64] */
#define IDR7 0x71 /* data [63-56] */
#define IDR6 0x72 /* data [55-48] */
#define IDR5 0x73 /* data [47-40] */
#define IDR4 0x74 /* data [39-32] */
#define IDR3 0x75 /* data [31-24] */
#define IDR2 0x76 /* data [23-16] */
#define IDR1 0x77 /* data [15-8] */
#define IDR0 0x78 /* data [7-0] */
