#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <endian.h>

#include <usb.h>

#include "opendevice.h"

#include "../firmware-usb/requests.h"

#define VENDOR  "www.fabiobaltieri.com"
#define PRODUCT "5p-switch"

#define N_PORTS 5

struct port_config {
        uint8_t ctrl[15];
        uint8_t status;
};

static void ksz_dump_port_status(usb_dev_handle *handle)
{
	struct port_config ports[N_PORTS];
	int ret;
	int i;

	ret = usb_control_msg(handle,
			      USB_TYPE_VENDOR | USB_RECIP_DEVICE |
			      USB_ENDPOINT_IN,
			      CUSTOM_RQ_GET_PORT_STATUS,
			      0, 0, (char *)ports, sizeof(ports), 1000);

	if (ret < 0) {
		printf("usb_control_msg: %s\n", usb_strerror());
		exit(1);
	}

	if (ret != sizeof(ports)) {
		printf("%s: short transfer\n", __func__);
		return;
	}

	for (i = 0; i < N_PORTS; i++)
		printf("port%d: <%s%s%s%s%s%s%s%s>\n", i,
		       ports[i].status & 0x80 ? "MDI" : "MDI-X",
		       ports[i].status & 0x40 ? ",AN" : "",
		       ports[i].status & 0x20 ? ",LINK" : "",
		       ports[i].status & 0x10 ? ",FLOW" : "",
		       ports[i].status & 0x08 ? ",100BT-FD" : "",
		       ports[i].status & 0x04 ? ",100BT-HD" : "",
		       ports[i].status & 0x02 ? ",10BT-FD" : "",
		       ports[i].status & 0x01 ? ",10BT-HD" : "");
}

static void ksz_dump_dm(usb_dev_handle *handle)
{
	int ret;
	int i;
	uint8_t buf[512];
	uint8_t *ptr;
	uint16_t offset;

	printf("port\tage\tfid\taddr\n");

	offset = 0;
	do {
		ret = usb_control_msg(handle,
				      USB_TYPE_VENDOR | USB_RECIP_DEVICE |
				      USB_ENDPOINT_IN,
				      CUSTOM_RQ_GET_DM,
				      0, offset, (char *)buf, sizeof(buf), 1000);

		if (ret < 0) {
			printf("usb_control_msg: %s\n", usb_strerror());
			exit(1);
		}

		ptr = buf;
		for (i = 0; i < ret / 8; i++) {
			printf("%d\t%d\t%d\t%02x:%02x:%02x:%02x:%02x:%02x\n",
			       (ptr[6] >> 4) & 0x07,
			       ptr[7] & 0x03,
			       ptr[6] & 0x0f,
			       ptr[5],
			       ptr[4],
			       ptr[3],
			       ptr[2],
			       ptr[1],
			       ptr[0]);
			ptr += 8;
		}

		offset += ret / 8;
	} while (ret == sizeof(buf));
}

static void ksz_dump_vlan(usb_dev_handle *handle)
{
	int ret;
	int i;
	uint8_t buf[16 * 4];
	uint8_t *ptr;

	ret = usb_control_msg(handle,
			      USB_TYPE_VENDOR | USB_RECIP_DEVICE |
			      USB_ENDPOINT_IN,
			      CUSTOM_RQ_GET_VLAN,
			      0, 0, (char *)buf, sizeof(buf), 1000);

	if (ret < 0) {
		printf("usb_control_msg: %s\n", usb_strerror());
		exit(1);
	}

	if (ret != sizeof(buf)) {
		printf("%s: short transfer\n", __func__);
		return;
	}

	printf("valid\tmember\tfid\tvid\n");

	ptr = buf;
	for (i = 0; i < ret / 4; i++) {
		printf("%d\t%x\t%d\t%d\n",
		       (ptr[2] >> 5) & 0x01,
		       (ptr[2] >> 0) & 0x1f,
		       (ptr[1] >> 4) & 0x0f,
		       (ptr[1] << 8 | ptr[0]) & 0x0fff);
		ptr += 4;
	}
}

static char *mib_names[32] = {
	"RxLoPriorityByte",
	"RxHiPriorityByte",
	"RxUndersizePkt",
	"RxFragments",
	"RxOversize",
	"RxJabbers",
	"RxSymbolError",
	"RxCRCerror",
	"RxAlignmentError",
	"RxControl8808Pkts",
	"RxPausePkts",
	"RxBroadcast",
	"RxMulticast",
	"RxUnicast",
	"Rx64Octets",
	"Rx65to127Octets",
	"Rx128to255Octets",
	"Rx256to511Octets",
	"Rx512to1023Octets",
	"Rx1024to1522Octets",
	"TxLoPriorityByte",
	"TxHiPriorityByte",
	"TxLateCollision",
	"TxPausePkts",
	"TxBroadcastPkts",
	"TxMulticastPkts",
	"TxUnicastPkts",
	"TxDeferred",
	"TxTotalCollision",
	"TxExcessiveCollision",
	"TxSingleCollision",
	"TxMultipleCollision",
};

