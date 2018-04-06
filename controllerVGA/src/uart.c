/*
  File: uart.c
  
  http://mainloop.ru/avr-atmega/avr-usart-setting.html
  http://www.google.ru/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&ved=0CCoQFjAA&url=http%3A%2F%2Favr-libc.narod.ru%2FDataseets%2F3_Atmega16_32%2F6_USART.doc&ei=YRG_UovPPPT2ygOH64HICg&usg=AFQjCNH03-SPFAIOnt6LvcKvdSLG0SXSmg&bvm=bv.58187178,d.bGQ&cad=rjt
  
*/


#define max_len_buffer_receiver 40
#define max_len_buffer_sender 10

#include <stdio.h>
#include <string.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include "uart.h"


// приёмный буфер
char buffer_receiver[max_len_buffer_receiver];

// индекс  очередного "свободного" элемента приёмного буфера 
int index_item_receiver;

// прередающий буфер
char buffer_sender[max_len_buffer_sender];

// индекс  очередного элемента передающего буфера 
int index_item_sender;

// счётсик запросов IF;
int coint_request = 0;

// функция вывода символа
static int my_putchar(char c, FILE *stream)
{
  loop_until_bit_is_set(UCSR0A, UDRE0);
  
  UDR0 = c;
  return 0;
}

// определяем дескриптор для стандартного вывода
static FILE mystdout = FDEV_SETUP_STREAM(
                my_putchar,     // функция вывода символа
                NULL,           // функция ввода символа, нам сейчас не нужна
                _FDEV_SETUP_WRITE // флаги потока - только вывод
);

void init_uart()
{
  
  // Для частоты микроконтроллера 16МГц и скорости 9600 
  // в регистр скорости следует записать значение 103. 
  UBRR0L = 103;
  UBRR0H = 103 >> 8;
  
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
  index_item_sender = -1;
  
  stdout = &mystdout;
}


void send_string(const char *in_string)
{
     
  if( strlen(in_string) >= max_len_buffer_sender )
    return;

  strcpy(buffer_sender, in_string);
  index_item_sender = 0;
  UDR0 = buffer_sender[index_item_sender++];
}


/*
  Обработчик прерывания по приёму байта через UART. 
*/

ISR( USART_RX_vect )
{
  /*
    + принять очередной символ
    + проверить на равенсво символу ';' код 0x3B
    + проверить на переполнение приёмного буфера
    + проверить, что в буфере лежит ответ 'IF' (0x49 0x46)
    - байты буфера с 6-10 включительно перевести из строки в целое число 
      ( частота в килогерцах см. файл bands.h )
    - сравнить частоту со значениями границ диапазонов и установить
      соответсвующий диапзон ( bands.h void set_band(int band_code); )
     
  */
  
  char rxbyte = UDR0;
/*  buffer_receiver[index_item_receiver] = rxbyte;
  index_item_receiver += 1;
  
  if ( rxbyte == ';')
  {
    index_item_receiver = 0;
    coint_request = 0;
  } */
}


/*
  Обработчик прерывания по освобождению регистра передатчика UART. 
*/

ISR( USART_UDRE_vect )
{
//  if( buffer_sender[index_item_sender] != 0x00 )
//    UDR0 = buffer_sender[index_item_sender++];
}



