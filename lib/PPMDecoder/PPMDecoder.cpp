/*
PPMDecoder library based on https://github.com/Hasi123/read-any-ppm
Works with Servo library

Paweł (DzikuVx) Spychalski 2017
https://quadmeup.com

License: GNU GPL v3
*/

#include "PPMDecoder.h"

#include "pico/stdlib.h"
#include "stdio.h"

PPMDecoder *PPMDecoder::shared;

void PPMDecoder::init(int pin, int addPinStart, int addPinCount)
{
    PPMDecoder::shared = new PPMDecoder(pin, addPinStart, addPinCount);

    for (int i = addPinStart; i < addPinStart + addPinCount; i++)
    {
        PPMDecoder::shared->ppm[(i - addPinStart) + PPMREADER_PPM_CHANNEL_COUNT] = gpio_get(i) ? 2000 : 1000; // gpio_get(i) ? 2000 : 1000; //Since we are using a pull up resistor, 1 is the default "off" state
    }
}

PPMDecoder::PPMDecoder(int pin, int addPinStart, int addPinCount)
{
    this->_pin = pin;
    this->_addPinStart = addPinStart;

    irq_set_priority(IO_IRQ_BANK0, PICO_HIGHEST_IRQ_PRIORITY);

    gpio_set_dir(pin, false);

    for (int i = addPinStart; i < addPinStart + addPinCount; i++)
    {
        gpio_set_dir(i, false);
        gpio_pull_up(i);
        gpio_set_irq_enabled(i, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    }

    gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_callback(PPMDecoder::handler);
    irq_set_enabled(IO_IRQ_BANK0, true);
}

bool PPMDecoder::isReceiving(void)
{
    if (to_ms_since_boot(get_absolute_time()) - lastPacketUpdate > PPMREADER_NO_UPDATE_THRESHOLD)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void PPMDecoder::sync()
{
    lastPacketUpdate = to_ms_since_boot(get_absolute_time());
}

void PPMDecoder::handler(uint gpio, uint32_t events)
{
    if (gpio != PPMDecoder::shared->_pin)
    {
        int val = (events & GPIO_IRQ_EDGE_RISE) != 0 ? 2000 : 1000; // Since we are using a pull up resistor, 1 is the default "off" state
        PPMDecoder::shared->ppm[(gpio - PPMDecoder::shared->_addPinStart) + PPMREADER_PPM_CHANNEL_COUNT] = gpio_get(gpio) ? 2000 : 1000;
        return;
    }

    static unsigned int pulse;
    static unsigned long counter;
    static uint8_t channel;
    static unsigned long previousCounter = 0;
    static unsigned long currentMicros = 0;
    int tmpVal;

    currentMicros = get_absolute_time();
    counter = currentMicros - previousCounter;
    previousCounter = currentMicros;
    if (counter < 510)
    { // must be a pulse
        pulse = counter;
    }

    else if (counter > 1910)
    { // sync
        channel = 0;
        PPMDecoder::shared->sync();
    }
    else
    { // servo values between 810 and 2210 will end up here
        tmpVal = counter + pulse;
        if (tmpVal > 810 && tmpVal < 2210)
        {
            PPMDecoder::shared->ppm[channel] = tmpVal;
        }
        channel++;
    }
}