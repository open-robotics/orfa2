
#include "ch.h"
#include "rhal.h"
#include "palmap.h"

#define ADC_PERIOD_MS	10
#define ADC_NUM			8

static VirtualTimer vtmr;

static adcsample_t adc_samples[ADC_NUM];
static int adc_sample_to_pin[ADC_NUM];
static size_t adc_sample_len = 0;

static void adc_cb(ADCDriver *adcp, adcsample_t *buffer, size_t n)
{
}

static ADCConversionGroup adc_grp_config = {
	FALSE,
	0,
	adc_cb,
	NULL,
	/* HW part */
	0,
	ADC_CR2_SWSTART,
	0,
	ADC_SMPR1_SMP_AN10(ADC_SAMPLE_55P5) |
		ADC_SMPR1_SMP_AN11(ADC_SAMPLE_55P5) |
		ADC_SMPR1_SMP_AN12(ADC_SAMPLE_55P5) |
		ADC_SMPR1_SMP_AN13(ADC_SAMPLE_55P5),
	ADC_SMPR2_SMP_AN0(ADC_SAMPLE_55P5) |
		ADC_SMPR2_SMP_AN1(ADC_SAMPLE_55P5) |
		ADC_SMPR2_SMP_AN2(ADC_SAMPLE_55P5) |
		ADC_SMPR2_SMP_AN3(ADC_SAMPLE_55P5),
	0,
	0
};


int pmGetPortOffset(char port_name)
{
	size_t i;

	for (i = 0; i < PALMAP_PORTS_SIZE; i++) {
		if (port_name == palmap_ports[i].port_name)
			return palmap_ports[i].pad_offset;
	}

	return -1;
}

int pmPortToPin(char port_name, int pin)
{
	int r = pmGetPortOffset(port_name);

	if (r == -1)
		return r;

	r += pin;
	return (r < PALMAP_PADS_SIZE)? r : -1;
}

int pmAnalogStart(int pin, bool_t state)
{
	size_t i;
	bool_t update_grp = FALSE;

	/* TODO: mutex */

	if (pmGetAdcChannel(pin) == -1)
		return -1;

	for (i = 0; i < adc_sample_len; i++) {
		if (adc_sample_to_pin[i] == pin)
			break;
	}

	if (i == adc_sample_len && state) {
		/* enable channel */
		adc_sample_to_pin[adc_sample_len++] = pin;

		update_grp = TRUE;
	}
	else if (!state) {
		/* disable channel */
		adc_sample_to_pin[i] = -1;
		adc_sample_len = 0;

		for (i = 0; i < ADC_NUM; i++) {
			if (adc_sample_to_pin[i] != -1)
				adc_sample_to_pin[adc_sample_len++] =
					adc_sample_to_pin[i];
		}

		update_grp = TRUE;
	}

	if (update_grp) {
		/* TODO */

		/* mark unused */
		for (i = adc_sample_len; i < ADC_NUM; i++)
			adc_sample_to_pin[i] = -1;

		chprintf(&SD1, "ADC pins:");
		for (i = 0; i < adc_sample_len; i++)
			chprintf(&SD1, " %d", adc_sample_to_pin[i]);
		chprintf(&SD1, "\n");
	}

	return 0;
}

int pmAnalogReadLast(int pin)
{
	size_t i;

	for (i = 0; i < adc_sample_len; i++) {
		if (adc_sample_to_pin[i] == pin)
			return adc_samples[i];
	}

	return -1;
}

