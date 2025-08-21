#include <lpc214x.h>




#define I2EN (1<<6) //Enable/Disable bit
#define STA  (1<<5) //Start Set/Clear bit
#define STO  (1<<4) //Stop bit
#define SI   (1<<3) //Serial Interrupt Flag Clear bit
#define AA   (1<<2) //Assert Acknowledge Set/Clear bit

	
void I2C0Init(void) 
{
	PINSEL0 |= (0<<7)|(1<<6)|(0<<5)|(1<<4); //Select SCL0(P0.2) and SDA0(P0.3)
	I2C0SCLL = 300;
	I2C0SCLH = 300; //I2C0 @ 100Khz, given PCLK @ 60Mhz
	I2C0CONCLR = STA | STO | SI | AA; //Clear these bits
	I2C0CONSET = I2EN; //Enable I2C0
	//After this we are ready to communicate with any other device connected to the same bus.
}
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

void uart_string(char*data){
	while(*data!='\0'){
		   uart_transmit(*(data++));
		   }
}

char uart_receive() {
    char received_char;
    
    // Wait until RDR is full
    while (!(U0LSR & (1 << 0)));  // Wait for RX data ready
    
    // Receive character
    received_char = U0RBR;  // Read received character from RBR
    
    return received_char;
}

void I2C_INIT(void) {
    // Configure P0.2 and P0.3 as SCL0 and SDA0 respectively
    PINSEL0 |= 0x00000050;
	I2C0CONCLR = STA | STO | SI | AA; //Clear these bits
	I2C0CONSET = I2EN; //Enable I2C0

    // Calculate the values for SCLL and SCLH for 100 kHz bitrate with 60 MHz PCLK
    // Desired bitrate = PCLK / (SCLL + SCLH)
    // For 100 kHz, PCLK = 60 MHz
    // SCLL + SCLH = PCLK / (2 * desired bitrate)
    // SCLL = SCLH = PCLK / (2 * desired bitrate)
    // SCLL = SCLH = 60000000 / (2 * 100000) = 300

    // Set SCLL and SCLH to achieve 100 kHz bitrate with 50% duty cycle
    I2C0SCLL = 300;  
    I2C0SCLH = 300;  
}

void I2C_START(void) {

  
	I2C0CONCLR = STA | STO | SI | AA; //Clear everything
	I2C0CONSET = STA; //Set start bit to send a start condition
	while ( !(I2C0CONSET & SI) ) ;
	
}

void I2C_STOP(void) {

    I2C0CONSET = STO; // Set Stop bit
  
	I2C0CONCLR = STA | STO | SI | AA; //Clear everything
	

}

void I2C_WRITE_BYTE(char data) {
  
 	I2C0DAT = data;
	I2C0CONCLR = STA | STO | SI; //Clear These to TX data
	while ( !(I2C0CONSET & SI) ) ;
}

char I2C_READ_BYTE(void) {
 I2C0CONSET = AA; //Send ACK to continue
 //I2C0CONCLR = AA;
 I2C0CONCLR = SI; //Clear SI to Start RX
 	while ( !(I2C0CONSET & SI) ) ;
	return I2C0DAT ;
}

char I2C_READ_BYTE_LAST(void) {
// I2C0CONSET = AA; //Send ACK to continue
 I2C0CONCLR = AA;
 I2C0CONCLR = SI; //Clear SI to Start RX
 	while ( !(I2C0CONSET & SI) ) ;
	return I2C0DAT ;
}


int main(void) {
    char received_char;
    char h,m,s,s1[20];
	int i,j;
    
    pll_config(); // Configure PLL
    uart_init(); // Initialize UART

    I2C_INIT(); // Initialize I2C
		 I2C_START();
					   I2C_WRITE_BYTE(0xd0);
					   I2C_WRITE_BYTE(0x00);
					   I2C_WRITE_BYTE(0x00);
					   I2C_WRITE_BYTE(0x00);
					   I2C_WRITE_BYTE(0x00);
		   I2C_STOP();

	
    while (1) {
				   I2C_START();
					   I2C_WRITE_BYTE(0xd0);
					    I2C_WRITE_BYTE(0x00);
					    		
							 	I2C0CONSET = STA; //Set start bit to send a start condition
									while ( !(I2C0CONSET & SI) ) ;
						  I2C_WRITE_BYTE(0xd1);
						 
								  s= I2C_READ_BYTE	();
								   m= I2C_READ_BYTE	();
								    h= I2C_READ_BYTE_LAST();
								
								
						     I2C_STOP();
							 sprintf(s1,"%x:%x:%x\r\n",h,m,s);
						    uart_string(s1);

						for(i=0;i<10000000;i++);
					


        // Main loop
        
    }
    
    return 0;
}