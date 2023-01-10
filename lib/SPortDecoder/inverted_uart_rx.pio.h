// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ---------------- //
// inverted_uart_rx //
// ---------------- //

#define inverted_uart_rx_wrap_target 0
#define inverted_uart_rx_wrap 10

static const uint16_t inverted_uart_rx_program_instructions[] = {
            //     .wrap_target
    0x20a0, //  0: wait   1 pin, 0                   
    0xea27, //  1: set    x, 7                   [10]
    0x4041, //  2: in     y, 1                       
    0xa0ca, //  3: mov    isr, !y                    
    0x0542, //  4: jmp    x--, 2                 [5] 
    0x00c8, //  5: jmp    pin, 8                     
    0x8020, //  6: push   block                      
    0x0000, //  7: jmp    0                          
    0xc014, //  8: irq    nowait 4 rel               
    0x2020, //  9: wait   0 pin, 0                   
    0x0000, // 10: jmp    0                          
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program inverted_uart_rx_program = {
    .instructions = inverted_uart_rx_program_instructions,
    .length = 11,
    .origin = -1,
};

static inline pio_sm_config inverted_uart_rx_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + inverted_uart_rx_wrap_target, offset + inverted_uart_rx_wrap);
    return c;
}

#include "hardware/clocks.h"
static inline void inverted_uart_rx_program_init(PIO pio, uint sm, uint offset, uint pin, uint baud) {
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, false);
    pio_gpio_init(pio, pin);
    gpio_pull_up(pin);
    pio_sm_config c = inverted_uart_rx_program_get_default_config(offset);
    sm_config_set_in_pins(&c, pin); // for WAIT, IN
    sm_config_set_jmp_pin(&c, pin); // for JMP
    // Shift to right, autopush disabled
    sm_config_set_in_shift(&c, true, false, 8);
    // Deeper FIFO as we're not doing any TX
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    // SM transmits 1 bit per 8 execution cycles.
    float div = (float)clock_get_hz(clk_sys) / (8 * baud);
    sm_config_set_clkdiv(&c, div);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

#endif
