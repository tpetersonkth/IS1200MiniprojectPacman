//Written by Thomas Peterson
#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"  /* Global variables declatations and .. stuff ..*/

void blitSprite(int X,int Y,int width,int height,uint8_t *images[4][3],int direction,int frame){/* Number after Images does not seem to matter
  Param X: x position as int
  Param Y: y position as int
  Param images: images as  uint8_t *p[4][2]
  Param direction: direction as int, 0 for left, 1 for up and so on
  Param frame: current frame
  Blits a sprite to the OLED Display
  Observe: Obsolete function!
  */
  int x;
  int y;
  for(x=0; x < width; x++){
    for(y=0; y < height;y++){
        setPixel(X+x,Y+y,(((images[direction][frame][x]&(0x1<<y))>>y)));
    }
  }
}

void blitSprite2(struct sprite *object){/* Number after Images does not seem to matter
  Param object = A pointer to a sprite object
  Blits a sprite to the screen using its attributes
  */

  int x;
  int y;
  for(x=0; x < object->width; x++){
    for(y=0; y < object->height;y++){
        setPixel(object->x+x,object->y+y,(((object->Images[object->direction][object->currentFrame][x]&(0x1<<y))>>y)));
    }
  }
}

void blitImage(int X,int Y,int width,int height,uint8_t *image){/*
  Param X = The target x position as int
  Param y = The target y position as int
  Param width: The image width
  Param height: The image height
  Param image: An image as an uint8_t *
  Blits an image to the screen
  */
  int x;
  int y;
  for(x=0; x < width; x++){
    for(y=0; y < height;y++){
        setPixel(X+x,Y+y,(((image[x]&(0x1<<y))>>y)));
    }
  }
}


void blitScore(){
  /*
  Blits the score in the top right corner of the OLED Display
  */
  int x,y;
  int digits = 1;
  int localScore = score;
  if (score == 0){
    digits=2;
    for(x = 120;x<128;x++){
      for(y=0; y < 8; y++){
        clearPixel(x,y);
      }
    }
    blitImage(124,1,3,5,numbers[0]);
  }
  else{
    while (localScore > 0){//blits the score in the topright corner of the screen
      for(x = 128-digits*4-4;x<128-(digits-1)*5;x++){//First clear the bits
        for(y=0; y < 8; y++){
          clearPixel(x,y);
        }
      }
      //Blit a digit
      blitImage(128-digits*4,1,3,5,numbers[localScore%10]);//This line creates problems. Verification error
      localScore = localScore/10;
      digits+=1;
    }
  }
  //Creates the lines around the score
  for(x = 128-digits*4; x < 128; x++){
    setPixel(x,8,1);
  }
  for(y = 0; y <= 8; y++){
    setPixel(128-digits*4,y,1);
  }
}

void blitScoreDst(int X){
  /*
  Param X = The X coordinate where the score will start
  Blits the score to the OLED display in the top row.
  */
  int x,y;
  int digits = 1;
  int localScore = score;
  if (score == 0){
    digits=2;
    for(x = X+1;x<X+4;x++){
      for(y=3; y < 9; y++){
        clearPixel(x,y);
      }
    }
    blitImage(X-4+1,3,3,5,numbers[0]);
  }
  else{
    while (localScore > 0){//blits the score in the topright corner of the screen
      for(x = X-digits*4-4;x<X-(digits-1)*5;x++){//First clear the bits
        for(y=3; y < 9; y++){
          if (digits == 4){
            break;
          }
          clearPixel(x,y);
        }
      }
      //Blit a digit
      blitImage(X-digits*4+1,3,3,5,numbers[localScore%10]);//This line creates problems. Verification error
      localScore = localScore/10;
      digits+=1;
    }
  }
}

int rectCollision(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2){//1 = True and 0 = False as usual
  /*
  Param x1 = The x coordinate of object 1
  Param y1 = The y coordinate of object 1
  Param width1 = The width of object 1
  Param height1 = The height of object 1
  Param x2 = The x coordinate of object 2
  Param y2 = The y coordinate of object 2
  Param width2 = The width of object 2
  Param height2 = The height of object 2
  Takes two objects and returns determines if they overlap eachother using rectangle collision detection
  returns boolean = boolean which is true if the objects are overlaping
  */
  if ((y1+height1) <= y2 || (y2+height2) <= y1|| x1 >= (x2+width2)|| (x1+width1) <= x2){//Calculate if the rectangles does not overlap
    return 0;
  }
  else{ //Else return True
    return 1;
  }
}

int spriteRectCollision(struct sprite *object1,struct sprite *object2){
  /*
  Param object1 = A pointer to the first sprite object
  Param object2 = A pointer to the second sprite object
  Takes two objects and returns determines if they overlap eachother using rectangle collision detection
  returns boolean = boolean which is true if the objects are overlaping
  */
  if ((object1->y+object1->height) <= object2->y || (object2->y+object2->height) <= object1->y|| object1->x >= (object2->x+object2->width)|| (object1->x+object1->width) <= object2->x){//Calculate if the rectangles does not overlap
    return 0;
  }
  else{ //Else return True
    return 1;
  }
}

void displayString(int x,int y, char *s,int len) {
  /*
  Param x = The x coordinate where the string will start
  Param y = The y coordinate where the string will begin
  Param s = A pointer pointing to the first element in the char array
  Param len = The length of the char array
  Takes a string an blits it to the screen
  */
	int i;
  for(i=0;i<len;i++){
    if (*s >= 0x30 && *s <= 0x39){//Numbers
      blitImage(x+6*i,y,3,5,numbers[*s-0x30]);
    }
    else if(*s==0x2E){
      blitImage(x+6*i,y,3,5,&DOT);
    }
    else if(*s==0x3A){
      blitImage(x+6*i,y,3,5,&COLON);
    }
    else if(*s==0x20){ // Space

    }
    else if(*s==0x7C){// a pipe character '|' becomes a box
      blitImage(x+6*i,y,5,5,&W);
    }
    else if(*s==0x2D){// A minus sign '-'
      blitImage(x+6*i,y,5,5,&MINUS);
    }
    else{
  	  displayChar(x+6*i,y,*s);
    }
  	s++;
  }
}

void displayChar(int x, int y, char s){
  /*
  Param x = The x coordinate of the char
  Param y = The y coordinate of the char
  Param s = The character to be printed out
  Takes a char and blits it to the screen
  */
  s=s-65;
  blitImage(x,y,5,5,lFont[s]);
}
