#ifndef _config_h_
#define _config_h_

#include "zf_common_headfile.h"
#include "pid.h"
#include "motor.h"
#include "menu.h"
#include "adc.h"
#include "filter.h"


extern uint8 uart[32];
extern bit Start;
extern bit Run;
extern float diff;



void All_init(void);
void pit_speed (void);
void pit_track (void);

float float_abs(float a);


#endif
