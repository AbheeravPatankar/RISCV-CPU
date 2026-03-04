#include "uart.h"


char uart_tx_buff[UART_TX_BUFFER_SIZE];

uint32 uart_tx_wr; 
uint32 uart_tx_r;

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
    WriteReg(IER, IER_RX_ENABLE);
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
            uart_printc(c);
        }
        else
        {
            // Receive buffer is empty
            break;
        }
    }
}

void uart_printc(char c)
{
    uint32* LSR_addr = Reg(LSR);
    if((ReadReg(LSR) & LSR_TX_IDLE) != 0)
    {
        // write the character to the transmit buffer
        WriteReg(THR,c);

    }
    else
    {
        // Receive buffer is empty
        return ;
    }
}


void uartintr()
{
    // read the char from the UART register and print it on the display
    
    uart_getc();

    

}

SEGMENT_HEADER* get_proc_elf_header(char* name)
{
    return (SEGMENT_HEADER*)0x80005000 ;
}