//Written by Thomas
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for this project */

 /*
TODO :s
Clean up code
comment stuff
 */


#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

char displayBits[4][128];//A 128*32 pixels image containing a whole frame
int frameCounter = 0;
int score = 0;
int powerUpMode = 0;
int powerUpCount = 0;	//Counter to create a time limit for pacman's powerup boost
int colourMode = 0; // 0 = normal, 1 = inverted
int at = 0; // 0 = menu, 1 = in pacman game, 2 = instructions, 3 = highscores, 4 = GameOver
int totalFrameCounter = 0;

//The design of the map, made by Sebastian
int wallsCoordinates[][4] = {{1,1,127,1},{1,2,1,8},{1,10,1,8},{1,18,1,8},{1,26,1,8},{2,15,8,3},{7,7,19,3},{39,7,3,8},{31,7,11,3},{31,15,11,3},
							{15,15,11,3},{23,18,3,5},{23,23,11,3},{7,23,11,3},{47,7,3,3},{55,2,1,8},{56,9,18,1},{73,2,1,8},{39,23,11,3},{47,18,3,5},
							{47,15,11,3},{55,23,19,3},{63,15,3,8},{79,7,3,3},{87,7,3,8},{87,15,11,3},{87,7,11,3},{103,7,19,3},{71,15,11,3},{79,18,3,5},
							{79,23,11,3},{95,23,11,3},{103,18,3,5},{103,15,11,3},{111,23,11,3},{119,15,8,3},{1,31,127,1},{127,1,1,8},{127,9,1,8},{127,17,1,8},
							{127,25,1,8}};

//For diretions, the following always applies
//0 = left, 1 = up, 2 = right, 3 = down

//Pacman
sprite pacman = {.x=0,.y=0,.width = 5,.height=5,.speedX=0,.speedY=0,.currentFrame=0,.frameNum=1,.direction=2,.Images = {{&LPS,&LPL0,&LPL1},{&LPS,&LPU0,&LPU1},{&LPS,&LPR0,&LPR1},{&LPS,&LPD0,&LPD1}},.frameAmount=3};

//Ghosts
int ghostAmount = 4;//Amount of ghosts
struct sprite ghosts[10];

//Walls
int wallAmount = 41;
struct sprite walls[41];

//Points
sprite point1 = {.x=50,.y=20,.width = 1,.height=1,.speedX=0,.speedY=0,.currentFrame=0,.frameNum=1,.direction=0,.Images = {{&P},{},{},{}},.frameAmount=1,.dead=0};
int pointAmount = 64;
struct sprite points[64];

//PowerUps
int powerUpAmount = 4;
struct sprite powerups[4];

int getbtns(){
	/*
	Fetches the input from the 4 pushbuttons in the bottom left corner of the IOshield
	Returns integer = 4 bits defining if btn4, btn3, btn2 and btn1 is held down
	*/
  int r = 0;
  //r = (PORTD&0xE0);
  //score = PORTF;   //PORTB Contains potentiometer
  r = ((PORTD&0x0E0)>>4);
  r = r|((PORTF&0x2)>>1);
  return r&0x0F;
}

void initialize(){
	/*
	Initializes buttons, timers and points
	*/
	volatile int* MYTRISE = (volatile int*) 0xbf886100;
  *MYTRISE = (~0XFF)&*MYTRISE;    //set LEDs to outpu rt
  TRISD = (0XFE0)|TRISD;  //set Button 2,3,4 and switch 1,2,3,4 as input
  TRISF = TRISF|(0x20);   //Set button 1 as inputm
  T2CON = (T2CON|0x08070); //Set timer to ON and prescale 111       // 80 000 000 Hz/256(prescale) = 312 500 ticks per second
  //T1CON = (T1CON|0x08000); //Set timer to ON and prescale 000       // 80 000 000 ticks per second
  PR2 = 15625; //Maxvalue is around 64000. This will create interrupts each 50th millisecond (31250/2)
  IPC(2) = IPC(2)|0x1F; //Set this interrupt to the highest prioristy   //T2IP and T"IS
  //IEC(0) = IEC(0)|0x10; // Enable the timer 1 Interrupt flag
  IEC(0) = IEC(0)|0x100; // Enable the timer 2 Interrupt flag
  //IFS(0) = IFS(0)&(~0x10);//T2IF timer 1 interrupt flag = 0
  IFS(0) = IFS(0)&(~0x100);//T2IF timer 2 interrupt flag = 0           Why parenthesis???
  enable_interrupt();
	resetHighscore();
  initializeObjects();

}

