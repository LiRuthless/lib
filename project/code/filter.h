#ifndef _filter_h_
#define _filter_h_


typedef struct {
    float last_output;
    float alpha;   // ÂË²¨ÏµÊý 0~1
} LowPassFilter;



void lowpass_init(LowPassFilter *filt, float alpha);
float lowpass_update(LowPassFilter *filt, float input);


#endif
