#include "FrSkySportDecoder.h"
#include "FrSkySportSensorInav.h"
#include "FrSkySportSensorXjt.h"

class SPortDecoder
{
public:
    static SPortDecoder *shared;
    static void init(int pin);
    FrSkySportSensorInav *inav;
    FrSkySportSensorXjt *xjt;

private:
    SPortDecoder(int pin);
    static void pio_callback();
    int _pin;
    uint8_t _rxBuffer[256];
    FrSkySportDecoder *_decoder;
};