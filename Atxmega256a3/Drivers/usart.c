#include "usart.h"

void initUSART(void){
        //Disable interrupts for setup
        cli();
        //Step 1) Set the TxD pin value high, and clear the RxD pin
        //TxD
        PORTD.OUTSET = PIN3_bm;
        //RxD
        PORTD.OUTCLR = PIN2_bm; 

        //Step 2) Set the TxD as output RxD as input
        PORTD.DIRSET = PIN3_bm;
        PORTD.DIRCLR = PIN2_bm; 

        //Step 3) Set the baud rate & frame format
        //BSEL = 103 BSCALE = 0 BAUD = 9600 
        //ActualBAUD = 9615.385 Error = 0.16%
        USART.BAUDCTRLA = (PIN6_bm|PIN5_bm|PIN2_bm|PIN1_bm|PIN0_bm);
        //Asynchronous mode, No parity, 1 Stop bit, 8 bits
        USART.CTRLC = PIN1_bm|PIN0_bm; 

        //Step 4) Set the mode of operation
        //RXCIE enable low
        USART.CTRLA    = PIN4_bm; 

        //Step 5) Enable the transmitter or the receiver, depending on the usage.
        USART.CTRLB = PIN4_bm | PIN3_bm; 

        /* Enable PMIC interrupt level low. */
        PMIC.CTRL |= PMIC_LOLVLEN_bm; 

        USART.STATUS &= (0<<PIN7_bp);
        sei();
      }


void usartTX(char data){
       while(!(usart->STATUS & 0x20));
       usart->DATA = data;
   }


//put this in your main.c
ISR(USARTD0_RXC_vect){
       char data = USART.DATA;
   }