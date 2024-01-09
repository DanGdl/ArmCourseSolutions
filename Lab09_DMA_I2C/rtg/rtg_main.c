
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "i2c.h"
#include "usart.h"
#include "rtg_main.h"

extern UART_HandleTypeDef huart3;
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c4;


#define LEN_ARRAY(x)		(sizeof(x)/sizeof(x[0]))
#define LEN_STR(x)			(sizeof(x)/sizeof(x[0]) - 1)
#define SIZE_BUFFER_PAGE 	10


uint8_t i2c1_received = 0;
uint8_t i2c4_received = 0;

uint32_t i2c1_error = 0;
uint32_t i2c4_error = 0;

void check_errors() {
	if (i2c1_error) {
		i2c1_error = 0;
		printf("Error on I2C 1: 0x%"PRIX32", state 0x%X\r\n", HAL_I2C_GetError(&hi2c1), HAL_I2C_GetState(&hi2c1));
	}
	if (i2c4_error) {
		i2c4_error = 0;
		printf("Error on I2C 4: 0x%"PRIX32", state 0x%X\r\n", HAL_I2C_GetError(&hi2c4), HAL_I2C_GetState(&hi2c4));
	}
}


void rtg_main(void) {
	printf("\r\n\r\n\r\nQ1 Ping pong on I2C\r\n");
	const char message[] = "That's a I2C message";
	char buffer1[128] = { '\0' };
	char buffer2[128] = { '\0' };
	const int len_msg = LEN_STR(message);
	uint8_t do_send = 1;
	while(1) {
		check_errors();
		if (do_send) {
			printf("I2C 1 sends message: %s\r\n", message);
			HAL_StatusTypeDef status = HAL_I2C_Slave_Receive_DMA(&hi2c4, (uint8_t*) buffer1, len_msg);
			if (status != HAL_OK) {
				printf("Failed to receiveIT on I2C: %d, error 0x%"PRIX32"\r\n", status, HAL_I2C_GetError(&hi2c4));
			}

			printf("I2C 1 sends message: %s\r\n", message);
			status = HAL_I2C_Master_Transmit_IT(&hi2c1, hi2c4.Init.OwnAddress1, (uint8_t*) message, len_msg);
			if (status != HAL_OK) {
				printf("Failed to sendIT on I2C: %d, error 0x%"PRIX32"\r\n", status, HAL_I2C_GetError(&hi2c1));
			}
			do_send = 0;
		}
		if (i2c4_received && HAL_I2C_STATE_READY == HAL_I2C_GetState(&hi2c4)) {
			i2c4_received = 0;
			printf("I2C 4 got   message: %s\r\n\r\n", buffer1);

			const int len_msg = strlen(buffer1);
			HAL_StatusTypeDef status = HAL_I2C_Slave_Receive_DMA(&hi2c1, (uint8_t*) buffer2, len_msg);
			if (status != HAL_OK) {
				printf("Failed to receiveIT on I2C: %d, error 0x%"PRIX32"\r\n", status, HAL_I2C_GetError(&hi2c1));
			}

			printf("I2C 4 sends message: %s\r\n", buffer1);
			status = HAL_I2C_Master_Transmit_DMA(&hi2c4, hi2c1.Init.OwnAddress1, (uint8_t*) buffer1, len_msg);
			if (status != HAL_OK) {
				printf("Failed to sendIT on I2C: %d, error 0x%"PRIX32"\r\n", status, HAL_I2C_GetError(&hi2c4));
			}
		}

		// clear buffers
		if (i2c1_received && HAL_I2C_STATE_READY == HAL_I2C_GetState(&hi2c1)) {
			i2c1_received = 0;
			printf("I2C 1 got   message: %s\r\n\r\n", buffer2);

			memset(buffer1, '\0', sizeof(buffer1));
			memset(buffer2, '\0', sizeof(buffer2));

			HAL_Delay(1000);
			do_send = 1;
		}
	}
}


// called when buffer is full
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c == &hi2c1) {
		i2c1_received = 1;
	}
	else if (hi2c == &hi2c4) {
		i2c4_received = 1;
	}
	else {
		UNUSED(hi2c);
	}
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c == &hi2c1) {
		i2c1_error = 1;
	}
	else if (hi2c == &hi2c4) {
		i2c4_error = 1;
	}
	else {
		UNUSED(hi2c);
	}
}
