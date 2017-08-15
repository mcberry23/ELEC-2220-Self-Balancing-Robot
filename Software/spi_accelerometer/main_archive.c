#include <stm32f4xx.h>
#define BufferSize 2
uint8_t SPI1_Buffer_Tx[BufferSize]={
		0x20, 0xC0
};	
uint8_t SPI1_Buffer_Rx[BufferSize] = {0xFF};
uint8_t Rx1_Counter = 0;
	
void SPI_Init(SPI_TypeDef * SPIx) {
	if(SPIx == SPI1){	// Set SPI1 as master
		RCC->APB2ENR	|= RCC_APB2ENR_SPI1EN;		// Enable SPI1 Clock
		RCC->APB2RSTR	|= RCC_APB2RSTR_SPI1RST;	// Reset SPI1
		RCC->APB2RSTR	&= ~RCC_APB2RSTR_SPI1RST;	// Clear reset of SPI1
		SPIx->CR1 |= SPI_CR1_MSTR;	// Master selection: 0 = slave, 1 = master
		SPIx->CR1 |= SPI_CR1_SSI;		// Manage slave selection by software
	}
	// Configure duplex or recieve-only
	// 0 = full duplex (transmit and receive); 1 = receive-only
	SPIx->CR1 &= ~SPI_CR1_RXONLY;		// full duplex
	
	// Data frame format
	// 0 = 8-bit frame; 1 = 16 bit frame
	SPIx->CR1 &= ~SPI_CR1_DFF;	// frame size = 8 bits
	
	// Frame format
	// 0 = MSB transmitted first
	// 1 = LSB transmitted first
	SPIx->CR1 |= SPI_CR1_LSBFIRST;
	
	// Software slave management (SSM)
	// 1 = software slave management enabled
	// 0 = software slave management disabled
	// When enabled, SSI is forced onto the NNSS, and the GPIO of NSS is ignored
	SPIx->CR1 |= SPI_CR1_SSM;
	
	// Clock phase (CPHA)
	// 0 = The first clock transistion is the first data capture edge
	// 1 = The secondd clock transition is the first data capture edge
	SPIx->CR1 |= SPI_CR1_CPHA;
	
	// Clock polarity (CPOL)
	// 0 = Set clock to low when idle
	// 1 = Set clock to high when idle
	SPIx->CR1 |= SPI_CR1_CPOL;
	
	// Baud rate control:
	// 000: f_PCLK/2	001: f_PCLK/4		010: f_PCLK/8		011: f_PCLK/16
	// 100: f_PCLK/32	101: f_PCLK/64	110: f_PCLK/128	111: f_PCLK/256
	SPIx->CR1 |= 4<<3;
	
	// Bidirectional data mode enable
	SPIx->CR1 &= ~SPI_CR1_BIDIMODE;
	// 0: 2-line unidirectional data mode selected
	
	// Output enable in bidirectional mode
	// 0: Output disable (receive-only mode)
	// 1: Output enabled (transmit-only mode)
	SPIx->CR1 |= SPI_CR1_BIDIOE;
	
	// Enable RX buffer not empty interrupt
	SPIx->CR2 |= SPI_CR2_RXNEIE;
	
	// Enable error interrupt
	SPIx->CR2 |= SPI_CR2_ERRIE;
	
	// Enable SPI
	SPIx->CR1 |= SPI_CR1_SPE;
}
void SPI_Write(SPI_TypeDef * SPIx, uint8_t *buffer, int size) {
	int i;
	for (i = 0; i < size; i++){
		while(!(SPIx->SR & SPI_SR_TXE)); 	// Wait for transmit buffer empty
		SPIx->DR = buffer[i];
	}
	while (SPIx->SR & SPI_SR_BSY);			// Wait for not busy
}

void SPI_Read(SPI_TypeDef * SPIx, uint8_t *buffer, int size) {
	int i;
	for (i = 0; i < size; i++){
		while(!(SPIx->SR & SPI_SR_TXE));	// Wait for TXE
		SPI1->DR = buffer[i];
		while(!(SPI1->SR & SPI_SR_TXE)); 	// Wait for TXE
		SPI1->DR = 0xFF;									// A dummy byte
	}
	while (SPI1->SR & SPI_SR_BSY); 			// Wait until SPI is not busy
}

void SPIx_IRQHandler(SPI_TypeDef *SPIx, uint8_t *buffer, uint8_t *counter) {
	if(SPIx->SR & SPI_SR_RXNE) { // if SPI Receive Register is not empty
		buffer[*counter] = SPIx->DR;
		// Reading SPI_DR automatically clears the RXNE flag
		(*counter)++;
		if ((*counter) >= BufferSize)
			(*counter)=0;
	}
}

void SPI1_IRQHandler(void) {
	SPIx_IRQHandler(SPI1,SPI1_Buffer_Rx, & Rx1_Counter);
}

void Init_Accelerometer() {
	//----Init PA4-7----
		RCC->AHB1ENR  |= 	0x01;     	//IO Port A clock enable  = 00000001
		GPIOA->MODER 	&= ~0x0000FF00; //clear PB4-7 mode 
		GPIOA->MODER  |= 	0x0000AA00; //PB4-7 = AF mode 
		GPIOA->AFR[0] |=  0x00000002; //PB4-7 = AF2
}

int main (void) {	
	Init_Accelerometer();
	NVIC_SetPriority(SPI1_IRQn,1);	// Set priority to 1
	NVIC_EnableIRQ(SPI1_IRQn);			// Enable interrupt of SPI peripheral
	NVIC_SetPriority(SPI2_IRQn,2);	// Set priority to 1
	NVIC_EnableIRQ(SPI2_IRQn);			// Enable interrupt of SPI2 peripheral
	SPI_Init(SPI1);								  // Initialize SPI 1
	SPI_Init(SPI2);									// Initialize SPI 2
	SPI_Write(SPI1, SPI1_Buffer_Tx, 32); 	// SPI 1 writes data out
	SPI_Read(SPI1, SPI1_Buffer_Tx, 32); 	// SPI 2 reads data
	while(1);	
}


