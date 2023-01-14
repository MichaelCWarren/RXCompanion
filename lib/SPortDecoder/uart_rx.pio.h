// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ------- //
// uart_rx //
// ------- //

#define uart_rx_wrap_target 0
#define uart_rx_wrap 9

static const uint16_t uart_rx_program_instructions[] = {
            //     .wrap_target
    0x2020, //  0: wait   0 pin, 0                   
    0xc001, //  1: irq    nowait 1                   
    0xe927, //  2: set    x, 7                   [9] 
    0x4001, //  3: in     pins, 1                    
    0x0643, //  4: jmp    x--, 3                 [6] 
    0x00c9, //  5: jmp    pin, 9                     
    0xc014, //  6: irq    nowait 4 rel               
    0x20a0, //  7: wait   1 pin, 0                   
    0x0000, //  8: jmp    0                          
    0x8020, //  9: push   block                      
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program uart_rx_program = {
    .instructions = uart_rx_program_instructions,
    .length = 10,
    .origin = -1,
};

static inline pio_sm_config uart_rx_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + uart_rx_wrap_target, offset + uart_rx_wrap);
    return c;
}

#include "hardware/clocks.h"
#include "hardware/gpio.h"
static inline void uart_rx_program_init(PIO pio, uint sm, uint offset, uint pin, uint baud) {
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, false);
    pio_gpio_init(pio, pin);
    gpio_pull_down(pin);
    pio_sm_config c = uart_rx_program_get_default_config(offset);
    sm_config_set_in_pins(&c, pin); // for WAIT, IN
    sm_config_set_jmp_pin(&c, pin); // for JMP
    // Shift to right, autopush disabled
    sm_config_set_in_shift(&c, true, false, 32);
    // Deeper FIFO as we're not doing any TX
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    // SM transmits 1 bit per 8 execution cycles.
    float div = (float)clock_get_hz(clk_sys) / (8 * baud);
    sm_config_set_clkdiv(&c, div);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
    gpio_set_inover(pin, GPIO_OVERRIDE_INVERT);
}
static inline char uart_rx_program_getc(PIO pio, uint sm) {
    // 8-bit read from the uppermost byte of the FIFO, as data is left-justified
    io_rw_8 *rxfifo_shift = (io_rw_8*)&pio->rxf[sm] + 3;
    while (pio_sm_is_rx_fifo_empty(pio, sm))
        tight_loop_contents();
    return (char)*rxfifo_shift;
}

#endif

