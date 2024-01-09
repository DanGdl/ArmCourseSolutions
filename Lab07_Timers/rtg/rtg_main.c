
#include <stdio.h>
#include <inttypes.h>

#include "tim.h"
#include "usart.h"
#include "rtg_main.h"

extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart3;


uint8_t elapsed_half = 0;
uint8_t elapsed_period = 0;

void q1_timer() {
	/*
	System timer 	- 16MHz
	prescaler 		- 1600
	period			- 10000

	16*10^6 / (16*10^2) = 10^4 MHz -> 0.0001 second
	10^(-4) * 10^4 = 1 second
	*/

	HAL_TIM_Base_Start_IT(&htim2);
	while(1) {
		if (elapsed_period) {
			printf("Elapsed\r\n");
			elapsed_period = 0;
		}
	}
}

void q2_timer_pwm() {
	HAL_TIM_Base_Start_IT(&htim2);
	// blue led or use any other pin
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	while(1) {
		if (elapsed_period) {
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
			elapsed_period = 0;
			printf("Led %s\r\n", HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) ? " ON" : "OFF");
		}
	}
}

void rtg_main(void) {
	// q1_timer();
	q2_timer_pwm();
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
	elapsed_period = 1;
}

void HAL_TIM_ErrorCallback(TIM_HandleTypeDef* htim) {
	printf("Timer error\n\r");
}
