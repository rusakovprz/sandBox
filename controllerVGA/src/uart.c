/*
  File: uart.c

  http://mainloop.ru/avr-atmega/avr-usart-setting.html
  http://www.google.ru/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&ved=0CCoQFjAA&url=http%3A%2F%2Favr-libc.narod.ru%2FDataseets%2F3_Atmega16_32%2F6_USART.doc&ei=YRG_UovPPPT2ygOH64HICg&usg=AFQjCNH03-SPFAIOnt6LvcKvdSLG0SXSmg&bvm=bv.58187178,d.bGQ&cad=rjt

*/


#define max_len_buffer_receiver 40
#define max_len_buffer_sender 256

#include <stdio.h>
#include <string.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include "uart.h"


// приёмный буфер
char buffer_receiver[max_len_buffer_receiver];

// индекс  очередного "свободного" элемента приёмного буфера
int index_item_receiver = 0;

typedef void (*call_back_rcv_uart_finction)(char byte);
call_back_rcv_uart_finction call_back_rcv_uart;
void call_back_rcv_uart_default(char byte);

// прередающий буфер
char buffer_sender[max_len_buffer_sender];

// индекс  очередного элемента передающего буфера
volatile unsigned int index_item_sender = 0;

// размер строки передающего буфера
volatile unsigned int len_buffer_sender = 0;

// счётсик запросов IF;
int coint_request = 0;

// функция вывода символа
static int my_putchar(char c, FILE *stream)
{
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
  return 0;
}

// функция ввода символа
static int my_getchar(FILE *stream)
{
  loop_until_bit_is_set(UCSR0A, RXC0);
  return UDR0;
}

// определяем дескриптор для стандартного вывода
static FILE mystdout = FDEV_SETUP_STREAM(
                my_putchar,     // функция вывода символа
                my_getchar,           // функция ввода символа
                _FDEV_SETUP_RW // флаги потока
);

void init_uart()
{
  //unsigned int ubrr = (unsigned int)(16000000/(baudrate*16))-1;
  // Для частоты микроконтроллера 16МГц и скорости 9600 
  // в регистр скорости следует записать значение 103. 
  //UBRR0L = 103;
  //UBRR0H = 103 >> 8;
  //Для baudrate = 115200 => ubrr = 7,68
  UBRR0L = 8;
  UBRR0H = 8 >> 8;

  // 8 бит данных.
  UCSR0C |= ( 1 << UMSEL01 ) | ( 0 << UCSZ02 ) | ( 1 << UCSZ01 ) | ( 1 << UCSZ00 );

  // Режим с отключенным контролем четности.
  UCSR0C |= ( 1 << UMSEL01 ) | ( 0 << UPM01 ) | ( 0 << UPM00 ) ;

  // Режим с одним стоп-битом.
  UCSR0C |= ( 1 << UMSEL01 ) | ( 0 << USBS0 );

  // Разрешение приема и передачи данных по UART.
  UCSR0B |= ( 1 << TXEN0 ) | ( 1 << RXEN0 );

  // Разрешение прерывания по завершению приема, по освобождению регистра данных передтчика.
  UCSR0B |= ( 1 << RXCIE0 ) | ( 1 << UDRIE0 );

  index_item_receiver = 0;
  index_item_sender = 0;

  stdout = &mystdout;
  stdin = &mystdout;

  call_back_rcv_uart = call_back_rcv_uart_default;
}


void send_string(const char *in_string)
{
  if( strlen(in_string) >= max_len_buffer_sender )
  {
    len_buffer_sender = max_len_buffer_sender;
  } else
  {
    len_buffer_sender = strlen(in_string);
  }

  strncpy(buffer_sender, in_string, len_buffer_sender);
  index_item_sender = 0;
  UDR0 = buffer_sender[index_item_sender++];
}


/*
  Обработчик прерывания по приёму байта через UART. 
*/

ISR( USART_RX_vect )
{
  if (call_back_rcv_uart)
  {
    call_back_rcv_uart(UDR0);
  }
}


/*
  Обработчик прерывания по освобождению регистра передатчика UART. 
*/

ISR( USART_UDRE_vect )
{
  if(index_item_sender >= len_buffer_sender)
  {
    return;
  }

  UDR0 = buffer_sender[index_item_sender++];
}


void call_back_rcv_uart_default(char byte)
{
  buffer_receiver[index_item_receiver++] = byte;

  if (byte == '\n' || byte == '\r' ||
       index_item_receiver >= max_len_buffer_receiver)
  {
    buffer_receiver[index_item_receiver-1] = '\0';

    const size_t n = 60;
    char s[n];
    snprintf(s, n, "receive %d bytes [%s]\n", strlen(buffer_receiver), buffer_receiver);
    send_string(s);

    index_item_receiver = 0;
  }
}

