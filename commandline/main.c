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
	fprintf(stderr, "Usage: %s [-h]\n", name);
	exit(1);
}

int main(int argc, char **argv)
{
	usb_dev_handle *handle = NULL;
	int opt;
	int reset = 0;

	usb_init();

	while ((opt = getopt(argc, argv, "hR")) != -1) {
		switch (opt) {
		case 'h':
			usage(argv[0]);
			break;
                case 'R':
                        reset = 1;
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

	ksz_dump_port_status(handle);
	printf("\n");
	ksz_dump_dm(handle);
	printf("\n");
	ksz_dump_vlan(handle);

	usb_close(handle);

	return 0;
}
