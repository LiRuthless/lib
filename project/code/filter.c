#include "config.h"



void lowpass_init(LowPassFilter *filt, float alpha) {
    filt->last_output = 0.0f;
    filt->alpha = alpha;
}

float lowpass_update(LowPassFilter *filt, float input) {
    filt->last_output = filt->alpha * input + (1.0f - filt->alpha) * filt->last_output;
    return filt->last_output;
}

