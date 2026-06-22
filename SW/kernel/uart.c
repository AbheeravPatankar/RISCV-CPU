#include "uart.h"
#include "paging.h"
#include "proc.h"
#include "utils.h"

char uart_tx_buff[UART_TX_BUFFER_SIZE];
char uart_rx_buff[UART_RX_BUFFER_SIZE];

uint32 uart_tx_wr; 
uint32 uart_tx_r;

uint32 uart_rx_wr; 
uint32 uart_rx_r;

int is_full(uint32 reader, uint32 writer, uint32 size)
{
    // when the writer is just one behind the reader then the buffer is full
    if((writer + 1) % size == reader)
        return 1;
    else 
        return 0;
}

int is_empty(uint32 reader, uint32 writer, uint32 size)
{
    if ( reader == writer)
        return 1;
    else
        return 0;
}


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
    else if(c == '\r' && !is_full(uart_rx_r, uart_rx_wr, UART_RX_BUFFER_SIZE))
    {
        // if the buffer is not full
        uart_rx_buff[uart_rx_wr] = '\n';
        uart_rx_wr = ( uart_rx_wr + 1 ) % UART_RX_BUFFER_SIZE;
        uart_printc('\n');
    }
    else
    {
        if(!is_full(uart_rx_r, uart_rx_wr, UART_RX_BUFFER_SIZE))
        {
            uart_rx_buff[uart_rx_wr] = c;
            uart_rx_wr = ( uart_rx_wr + 1 ) % UART_RX_BUFFER_SIZE;
            uart_printc(c);
        }
    }

    return ;
}


void uart_insert_char_into_tx(char c)
{
    // check if tx buffer is full 
    if(!is_full(uart_tx_r, uart_tx_wr, UART_TX_BUFFER_SIZE))
    {
        uart_tx_buff[uart_tx_wr] = c;
        uart_tx_wr = ( uart_tx_wr + 1 ) % UART_TX_BUFFER_SIZE;
    }
}

void uart_extract_chars_from_tx_buff()
{
    while(!is_empty(uart_tx_r, uart_tx_wr, UART_TX_BUFFER_SIZE))
    {
        // uart buffer is not empty
        char c = uart_tx_buff[uart_tx_r];
        uart_tx_r = (uart_tx_r + 1) % UART_TX_BUFFER_SIZE ;
        uart_printc(c);
    }
}

char uart_extract_chars_from_rx_buffer()
{
    // check if the rx char is not empty
    if(!is_empty(uart_rx_r, uart_rx_wr, UART_RX_BUFFER_SIZE))
    {   
        char c = uart_rx_buff[uart_rx_r];
        uart_rx_r = (uart_rx_r + 1) % UART_RX_BUFFER_SIZE ;
        return c;
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
    uart_extract_chars_from_tx_buff(); 
    
    // wakeup process sleeping on tx_fifo buffer
    kwakeup(uart_tx_buff);
}

void uartintr()
{
    handle_rx_intr();

    handle_tx_intr();   
}

SEGMENT_HEADER* get_proc_elf_header(char* name)
{
    return (SEGMENT_HEADER*)0x80012000 ;
}


// this function writes to to the tx_buffer to display characters on the buffer
// sleeps when the tx_buffer is full
void console_write(char* buffer, uint32 size)
{

    char temp[UART_TX_BUFFER_SIZE];
    uint32 size_copied = 0;
    while(size > 0)
    {
        // copy the chars from user buffer into some temp buffer
        copyin(myproc()->pagetable, (uint32*)temp, (uint32)buffer, min(UART_TX_BUFFER_SIZE, size));
        size_copied = min(UART_TX_BUFFER_SIZE, size) ;

        for(int i = 0 ; i < size_copied; i++)
        {
            if(!is_full(uart_tx_r, uart_tx_wr, UART_TX_BUFFER_SIZE))
            {
                // insert characters into uart tx buffer 
                uart_insert_char_into_tx(temp[i]);

            }
            else
            {
                // tx is full so process will sleep until some char is displayed
                ksleep(uart_tx_buff);
            }
        }

        size = size - size_copied;
    }

    // print the chars on the display
    handle_tx_intr();

}


// this function reads from the rx_buffer to recieve any input from the console
// reads one line at a time
// sleeps when the rx buffer is empty
void console_read(char* buffer, uint32 size)
{
    char temp[UART_RX_BUFFER_SIZE];
    int size_copied = 0;
    for(int i = 0; i < size ; i ++)
    {
        // check if the rx_buffer is not empty
        if(!is_empty(uart_rx_r, uart_rx_wr, UART_RX_BUFFER_SIZE))
        {
            temp[size_copied] = uart_extract_chars_from_rx_buffer();
            size_copied++;
            // break if a new line or NULL character is recieved 
            if(temp[i] == '\n' || temp[i] == 0)
            {
                break;
            }
        }
        else
        {
            // buffer is empty so sleep until char is recieved
            ksleep(uart_rx_buff);
            i--;
        }
    }

    // copy the data from kernel buffer to user buffer
    copyout(myproc()->pagetable,(uint32*)temp, (uint32)buffer, size_copied);

    return ;
}
