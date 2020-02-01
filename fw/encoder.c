/*
    This code is inspired by Peter Dannegger's code:
    https://www.mikrocontroller.net/articles/Drehgeber#Solide_L.C3.B6sung:_Beispielcode_in_C
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "encoder.h"

static volatile int8_t enc_delta;
static int8_t last;
static struct
{
    volatile uint8_t press; // pending press
    uint8_t state;          // debounced state
    uint8_t cnt;
} button;

static inline bool phase_a(void)
{
    return !!(PINB & _BV(PIN3));
}

static inline bool phase_b(void)
{
    return !!(PINB & _BV(PIN4));
}

static inline bool button_active(void)
{
    return !(PINB & _BV(PIN2));
}

void encoder_init(void)
{
    enc_delta = 0;
    last = 0;

    DDRB &= ~(_BV(DDB2) | _BV(DDB3) | _BV(DDB4));
    PORTB |= (_BV(DDB2) | _BV(DDB3) | _BV(DDB4));

    if (phase_a())
        last = 3;
    if (phase_b())
        last ^= 1; // convert gray to binary

    // Enable overflow interrupt
    TIMSK0 |= _BV(TOIE0);
}

int8_t encoder_read(void)
{
    int8_t val;
    cli();
    val = enc_delta;
    enc_delta = val & ((1 << STEPS_PER_INDENT) - 1);
    sei();
    return val >> STEPS_PER_INDENT;
}

bool encoder_pushed(void)
{
    bool state;
    cli();
    state = button.press;
    button.press = 0;
    sei();
    return state;
}

ISR(TIM0_OVF_vect)
{
    int8_t diff;
    int8_t new = 0;
    if (phase_a())
        new = 3;
    if (phase_b())
        new ^= 1;      // convert gray to binary
    diff = last - new; // difference last - new
    if (diff & 1)
    {                                // bit 0 = value (1)
        last = new;                  // store new as next last
        enc_delta += (diff & 2) - 1; // bit 1 = direction (+/-)
    }

    if (button_active() != !!button.state)
    {
        if (++button.cnt == 0x3f)
        {
            button.state = !button.state;
            if (button.state)
                button.press = 1;
        }
    }
    else
        button.cnt = 0;
}
