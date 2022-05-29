#ifndef _PWM_H_
#define _PWM_H_

extern int initPwm(int initWiring);
extern int disablePwm(int pin);
extern int setPwm(int pin, int period, int highInterval);

#endif
