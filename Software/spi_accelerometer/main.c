#include <stm32f4xx.h>
#define BufferSize 32
uint8_t SPI1_Buffer_Tx[BufferSize]={
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
		0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,
		0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20
	};
uint8_t SPI2_Buffer_Tx[BufferSize]={
		0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,
		0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,0x60,
		0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,
		0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70
	};	
uint8_t SPI1_Buffer_Rx[BufferSize] = {0xFF};
uint8_t SPI2_Buffer_Rx[BufferSize] = {0xFF};
uint8_t Rx1_Counter = 0;
uint8_t	Rx2_Counter = 0;
	
void SPI_Init(SPI_TypeDef * SPIx) {
	if(SPIx == SPI1){	// Set SPI1 as master
		RCC->APB2ENR	|= RCC_APB2ENR_SPI1EN;		// Enable SPI1 Clock
		RCC->APB2RSTR	|= RCC_APB2RSTR_SPI1RST;	// Reset SPI1
		RCC->APB2RSTR	&= ~RCC_APB2RSTR_SPI1RST;	// Clear reset of SPI1
		SPIx->CR1 |= SPI_CR1_MSTR;	// Master selection: 0 = slave, 1 = master
		SPIx->CR1 |= SPI_CR1_SSI;		// Manage slave selection by software
	}
	else if (SPIx == SPI2) {	// Set SPI2 as slave
		RCC->APB1ENR	|= RCC_APB1ENR_SPI2EN;		// Enable SPI2 Clock
		RCC->APB1RSTR	|= RCC_APB1RSTR_SPI2RST;	// Reset SPI2
		RCC->APB1RSTR	&= ~RCC_APB1RSTR_SPI2RST;	// Clear reset of SPI2
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
	SPIx->CR1 &= ~SPI_CR1_CPOL;
	
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

int main (void) {	
	//GPIO_Init();
	NVIC_SetPriority(SPI1_IRQn,1);	// Set priority to 1
	NVIC_EnableIRQ(SPI1_IRQn);			// Enable interrupt of SPI peripheral
	NVIC_SetPriority(SPI2_IRQn,2);	// Set priority to 1
	NVIC_EnableIRQ(SPI2_IRQn);			// Enable interrupt of SPI2 peripheral
	SPI_Init(SPI1);								  // Initialize SPI 1
	SPI_Init(SPI2);									// Initialize SPI 2
	SPI_Write(SPI1, SPI1_Buffer_Tx, 32); 	// SPI 1 writes data out
	SPI_Read(SPI2, SPI2_Buffer_Tx, 32); 	// SPI 2 reads data
	while(1);	
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

void SPI2_IRQHandler(void) {
	SPIx_IRQHandler(SPI2,SPI2_Buffer_Rx, & Rx2_Counter);
}
