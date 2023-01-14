#include "pico/stdlib.h"

#include "pico/multicore.h"
#include "stdio.h"

#include "MessagingService.h"
#include "PPMDecoder.h"
#include "PXXEncoder.h"
#include "SPortDecoder.h"

#define PXX_PIN 0
#define PPM_PIN 1
#define SPORT_PIN 22
#define PPM_ADD_PIN_START 12
#define PPM_ADD_PIN_COUNT 4
#define MESSAGE_RX_PIN 4
#define MESSAGE_TX_PIN 5

void core1_entry()
{
	multicore_fifo_push_blocking(1);
	PPMDecoder::init(PPM_PIN, PPM_ADD_PIN_START, PPM_ADD_PIN_COUNT);
}

int main(void)
{
	stdio_init_all();
	multicore_launch_core1(core1_entry);

	// // Wait for the second core to start up
	multicore_fifo_pop_blocking();

	PXXEncoder::init(PXX_PIN);
	//MessagingService::init(MESSAGE_RX_PIN, MESSAGE_TX_PIN);

	while (1)
	{
		printf("Alive\r\n");
		sleep_ms(1000);
	}
}
