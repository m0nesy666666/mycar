#include "Drivers/Bluetooth/Bluetooth.h"

#include <stddef.h>

#include "ti_msp_dl_config.h"

/* Power-of-two size; one slot is left empty to distinguish full from empty. */
#define BT_TX_BUFFER_SIZE (256U)
#define BT_TX_BUFFER_MASK (BT_TX_BUFFER_SIZE - 1U)
#define BT_RX_LINE_SIZE   (32U)

static uint8_t bt_tx_buffer[BT_TX_BUFFER_SIZE];
static volatile uint16_t bt_tx_head;
static volatile uint16_t bt_tx_tail;
static char bt_rx_build[BT_RX_LINE_SIZE];
static char bt_rx_line[BT_RX_LINE_SIZE];
static volatile uint16_t bt_rx_length;
static volatile bool bt_rx_line_ready;
static volatile bool bt_rx_discarding;

static uint16_t BT_TxNext(uint16_t index)
{
    return (uint16_t)((index + 1U) & BT_TX_BUFFER_MASK);
}

static void BT_ReceiveByte(uint8_t data)
{
    uint16_t i;

    if ((data == (uint8_t)'\r') || (data == (uint8_t)'\n')) {
        if (bt_rx_discarding) {
            bt_rx_discarding = false;
            bt_rx_length = 0U;
            return;
        }

        /* Ignore the second byte of CR/LF and other empty lines. */
        if (bt_rx_length == 0U) {
            return;
        }

        if (!bt_rx_line_ready) {
            for (i = 0U; i < bt_rx_length; i++) {
                bt_rx_line[i] = bt_rx_build[i];
            }
            bt_rx_line[bt_rx_length] = '\0';
            bt_rx_line_ready = true;
        }
        bt_rx_length = 0U;
        return;
    }

    if (bt_rx_discarding) {
        return;
    }

    if (bt_rx_length < (BT_RX_LINE_SIZE - 1U)) {
        bt_rx_build[bt_rx_length] = (char)data;
        bt_rx_length++;
    } else {
        /* Drop the whole overlong command, not only its last characters. */
        bt_rx_length = 0U;
        bt_rx_discarding = true;
    }
}

static void BT_DrainRxFifo(void)
{
    uint8_t data;

    while (DL_UART_Main_receiveDataCheck(BLUETOOTH_UART_INST, &data)) {
        BT_ReceiveByte(data);
    }
}

/* Caller must prevent the UART TX ISR from running concurrently. */
static void BT_FillTxFifo(void)
{
    uint16_t tail = bt_tx_tail;

    while ((tail != bt_tx_head) &&
           !DL_UART_Main_isTXFIFOFull(BLUETOOTH_UART_INST)) {
        DL_UART_Main_transmitData(
            BLUETOOTH_UART_INST, bt_tx_buffer[tail]);
        tail = BT_TxNext(tail);
    }

    bt_tx_tail = tail;

    if (tail == bt_tx_head) {
        DL_UART_Main_disableInterrupt(
            BLUETOOTH_UART_INST, DL_UART_MAIN_INTERRUPT_TX);
    } else {
        DL_UART_Main_enableInterrupt(
            BLUETOOTH_UART_INST, DL_UART_MAIN_INTERRUPT_TX);
    }
}

static bool BT_QueueBytes(const uint8_t *data, uint16_t length)
{
    uint16_t head;
    uint16_t tail;
    uint16_t used;
    uint16_t free_space;
    uint16_t write_index;
    uint16_t i;
    uint32_t irq_state;

    if ((data == NULL) || (length == 0U)) {
        return (length == 0U);
    }

    /*
     * Only the main context writes head and only the UART ISR advances tail.
     * A concurrent tail advance can only increase the available free space.
     */
    head = bt_tx_head;
    tail = bt_tx_tail;
    used = (uint16_t)((head - tail) & BT_TX_BUFFER_MASK);
    free_space = (uint16_t)(BT_TX_BUFFER_MASK - used);

    if (length > free_space) {
        return false;
    }

    /* Copy first, then publish the new head in one short critical section. */
    write_index = head;
    for (i = 0U; i < length; i++) {
        bt_tx_buffer[write_index] = data[i];
        write_index = BT_TxNext(write_index);
    }

    irq_state = __get_PRIMASK();
    __disable_irq();
    bt_tx_head = write_index;
    BT_FillTxFifo();
    __set_PRIMASK(irq_state);

    return true;
}

void BT_Init(void)
{
    uint32_t irq_state = __get_PRIMASK();

    __disable_irq();
    bt_tx_head = 0U;
    bt_tx_tail = 0U;
    bt_rx_length = 0U;
    bt_rx_line_ready = false;
    bt_rx_discarding = false;

    DL_UART_Main_disableInterrupt(
        BLUETOOTH_UART_INST, DL_UART_MAIN_INTERRUPT_TX);
    DL_UART_Main_enableFIFOs(BLUETOOTH_UART_INST);
    DL_UART_Main_setTXFIFOThreshold(
        BLUETOOTH_UART_INST, DL_UART_TX_FIFO_LEVEL_1_2_EMPTY);
    DL_UART_Main_setRXFIFOThreshold(
        BLUETOOTH_UART_INST, DL_UART_MAIN_RX_FIFO_LEVEL_ONE_ENTRY);
    DL_UART_Main_enableInterrupt(
        BLUETOOTH_UART_INST, DL_UART_MAIN_INTERRUPT_RX);

    NVIC_ClearPendingIRQ(BLUETOOTH_UART_INST_INT_IRQN);
    NVIC_SetPriority(BLUETOOTH_UART_INST_INT_IRQN, 3U);
    NVIC_EnableIRQ(BLUETOOTH_UART_INST_INT_IRQN);
    __set_PRIMASK(irq_state);
}

bool BT_SendStr(const char *str)
{
    uint16_t length = 0U;

    if (str == NULL) {
        return false;
    }

    while ((str[length] != '\0') && (length < BT_TX_BUFFER_MASK)) {
        length++;
    }

    /* Reject strings that cannot fit in an empty queue. */
    if (str[length] != '\0') {
        return false;
    }

    return BT_QueueBytes((const uint8_t *)str, length);
}

bool BT_GetLine(char *buffer, uint16_t capacity)
{
    uint16_t i;
    uint32_t irq_state;

    if ((buffer == NULL) || (capacity == 0U)) {
        return false;
    }

    irq_state = __get_PRIMASK();
    __disable_irq();

    /* Polling fallback: receive still works if the UART RX IRQ is not raised. */
    BT_DrainRxFifo();

    if (!bt_rx_line_ready) {
        __set_PRIMASK(irq_state);
        return false;
    }

    for (i = 0U;
         (i < (capacity - 1U)) && (bt_rx_line[i] != '\0');
         i++) {
        buffer[i] = bt_rx_line[i];
    }
    buffer[i] = '\0';
    bt_rx_line_ready = false;

    __set_PRIMASK(irq_state);
    return true;
}

void BLUETOOTH_UART_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(BLUETOOTH_UART_INST)) {
        case DL_UART_MAIN_IIDX_TX:
            BT_FillTxFifo();
            break;

        case DL_UART_MAIN_IIDX_RX:
            BT_DrainRxFifo();
            break;

        default:
            break;
    }
}
