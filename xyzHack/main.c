#include "iostm8s.h"

void setupIO(void);
void writeBit(unsigned char bit);
void sendStandby(void);
void sendByte(unsigned char sByte);
unsigned char readBit(void);
unsigned char readByte(void);
unsigned char lastRead(void);
void sak(void);

unsigned char tbit;
volatile unsigned char done;
unsigned char write;

unsigned char b1=0, b2=0, b3=0;


main()
{
	unsigned int x=0;
	setupIO();
	_asm("rim\n");
	PB_ODR |= 0x20;
	
	while(1){
		sendStandby();
		sendByte(0x55);
		sendByte(0xa0);
		sendByte(0x03);
		sendByte(0x00);
		sendByte(0x00);
		b1 = readByte();
		b2 = readByte();
		b3 = lastRead();
	
		for(x=0;x<60000;x++);
		for(x=0;x<60000;x++);
		for(x=0;x<60000;x++);
		for(x=0;x<60000;x++);
		for(x=0;x<60000;x++);
	}

}

void sendByte(unsigned char sByte){
	unsigned char i=0;
	for(i=0;i<8;i++){
		writeBit(sByte>>(7-i));
	}
	writeBit(1); //MACK
	readBit(); //nosak
}

unsigned char readByte(void){
	unsigned char i=0;
	unsigned char dummy = 0;
		
	for(i=0;i<8;i++){
		dummy |= readBit();
		dummy <<= 1;
	}
	writeBit(1); //MACK
	sak(); //sak
	
	return dummy;
}

unsigned char lastRead(void){
	unsigned char i=0;
	unsigned char dummy = 0;
	for(i=0;i<8;i++){
		dummy |= readBit();
		dummy <<= 1;
	}
	writeBit(0); //NoMACK
	sak(); //sak
	
	return dummy;
}

void sak(void){
	PB_DDR &= ~0x10; //pin is input
	
	//first int is dummy
	write = 0;
	tbit = 0;
	done = 0;
	TIM2_PSCR = 1;
	TIM2_ARRH = 0;
	TIM2_ARRL = 200;
	TIM2_SR1 = 0x00;
	TIM2_IER = 0x01;
	TIM2_CR1 = 0x09;
	
	while(done!=1);
	
	//data
	write = 0;
	tbit = 0;
	done = 0;
	TIM2_PSCR = 1;
	TIM2_ARRH = 0;
	TIM2_ARRL = 100;
	TIM2_SR1 = 0x00;
	TIM2_IER = 0x01;
	TIM2_CR1 = 0x09;
	
	while(done!=1);
}
unsigned char readBit(void){
	PB_DDR &= ~0x10; //pin is input
	
	//first int is dummy
	write = 0;
	tbit = 0;
	done = 0;
	TIM2_PSCR = 1;
	TIM2_ARRH = 0;
	TIM2_ARRL = 100;
	TIM2_SR1 = 0x00;
	TIM2_IER = 0x01;
	TIM2_CR1 = 0x09;
	
	while(done!=1);
	
	//data
	write = 0;
	tbit = 0;
	done = 0;
	TIM2_PSCR = 1;
	TIM2_ARRH = 0;
	TIM2_ARRL = 100;
	TIM2_SR1 = 0x00;
	TIM2_IER = 0x01;
	TIM2_CR1 = 0x09;
	
	while(done!=1);
	return tbit;
}

void writeBit(unsigned char bit){
	PB_DDR |= 0x10; //pin is output
	
	//first send inverse of bit
	write = 1;
	tbit = ~bit;
	tbit &= 0x01;
	done = 0;
	TIM2_PSCR = 1;
	TIM2_ARRH = 0;
	TIM2_ARRL = 100;
	TIM2_SR1 = 0x00;
	TIM2_IER = 0x01;
	TIM2_CR1 = 0x09;
	
	while(done!=1);
	
	write = 1;
	tbit = bit;
	tbit &= 0x01;
	done = 0;
	TIM2_PSCR = 1;
	TIM2_ARRH = 0;
	TIM2_ARRL = 100;
	TIM2_SR1 = 0x00;
	TIM2_IER = 0x01;
	TIM2_CR1 = 0x09;
	
	while(done!=1);
}

void sendStandby(void){
	unsigned char i =0;
	PB_DDR = 0x30; //pin is output
	
	//first send low
	for(i=0;i<3;i++){
		write = 1;
		tbit = 0;
		tbit &= 0x01;
		done = 0;
		TIM2_PSCR = 1;
		TIM2_ARRH = 0;
		TIM2_ARRL = 240;
		TIM2_SR1 = 0x00;
		TIM2_IER = 0x01;
		TIM2_CR1 = 0x09;
		while(done!=1);
	}

	for(i=0;i<50;i++){
			write = 1;
			tbit = 1;
			tbit &= 0x01;
			done = 0;
			TIM2_PSCR = 1;
			TIM2_ARRH = 0;
			TIM2_ARRL = 240;
			TIM2_SR1 = 0x00;
			TIM2_IER = 0x01;
			TIM2_CR1 = 0x09;
			while(done!=1);
	}
	
	for(i=0;i<3;i++){
		write = 1;
		tbit = 0;
		tbit &= 0x01;
		done = 0;
		TIM2_PSCR = 1;
		TIM2_ARRH = 0;
		TIM2_ARRL = 240;
		TIM2_SR1 = 0x00;
		TIM2_IER = 0x01;
		TIM2_CR1 = 0x09;
		while(done!=1);
	}
}



void setupIO(void){
	
	PB_DDR = 0x30; PB_CR1 = 0xCF; PB_CR2 = 0x00;
	
	CLK_ICKCR = 0;
	CLK_ICKCR |= 0x01;                	//  Enable the HSI.
	CLK_ECKCR = 0;                      //  Disable the external clock.
	while ((CLK_ICKCR&0x02) == 0);			//  Wait for the HSI to be ready for use.
	CLK_CKDIVR = 0;                    	//  the clocks are running at full speed.
	CLK_PCKENR1 = 0xff;                	//  Enable all peripheral clocks.
	CLK_PCKENR2 = 0xff;                	//  Ditto.
	CLK_CCOR = 0;                      	//  Turn off CCO.
	CLK_HSITRIMR = 0;                  	//  Turn off any HSIU trimming.
	CLK_SWIMCCR = 0;                   	//  Set SWIM to run at clock / 2.
	CLK_SWR = 0xe1;                    	//  Use HSI as the clock source.
	CLK_SWCR = 0;                      	//  Reset the clock switch control register.
	CLK_SWCR |= 0x02;                		//  Enable switching.
	while ((CLK_SWCR&0x01) != 0);       //  Pause while the clock switch is busy.
	
	//Setup TIM2
	TIM2_CR1 = 0x00;
	TIM2_IER = 0x00;
	TIM2_SR1 = 0x00;
	TIM2_SR2 = 0x00;
	TIM2_EGR = 0x00;
	TIM2_CCMR1 = 0x00;
	TIM2_CCMR2 = 0x00;
	TIM2_CCMR3 = 0x00;
	TIM2_CCER1 = 0x00;
	TIM2_CCER2 = 0x00;	
	TIM2_CNTRH = 0x00;
	TIM2_CNTRL = 0x00;
	TIM2_PSCR = 0x00;	
	TIM2_ARRH = 0x00;
	TIM2_ARRL = 0x00;
	TIM2_CCR1H = 0x00;
	TIM2_CCR1L = 0x00;
	TIM2_CCR2H = 0x00;
	TIM2_CCR2L = 0x00;
	TIM2_CCR3H = 0x00;
	TIM2_CCR3L = 0x00;
}