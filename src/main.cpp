#include "pico/stdlib.h"

#include "pico/multicore.h"
#include "stdio.h"

#include "MessagingService/MessagingService.h"
#include "PPMDecoder/PPMDecoder.h"
#include "PXXEncoder/PXXEncoder.h"
#include "SPortDecoder/SPortDecoder.h"
#include "Settings/Settings.h"

#define PXX_PIN 1
#define PPM_PIN 0
#define SPORT_PIN 22
#define PPM_ADD_PIN_START 16
#define PPM_ADD_PIN_COUNT 4
#define MESSAGE_TX_PIN 4
#define MESSAGE_RX_PIN 5

void core1_entry()
{
	PPMDecoder::init(PPM_PIN, PPM_ADD_PIN_START, PPM_ADD_PIN_COUNT);
	multicore_fifo_push_blocking(1);
}

int main(void)
{
	stdio_init_all();
	multicore_launch_core1(core1_entry);

	// // Wait for the second core to start up
	multicore_fifo_pop_blocking();

	PXXEncoder::init(PXX_PIN);
	MessagingService::init(MESSAGE_RX_PIN, MESSAGE_TX_PIN);
	Settings::init();

	while (1)
	{
		printf("Alive\r\n");
		sleep_ms(1000);
	}
}
