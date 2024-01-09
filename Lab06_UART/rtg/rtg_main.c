
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "usart.h"
#include "rtg_main.h"

#define LEN_ARRAY(x)		(sizeof(x)/sizeof(x[0]))
#define LEN_STR(x)			(sizeof(x)/sizeof(x[0]) - 1)
#define SIZE_BUFFER_PAGE 	10


extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart7;


void send_receive(
		UART_HandleTypeDef* const sender, UART_HandleTypeDef* const receiver,
		const uint8_t* const message, unsigned int len_msg, uint8_t* const buffer
) {
	for (int i = 0; i < len_msg; i++) {
		HAL_StatusTypeDef status = HAL_UART_Transmit(sender, message + i, 1, HAL_MAX_DELAY);
		if (status != HAL_OK) {
			printf("Failed to send data via UART: %d, error 0x%"PRIX32"\r\n", status, HAL_UART_GetError(sender));
		}
		status = HAL_UART_Receive(receiver, buffer + i, 1, HAL_MAX_DELAY);
		if (status != HAL_OK) {
			printf("Failed to receive data via UART: %d, error 0x%"PRIX32"\r\n", status, HAL_UART_GetError(receiver));
		}
	}
}

void q1_uart_ping_pong() {
	HAL_Delay(3000);
	printf("\r\n\r\n\r\nQ1 Ping pong on UARTs\r\n");
	const char message[] = "That's a UART message";
	char buffer1[128] = { '\0' };
	char buffer2[128] = { '\0' };
	while(1) {
		printf("UART 5 sends message: %s\r\n", message);
		send_receive(&huart5, &huart7, (uint8_t*) message, LEN_STR(message), (uint8_t*) buffer1);
		printf("UART 7 got   message: %s\r\n\r\n", buffer1);

		printf("UART 7 sends message: %s\r\n", buffer1);
		send_receive(&huart7, &huart5, (uint8_t*) buffer1, strlen(buffer1), (uint8_t*) buffer2);
		printf("UART 5 got   message: %s\r\n\r\n", buffer2);

		memset(buffer1, '\0', sizeof(buffer1));
		memset(buffer2, '\0', sizeof(buffer2));

		HAL_Delay(1000);
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
			printf("Failed to receive data via UART3: %d, error 0x%"PRIX32"\r\n", status, HAL_UART_GetError(&huart3));
		}
	}
}

void q2_uart_ping_pong_console() {
	printf("\r\n\r\n\r\nQ2 Ping pong on UARTs\r\n");
	char message[1024] = { '\0' };
	char buffer1[1024] = { '\0' };
	char buffer2[1024] = { '\0' };
	while(1) {
		get_console_message(message, LEN_STR(message));

		printf("UART 5 sends message: %s\r\n", message);
		send_receive(&huart5, &huart7, (uint8_t*) message, strlen(message), (uint8_t*) buffer1);
		printf("UART 7 got   message: %s\r\n\r\n", buffer1);

		printf("UART 7 sends message: %s\r\n", buffer1);
		send_receive(&huart7, &huart5, (uint8_t*) buffer1, strlen(buffer1), (uint8_t*) buffer2);
		printf("UART 5 got   message: %s\r\n\r\n", buffer2);

		memset(buffer1, '\0', sizeof(buffer1));
		memset(buffer2, '\0', sizeof(buffer2));
	}
}


uint8_t uart7_received = 0;
uint8_t uart5_received = 0;

