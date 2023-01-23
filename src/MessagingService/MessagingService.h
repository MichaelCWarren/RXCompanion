#include "../PXXEncoder/PXXEncoder.h"
#include "hardware/uart.h"
#include "memory.h"
#include "pico/stdlib.h"
#include "string.h"

#define UART_ID uart1
#define UART_IRQ UART1_IRQ
#define BUFFER_SIZE 1024
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

// #define UART_TX_PIN 4 // UART1 TX - 4, 8, 20, 24
// #define UART_RX_PIN 5 // UART1 RX - 5, 9, 21, 25

typedef enum
{
    IDLE,
    HEADER_START,
    HEADER,
    PAYLOAD,
    CHECKSUM,
    RECEIVED
} state_e;

typedef struct __attribute__((packed))
{
    uint16_t size;
    uint8_t message;
} header_t;

class MessagingService
{
public:
    static MessagingService *shared;
    static void init(int rxPin, int txPin);
    void sendMessage(uint8_t messageId, uint8_t *buffer, uint16_t size);

private:
    uint8_t _buffer[BUFFER_SIZE];
    uint16_t _offset = 0;
    state_e _state = IDLE;
    uint8_t _checksum = 0;
    uint8_t _message = 0;
    uint16_t _size = 0;
    repeating_timer_t _timer;

    static bool timer_callback(repeating_timer_t *rt);
    MessagingService(int rxPin, int txPin);
    static void uart_callback();
    static uint8_t createChecksum(uint8_t checksum, const uint8_t *data, int len);
    void receivedData(uint8_t data);
    void messageComplete();
};