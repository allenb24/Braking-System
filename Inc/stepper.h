#ifndef STEPPER_H
#define STEPPER_H

#include <stdint.h>

// Initializes stepper motor
void stepper_init(void);

/**
 * Rotates the stepper motor.
 * @param steps     Number of steps to rotate
 * @param direction 1 = forward (CW); 0 = backward (CCW)
 * @param delay_ms  Delay between steps in milliseconds (controls speed)
 */
void stepper_step(int steps, int dir, int delay_ms);

#endif
