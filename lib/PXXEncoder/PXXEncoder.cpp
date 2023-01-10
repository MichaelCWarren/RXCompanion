#include "PXXEncoder.h"
#include "PPMDecoder.h"

#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "pxx.pio.h"
#include "stdio.h"

PXXEncoder *PXXEncoder::shared;

#define PXX_DMA_CHANNEL 0
#define PXX_SM 0
#define PXX_PIO pio0

PXXEncoder::PXXEncoder(int pin)
{
    uint offset = pio_add_program(PXX_PIO, &pxx_program);
    pxx_program_init(PXX_PIO, PXX_SM, offset, pin);

    dma_channel_claim(PXX_DMA_CHANNEL);
    dma_channel_config dma_conf = dma_channel_get_default_config(PXX_DMA_CHANNEL);
    channel_config_set_transfer_data_size(&dma_conf, DMA_SIZE_32);
    channel_config_set_write_increment(&dma_conf, false);
    channel_config_set_dreq(&dma_conf, pio_get_dreq(PXX_PIO, PXX_SM, true));
    dma_channel_configure(PXX_DMA_CHANNEL, &dma_conf, &PXX_PIO->txf[PXX_SM], NULL, 0, false);

    add_repeating_timer_us(-9000, timer_callback, NULL, &this->_timer);

    // irq_set_exclusive_handler(PIO0_IRQ_0, PXXEncoder::pio_callback);
    // irq_set_enabled(PIO0_IRQ_0, true);
    // irq_set_priority(PIO0_IRQ_0, PICO_LOWEST_IRQ_PRIORITY);
    // pio_set_irq0_source_enabled(PIO0, pis_interrupt0, true);

    // dma_channel_set_irq0_enabled(DMA_CHANNEL, true);
    // irq_set_exclusive_handler(DMA_IRQ_0, PXXEncoder::dma_callback);
    // irq_set_enabled(DMA_IRQ_0, true);
}

bool PXXEncoder::timer_callback(repeating_timer_t *rt)
{
    PXXEncoder::shared->send();
    return true;
}

void PXXEncoder::init(int pin)
{
    PXXEncoder::shared = new PXXEncoder(pin);
}

void PXXEncoder::send()
{
    PXXPacket packet(this->channel, this->bind, this->failsafe, this->_upperChannels);
    packet.render();
    dma_channel_transfer_from_buffer_now(PXX_DMA_CHANNEL, packet.buffer, packet.length);
    this->_upperChannels = !this->_upperChannels;
}

// void PXXEncoder::dma_callback()
// {
//      dma_hw->ints0 = 1u << DMA_CHANNEL;
// }

// void PXXEncoder::pio_callback()
// {
//     if (pio_interrupt_get(PIO0, 0))
//     {
//         pio_interrupt_clear(PIO0, 0);
//     }
// }