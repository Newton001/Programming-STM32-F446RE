#ifndef USARTBUFFER_H_
#define USARTBUFFER_H_

#include "stm32f4xx_hal.h"

/* Change the size of the buffer */
#define UART_BUFFER_SIZE 64

typedef struct
{
  unsigned char buffer[UART_BUFFER_SIZE];
  volatile unsigned int head;
  volatile unsigned int tail;
} ring_buffer;

/* Reads the data in the rx_buffer and increments the tail count in rx_buffer */
int Usart_read(void);

/* Writes the data to the tx_buffer and increments the head count in tx_buffer */
void Usart_write(int c);

/* Function to send the string to the USART */
void Usart_sendstring(const char *s);

/* Print a number with any base
 * base can be 10, 8, etc.*/
void Usart_printbase (long n, uint8_t base);

/* Initialize the ring buffer */
void Ringbuf_init(void);

/* Checks if the data is available to read in the rx_buffer */
int IsDataAvailable(void);

/* Get the position of the given string within the incoming data.
 * It returns the position where the string ends */
uint16_t get_pos (char *string);

/* The ISR for the USART. Put it in the IRQ handler */
void Usart_isr (UART_HandleTypeDef *huart);

/* Once you hit 'enter' (\r\n), it copies the entire string to the buffer */
void Get_string (char *buffer);

/* Keep waiting until the given string has arrived and stores the data in the buffertostore
 * On success, return 1
 * Or else returns 0
 * @usage :  while (!(wait_until("\r\n", buffer)));
 */
int wait_until (char *string, char *buffertostore);

#endif
