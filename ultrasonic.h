#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#include <avr/io.h>
#include <avr/interrupt.h>

// Pin Definitions
#define TRIGGER_PIN     PB0
#define ECHO_PIN        PD2

// Function Prototypes
void Ultrasonic_Init(void);
uint16_t Ultrasonic_ReadDistance(void);


#endif /* ULTRASONIC_H_ */