void initializeObjects(){
	/*
		Initializes all the sprite objects of the game
	*/

  int i;
  int j;

	//Reset pacman
	pacman.x = 62;
	pacman.y = 10;
	pacman.speedX = 0;
	pacman.speedY = 0;

	//Generate walls
  for(i = 0; i < wallAmount; i++){
        sprite wall = {.x=wallsCoordinates[i][0],.y=wallsCoordinates[i][1],.width = wallsCoordinates[i][2],.height=wallsCoordinates[i][3],.speedX=0,.speedY=0,.currentFrame=0,.frameNum=1,.direction=0,.Images = {{&W},{},{},{}},.frameAmount=1};
        walls[i] = wall;
  }

	//Generate Ghosts
	for(i = 0; i < ghostAmount; i++){
  			sprite ghost = {.x=(34+(56*(rand()%2))),.y=10,.width = 5,.height=5,.speedX=0,.speedY=0,.currentFrame=0,.frameNum=1,.direction=1,.Images = {{&G0,&G1},{&G0,&G1},{&G0,&G1},{&G0,&G1}},.frameAmount=2};
        ghosts[i] = ghost;
  }


	for(i=0; i < 4; i++){//Generate points. If statements to exclude some of them
		for(j=0; j*8+4 < 128;j++){

			sprite point = {.x=4+j*8,.y=4+8*i,.width = 1,.height=1,.speedX=0,.speedY=0,.currentFrame=0,.frameNum=1,.direction=0,.Images = {{&P},{},{},{}},.frameAmount=1};
			if (i == 0){
				if (j == 0 || j==15 || (j > 6 && j < 9)){
					point.dead = 1;
				}
			}
			else if(i == 1){
				if(j == 4 || j == 11){
					point.dead = 1;
				}
			}
			else if(i == 3){
				if(j==0 || j==15){
					point.dead = 1;
				}
			}
			points[i*16+j] = point;
		}
	}

	int coordinates[][2] = {{2,2},{2,26},{122,2},{122,26}};
	//Generate powerups
	powerups[0];
	for(i = 0; i < powerUpAmount;i++){
		sprite powerUp = {.x=coordinates[i][0],.y=coordinates[i][1],.width = 5,.height=5,.speedX=0,.speedY=0,.currentFrame=0,.frameNum=1,.direction=0,.Images = {{&PU},{},{},{}},.frameAmount=1,.dead=0};
		powerups[i] = powerUp;
	}
}






int generateDisplay(){
  /*
	Generates the image stored in the array displayBits
  */
  int row;
  int col;
  int x;
  int y;
  //Outer box


  //Pacman
  blitSprite2(&pacman);

  //Ghosts, walls and points
  int i;
  for (i = 0; i < ghostAmount; i++){
    if (ghosts[i].dead == 0){
      blitSprite2(&ghosts[i]);
    }
  }
  for (i = 0; i < wallAmount; i++){
      blitSprite2(&walls[i]);
  }
  for (i = 0; i < pointAmount; i++){
    if (points[i].dead == 0){
      blitSprite2(&points[i]);
    }
  }
  for (i = 0; i < powerUpAmount; i++){
    if (powerups[i].dead == 0){
      blitSprite2(&powerups[i]);
    }
  }

  //Score
	blitScoreDst(72);
  return 0;
};

void spriteMoving(){
	/*
	Handles all the sprite Moving of the game
	*/
  move(&pacman);
  stopAtEdge(&pacman);
  int i = 0;

  for(i = 0; i < ghostAmount;i++){
		if (ghosts[i].dead == 0){
			move(&ghosts[i]);
	    stopAtEdge(&ghosts[i]);
		}
    ghostAI(&ghosts[i]);
  }
  collisions();
}

void collisions(){
	/*
	Handles all the collisions in the game
	*/
  int i;
  int j;

  //Check pacman and ghost collision
  for(i = 0; i < ghostAmount; i++){
    if (spriteRectCollision(&pacman,&ghosts[i])){
      if (powerUpMode){
					if (ghosts[i].dead == 0){
						ghosts[i].dead = 1;
						score += 30;
						ghosts[i].x =(34+(56*(rand()%2)));
						ghosts[i].y = 10;
						ghosts[i].lastX = ghosts[i].x;
						ghosts[i].lastY = 10;
					}
      }
			else{
				at = 4;
				namePointer = 0;
				resetGame();
			}
    }
  }

  //Check pacman and walls collision
  for(i = 0; i < wallAmount; i++){
    checkSpriteCollision(&pacman,&walls[i],0);
  }

  //Check pacman and points collision
	int goToNextLevel = 1;
	for(i = 0; i < pointAmount;i++){
    if (points[i].dead == 0){
			if(spriteRectCollision(&pacman,&points[i])){
        score+=1;
        points[i].dead = 1;
      }
			else{
				goToNextLevel = 0;
			}
    }
  }
	if (goToNextLevel){
		nextLevel();
	}


  //Check pacman and powerup collision
  for(i = 0; i < powerUpAmount;i++){
    if (powerups[i].dead == 0){
      if(spriteRectCollision(&pacman,&powerups[i])){
        score+=10;
        powerups[i].dead = 1;
        powerUpMode = 1;
        powerUpCount = 0;
      }
    }
  }
  //Check ghosts and walls collision
  for(i = 0; i < ghostAmount; i++){
    for(j=0; j < wallAmount; j++){
      checkSpriteCollision(&ghosts[i],&walls[j],0);
    }
  }
}

