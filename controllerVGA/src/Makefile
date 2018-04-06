
TARGET = cvga

ifndef SERIAL_PORT
SERIAL_PORT = /dev/ttyUSB0
endif

MMCU = -mmcu=atmega328p

#OPTIONS =  

all: obj_all.o 
	avr-objcopy -j .text -j .data -O ihex obj_all.o $(TARGET).hex

obj_all.o: main.o uart.o
	avr-gcc $(MMCU) -Os main.o uart.o -o obj_all.o

main.o: main.c
	avr-gcc -c $(MMCU) -Os main.c -o main.o 

uart.o: uart.c
	avr-gcc -c $(MMCU) -Os uart.c -o uart.o 

	
clean:
	rm -f *.o

distclean: clean
	rm -f $(TARGET).hex

prog: $(TARGET).hex
	avrdude -p m328p -c usbasp -U flash:w:$(TARGET).hex:i

