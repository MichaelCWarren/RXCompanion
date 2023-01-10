#include "hardware/uart.h"
#include "pico/stdlib.h"
#include "string.h"
#include "memory.h"

#define UART_ID uart1
#define UART_IRQ UART1_IRQ
#define RX_BUFFER_SIZE 1024
#define MAX_MESSAGE_LENGTH 1024
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

#define UART_TX_PIN 4 // UART1 TX - 4, 8, 20, 24
#define UART_RX_PIN 5 // UART1 RX - 5, 9, 21, 25

class MessagingService
{
public:
    static MessagingService *shared;
    static void init();

private:
    uint8_t _message[MAX_MESSAGE_LENGTH];
    uint16_t _length = 0;

    MessagingService();
    static void uart_callback();
    void processMessage(uint8_t *message, int length);
};

void MessagingService::init()
{
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    uart_set_hw_flow(UART_ID, false, false);
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
    uart_set_fifo_enabled(UART_ID, true);
    irq_set_exclusive_handler(UART_IRQ, MessagingService::uart_callback);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only, we don't care when the TX FIFO is empty
    uart_set_irq_enables(UART_ID, true, false);

    MessagingService::shared = new MessagingService();
}

void MessagingService::uart_callback()
{
    while (uart_is_readable(UART_ID))
    {
        uint8_t buffer[RX_BUFFER_SIZE];
        int bufferIndex;

        while (uart_is_readable(UART_ID))
        {
            if (bufferIndex >= RX_BUFFER_SIZE)
            {
                break;
            }

            uint8_t val = uart_getc(UART_ID);
            buffer[bufferIndex] = val;
            bufferIndex++;
        }

        MessagingService::shared->processMessage(buffer, bufferIndex);
    }
}

MessagingService::MessagingService()
{
}

void MessagingService::processMessage(uint8_t *message, int length)
{

    // if(length + this->_length > MAX_MESSAGE_LENGTH) //We have two separate messages probably
    // {

    // }
    //memcpy(&_message[_length], message, );
    // add the new buffer to the current message and process
}