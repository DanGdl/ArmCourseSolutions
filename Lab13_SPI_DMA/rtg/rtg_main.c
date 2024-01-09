
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "spi.h"
#include "usart.h"
#include "rtg_main.h"


extern UART_HandleTypeDef huart3;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi4;

#define SPI_M &hspi1
#define SPI_S &hspi4


#define LEN_ARRAY(x)		(sizeof(x)/sizeof(x[0]))
#define LEN_STR(x)			(sizeof(x)/sizeof(x[0]) - 1)
#define SIZE_BUFFER_PAGE 	10


uint8_t spi1_received = 0;
uint8_t spi4_received = 0;


void q1_SPI_ping_pong() {
	printf("\r\n\r\n\r\nQ1 Ping pong on SPI\r\n");
	char message[] = "That's a SPI message";
	char buffer1[128] = { '\0' };
	char buffer2[128] = { '\0' };
	const unsigned int len_msg = LEN_STR(message);
	while(1) {
		HAL_StatusTypeDef status = HAL_SPI_Receive_IT(SPI_S, (uint8_t*) buffer1, len_msg);
		if (status != HAL_OK) {
			printf("Failed to receiveIT on SPI_S: %d, error 0x%"PRIX32"\r\n", status, HAL_SPI_GetError(SPI_S));
		}
		printf("SPI_M sends message: %s\r\n", message);
		status = HAL_SPI_Transmit(SPI_M, (uint8_t*) message, len_msg, HAL_MAX_DELAY);
		if (status != HAL_OK) {
			printf("Failed to send data on SPI_M: %d, error 0x%"PRIX32"\r\n", status, HAL_SPI_GetError(SPI_M));
		}
		// wait till data received
		while(!spi4_received);
		printf("SPI_S got   message: %s\r\n\r\n", buffer1);



		printf("SPI_S sends message: %s\r\n", buffer1);
//		status = HAL_SPI_TransmitReceive_IT(SPI_S, (uint8_t*) buffer1, (uint8_t*) buffer1, len_msg);
		status = HAL_SPI_Transmit_IT(SPI_S, (uint8_t*) buffer1, len_msg);
		if (status != HAL_OK) {
			printf("Failed to send data on SPI_S: %d, error 0x%"PRIX32"\r\n", status, HAL_SPI_GetError(SPI_S));
		}
		status = HAL_SPI_Receive(SPI_M, (uint8_t*) buffer2, len_msg, HAL_MAX_DELAY);
		if (status != HAL_OK) {
			printf("Failed to receiveIT on SPI_M: %d, error 0x%"PRIX32"\r\n", status, HAL_SPI_GetError(SPI_M));
		}
		printf("SPI_M got   message: %s\r\n\r\n", buffer2);

		memset(buffer1, '\0', sizeof(buffer1));
		memset(buffer2, '\0', sizeof(buffer2));

		HAL_Delay(3000);

		spi4_received = 0;
		spi1_received = 0;
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
		}
		else {
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

void printSPI3Flags() {
	uint32_t itsource = hspi4.Instance->CR2;
	uint32_t itflag   = hspi4.Instance->SR;
	printf("SPI3 flags: OVR %d, RXNE %d, IT_RXNE %d\r\n",
		(SPI_CHECK_FLAG(itflag, SPI_FLAG_OVR) == RESET),
		(SPI_CHECK_FLAG(itflag, SPI_FLAG_RXNE) != RESET),
		(SPI_CHECK_IT_SOURCE(itsource, SPI_IT_RXNE) != RESET)
	);
}

void q2_spi_ping_pong_irq_console() {
	printf("\r\n\r\n\r\nQ3 Ping pong on SPI\r\n");

	char message[128] = { '\0' };
	char buffer1[128] = { '\0' };
	char buffer2[128] = { '\0' };
	uint8_t do_send = 1;
	while(1) {
		if (do_send) {
			get_console_message(message, LEN_STR(message));

			HAL_StatusTypeDef status = HAL_SPI_Receive_IT(SPI_S, (uint8_t*) buffer1, strlen(message));
			if (status != HAL_OK) {
				printf("Failed to receiveIT on SPI_S: %d, error 0x%"PRIX32"\r\n", status, HAL_SPI_GetError(SPI_S));
			}
			printSPI3Flags();

			printf("SPI_M sends message: %s\r\n", message);
			status = HAL_SPI_Transmit_IT(SPI_M, (uint8_t*) message, strlen(message));//, HAL_MAX_DELAY);
			if (status != HAL_OK) {
				printf("Failed to TransmitIT data on SPI_M: %d, error 0x%"PRIX32"\r\n", status, HAL_SPI_GetError(SPI_M));
			}
			do_send = 0;
			printSPI3Flags();
		}

		if (spi4_received) {
			spi4_received = 0;
			printf("SPI_S got   message: %s\r\n\r\n", buffer1);
			printSPI3Flags();

			HAL_StatusTypeDef status = HAL_SPI_TransmitReceive_IT(SPI_S, (uint8_t*) buffer1, (uint8_t*) buffer1, strlen(buffer1));
			if (status != HAL_OK) {
				printf("Failed to tr-recIT on SPI_S: %d, error 0x%"PRIX32"\r\n", status, HAL_SPI_GetError(SPI_S));
			}

			printf("SPI_S sends message: %s\r\n", buffer1);
			status = HAL_SPI_Receive(SPI_M, (uint8_t*) buffer2, strlen(buffer1), HAL_MAX_DELAY);
			if (status != HAL_OK) {
				printf("Failed to receiveIT on SPI_M: %d, error 0x%"PRIX32"\r\n", status, HAL_SPI_GetError(SPI_S));
			}
			spi1_received = 1;
		}
		if (spi1_received) {
			spi1_received = 0;
			printf("SPI_M got   message: %s\r\n\r\n", buffer2);

			memset(buffer1, '\0', sizeof(buffer1));
			memset(buffer2, '\0', sizeof(buffer2));

			HAL_Delay(1000);
			do_send = 1;
		}
	}
}


void rtg_main(void) {
	q1_SPI_ping_pong();
	// q2_spi_ping_pong_irq_console();
}



// called when buffer is full
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
	if (hspi == SPI_M) {
		spi1_received = 1;
	}
	else if (hspi == SPI_S) {
		spi4_received = 1;
	}
	else {
		UNUSED(hspi);
	}
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi){
	if (hspi == SPI_M) {
		printf("Error on SPI_M: 0x%"PRIX32", state 0x%d\r\n", HAL_SPI_GetError(hspi), HAL_SPI_GetState(hspi));
	}
	else if (hspi == SPI_S) {
		printf("Error on SPI_S: 0x%"PRIX32", state 0x%d\r\n", HAL_SPI_GetError(hspi), HAL_SPI_GetState(hspi));
	}
	else {
		UNUSED(hspi);
	}
}
