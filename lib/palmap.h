
#ifndef PALMAP_H
#define PALMAP_H

#include "rhal.h"
#include "common.h"

struct palmap_pad_config {
	ioportid_t port;
	uint16_t pad;
	int16_t adc_in;
};

struct palmap_port_config {
	char port_name;
	uint16_t pad_offset;
};

extern const struct palmap_pad_config palmap_pads[];
extern const struct palmap_port_config palmap_ports[];

#define PM_PAD(gpid, pad)				{ (gpid), (pad), -1 }
#define PM_PAD_ADC(gpid, pad, adc_in)	{ (gpid), (pad), (adc_in) }

#define palmapGetPortId(id)		(palmap_pads[id].port)
#define palmapGetPad(id)		(palmap_pads[id].pad)
#define palmapGetAdcChannel(id)	(palmap_pads[id].adc_in)

static inline int palmapGetPortOffset(char port_name)
{
	int i;

	for (i = 0; i < PALMAP_PORTS_SIZE; i++) {
		if (port_name == palmap_ports[i].port_name)
			return palmap_ports[i].pad_offset;
	}

	return -1;
}

#endif /* PALMAP_H */
