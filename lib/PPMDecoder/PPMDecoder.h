/*
PPMDecoder library based on https://github.com/Hasi123/read-any-ppm
Works with Servo library
Paweł (DzikuVx) Spychalski 2017
https://quadmeup.com
License: GNU GPL v3
*/

#ifndef PPMReader_h
#define PPMReader_h

#include "pico/stdlib.h"

#define PPMREADER_PPM_CHANNEL_COUNT 7
#define PPMREADER_NO_UPDATE_THRESHOLD 500

class PPMDecoder
{
public:
	static PPMDecoder *shared;
	static void init(int pin, int addPinStart, int addPinCount);
	int ppm[PPMREADER_PPM_CHANNEL_COUNT] = {1500, 1500, 1500, 1500, 1500, 1500, 1500 };
	bool isReceiving(void);

private:
	static void handler(uint gpio, uint32_t events);
	PPMDecoder(int pin, int addPinStart, int addPinCount);
	int _pin;
	int _addPinStart;
	uint32_t lastPacketUpdate = 0; 
	void sync();
};

#endif