static void ksz_dump_mib(usb_dev_handle *handle)
{
	int ret;
	int i, j;
	uint32_t buf[N_PORTS][32];
	uint32_t val;

	for (i = 0; i < N_PORTS; i++) {
		ret = usb_control_msg(handle,
				      USB_TYPE_VENDOR | USB_RECIP_DEVICE |
				      USB_ENDPOINT_IN,
				      CUSTOM_RQ_GET_MIB,
				      0, i, (char *)buf[i], sizeof(*buf), 1000);

		if (ret < 0) {
			printf("usb_control_msg: %s\n", usb_strerror());
			exit(1);
		}

		if (ret != sizeof(*buf)) {
			printf("%s: short transfer\n", __func__);
			return;
		}
	}

	printf("                           port0"
	       "       port1"
	       "       port2"
	       "       port3"
	       "       port4\n");
	for (j = 0; j < 32; j++) {
		printf("%20s", mib_names[j]);
		for (i = 0; i < N_PORTS; i++) {
			val = buf[i][j];
			printf(" %s%10d",
			       val & 0x80000000 ? "*" : " ",
			       val & 0x3fffffff);
		}
		printf("\n");
	}
}

static void send_reset(usb_dev_handle *handle)
{
	int ret;

	ret = usb_control_msg(handle,
			      USB_TYPE_VENDOR | USB_RECIP_DEVICE |
			      USB_ENDPOINT_IN,
			      CUSTOM_RQ_RESET,
			      0, 0, NULL, 0, 1000);

	if (ret < 0) {
		printf("usb_control_msg: %s\n", usb_strerror());
		exit(1);
	}
}

static void usage(char *name)
{
	fprintf(stderr, "Usage: %s -h\n", name);
	fprintf(stderr, "       %s -R\n", name);
	fprintf(stderr, "       %s [options]\n", name);
	fprintf(stderr, "options\n");
	fprintf(stderr, "  -h         this help\n");
	fprintf(stderr, "  -R         reset device\n");
	fprintf(stderr, "  -p         dump port status\n");
	fprintf(stderr, "  -m         dump mib tables\n");
	fprintf(stderr, "  -v         dump vlan tables\n");
	fprintf(stderr, "  -d         dump dynamic MAC table\n");
	fprintf(stderr, "  -a         dump all (default)\n");
	exit(1);
}

#define DUMP_PORTS	0x01
#define DUMP_MIB	0x02
#define DUMP_VLAN	0x04
#define DUMP_DM		0x08
#define DUMP_ALL	0xff

int main(int argc, char **argv)
{
	usb_dev_handle *handle = NULL;
	int opt;
	int reset = 0;
	int dump_flags = 0;

	usb_init();

	while ((opt = getopt(argc, argv, "hRpmvda")) != -1) {
		switch (opt) {
		case 'h':
			usage(argv[0]);
			break;
                case 'R':
                        reset = 1;
                        break;
                case 'p':
			dump_flags |= DUMP_PORTS;
                        break;
                case 'm':
			dump_flags |= DUMP_MIB;
                        break;
                case 'v':
			dump_flags |= DUMP_VLAN;
                        break;
                case 'd':
			dump_flags |= DUMP_DM;
                        break;
                case 'a':
			dump_flags |= DUMP_ALL;
                        break;
		default:
			usage(argv[0]);
		}
	}

	if (optind < argc)
		usage(argv[0]);

	if (usbOpenDevice(&handle, 0, VENDOR, 0, PRODUCT, NULL, NULL, NULL) != 0) {
		fprintf(stderr, "error: could not find USB device \"%s\"\n", PRODUCT);
		exit(1);
	}

	if (reset) {
		send_reset(handle);
		return 0;
	}

	if (!dump_flags)
		dump_flags = DUMP_ALL;

	if (dump_flags & DUMP_PORTS) {
		ksz_dump_port_status(handle);
		printf("\n");
	}
	if (dump_flags & DUMP_MIB) {
		ksz_dump_mib(handle);
		printf("\n");
	}
	if (dump_flags & DUMP_VLAN) {
		ksz_dump_vlan(handle);
		printf("\n");
	}
	if (dump_flags & DUMP_DM) {
		ksz_dump_dm(handle);
		printf("\n");
	}

	usb_close(handle);

	return 0;
}
