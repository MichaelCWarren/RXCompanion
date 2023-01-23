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
FrSkySportDecoder *_decoder;
private:
    SPortDecoder(int pin);
    static void pio_callback();
    int _pin;
    uint8_t _rxBuffer[256];
    
};