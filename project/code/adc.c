#include "config.h"

//uint16 adc1 = 0;
//uint16 adc2 = 0;
//uint16 adc3 = 0;
//uint16 adc4 = 0;

uint16 adc_raw[4] = {0};
uint32 adc_filted[4] = {0}; 

uint32 adc_sum[4] = {0};
uint16 max[4] = {0};
uint16 min[4] = {4095,4095,4095,4095};

uint8 i = 0;
uint8 j = 0;

float track_error(void)
{
	float diff = 0.0;
	diff = ((adc_filted[0] - adc_filted[3]) + (adc_filted[1] - adc_filted[2])) / ((adc_filted[0] + adc_filted[3]) + float_abs(adc_filted[1] - adc_filted[2]));
	return diff;
}

void read_adc(void)
{
	for( i = 0; i < 4; i++ )
	{
		max[i] = 0;
		min[i] = 4095;
		adc_sum[i] = 0;	
	}
	
//	adc_raw[4] = adc_convert(ADC_P16,ADC_12BIT);		//采集ADC_P05电压，右边竖向电感Y
	
	for( i = 0; i < 10; i++ ) 
	{
		adc_raw[0] = adc_convert(ADC_CHANNEL1); 
		adc_raw[1] = adc_convert(ADC_CHANNEL2);	
		adc_raw[2] = adc_convert(ADC_CHANNEL3);	
		adc_raw[3] = adc_convert(ADC_CHANNEL4);	
		
		for( j = 0; j < 4; j++ )
		{
			adc_sum[j] += adc_raw[j];
			if(max[j] < adc_raw[j]) max[j] = adc_raw[j];
			if(min[j] > adc_raw[j]) min[j] = adc_raw[j];	
		}
	}
	
	for( i = 0; i < 4; i++ )
	{
		adc_sum[i] -= max[i];
		adc_sum[i] -= min[i];
		adc_filted[i] = adc_sum[i] / 8;
	}
	
//	adc_data_filted[4] = adc_once(ADC_P16,ADC_12BIT);		//采集ADC_P05电压，右边竖向电感Y
//	round_sum = adc_data_filted[0] + adc_data_filted[1];             //环岛阈值
}

void front_adc_init(void)
{
	adc_init(ADC_CHANNEL1, ADC_12BIT);
	adc_init(ADC_CHANNEL2, ADC_12BIT);
    adc_init(ADC_CHANNEL3, ADC_12BIT);
	adc_init(ADC_CHANNEL4, ADC_12BIT);
}

//void read_adc(void)
//{
//	adc1 = adc_convert(ADC_CHANNEL1); 	
//	adc2 = adc_convert(ADC_CHANNEL2);	
//	adc3 = adc_convert(ADC_CHANNEL3);
//	adc4 = adc_convert(ADC_CHANNEL4);
//}

