#include "PXXPacket.h"
#include "../PPMDecoder/PPMDecoder.h"
#include "stdio.h"

#define PPM_CENTER 1500
#define PPM_LOW 817
#define PPM_HIGH 2182
#define PPM_HIGH_ADJUSTED (PPM_HIGH - PPM_LOW)
#define PXX_CHANNEL_WIDTH 2048
#define PXX_UPPER_LOW 2049
#define PXX_UPPER_HIGH 4094
#define PXX_LOWER_LOW 1
#define PXX_LOWER_HIGH 2046

PXXPacket::PXXPacket(uint8_t channel, bool bind, bool failsafe, bool upperChannels)
{
    this->channel = channel;
    this->bind = bind;
    this->failsafe = failsafe;
    this->upperChannels = upperChannels;
}

void PXXPacket::render()
{
    uint16_t chan = 0, chan_low = 0;

    /* Sync */
    putHead(); // 2

    // // Rx Number
    putCrcByte(channel); // 3

    // Flags
    // 00000000
    //        ^ Bind
    //    ^ Failsafe
    //   ^ Range Check?
    //  ^ Subtype 0 - D16, 1 - D8

    uint8_t flags = (bind ? 1 : 0) | ((failsafe ? 1 : 0) << 4);
    // FLAG1 - Fail Safe Mode, nothing currently set, maybe want to do this
    putCrcByte(flags); // 4

    // FLAG2
    putCrcByte(0); // 5

    int CHAN_MIN = 1;
    int CHAN_MAX = PXX_CHANNEL_WIDTH - 2;

    // Update the channels with the ppm values
    int allChannels[16];
    for (int c = 0; c < 16; c++)
    {
        allChannels[c] = PPMDecoder::shared->ppm[c];
    }

    // PPM  //17
    for (int i = 0; i < 8; i++)
    {
        int channel = (upperChannels ? (8 + i) : i);
        int channelPPM = allChannels[channel];
        
        if (failsafe)
        {
            if (channelPPM == PXX_FAILSAFE_HOLD)
            {
                channelPPM = PXX_CHANNEL_WIDTH - 1; // Max value
            }
            else if (channelPPM == PXX_FAILSAFE_NO_PULSE)
            {
                channelPPM = 0;
            }
            else
            {
                channelPPM = 1500;
            }
        }
        else
        {
            float convertedChan = ((float(channelPPM) - float(PPM_LOW)) / (float(PPM_HIGH_ADJUSTED))) * float(PXX_CHANNEL_WIDTH);
            chan = PXXPacket::limit(CHAN_MIN, convertedChan, CHAN_MAX) + (upperChannels ? PXX_CHANNEL_WIDTH : 0);
        }

        if (i & 1)
        {
            putCrcByte(chan_low);                               // Low byte of channel
            putCrcByte(((chan_low >> 8) & 0x0F) | (chan << 4)); // 4 bits each from 2 channels
            putCrcByte(chan >> 4);                              // High byte of channel
        }
        else
        {
            chan_low = chan;
        }
    }

    uint8_t extraFlags = 0;
    extraFlags |= (1 << 2); // Channels 9-16 Enabled
    putCrcByte(extraFlags); // 18

    // CRC16
    chan = this->_pcmCrc;
    putByte(chan >> 8); // 19
    putByte(chan);      // 20

    // Sync
    putHead(); // 21

    if (this->_currentBitCount != 0)
    {
        for (int i = this->_currentBitCount; i < 32; i++)
        {
            this->_serialByte >>= 1;
        }
        this->buffer[this->length++] = this->_serialByte;
        this->_serialByte = 0;
    }

    uint32_t ones = this->_totalOneBitCount;
    uint32_t zeros = (this->_totalBitCount - ones) + 4; // Account for preamble

    this->estimatedTrasferTime = zeros * 2;
    this->estimatedTrasferTime += ones * 3;
    this->estimatedTrasferTime *= 8; // 8us pulses  //6

    this->buffer[0] = this->_totalBitCount - 1; // This makes the loops right
}

uint16_t CRCTable(uint8_t val)
{
    static const uint16_t CRC_SHORT[] = {0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
                                         0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7};

    return CRC_SHORT[val & 0x0F] ^ (0x1081 * (val >> 4));
}

void PXXPacket::crc(uint8_t data)
{
    this->_pcmCrc = (this->_pcmCrc << 8) ^ CRCTable((this->_pcmCrc >> 8) ^ data);
}

void PXXPacket::putSerialBit(uint8_t bit)
{
    this->_serialByte >>= 1;
    if (bit & 1)
    {
        this->_totalOneBitCount++;
        this->_serialByte |= 0x80000000;
    }

    if (++this->_currentBitCount >= 32)
    {
        this->buffer[this->length++] = this->_serialByte;
        this->_currentBitCount = 0;
        this->_serialByte = 0;
    }

    this->_totalBitCount++;
}

void PXXPacket::putBit(uint8_t bit)
{
    if (bit)
    {
        this->_pcmOnesCount += 1;
        putSerialBit(1);
    }
    else
    {
        this->_pcmOnesCount = 0;
        putSerialBit(0);
    }

    if (this->_pcmOnesCount >= 5)
    {
        putBit(0); // Stuff a 0 bit in
    }
}

void PXXPacket::putByte(uint8_t byte)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        putBit(byte & 0x80);
        byte <<= 1;
    }
}

void PXXPacket::putCrcByte(uint8_t byte)
{
    crc(byte);
    putByte(byte);
}

void PXXPacket::putHead()
{
    // send 7E, do not CRC
    // 01111110
    putSerialBit(0);
    putSerialBit(1);
    putSerialBit(1);
    putSerialBit(1);
    putSerialBit(1);
    putSerialBit(1);
    putSerialBit(1);
    putSerialBit(0);
}

uint16_t PXXPacket::limit(uint16_t low, uint16_t val, uint16_t high)
{
    if (val < low)
    {
        return low;
    }

    if (val > high)
    {
        return high;
    }

    return val;
}