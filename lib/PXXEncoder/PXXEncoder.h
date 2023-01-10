#ifndef PXXEngine_h
#define PXXEngine_h

#include "PXXPacket.h"
#include "pico/stdlib.h"

class PXXEncoder
{
public:
    static PXXEncoder *shared;
    static void init(int pin);

    bool bind = false;
    bool failsafe = false;
    uint8_t channel = 16;
private:
    static void transfer_finished_handler();
    static bool timer_callback(repeating_timer_t *rt);
    static void pio_callback();
    static void dma_callback();

    PXXEncoder(int pin);
    void send();

    PXXPacket *_currentPacket;
    bool _upperChannels = false;
    repeating_timer_t _timer;
};

#endif