void q3_uart_ping_pong_irq() {
	printf("\r\n\r\n\r\nQ3 Ping pong on UARTs\r\n");
	const char message[] = "That's a UART message";
	char buffer1[56] = { '\0' };
	char buffer2[56] = { '\0' };
	uint8_t do_send = 1;
	while(1) {
		if (do_send) {
			HAL_StatusTypeDef status = HAL_UARTEx_ReceiveToIdle_IT(&huart7, (uint8_t*) buffer1, LEN_STR(buffer1));
			if (status != HAL_OK) {
				printf("Failed to receiveIT on UART: %d, error 0x%"PRIX32"\r\n", status, HAL_UART_GetError(&huart7));
			}

			printf("UART 5 sends message: %s\r\n", message);
			status = HAL_UART_Transmit_IT(&huart5, (uint8_t*) message, LEN_STR(message));
			if (status != HAL_OK) {
				printf("Failed to sendIT on UART: %d, error 0x%"PRIX32"\r\n", status, HAL_UART_GetError(&huart5));
			}
			do_send = 0;
		}
		if (uart7_received) {
			uart7_received = 0;
			printf("UART 7 got   message: %s\r\n\r\n", buffer1);

			HAL_StatusTypeDef status = HAL_UARTEx_ReceiveToIdle_IT(&huart5, (uint8_t*) buffer2, LEN_STR(buffer2));
			if (status != HAL_OK) {
				printf("Failed to receiveIT on UART: %d, error 0x%"PRIX32"\r\n", status, HAL_UART_GetError(&huart5));
			}

			printf("UART 7 sends message: %s\r\n", buffer1);
			status = HAL_UART_Transmit_IT(&huart7, (uint8_t*) buffer1, strlen(buffer1));
			if (status != HAL_OK) {
				printf("Failed to sendIT on UART: %d, error 0x%"PRIX32"\r\n", status, HAL_UART_GetError(&huart7));
			}
		}
		if (uart5_received) {
			uart5_received = 0;
			printf("UART 5 got   message: %s\r\n\r\n", buffer2);

			memset(buffer1, '\0', sizeof(buffer1));
			memset(buffer2, '\0', sizeof(buffer2));

			HAL_Delay(1000);
			do_send = 1;
		}
	}
}

void q4_uart_ping_pong_irq_console() {
	printf("\r\n\r\n\r\nQ3 Ping pong on UARTs\r\n");
	char message[1024] = { '\0' };
	char buffer1[1024] = { '\0' };
	char buffer2[1024] = { '\0' };

	uint8_t do_send = 1;
	while(1) {
		if (do_send) {
			get_console_message(message, LEN_STR(message));

			HAL_StatusTypeDef status = HAL_UARTEx_ReceiveToIdle_IT(&huart7, (uint8_t*) buffer1, LEN_STR(buffer1));
			if (status != HAL_OK) {
				printf("Failed to receiveIT on UART: %d, error 0x%"PRIX32"\r\n", status, HAL_UART_GetError(&huart7));
			}

			printf("UART 5 sends message: %s\r\n", message);
			status = HAL_UART_Transmit_IT(&huart5, (uint8_t*) message, LEN_STR(message));
			if (status != HAL_OK) {
				printf("Failed to sendIT on UART: %d, error 0x%"PRIX32"\r\n", status, HAL_UART_GetError(&huart5));
			}
			do_send = 0;
		}
		if (uart7_received) {
			uart7_received = 0;
			printf("UART 7 got   message: %s\r\n\r\n", buffer1);

			HAL_StatusTypeDef status = HAL_UARTEx_ReceiveToIdle_IT(&huart5, (uint8_t*) buffer2, LEN_STR(buffer2));
			if (status != HAL_OK) {
				printf("Failed to receiveIT on UART: %d, error 0x%"PRIX32"\r\n", status, HAL_UART_GetError(&huart5));
			}

			printf("UART 7 sends message: %s\r\n", buffer1);
			status = HAL_UART_Transmit_IT(&huart7, (uint8_t*) buffer1, strlen(buffer1));
			if (status != HAL_OK) {
				printf("Failed to sendIT on UART: %d, error 0x%"PRIX32"\r\n", status, HAL_UART_GetError(&huart7));
			}
		}
		if (uart5_received) {
			uart5_received = 0;
			printf("UART 5 got   message: %s\r\n\r\n", buffer2);

			memset(buffer1, '\0', sizeof(buffer1));
			memset(buffer2, '\0', sizeof(buffer2));

			HAL_Delay(1000);
			do_send = 1;
		}
	}
}

void rtg_main(void) {
	// q1_uart_ping_pong();
	// q2_uart_ping_pong_console();
	// q3_uart_ping_pong_irq();
	q4_uart_ping_pong_irq_console();
}


// called when buffer is full
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if (huart == &huart5) {
		uart5_received = 1;
	}
	else if (huart == &huart7) {
		uart7_received = 1;
	}
	else {
		UNUSED(huart);
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t offset) {
	if (huart == &huart5) {
		uart5_received = 1;
	}
	else if (huart == &huart7) {
		uart7_received = 1;
	}
	else {
		UNUSED(huart);
		UNUSED(offset);
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
	if (huart == &huart5) {
		printf("Error on UART 5: 0x%"PRIX32"\r\n", HAL_UART_GetError(huart));
	}
	else if (huart == &huart7) {
		printf("Error on UART 7: 0x%"PRIX32"\r\n", HAL_UART_GetError(huart));
	}
	else {
		UNUSED(huart);
	}
}
