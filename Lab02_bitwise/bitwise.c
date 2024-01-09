
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include "inputs.h"


#define IS_BIT_ON(val, idx_bit)		((val) & (1 << (idx_bit)))
#define IS_BIT_OFF(val, idx_bit)	(!IS_BIT_ON(val, idx_bit))
#define SET_BIT_ON(val, idx_bit)	((val) |= (1 << (idx_bit)))
#define SET_BIT_OFF(val, idx_bit)	((val) &= (~(1 << (idx_bit))))
#define TOGGLE_BIT(val, idx_bit)	((val) ^= (1 << (idx_bit)))

// Question 3
void to_binary(char* const buffer, uint8_t value) {
	unsigned int index = 0;
	while (index < 8) {
		buffer[7 - index] = (value & 1) ? '1' : '0';
		value >>= 1;
		index++;
	}
	buffer[8] = '\0';
}


// Question 1
void count_ones_and_zeroes(uint8_t number) {
	unsigned int ones = 0;
	unsigned int zeroes = sizeof(number) * 8;
	uint8_t tmp = number;
	while (tmp) {
		if (tmp & 1) {
			ones++;
			zeroes--;
		}
		tmp = tmp >> 1;
	}
	printf("Number %u contains %u ones and %u zeroes\n", number, ones, zeroes);
}

// Question 2
void rotate_bits(uint8_t number) {
	const unsigned int size = sizeof(number) * 8;
	const unsigned int half_size = sizeof(number) * 8/2;
	unsigned int idx = 0;
	char buffer[9] = { '\0' };
	to_binary(buffer, number);
	printf("Number %03u before rotation %s\n", number, buffer);
	while (idx <= half_size) {
		if (IS_BIT_ON(number, idx) != IS_BIT_ON(number, (size - idx - 1))) {
			TOGGLE_BIT(number, idx);
			TOGGLE_BIT(number, size - idx - 1);
		}
		idx++;
	}
	to_binary(buffer, number);
	printf("Number %03u after  rotation %s\n", number, buffer);
}

// Question 4
void swap(uint8_t val1, uint8_t val2) {
	const unsigned int size = sizeof(val1) * 8;
	unsigned int idx = 0;
	char buffer[9] = { '\0' };
	to_binary(buffer, val1);
	printf("Before value1 %03u: %s\n", val1, buffer);

	to_binary(buffer, val2);
	printf("Before value2 %03u: %s\n", val2, buffer);

	while (idx <= size) {
		if (IS_BIT_ON(val1, idx) != IS_BIT_ON(val2, idx)) {
			TOGGLE_BIT(val1, idx);
			TOGGLE_BIT(val2, idx);
		}
		idx++;
	}
	to_binary(buffer, val1);
	printf("After  value1 %03u: %s\n", val1, buffer);

	to_binary(buffer, val2);
	printf("After  value2 %03u: %s\n", val2, buffer);
}

// Question 5
uint8_t is_even(uint8_t number) {
	return IS_BIT_OFF(number, 1);
}

// Question 5
uint8_t is_odd(uint8_t number) {
	return IS_BIT_ON(number, 1);
}

int main(void) {
	count_ones_and_zeroes(7);
	rotate_bits(7);
	swap(7, 224);

	char buffer[9] = { '\0' };
	for (uint8_t i = 0; i <= 20; i++) {
		to_binary(buffer, i);
		printf("Number %03u: %s is even %s\n", i, buffer, is_even(i) ? "YES" : "NO");
		printf("Number %03u: %s is odd  %s\n", i, buffer, is_odd(i) ? "YES" : "NO");
	}

	return EXIT_SUCCESS;
}
