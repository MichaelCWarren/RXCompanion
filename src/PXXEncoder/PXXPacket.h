#ifndef PXXPacket_h
#define PXXPacket_h

#include "pico/stdlib.h"

#define PXX_FAILSAFE_HOLD 0
#define PXX_FAILSAFE_NO_PULSE 1
#define PXX_FAILSAFE_MID_RANGE 2

#define PXX_BUFFER_SIZE 10

class PXXPacket
{
public:
    PXXPacket(uint8_t channel, bool bind, bool failsafe, bool upperChannels);
    uint32_t buffer[PXX_BUFFER_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint32_t length = 1;
    uint32_t estimatedTrasferTime = 0;
    uint8_t channel = 16;
    bool bind = false;
    bool failsafe = false;
    bool upperChannels = false;
    void render();
    
private:
    uint16_t _pcmCrc = 0;
    uint8_t _pcmOnesCount = 0;
    uint32_t _serialByte = 0;
    uint8_t _currentBitCount = 0;
    uint16_t _totalBitCount = 0;
    uint16_t _totalOneBitCount = 0;

    static uint16_t limit(uint16_t low, uint16_t val, uint16_t high);
    void putHead();
    void putCrcByte(uint8_t byte);
    void putByte(uint8_t byte);
    void putBit(uint8_t bit);
    void putSerialBit(uint8_t bit);
    void putPcmBit(uint8_t bit);
    void crc(uint8_t data);
};

#endif