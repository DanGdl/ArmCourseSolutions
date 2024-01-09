
#include <stdio.h>

#include "usart.h"
#include "rtg_main.h"



void q1_traffic_light() {
	uint8_t lights_state = 3;
	const uint32_t delay_change = 4000;
	const uint32_t delay_blink = 500;
	while(1) {
		const uint32_t now = HAL_GetTick();
		if ((now % delay_change) == 0) {
			lights_state = (lights_state + 1) % 4;
			// red
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, lights_state == 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
			// blue
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, lights_state == 1 ?  GPIO_PIN_SET : GPIO_PIN_RESET);
			// green
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, lights_state == 2 ?  GPIO_PIN_SET : GPIO_PIN_RESET);
		}
		if (lights_state == 3 && (now % delay_blink) == 0) {
			// green
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
		}
	}
}

void q2_led_with_btn() {
	uint8_t handled = 0;
	uint32_t tick_start = 0;
	while(1) {
		if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) {
			if (handled) {
				continue;
			}
			if (tick_start == 0) {
				tick_start = HAL_GetTick();
			}
			else if(10 <= (HAL_GetTick() - tick_start)) {
				handled = 1;
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
			}
		}
		else if (tick_start != 0) {
			tick_start = 0;
			handled = 0;
		}
	}
}


void setup_leds(uint8_t state) {
	const uint8_t red_on	= state == 0 || state == 6 || state == 3 || 			  state == 5;
	const uint8_t blue_on	= state == 1 || state == 6 || state == 3 || state == 4;
	const uint8_t green_on	= state == 2 || state == 6 ||				state == 4 || state == 5;
	// red
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, red_on ? GPIO_PIN_SET : GPIO_PIN_RESET);
	// blue
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, blue_on ?  GPIO_PIN_SET : GPIO_PIN_RESET);
	// green
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, green_on ?  GPIO_PIN_SET : GPIO_PIN_RESET);
}

void q3_button_switch() {
	const uint8_t states = 8;
	uint8_t state = states - 1;
	uint8_t handled = 0;
	uint32_t tick_start = 0;
	while(1) {
		if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) {
			if (handled) {
				continue;
			}
			if (tick_start == 0) {
				tick_start = HAL_GetTick();
			}
			else if(10 <= (HAL_GetTick() - tick_start)) {
				handled = 1;
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);

				state = (state + 1) % states;
				setup_leds(state);
			}
		}
		else if (tick_start != 0) {
			tick_start = 0;
			handled = 0;
		}
	}
}

void q4_uart_write() {
	uint8_t handled = 0;
	uint32_t tick_start = 0;
	while(1) {
		if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) {
			if (handled) {
				continue;
			}
			if (tick_start == 0) {
				tick_start = HAL_GetTick();
			}
			else if(10 <= (HAL_GetTick() - tick_start)) {
				handled = 1;
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
				printf("Button clicked\r\n");
			}
		}
		else if (tick_start != 0) {
			if (handled) {
				printf("Button freed\r\n");
			}
			tick_start = 0;
			handled = 0;
		}
	}
}

void q5_uart_read() {
	while(1) {
		char c = 0;
		scanf("%c", &c);
		if (c == 'y' || c == 'Y') {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
		}
		else if (c == 'n' || c == 'N') {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
		}
	}
}

void q6_uart_leds() {
	const uint8_t states = 8;
	uint8_t state = states - 1;
	uint8_t handled = 0;
	uint32_t tick_start = 0;
	while(1) {
		if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) {
			if (handled) {
				continue;
			}
			if (tick_start == 0) {
				tick_start = HAL_GetTick();
			}
			else if(10 <= (HAL_GetTick() - tick_start)) {
				handled = 1;
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);

				state = (state + 1) % states;
				const uint8_t red_on	= state == 0 || state == 6 || state == 3 || 			  state == 5;
				const uint8_t blue_on	= state == 1 || state == 6 || state == 3 || state == 4;
				const uint8_t green_on	= state == 2 || state == 6 ||				state == 4 || state == 5;
				// red
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, red_on ? GPIO_PIN_SET : GPIO_PIN_RESET);
				// blue
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, blue_on ?  GPIO_PIN_SET : GPIO_PIN_RESET);
				// green
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, green_on ?  GPIO_PIN_SET : GPIO_PIN_RESET);

				printf("Red is %s, blue is %s, green is %s\r\n", red_on ? " ON" : "OFF", blue_on ? " ON" : "OFF", green_on ? " ON" : "OFF");
			}
		}
		else if (tick_start != 0) {
			tick_start = 0;
			handled = 0;
		}
	}
}

void q7_uart_btn_and_led() {
	uint8_t handled = 0;
	uint32_t tick_start = 0;
	while(1) {
		if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) {
			if (handled) {
				continue;
			}
			if (tick_start == 0) {
				tick_start = HAL_GetTick();
			}
			else if(10 <= (HAL_GetTick() - tick_start)) {
				handled = 1;
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
				printf("Button pushed, led is %s\r\n", HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) ? " ON" : "OFF");
			}
		}
		else if (tick_start != 0) {
			if (handled) {
				printf("Button freed, led is %s\r\n", HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) ? " ON" : "OFF");
			}
			tick_start = 0;
			handled = 0;
		}
	}
}

void rtg_main(void) {
	// q1_traffic_light();
	// q2_led_with_btn();
	// q3_button_switch();
	// q4_uart_write();
	// q5_uart_read();
	// q6_uart_leds();
	q7_uart_btn_and_led();
}
