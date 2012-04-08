
#ifndef PALMAP_H
#define PALMAP_H

#include "rhal.h"
#include "common.h"

struct palmap_pad_config {
	ioportid_t port;
	uint8_t pad;
	int8_t adc_in;
};

struct palmap_port_config {
	char port_name;
	uint8_t pad_offset;
};

extern const struct palmap_pad_config palmap_pads[];
extern const struct palmap_port_config palmap_ports[];

#define PM_PAD(gpid, pad)				{ (gpid), (pad), -1 }
#define PM_PAD_ADC(gpid, pad, adc_in)	{ (gpid), (pad), (adc_in) }

#define pmGetPortId(id)		(palmap_pads[id].port)
#define pmGetPad(id)		(palmap_pads[id].pad)
#define pmGetAdcChannel(id)	(palmap_pads[id].adc_in)

#define pmSetMode(pin, mode)		palSetPadMode(pmGetPortId(pin), pmGetPad(pin), mode)
#define pmDigitalRead(pin)			palReadPad(pmGetPortId(pin), pmGetPad(pin))
#define pmDigitalWrite(pin, val)	palWritePad(pmGetPortId(pin), pmGetPad(pin), val)

#ifdef __cplusplus
extern "C" {
#endif

	int pmGetPortOffset(char port_name);
	int pmPortToPin(char port_name, int pin);

	int pmAnalogStart(int pin, bool_t state);
	int pmAnalogReadLast(int pin);

#ifdef __cplusplus
}; /* extern */
#endif

#endif /* PALMAP_H */
