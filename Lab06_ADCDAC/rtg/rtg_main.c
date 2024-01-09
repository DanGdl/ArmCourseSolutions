#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "adc.h"
#include "dac.h"
#include "tim.h"
#include "gpio.h"
#include "usart.h"
#include "rtg_main.h"


extern ADC_HandleTypeDef hadc3;
extern DAC_HandleTypeDef hdac;
extern TIM_HandleTypeDef htim2;


#define LEN_ARRAY(x)		(sizeof(x)/sizeof(x[0]))
#define LEN_STR(x)			(sizeof(x)/sizeof(x[0]) - 1)
#define SIZE_BUFFER_PAGE 	10


uint8_t adc_conv_completed = 0;
uint8_t dac_conv_completed = 0;
uint8_t button_clicked = 0;


uint32_t dac_error = 0;
uint32_t adc_error = 0;

void check_errors() {
	if (dac_error) {
		dac_error = 0;
		printf("Error on DAC: 0x%"PRIX32"\r\n", HAL_DAC_GetError(&hdac));
	}
	if (adc_error) {
		adc_error = 0;
		printf("Error on ADC 3: 0x%"PRIX32"\r\n", HAL_ADC_GetError(&hadc3));
	}
}

void rtg_main(void) {
	int num = 1000;

	HAL_TIM_Base_Start_IT(&htim2);
	// Sets the DAC output according to num (0-4k)
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, num);

	// Enables DAC conversion
	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
	while (1) {
		check_errors();

		// When TIM2 triggers
		if (adc_conv_completed == 1) {
			 // Sample the DAC value using the ADC
			printf("DAC value = %d >> ADC value = %ld\n\r", num, HAL_ADC_GetValue(&hadc3));
			adc_conv_completed = 0;
		}
		if (button_clicked) {
			// When USER GPIO triggers
			num += 100;
			num %= 4000;
			printf("DAC value is: %d\n\r", num);
			// Sets the DAC output according to num (0-4k == 0v-3.3v)
			HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, num);
			button_clicked = 0;
		}
	}
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	button_clicked = 1;
}

// Timer interrupt Callback is triggered every 1 second
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	HAL_ADC_Start_IT(&hadc3);
}


void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac_) {
	if (hdac_ == &hdac) {
		dac_conv_completed = 1;
	} else {
		UNUSED(hdac_);
	}
}

void HAL_DAC_ErrorCallbackCh1(DAC_HandleTypeDef *hdac_) {
	if (hdac_ == &hdac) {
		dac_error = 1;
	} else {
		UNUSED(hdac_);
	}
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	if (hadc == &hadc3) {
		adc_conv_completed = 1;
	} else {
		UNUSED(hadc);
	}
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
	if (hadc == &hadc3) {
		adc_error = 1;
	} else {
		UNUSED(hadc);
	}
}
