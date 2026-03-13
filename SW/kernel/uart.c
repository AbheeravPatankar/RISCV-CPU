#include "uart.h"

char uart_tx_buff[UART_TX_BUFFER_SIZE];
char uart_rx_buff[UART_RX_BUFFER_SIZE];

uint32 uart_tx_wr; 
uint32 uart_tx_r;

uint32 uart_rx_wr; 
uint32 uart_rx_r;




void uartinit()
{
     // disable interrupts.
    WriteReg(IER, 0x00);
    
    // special mode to set baud rate.
    WriteReg(LCR, LCR_BAUD_LATCH);
    
    // LSB for baud rate of 38.4K.
    WriteReg(0, 0x03);
    
    // MSB for baud rate of 38.4K.
    WriteReg(1, 0x00);
    
    // leave set-baud mode,
    // and set word length to 8 bits, no parity.
    WriteReg(LCR, LCR_EIGHT_BITS);
    
    // reset and enable FIFOs.
    WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);
    
    // enable transmit and receive interrupts.
    WriteReg(IER, IER_RX_ENABLE | IER_TX_ENABLE);
}

// prints a character on display 
void uart_printc(char c) {  if((ReadReg(LSR) & LSR_TX_IDLE) != 0) { WriteReg(THR,c); } }


void uart_insert_into_rx_buff(char c)
{
    // check for special characters like /r and /n
    if(( c == '\b' ||  c == 0x7F  ) && uart_rx_wr != uart_rx_r)
    {
        // if buffer is not empty
        uart_rx_wr = (uart_rx_wr == 0) ? UART_RX_BUFFER_SIZE - 1 : uart_rx_wr - 1;
        uart_printc('\b');
        uart_printc(' ');
        uart_printc('\b');
    }
    else if(c == '\r' && uart_rx_wr + uart_rx_r != UART_RX_BUFFER_SIZE)
    {
        // if the buffer is not full
        uart_rx_buff[uart_rx_wr] = '\n';
        uart_rx_wr = ( uart_rx_wr + 1 ) % UART_RX_BUFFER_SIZE;
        uart_printc('\n');
    }
    else
    {
        if(uart_rx_wr + uart_rx_r != UART_RX_BUFFER_SIZE)
        {
            uart_rx_buff[uart_rx_wr] = c;
            uart_rx_wr = ( uart_rx_wr + 1 ) % UART_RX_BUFFER_SIZE;
            uart_printc(c);
        }
    }
}

void uart_extract_chars_from_tx_buff()
{
    while(uart_tx_wr != uart_tx_r)
    {
        // uart buffer is not empty
        char c = uart_tx_buff[uart_tx_r];
        uart_tx_r = (uart_tx_r + 1) % UART_TX_BUFFER_SIZE ;
        uart_printc(c);
    }
}


char uart_getc()
{
    char c;
    while(1)
    {
        uint32* LSR_addr = Reg(LSR);
        if(ReadReg(LSR) & 0x01)
        {
            // input data is ready.
            c = ReadReg(RHR);
            // print the char recieved on display
            uart_insert_into_rx_buff(c);
        }
        else
        {
            // Receive buffer is empty
            break;
        }
    }
}

void handle_rx_intr()
{
    // read the characters from RHR and insert them in the rx buffer
    uart_getc();

    // wakeup the processes sleeping on uart_read
    kwakeup(uart_rx_buff);
}

void handle_tx_intr()
{
    // send all the characters in the tx_fifo to the THR for displaying the characters 
    
    // wakeup process sleeping on tx_fifo buffer 
}

void uartintr()
{
    handle_rx_intr();

    handle_tx_intr();   
}

SEGMENT_HEADER* get_proc_elf_header(char* name)
{
    return (SEGMENT_HEADER*)0x80005000 ;
}