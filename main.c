#include <stdio.h>
#include <string.h>
#include <math.h>
#include "avr.h"
#include "lcd.h"


void adc_init(void) {
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
}

uint16_t adc_read(void) {
	ADMUX = (ADMUX & 0xF8);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

float calculate_dB(uint16_t adc_value) {
	float dB = 31.14 * log10(adc_value / 128.06) + 109.93;
	return dB;
}

int main(void) {
	char bufone[16];
	char buftwo[16];
	char bufthree[16];
	avr_init();
	lcd_init();
	lcd_clr();
	lcd_pos(0, 0);
	lcd_puts2("Initializing...");
	avr_wait(1000);

	adc_init();
	DDRA &= ~(1 << PA1);
	PORTA |= (1 << PA1);

	while (1) {
		uint16_t adc_value = adc_read();
		float dB = calculate_dB(adc_value);
		
		if (adc_value < 7) {
			sprintf(bufone, "QUIET");
			} else if (adc_value > 30) {
			sprintf(bufone, "LOUD");
			} else {
			sprintf(bufone, "%.2f dB", dB);
		}
		
		int gate_status = PINA & (1 << PA1);
		sprintf(buftwo, "ENV:%d", adc_value);
		sprintf(bufthree,"GATE:%d", gate_status ? 1 : 0);
		lcd_clr();
		lcd_pos(0, 0);
		lcd_puts2(bufone);
		lcd_pos(1, 0);
		lcd_puts2(buftwo);
		lcd_pos(1, 10);
		lcd_puts2(bufthree);

		avr_wait(400);
	}
}