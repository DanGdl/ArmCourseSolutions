
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

void send_receive(
		I2C_HandleTypeDef* const sender, I2C_HandleTypeDef* const receiver,
		uint8_t* const receiver_flag,
		uint8_t* const message, unsigned int len_msg, uint8_t* const buffer
) {
	if (*receiver_flag) {
		HAL_StatusTypeDef status = HAL_I2C_Slave_Receive_IT(receiver, buffer, len_msg);
		if (status != HAL_OK) {
			printf("Failed to receiveIT on I2C: %d, error 0x%"PRIX32"\r\n", status, HAL_I2C_GetError(receiver));
		}
		*receiver_flag = 0;
	}
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(
			sender, receiver->Init.OwnAddress1, message, len_msg, HAL_MAX_DELAY
	);
	if (status != HAL_OK) {
		printf("Failed to send data via I2C: %d, error 0x%"PRIX32"\r\n", status, HAL_I2C_GetError(sender));
	}

	// wait till data received
	while(!(*receiver_flag));
}

void q1_I2C_ping_pong() {
	printf("\r\n\r\n\r\nQ1 Ping pong on I2C\r\n");
	const char message[] = "That's a I2C message";
	char buffer1[128] = { '\0' };
	char buffer2[128] = { '\0' };
	i2c1_received = 1;
	i2c4_received = 1;
	while(1) {
		check_errors();

		printf("I2C 1 sends message: %s\r\n", message);
		send_receive(
				&hi2c1, &hi2c4, &i2c4_received,
				(uint8_t*) message, LEN_STR(message),
				(uint8_t*) buffer1
		);
		printf("I2C 4 got   message: %s\r\n\r\n", buffer1);

		printf("I2C 4 sends message: %s\r\n", buffer1);
		send_receive(
				&hi2c4, &hi2c1, &i2c1_received,
				(uint8_t*) buffer1, strlen(buffer1),
				(uint8_t*) buffer2
		);
		printf("I2C 1 got   message: %s\r\n\r\n", buffer2);

		memset(buffer1, '\0', sizeof(buffer1));
		memset(buffer2, '\0', sizeof(buffer2));

		HAL_Delay(3000);
	}
}


void get_console_message(char* const buffer, unsigned int len_max) {
	uint32_t count_symbols = 0;
	printf("Please enter a message (max size 1024 symbols):\r\n");
	while (1) {
		uint8_t c;
		HAL_StatusTypeDef status = HAL_UART_Receive(&huart3, &c, 1, HAL_MAX_DELAY);
		if (c == '\r') {
			HAL_UART_Transmit(&huart3, &c, 1, HAL_MAX_DELAY);
			c = '\n';
			HAL_UART_Transmit(&huart3, &c, 1, HAL_MAX_DELAY);
			break;
		} else {
			HAL_UART_Transmit(&huart3, &c, 1, HAL_MAX_DELAY);
		}
		if (status == HAL_OK) {
			buffer[count_symbols] = c;
			count_symbols++;
			if (count_symbols == (len_max - 1)) {
				buffer[len_max] = '\0';
				break;
			}
		}
		else {
			printf("Failed to receive data to uart3: %d\r\n", status);
		}
	}
}

void q2_i2c_ping_pong_irq_console() {
	printf("\r\n\r\n\r\nQ3 Ping pong on I2C\r\n");
	char message[1024] = { '\0' };
	char buffer1[1024] = { '\0' };
	char buffer2[1024] = { '\0' };

	uint8_t do_send = 1;
	while(1) {
		check_errors();

		if (do_send) {
			get_console_message(message, LEN_STR(message));

			HAL_StatusTypeDef status = HAL_I2C_Slave_Receive_IT(&hi2c4, (uint8_t*) buffer1, LEN_STR(buffer1));
			if (status != HAL_OK) {
				printf("Failed to receiveIT on I2C: %d, error 0x%"PRIX32"\r\n", status, HAL_I2C_GetError(&hi2c4));
			}

			printf("I2C 1 sends message: %s\r\n", message);
			status = HAL_I2C_Master_Transmit_IT(&hi2c1, hi2c4.Init.OwnAddress1, (uint8_t*) message, strlen(message));
			if (status != HAL_OK) {
				printf("Failed to sendIT on I2C: %d, error 0x%"PRIX32"\r\n", status, HAL_I2C_GetError(&hi2c1));
			}
			do_send = 0;
		}

		if (i2c4_error && HAL_I2C_STATE_READY == HAL_I2C_GetState(&hi2c4)) {
			i2c4_received = 1;
			i2c4_error = 0;
		}
		// send data back, when it's arrived
		if (i2c4_received && HAL_I2C_STATE_READY == HAL_I2C_GetState(&hi2c4)) {
			i2c4_received = 0;
			printf("I2C 4 got   message: %s\r\n\r\n", buffer1);

			HAL_StatusTypeDef status = HAL_I2C_Slave_Receive_IT(&hi2c1, (uint8_t*) buffer2, LEN_STR(buffer2));
			if (status != HAL_OK) {
				printf("Failed to receiveIT on I2C: %d, error 0x%"PRIX32"\r\n", status, HAL_I2C_GetError(&hi2c1));
			}

			printf("I2C 4 sends message: %s\r\n", buffer1);
			status = HAL_I2C_Master_Transmit_IT(&hi2c4, hi2c1.Init.OwnAddress1, (uint8_t*) buffer1, strlen(buffer1));
			if (status != HAL_OK) {
				printf("Failed to sendIT on I2C: %d, error 0x%"PRIX32"\r\n", status, HAL_I2C_GetError(&hi2c4));
			}
		}

		if (i2c1_error && HAL_I2C_STATE_READY == HAL_I2C_GetState(&hi2c1)) {
			i2c1_received = 1;
			i2c1_error = 0;
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


void rtg_main(void) {
	q1_I2C_ping_pong();
	// q2_i2c_ping_pong_irq_console();
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
