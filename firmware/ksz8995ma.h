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

/* Port 1 Control Registers */
#define P1C0  0x10 /* 7 broadacast storm protection, 5 802.1p priority, 4 port based priority */
                   /* 2 tag insertion, 1 tag removal, 0 priority enable */
#define P1C1  0x11 /* 4-0 port VLAN membership */
#define P1C2  0x12 /* 6 ingress VLAN filtering, 5 discard non-PVID, 2 txen, 1 rxen, 0 learnen */
#define P1C3  0x13 /* default tag[15:8] */
#define P1C4  0x14 /* default tag[7:0]  */
#define P1C5  0x15 /* tx high priority rate LSB */
#define P1C6  0x16 /* tx low priority rate LSB */
#define P1C7  0x17 /* tx priority rate MSB */
#define P1C8  0x18 /* rx high priority rate LSB */
#define P1C9  0x19 /* rx low priority rate LSB */
#define P1C10 0x1a /* rx priority rate MSB */
#define P1C11 0x1b /* priority... */
#define P1C12 0x1c /* 7 auto negotiation, 6 force speed, 5 force duplex, 4-0 advertise... */
#define P1C13 0x1d /* 7 LED, 3 power down, 2 disable auto mdi-x, 1 force mdi */
#define P1C14 0x1f
/* Port 2 Control Registers */
#define P2C0  0x20
#define P2C1  0x21
#define P2C2  0x22
#define P2C3  0x23
#define P2C4  0x24
#define P2C5  0x25
#define P2C6  0x26
#define P2C7  0x27
#define P2C8  0x28
#define P2C9  0x29
#define P2C10 0x2a
#define P2C11 0x2b
#define P2C12 0x2c
#define P2C13 0x2d
#define P2C14 0x2f
/* Port 3 Control Registers */
#define P3C0  0x30
#define P3C1  0x31
#define P3C2  0x32
#define P3C3  0x33
#define P3C4  0x34
#define P3C5  0x35
#define P3C6  0x36
#define P3C7  0x37
#define P3C8  0x38
#define P3C9  0x39
#define P3C10 0x3a
#define P3C11 0x3b
#define P3C12 0x3c
#define P3C13 0x3d
#define P3C14 0x3f
/* Port 4 Control Registers */
#define P4C0  0x40
#define P4C1  0x41
#define P4C2  0x42
#define P4C3  0x43
#define P4C4  0x44
#define P4C5  0x45
#define P4C6  0x46
#define P4C7  0x47
#define P4C8  0x48
#define P4C9  0x49
#define P4C10 0x4a
#define P4C11 0x4b
#define P4C12 0x4c
#define P4C13 0x4d
#define P4C14 0x4f
/* Port 5 Control Registers */
#define P5C0  0x50
#define P5C1  0x51
#define P5C2  0x52
#define P5C3  0x53
#define P5C4  0x54
#define P5C5  0x55
#define P5C6  0x56
#define P5C7  0x57
#define P5C8  0x58
#define P5C9  0x59
#define P5C10 0x5a
#define P5C11 0x5b
#define P5C12 0x5c
#define P5C13 0x5d
#define P5C14 0x5f


/* Port 1 Status Registers */
#define P1S0 0x1e /* 7 mdi-x status, 6 AN done, 6 link good, 4-0 partner flow control */
/* Port 2 Status Registers */
#define P2S0 0x2e
/* Port 3 Status Registers */
#define P3S0 0x3e
/* Port 4 Status Registers */
#define P4S0 0x4e
/* Port 5 Status Registers */
#define P5S0 0x5e

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
