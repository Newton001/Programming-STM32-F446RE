/*
 * UsartBuffer.c
 *
 *  Created on: Dec 3, 2024
 *      Author: newton
 */
#include "UartBuffer.h"
#include "string.h"
#include "usart.h"

/* Put the following in the ISR

extern void Usart_isr (UART_HandleTypeDef *huart);
]

 */
#define usart &huart2

/**************** No changes after this **********************/

ring_buffer rx_buffer = { { 0 }, 0, 0};
ring_buffer tx_buffer = { { 0 }, 0, 0};

ring_buffer *_rx_buffer;
ring_buffer *_tx_buffer;

void store_char(unsigned char c, ring_buffer *buffer);


void Ringbuf_init(void)
{
	_rx_buffer = &rx_buffer;
	_tx_buffer = &tx_buffer;

	/* Enable the USART Error Interrupt: (Frame error, noise error, overrun error) */
	__HAL_UART_ENABLE_IT(usart, UART_IT_ERR);

	/* Enable the USART Data Register not empty Interrupt */
	__HAL_UART_ENABLE_IT(usart, UART_IT_RXNE);
}

void store_char(unsigned char c, ring_buffer *buffer)
{
	int i = (unsigned int)(buffer->head + 1) % UART_BUFFER_SIZE;

	// If the head would advance to the tail, we're about to overflow the buffer
	if(i != buffer->tail) {
		buffer->buffer[buffer->head] = c;
		buffer->head = i;
	}
}

int Usart_read(void)
{
	// If the head isn't ahead of the tail, we don't have any characters
	if(_rx_buffer->head == _rx_buffer->tail)
	{
		return -1;
	}
	else
	{
		unsigned char c = _rx_buffer->buffer[_rx_buffer->tail];
		_rx_buffer->tail = (unsigned int)(_rx_buffer->tail + 1) % UART_BUFFER_SIZE;
		return c;
	}
}


void Usart_write(int c) {
    if (c > 0) {
        int i = (_tx_buffer->head + 1) % UART_BUFFER_SIZE;
        if (i != _tx_buffer->tail) { // Ensure buffer is not full
            _tx_buffer->buffer[_tx_buffer->head] = (uint8_t)c;
            _tx_buffer->head = i;
            __HAL_UART_ENABLE_IT(usart, UART_IT_TXE); // Trigger transmission interrupt
        } else {
            HAL_UART_Transmit(usart, (uint8_t *)&c, 1, HAL_MAX_DELAY); // Fallback
        }
    }
}

int IsDataAvailable(void)
{
	return (uint16_t)(UART_BUFFER_SIZE + _rx_buffer->head - _rx_buffer->tail) % UART_BUFFER_SIZE;
}

uint16_t get_pos(char *string)
{
	static uint8_t so_far;
	uint16_t counter;
	int len = strlen(string);

	if (_rx_buffer->tail > _rx_buffer->head)
	{
		if (Usart_read() == string[so_far])
		{
			counter = UART_BUFFER_SIZE - 1;
			so_far++;
		}
		else
			so_far = 0;
	}

	unsigned int start = _rx_buffer->tail;
	unsigned int end = _rx_buffer->head;
	for (unsigned int i = start; i < end; i++)
	{
		if (Usart_read() == string[so_far])
		{
			counter = i;
			so_far++;
		}
		else
			so_far = 0;
	}

	if (so_far == len)
	{
		so_far = 0;
		return counter;
	}
	else
		return -1;
}

void Usart_sendstring(const char *s)
{
	while (*s)
		Usart_write(*s++);
	Usart_write('\n'); // Add newline for terminal clarity
}

void Usart_printbase(long n, uint8_t base)
{
	char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
	char *s = &buf[sizeof(buf) - 1];

	*s = '\0';

	// Prevent crash if called with base == 1
	if (base < 2)
		base = 10;

	do {
		unsigned long m = n;
		n /= base;
		char c = m - base * n;
		*--s = c < 10 ? c + '0' : c + 'A' - 10;
	} while (n);

	while (*s)
		Usart_write(*s++);
}

void Get_string(char *buffer)
{
	int index = 0;

	while (_rx_buffer->tail > _rx_buffer->head)
	{
		if ((_rx_buffer->buffer[_rx_buffer->head - 1] == '\n') || ((_rx_buffer->head == 0) && (_rx_buffer->buffer[UART_BUFFER_SIZE - 1] == '\n')))
		{
			buffer[index] = Usart_read();
			index++;
		}
	}
	unsigned int start = _rx_buffer->tail;
	unsigned int end = (_rx_buffer->head);
	if ((_rx_buffer->buffer[end - 1] == '\n'))
	{
		for (unsigned int i = start; i < end; i++)
		{
			buffer[index] = Usart_read();
			index++;
		}
	}
}

int wait_until(char *string, char *buffertostore)
{
	while (!(IsDataAvailable()));
	int index = 0;

	while (_rx_buffer->tail > _rx_buffer->head)
	{
		if ((_rx_buffer->buffer[_rx_buffer->head - 1] == '\n') || ((_rx_buffer->head == 0) && (_rx_buffer->buffer[UART_BUFFER_SIZE - 1] == '\n')))
		{
			buffertostore[index] = Usart_read();
			index++;
		}
	}

	unsigned int start = _rx_buffer->tail;
	unsigned int end = (_rx_buffer->head);
	if ((_rx_buffer->buffer[end - 1] == '\n'))
	{
		for (unsigned int i = start; i < end; i++)
		{
			buffertostore[index] = Usart_read();
			index++;
		}
		return 1;
	}
	return 0;
}

void Usart_isr(UART_HandleTypeDef *huart)
{
	uint32_t isrflags = READ_REG(huart->Instance->SR);
	uint32_t cr1its = READ_REG(huart->Instance->CR1);

	/* If DR is not empty and the Rx Int is enabled */
	if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
	{
		huart->Instance->SR;                       /* Read status register */
		unsigned char c = huart->Instance->DR;     /* Read data register */
		store_char(c, _rx_buffer);  // Store data in buffer
		return;
	}

	/* If interrupt is caused due to Transmit Data Register Empty */
	if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
	{
		if (tx_buffer.head == tx_buffer.tail)
		{
			// Buffer empty, so disable interrupts
			__HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
		}
		else
		{
			// There is more data in the output buffer. Send the next byte
			unsigned char c = tx_buffer.buffer[tx_buffer.tail];
			tx_buffer.tail = (tx_buffer.tail + 1) % UART_BUFFER_SIZE;

			huart->Instance->SR;
			huart->Instance->DR = c;
		}
		return;
	}
}
