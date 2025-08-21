#include <lpc214x.h>
void pll_config() {
    // Enable PLL and set PLL multiplier and divider values
    PLL0CON = 0x01;      // Enable PLL
    PLL0CFG = 0x24;      // M = 5, P = 2 (PLL multiplier and divider values)
    PLL0FEED = 0xAA;     // Feed sequence
    PLL0FEED = 0x55;     // Feed sequence
    
    // Wait until PLL is locked
    while (!(PLL0STAT & (1 << 10)));  // Wait for PLL lock
    
    // Connect PLL as system clock
    PLL0CON = 0x03;      // Connect PLL
    PLL0FEED = 0xAA;     // Feed sequence
    PLL0FEED = 0x55;     // Feed sequence
    
    // Configure peripheral clock divider
    VPBDIV = 0x01;       // PCLK = CCLK (divided by 1)
}
void uart_init() {
    // Configure UART0 pins
    PINSEL0 = 0x5;       // Select TXD0 and RXD0 functions for P0.0 and P0.1
    
    // Configure UART0 settings: 8-bit data, 1 stop bit, no parity
    U0LCR = 0x83;        // Enable DLAB, set word length to 8-bit, enable break control
    U0DLL = 0x87;        // Set baud rate to 9600 (for PCLK = 15MHz)
    U0DLM = 0x01;        // Set baud rate to 9600 (for PCLK = 15MHz)
    U0LCR = 0x03;        // Disable DLAB, set word length to 8-bit, disable break control
}
void uart_transmit(char c) {
    // Wait until THR is empty
    while (!(U0LSR & (1 << 5)));  // Wait for TX buffer to be empty
    
    // Transmit character
    U0THR = c;  // Load character to be transmitted into THR
}
char uart_receive() {
    char received_char;
    
    // Wait until RDR is full
    while (!(U0LSR & (1 << 0)));  // Wait for RX data ready
    
    // Receive character
    received_char = U0RBR;  // Read received character from RBR
    
    return received_char;
}
int main(void) {
    char received_char;
    
  
    pll_config();    // Configure PLL
    uart_init();     // Initialize UART
    

    while (1) {
        // Receive character
        received_char = uart_receive();
        
        // Transmit received character back
        uart_transmit(received_char);
    }
    
    return 0;
}
