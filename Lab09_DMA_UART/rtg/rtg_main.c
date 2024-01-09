
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "usart.h"
#include "rtg_main.h"

extern UART_HandleTypeDef huart3;

extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart7;


#define LEN_ARRAY(x)		(sizeof(x)/sizeof(x[0]))
#define LEN_STR(x)			(sizeof(x)/sizeof(x[0]) - 1)
#define SIZE_BUFFER_PAGE 	10


uint8_t uart5_received = 0;
uint8_t uart7_received = 0;

uint32_t uart5_error = 0;
uint32_t uart7_error = 0;

void check_errors() {
	if (uart5_error) {
		uart5_error = 0;
		printf("Error on UART 5: 0x%"PRIX32"\r\n", HAL_UART_GetError(&huart5));
	}
	if (uart7_error) {
		uart7_error = 0;
		printf("Error on UART 4: 0x%"PRIX32"\r\n", HAL_UART_GetError(&huart7));
	}
}

void rtg_main(void) {
	printf("\r\n\r\n\r\nQ1 Ping pong on UARTs\r\n");
	const char message[] = "That's a UART message";
	char buffer1[56] = { '\0' };
	char buffer2[56] = { '\0' };
	uint8_t do_send = 1;
	while(1) {
		check_errors();

		if (do_send) {
			HAL_StatusTypeDef status = HAL_UARTEx_ReceiveToIdle_DMA(&huart7, (uint8_t*) buffer1, LEN_STR(message));
			if (status != HAL_OK) {
				printf("Failed to receiveIT on UART: %d, error 0x%"PRIX32"\r\n", status, HAL_UART_GetError(&huart7));
			}

			printf("UART 5 sends message: %s\r\n", message);
			status = HAL_UART_Transmit_DMA(&huart5, (uint8_t*) message, LEN_STR(message));
			if (status != HAL_OK) {
				printf("Failed to sendIT on UART: %d, error 0x%"PRIX32"\r\n", status, HAL_UART_GetError(&huart5));
			}
			do_send = 0;
		}
		if (uart7_received) {
			uart7_received = 0;
			printf("UART 4 got   message: %s\r\n\r\n", buffer1);

			HAL_StatusTypeDef status = HAL_UARTEx_ReceiveToIdle_DMA(&huart5, (uint8_t*) buffer2, LEN_STR(buffer2));
			if (status != HAL_OK) {
				printf("Failed to receiveIT on UART: %d, error 0x%"PRIX32"\r\n", status, HAL_UART_GetError(&huart5));
			}

			printf("UART 4 sends message: %s\r\n", buffer1);
			status = HAL_UART_Transmit_DMA(&huart7, (uint8_t*) buffer1, strlen(buffer1));
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
		uart5_error = 1;
	}
	else if (huart == &huart7) {
		uart7_error = 1;
	}
	else {
		UNUSED(huart);
	}
}
