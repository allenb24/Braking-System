#ifndef ULTRASONIC_H
#define ULTRASONIC_H

void ultrasonic_init(void);
void send_trigger_pulse(void);
void measure_echo_pulse(void);
void ultrsonic_get_distance_cm(int);
int ultrasonic_get_distance_in(int);

#endif
