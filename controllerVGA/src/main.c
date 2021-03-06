/*
  File: main.c
  
  Главный модуль. Содержит:
  - точку входа (функция main)
  - инициализация таймерасчётчика
  - обработку прерывания по переполнению счётчика
  - отладочные функции

*/

#include <stdio.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#include "uart.h"

// Константа инициализации таймера-счётчика №1
#define TCNT1_INIT 61630

void inversion_led();


/*
  Инициализация таймера №1.
*/

void init_timer1()
{
  TCCR1B |= (1<<CS12)|(0<<CS11)|(1<<CS10);// настраиваем делитель.
  TIMSK1 |= (1<<TOIE1);                   // разрешаем прерывание по переполнению таймера.
  TCNT1 = TCNT1_INIT;                     // выставляем начальное значение TCNT1.
} 


/*
  Обработчик прерывания таймера №1.
*/

ISR( TIMER1_OVF_vect )
{
  TCNT1 = TCNT1_INIT; //выставляем начальное значение TCNT1.
  //send_string("IF;");
  inversion_led();

  static int counter = 0;
  
  if (counter%2 != 0)
  {
    printf("(p) counter %d\n", counter++);
  }
  else
  {
    const size_t n = 100;
    char s[n]; 
    snprintf(s, n, "(s) counter %d\n", counter++);
    send_string(s);
  }
}


/*
  Инициализация пина порта микроконтроллера, 
  управляющего светодиодом расположенным на плате.
*/

void init_PB5()
{
  DDRB |= 1<< PB5;    // Настраиваем пин PB5 на выход.
}


/*
  Инверсия состояния светодиода (свечение/ не свечение).
  Используется для отладки, обозначение прохождений "контрольных точек".
*/

void inversion_led()
{
  if( PINB & (1 << PB5) )
    PORTB &= ~( 1 << PB5 );
  else
    PORTB |= ( 1 << PB5 );
}


void main_loop()
{
  while(1)
  {
    char buf[20];
    printf("Input sting > ");
    int n = scanf("%s", buf);
    
    if (n > 0)
    {
      printf("\nReading %d param [%s]\n", n, buf);
    }
  };
}


main()
{
  init_PB5();
  init_uart();
  init_timer1();
  sei();         // выставляем бит общего разрешения прерываний.
  
  printf("Firmware start.\n");
  //printf("\0");
  //printf("Second string.\n");
  //printf("\0");
  
  while(1){};
  //main_loop();             
}