void checkSpriteCollision(struct sprite *object1, struct sprite *object2, int bounce){
	/*
	Param object1 = The target sprite
	Param object2 = The second target sprite
	Param bounce = Decides whetever to bounce object1 or stop it if it overlaps object2
	Stops or bounces the first target sprite if it hits the other sprite
	*/
  int collision = spriteRectCollision(object1, object2);
  if (bounce == 1){
    if (collision == 1){
      object1->x = object1->lastX;
      object1->y = object1->lastY;
      object1->speedX *= -1;
      object1->speedY *= -1;
    }
  }
  else{
    if (collision == 1){
      object1->x = object1->lastX;
      object1->y = object1->lastY;
      object1->speedX = 0;
      object1->speedY = 0;
    }
  }
}

void animation(){
	/*
	Handles the display animations
	*/

  //PowerUpMode
  if(powerUpMode){
    powerUpCount+=1;
    if (powerUpCount >= 100){
      powerUpMode = 0;
      DISPLAY_CHANGE_TO_COMMAND_MODE;
      spi_send_recv(0xA6);
    }
    else{
      DISPLAY_CHANGE_TO_COMMAND_MODE;
      if (frameCounter%10==0){
        if (colourMode == 1){
          spi_send_recv(0xA6);//A6 = Normal colours
          colourMode = 0;
        }
        else{
          spi_send_recv(0xA7);//A/ = Invert colours
          colourMode = 1;
        }
      }
    }
  }

  //Sprite Animation
  frameCounter+=1;
  animate(&pacman);
  int i;
  for(i=0; i < ghostAmount; i++){
    animate(&ghosts[i]);
  }
}

int animate(struct sprite *object){
	/*
	Param object = The target sprite
	Animates the target sprite
	*/
  if (object->frameAmount > 1){
    if ((frameCounter % 2) == 0){
      object->currentFrame += object->frameNum;
      if (object->currentFrame >= object->frameAmount-1){
        object->frameNum = -1;
      }
      if (object->currentFrame <= 0){
        object->frameNum = 1;
      }
    }
  }
}

int move(struct sprite *object){
	/*
	Param object = The target sprite
	Moves the target sprite using its velocity attributes
	*/
  object->lastX = object->x;
  object->lastY = object->y;
  object->x += object->speedX;
  object->y += object->speedY;

}

int bounce(struct sprite *object){
	/*
	Param object = The target sprite
	Bounces the target sprite if it hits an edge of the OLED Display
	*/
  if (object->x >= 123){
    object->x = 123;
    object->speedX = -1;
    object->direction = 0;
  }
  else if (object->x <= 0){
    object->x = 0;
    object->speedX = 1;
    object->direction = 2;
  }
  if (object->y >= 27){
    object->y = 27;
    object->speedY = -1;
    object->direction = 1;
  }
  else if(object->y <= 0){
    object->y = 0;
    object->speedY = 1;
    object->direction = 3;
  }
}

void stopAtEdge(struct sprite *object){
	/*
	Param object = The target sprite
	Stops the target sprite if it hits an edge of the OLED Display
	*/
  if (object->x >= 122){
    object->x = 122;
    object->speedX = 0;
  }
  else if (object->x <= 1){
    object->x = 1;
    object->speedX = 0;
  }
  if (object->y >= 26){
    object->y = 26;
    object->speedY = 0;
  }
  else if(object->y <= 1){
    object->y = 1;
    object->speedY = 0;
  }
}

void IOEvent(){
	/*
	The IO event handler functions.
	Decides which IOfunction will be called
	*/
  if (at == 0){
    menuIOEvent();
  }
  else if (at==1){
    PIOEvent();
  }
  else if (at==2){
    instructionsIOEvent();
  }
  else if (at==3){
    highscoreIOEvent();
  }
	else if (at==4){
		gameOverIOEvent();
	}
	else if (at==5){
		creditsIOEvent();
	}
}

void PIOEvent(){
	/*
	The pacman game's input and output events
	*/
  if(getbtns()&0x1){
		checkDirection(&pacman,1);
  }
  else if(getbtns()&0x2){
    checkDirection(&pacman,3);
  }
  else if(getbtns()&0x4){
    checkDirection(&pacman,2);
  }
  else if(getbtns()&0x8){
    checkDirection(&pacman,0);
  }
}

void user_isr(){
	/*
	The interrupt handler function.
	Run every 50th millisecond
	*/
  IOEvent(); //Check for events
  clearDisplay();
  if (at == 0){
    menuGenerateDisplay();
  }
  else if(at == 1){
    //score = TMR2;
    spriteMoving();
    animation();
    generateDisplay();
  }
  else if(at == 2){
    instructionsGenerateDisplay();
  }
  else if(at == 3){
    highscoreGenerateDisplay();
  }
	else if(at == 4){
    gameOverGenerateDisplay();
  }
	else if(at == 5){
    creditsGenerateDisplay();
  }
  updateDisplay();
  IFS(0) = IFS(0)&(~0x100);

	totalFrameCounter++;
}

int mainloop(){
	/*
	The mainloop for the whole game
	*/
  initialize();
  while(1){} //Everything is handled with interrupts
  return 0;
}

void resetGame(){
	initializeObjects();
}
void nextLevel(){
	initializeObjects();
}
