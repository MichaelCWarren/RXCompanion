#include "SPortDecoder.h"
#include "hardware/dma.h"
#include "inverted_uart_rx.pio.h"

#define SPORT_SM 1
#define SPORT_PIO pio0
#define SPORT_DMA_CHANNEL 1

SPortDecoder *SPortDecoder::shared;

void SPortDecoder::init(int pin)
{
    SPortDecoder::shared = new SPortDecoder(pin);
}

SPortDecoder::SPortDecoder(int pin)
{
    this->_pin = pin;

    uint offset = pio_add_program(SPORT_PIO, &inverted_uart_rx_program);
    inverted_uart_rx_program_init(SPORT_PIO, SPORT_SM, offset, pin, 56700);

    irq_set_exclusive_handler(PIO0_IRQ_1, SPortDecoder::pio_callback);
    irq_set_enabled(PIO0_IRQ_1, true);
    irq_set_priority(PIO0_IRQ_1, PICO_LOWEST_IRQ_PRIORITY);
    pio_set_irq0_source_enabled(SPORT_PIO, pis_sm1_rx_fifo_not_empty, true);

    this->inav = new FrSkySportSensorInav();
    this->xjt = new FrSkySportSensorXjt();
    this->_decoder = new FrSkySportDecoder();
    this->_decoder->begin(this->inav, this->xjt);
}

void SPortDecoder::pio_callback()
{
    while (!pio_sm_is_rx_fifo_empty(SPORT_PIO, SPORT_SM))
    {
        // 8-bit read from the uppermost byte of the FIFO, as data is left-justified
        io_rw_8 *rxfifo_shift = (io_rw_8 *)&SPORT_PIO->rxf[SPORT_SM] + 3;
        uint8_t byte = (uint8_t)*rxfifo_shift;

        SPortDecoder::shared->_decoder->decode(byte);
    }
}