#define F_CPU 16000000UL
#include "ultrasonic.h"
#include <util/delay.h>

volatile uint16_t echo_ticks = 0;
volatile uint8_t overflow_count = 0;
volatile uint8_t edge_state = 0;

static void Ultrasonic_TriggerPulse(void);
static void Timer2_Init(void);
void Ultrasonic_Init(void) {
	// Trigger: Output
	DDRB |= (1 << TRIGGER_PIN);
	PORTB &= ~(1 << TRIGGER_PIN);

	// Echo: Input
	DDRD &= ~(1 << ECHO_PIN);

	// Enable INT0 on rising edge
	MCUCR |= (1 << ISC00) | (1 << ISC01);
	GICR |= (1 << INT0);
	
	Timer2_Init();
	sei();
}

static void Ultrasonic_TriggerPulse(void) {
	PORTB |= (1 << TRIGGER_PIN);
	_delay_us(10);
	PORTB &= ~(1 << TRIGGER_PIN);
}

uint16_t Ultrasonic_ReadDistance(void) {
	Ultrasonic_TriggerPulse();
	_delay_ms(100);
	uint32_t total_ticks;

	cli();
	total_ticks = (overflow_count * 256UL + echo_ticks);
	sei();

	// Convert ticks to distance
	// Timer2 tick time = (Prescaler / F_CPU) = 4us per tick with prescaler 64
	// Speed of sound = 340 m/s = 0.034 cm/us
	// Distance (cm) = (time_us / 58)
	uint32_t time_us = total_ticks * 4;
	uint16_t distance_cm = time_us / 58;

	return distance_cm;
}

static void Timer2_Init(void) {
	TCCR2 = (1 << CS22);      // Prescaler = 64 → 4us per tick
	TCNT2 = 0;
	TIMSK |= (1 << TOIE2);    // Enable Timer2 overflow interrupt
}

// External Interrupt ISR
ISR(INT0_vect) {
	if (edge_state == 0) {
		// Rising edge: start measuring
		TCNT2 = 0;
		overflow_count = 0;
		edge_state = 1;

		MCUCR &= ~(1 << ISC01);  // Switch to falling edge
		MCUCR |= (1 << ISC00);
		} else {
		// Falling edge: stop measuring
		echo_ticks = TCNT2;
		edge_state = 0;

		MCUCR |= (1 << ISC01) | (1 << ISC00);  // Back to rising
	}
}

// Timer2 Overflow ISR
ISR(TIMER2_OVF_vect) {
	if (edge_state == 1) {
		overflow_count++;
	}
}
