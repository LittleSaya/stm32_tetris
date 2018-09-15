#include "adc_rand.h"
#include "systick.h"
#include "tetris.h"
#include "stdio.h"
#include <stdlib.h>

void initAdcRand(void)
{
    // 初始化 ADC
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB, ENABLE);

	//PB1 channel 9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// ADC1 工作模式配置
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // 单次转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_13Cycles5);
	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));
    
    // 生成并填充随机数种子
    adcRandFeedSeed();
}

void adcRandFeedSeed(void)
{
    u16 val, i;
    
    for (val = 0, i = 0; i < 3; ++i)
    {
        val += adcRandReadAdc();
        delayMs(100);
    }
    val /= 3;
    srand(val);
}

u16 adcRandReadAdc(void)
{
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	delayMs(5);
	return ADC_GetConversionValue(ADC1);
}

int adcRand(void)
{
    static int result;
    static s32 forward, adc;
    char buf[12];
    
    result = rand();
    
    adc = adcRandReadAdc();
    forward = ((adc >> 12) & 0x0000000F + (adc >> 8) & 0x0000000F + (adc >> 4) & 0x0000000F + adc & 0x000F) & 0x0000000F;
    for (; forward >= 0; --forward)
    {
        rand();
    }
    
    return result;
}
