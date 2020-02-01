#include "encoder.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define MAX_BRIGHTNESS 7

static void enable_fast_pwm(bool enable) {
    // Non-inverted fast PWM on OC0A (PB0)
    TCCR0A = enable ? (_BV(COM0A1) | _BV(WGM01) | _BV(WGM00)) : 0x00;
}

static int8_t set_brightness(int8_t v) {
    if (v <= 0) {
        enable_fast_pwm(false);
        return 0;        
    }

    enable_fast_pwm(true);
    if (v >= MAX_BRIGHTNESS) {
        OCR0A = 0xff;
        return MAX_BRIGHTNESS;
    }
    OCR0A = 1 << (v+1);
    return v;
}

int main() {
    // PB0 as output
    DDRB |= _BV(0);
    // prescaler = 8
    TCCR0B = _BV(CS01);

    encoder_init();
    sei();

    int8_t brightness = 0;
    while(1) {
        if (encoder_pushed()) {
            if (brightness == MAX_BRIGHTNESS)
                brightness = 0;
            else
                brightness = MAX_BRIGHTNESS;
        } else {
            brightness += encoder_read();
        }

        brightness = set_brightness(brightness);
    }
    return 0;
}
