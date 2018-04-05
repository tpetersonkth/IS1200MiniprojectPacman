//Written by Thomas Peterson

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for this project */



#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)


void displayInit(void) {
  /*
  Initializes the display
  */
  DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);

	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);

	spi_send_recv(0x8D);
	spi_send_recv(0x14);

	spi_send_recv(0xD9);
	spi_send_recv(0xF1);

	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);

	spi_send_recv(0xA1);
	spi_send_recv(0xC8);

	spi_send_recv(0xDA);
	spi_send_recv(0x20);

	spi_send_recv(0xAF);
}


int setPixel(int x, int y, int State){
  /*
  X = The x coordinate on the display
  y = The y coordinate on the display
  State: The value to be set. Either 1 for on or 0 for ignore
  Sets a pixel to on or ignores it
  Returns boolean = 0 if successfull execution, 1 otherwise
  Observe: (0,0) is in the top left corner!
  */
  if ((y >= 32) || (x >= 128 )||(y < 0)||(x < 0)){
    return 1; //Incorrect coordinates
  }

  int row = 0;
  while(y > 7){
    y -= 8;
    row += 1;
  }
  displayBits[row][x] |= State<<y;

  return 0;
}

void clearPixel(int x, int y){
  /*
  x = The x coordinate on the display
  y = The y coordinate on the display
  Clears a pixel on the display
  Observe: (0,0) is in the top left corner!
  */
  if ((y >= 32) || (x >= 128 )||(y < 0)||(x < 0)){
    return 1; //Incorrect coordinates
  }

  int row = 0;
  while(y > 7){
    y -= 8;
    row += 1;
  }
  displayBits[row][x] &= ~(1<<y);
}

void clearDisplay(){
  /*
  Clears the display buffer
  */
  int row = 0;
  int col = 0;
  for (row = 0; row < 4; row++){
    for( col = 0; col < 128; col++){
      displayBits[row][col] = 0x00;
    }
  }
}

void updateDisplay(){
    /*
    Outputs all the bits in the displayBits array to the display
    */
    int row, col;
  	for(row = 0; row < 4; row++) {
  		DISPLAY_CHANGE_TO_COMMAND_MODE;
  		spi_send_recv(0x22);  //Tell SPI that we want to set the row
  		spi_send_recv(row);

  		spi_send_recv(0x0);    //Set the 4 LSB in the offset to 0
  		spi_send_recv(0x10);   //Set the next 4 bits in the offset to 0

  		DISPLAY_CHANGE_TO_DATA_MODE;

  			for(col = 0; col < 128; col++){
  				spi_send_recv(displayBits[row][col]);
  		  }
  	}
}
