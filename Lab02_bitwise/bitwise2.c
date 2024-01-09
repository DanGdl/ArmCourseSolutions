
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

// Question 6
#define MASK_APPLY(val, mask)		((val) |= (mask))
#define MASK_IS_SET(val, mask)		(m = (mask); ((val) & m) == m)


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
void q1(uint8_t number) {
	char buffer[9] = { '\0' };
	to_binary(buffer, number);
	printf("Before: %u %s\n", number, buffer);
	SET_BIT_ON(number, 2);
	SET_BIT_OFF(number, 4);
	to_binary(buffer, number);
	printf(" After: %u %s\n", number, buffer);
}

// Question 2
void q2(uint8_t number) {
	printf("Bit 4 in number %u is set to %c\n", number, IS_BIT_ON(number, 3) ? '1' : '0');
}

// Question 3
void q3(uint8_t number) {
	for (int i = 0; i < 8; i++) {
		TOGGLE_BIT(number, i);
		printf("Led 1 is %s, led 2 is %s, led 3 is %s, led 4 is %s, led 5 is %s, led 6 is %s, led 7 %s, led 8 %s\n",
				IS_BIT_ON(number, 0) ? " ON" : "OFF",
				IS_BIT_ON(number, 1) ? " ON" : "OFF",
				IS_BIT_ON(number, 2) ? " ON" : "OFF",
				IS_BIT_ON(number, 3) ? " ON" : "OFF",
				IS_BIT_ON(number, 4) ? " ON" : "OFF",
				IS_BIT_ON(number, 5) ? " ON" : "OFF",
				IS_BIT_ON(number, 6) ? " ON" : "OFF",
				IS_BIT_ON(number, 7) ? " ON" : "OFF"
		);
	}
	for (int i = 0; i < 8; i++) {
		TOGGLE_BIT(number, i);
		printf("Led 1 is %s, led 2 is %s, led 3 is %s, led 4 is %s, led 5 is %s, led 6 is %s, led 7 %s, led 8 %s\n",
				IS_BIT_ON(number, 0) ? " ON" : "OFF",
				IS_BIT_ON(number, 1) ? " ON" : "OFF",
				IS_BIT_ON(number, 2) ? " ON" : "OFF",
				IS_BIT_ON(number, 3) ? " ON" : "OFF",
				IS_BIT_ON(number, 4) ? " ON" : "OFF",
				IS_BIT_ON(number, 5) ? " ON" : "OFF",
				IS_BIT_ON(number, 6) ? " ON" : "OFF",
				IS_BIT_ON(number, 7) ? " ON" : "OFF"
		);
	}
}

// Question 4
void q4(uint8_t number) {
	char buffer[9] = { '\0' };
	to_binary(buffer, number);
	printf("Origin: %u %s\n", number, buffer);

	for (int i = 0; i < 8; i++) {
		int addition = IS_BIT_ON(number, 7) ? 1 : 0;
		number = addition + (number << 1);

		to_binary(buffer, number);
		printf("Iteraction %d: %03u %s\n", i, number, buffer);
	}
}

// Question 5
void q5(uint8_t number) {
	const uint8_t mask = (1) | (1 << 1) | (1 << 2) | (1 << 3);
	char buffer[9] = { '\0' };
	to_binary(buffer, number);
	printf("Q5 Before mask: %03u %s\n", number, buffer);

	MASK_APPLY(number, mask);

	to_binary(buffer, number);
	printf("Q5 After  mask: %03u %s\n", number, buffer);
}

void q6(uint8_t number) {
	const uint8_t mask = (1) | (1 << 2) | (1 << 4) | (1 << 6);
	char buffer[9] = { '\0' };
	to_binary(buffer, number);
	printf("Q6 Before mask: %03u %s\n", number, buffer);

	MASK_APPLY(number, mask);

	to_binary(buffer, number);
	printf("Q6 After  mask: %03u %s\n", number, buffer);
}

// question 7
void q7(uint8_t number, unsigned int idx) {
	printf("Number %03u, bit %d is %u\n", number, idx, ((number >> idx) & 1));
}

// Question 8
void q8(uint8_t number) {
	unsigned int ones = 0;
	uint8_t tmp = number;
	while (tmp) {
		if (tmp & 1) {
			ones++;
		}
		tmp = tmp >> 1;
	}
	printf("Number %u contains %u ones\n", number, ones);
}

// Question 9
uint8_t is_even(uint8_t number) {
	return IS_BIT_OFF(number, 1);
}

// Question 10
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

uint8_t is_power_of_2(uint8_t number) {
	unsigned int ones = 0;
	while (number) {
		if (number & 1) {
			ones++;
			if (ones > 1) {
				return 0;
			}
		}
		number = number >> 1;
	}
	return ones == 1;
}

int main(void) {
	q1(91);
	q2(91);
	q3(0);
	q4(86);
	q5(0);
	q6(0);
	q7(85, 5);
	q8(85);
	printf("Number %d is even %s\n", 1, is_even(1) ? "YES" : "NO");
	printf("Number %d is even %s\n", 2, is_even(2) ? "YES" : "NO");
	swap(7, 224);

	// Question 11
	int8_t val1 = -1, val2 = 1;
	printf("Numbers %d and %d have %s\n", val1, val2, (IS_BIT_ON(val1, 7) == IS_BIT_ON(val2, 7)) ? "same sign" : "different signs");

	// Question 12
	for (int i = 0; i <= 16; i++) {
		printf("Number %d is power of 2: %s\n", i, is_power_of_2(i) ? "YES" : "NO");
	}
	return EXIT_SUCCESS;
}
