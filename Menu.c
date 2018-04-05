//Written by Thomas

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for this project */
#include <math.h>

//Used to check when a button is pressed for the events
int BTN1held = 0;
int BTN2held = 0;
int BTN3held = 0;
int BTN4held = 0;

//Menu
int pointer = 0;
char options[4][12] = {"PLAY        ","INSTRUCTIONS","HIGHSCORE   ","CREDITS     "};
int optionsAmount = 4;//Number of options available


//Instruction page
char instructions[][4][21] = {{"THE GOAL OF THIS GAME","IS TO EARN THE MOST  ","AMOUNT OF POINTS     ","BY EATING DOTS       "},
{"MAKE SURE TO AVOID   ","GHOSTS. OR YOU CAN   ","EAT THEM             ","                     "},
{"THE FOLLOWING PAGE   ","CONTAINS CONTROLS TO ","CONTROL PACMAN       ","                     "},
{"BTN4 - LEFT          ","BTN3 - RIGHT         ","BTN2 - DOWN          ","BTN1 - UP            "}
};
int instructionIndex = 0;
int instructionAmount = 3;//Number of instruction pages

//Highscore pages
int highscoreIndex = 0;
int highscorePagesAmount = 4; //The amount of highscore pages
int highscoreAmount = 20;
int highscores[20];
char highscoreNames[20][7];

//Game over
int submittingHighscore = 0;
int namePointer = 0;//Decides which letters should be submitted to the highscore list
int currentCharacter = 0;
char choosenName[7];
int currentLetter = 0;
int animationCounter = 0;

//Credits page
char authors[2][21] ={"THOMAS PETERSON      ","SEBASTIAN MARTINELLE "};
int creditsIndex = 0;

/* INSTRUCTIONS
THE GOAL OF THIS GAME IS TO EARN THE MOST AMOUNT OF POINTS BY EATING DOTS.

BUT REMEMBER TO AVOID GHOSTS. HOWEVER, IF YOU EAT A POWERUP YOU CAN EAT THE GHOSTS AND EARN BONUS POINTS.

THE FOLLOWING PAGE CONTAINS THE CONTROLS

BTN1 - UP
BTN2 - DOWN
BTN3 - RIGHT
BTN4 - LEFT
*/


//Menu

void menuGenerateDisplay(){
  blitImage(0,0,3,5,numbers[pointer%optionsAmount]);
  displayString(20,7,options[1],12);
  displayString(20,1,options[0],5);
  displayString(20,13,options[2],9);
  displayString(20,19,options[3],7);
  displayString(10,1+6*(pointer%optionsAmount),"|",1);

  //Print Button instructions
  displayString(10,26,"-",1);
  displayString(40,26,"U",1);
  displayString(70,26,"OK",2);
  displayString(100,26,"D",1);
}

void menuIOEvent(){
  if (BTN1held != (getbtns()&0x1) && getbtns()&0x1){//Go up in the menu
    pointer+=1;
  }
  if (BTN2held != ((getbtns()&0x2)>>1) && ((getbtns()&0x2))>>1){
    if (pointer%optionsAmount == 0){
      score=0;
      at = 1;//Switch to pacman game
	    srand(totalFrameCounter);
    }
    else if(pointer%optionsAmount == 1){
      instructionIndex = 0;
      at = 2;//Switch to instructions;
    }
    else if(pointer%optionsAmount == 2){
      at = 3;//Switch to highscore page
    }//at = 4 is the gameover page
    else if(pointer%optionsAmount == 3){
      creditsIndex = 0;
      at = 5;//switch to credit page
    }
  }
  if (BTN3held != ((getbtns()&0x4)>>2) && ((getbtns()&0x4))>>2){//Go down in the menu
    if (pointer == 0){
      pointer = 4;
    }
    else{
      pointer-=1;
    }
  }


  BTN1held = getbtns()&0x1;
  BTN2held = (getbtns()&0x2)>>1;
  BTN3held = (getbtns()&0x4)>>2;
}


//Instructions

void instructionsGenerateDisplay(){
  displayString(0,0,instructions[instructionIndex][0],21);
  displayString(0,6,instructions[instructionIndex][1],21);
  displayString(0,12,instructions[instructionIndex][2],21);
  displayString(0,18,instructions[instructionIndex][3],21);

  //Print Button instructions
  displayString(10,26,"B",1);
  displayString(40,26,"L",1);
  displayString(70,26,"-",1);
  displayString(100,26,"R",1);
}

void instructionsIOEvent(){
  if (BTN1held != (getbtns()&0x1) && getbtns()&0x1){//Go up in the menu
    if (instructionIndex >= instructionAmount){
      instructionIndex = 0;
    }
    else{
      instructionIndex+=1;
    }
  }

  if (BTN3held != ((getbtns()&0x4)>>2) && ((getbtns()&0x4))>>2){//Go down in the menu
    if (instructionIndex == 0){
      instructionIndex = instructionAmount;
    }
    else{
      instructionIndex-=1;
    }
  }
  if(getbtns()&0x8){ //Go backwards
    at = 0;//back
  }


  BTN1held = getbtns()&0x1;
  BTN3held = (getbtns()&0x4)>>2;
}


//Highscore

void highscoreGenerateDisplay(){
  int i;
  int len;
  for(i=0; i < 4; i++){
    len = 1;
    if (i+highscoreIndex*4+1 >= 10){
        len = 2;
    }
    displayString(10,6*i,itoaconv(i+highscoreIndex*4+1),len);
    blitImage(10+len*5,6*i,5,5,&COLON);

    if (highscores[i+highscoreIndex*4]==-1){
      displayString(50,6*i,"-",1);
    }
    else{
      char str[15];
      sprintf(str, "%d", highscores[i+highscoreIndex*4]);
      displayString(100,6*i,str,getLength(highscores[i+highscoreIndex*4]));
      displayString(50,6*i,highscoreNames[i+highscoreIndex*4],6);
    }
  }

  //Buttons
  displayString(10,26,"B",1);
  displayString(40,26,"L",1);
  displayString(70,26,"-",1);
  displayString(100,26,"R",1);
}

void highscoreIOEvent(){
  if (BTN1held != (getbtns()&0x1) && getbtns()&0x1){//Go up in the menu
    if (highscoreIndex >= highscorePagesAmount){
      highscoreIndex = 0;
    }
    else{
      highscoreIndex += 1;
    }
  }

  if (BTN3held != ((getbtns()&0x4)>>2) && ((getbtns()&0x4))>>2){//Go down in the menu
    if (highscoreIndex == 0){
      highscoreIndex = highscorePagesAmount;
    }
    else{
      highscoreIndex-=1;
    }
  }

  if(getbtns()&0x8){
    highscoreIndex = 0;
    at = 0;//back
  }

  BTN1held = getbtns()&0x1;
  BTN3held = (getbtns()&0x4)>>2;
}

void resetHighscore(){
  /*
  Resets the highscore list
  */
  int i;
  for(i = 0; i < highscoreAmount;i++){
      highscores[i] = -1;
      strncpy(highscoreNames[i],"THOMAS", 7);//In case of missing name, Thomas is default hehe ..
  }
}

int saveScore(int score, char name[6]){
  /*
  Param score = The score to be saved
  Param name = The player's name
  Saves a player's name and score to the highscore list
  Returns integer = Number representing the row which was replaced
  */
  int saved = -1;
  int firstLoop = 1;
  int buffer = score;
  char nameBuffer[7];
  int temp;
  int i;
  for(i=0;i<highscoreAmount;i++){
    if (buffer > highscores[i]){
      if (firstLoop){
        saved = i;
      }
      //switch the scores' places
      temp = highscores[i];
      highscores[i] = buffer;
      buffer = temp;

      //Switch the names corresponding to the score
      strncpy(nameBuffer, highscoreNames[i], 7);
      strncpy(highscoreNames[i],name, 7);
      strncpy(name, nameBuffer, 7);
    }
  }
  return saved;
}

int checkScore(int score){
  /*
  Param score = The score used when comparing to the highscores
  Checks which position the score will get in the highscore list
  Returns integer = Number representing the target tow for the score in the highscore array
  */
  int savedPosition = -1;
  int firstLoop = 1;
  int buffer = score;
  int temp;
  int i;
  for(i=0;i<highscoreAmount;i++){
    if (buffer > highscores[i]){
      if (firstLoop){
        savedPosition = i;
        firstLoop = 0;
      }
    }
  }
  return savedPosition;
}

int getLength(int score){
  /*
  Param score = The value to get the length from
  Returns: The length of the value
  */
  int len = 0;
  if (score == 0){
    return 1;
  }
  while (score>=1){
    len+=1;
    score /= 10;
  }
  return len;
}

//GameOver

void gameOverGenerateDisplay(){
  displayString(42,0,"GAME OVER",9);
  displayString(10,6,"SCORE:",6);
  char str[15];
  sprintf(str, "%d", score);
  displayString(46,6,str,getLength(score));
  if (checkScore(score) == -1){
    displayString(10,26,"-",1);
    displayString(40,26,"-",1);
    displayString(70,26,"OK",2);
    displayString(100,26,"-",1);
  }
  else{
    displayString(10,12,"YOUR POSITION:",14);
    char str2[15];
    sprintf(str2, "%d", checkScore(score)+1);
    displayString(94,12,str2,getLength(checkScore(score)+1));

    displayString(10,18,"ENTER NAME:",11);
    int i;
    for(i = 0; i < currentLetter;i++){//Display the already set character
      if (choosenName[i] == ' '){
        displayString(76+6*i,18," ",1);
      }
      else{
        displayChar(76+6*i,18,choosenName[i]);
      }
    }

    //Display the currently selected character
    if (namePointer == 26){//If spcae is selected
        displayString(76+6*currentLetter,18," ",1); //Unnecessary, kept for clarity, it exists
    }
    else{
      displayChar(76+6*currentLetter,18,65+namePointer);
    }

    animationCounter++;
    if (animationCounter<=10){//Animate the line under the selected character
        displayString(76+6*currentLetter,21,"-",1);
    }
    if (animationCounter >= 20){
      animationCounter = 0;
    }

    displayString(10,26,"B",1);
    displayString(40,26,"L",1);
    displayString(70,26,"OK",2);
    displayString(100,26,"R",1);
  }
}

void gameOverIOEvent(){
  if (BTN1held != (getbtns()&0x1) && getbtns()&0x1){//Go up in the menu
    if (namePointer >= 26){//0 to 15 is letters, 26 is a space
      namePointer = 0;
    }
    else{
      namePointer+=1;
    }
  }
  if (BTN2held != ((getbtns()&0x2)>>1) && ((getbtns()&0x2))>>1){
    //saveScore(score);
    if (checkScore(score) != -1){
      if (namePointer == 26){
        choosenName[currentLetter] = ' ';
      }
      else{
        choosenName[currentLetter] = namePointer+65;
      }
      currentLetter+=1;
      if (currentLetter >= 6){
        int temp = checkScore(score);
        highscoreIndex = 0;
        while(temp >= 4){
          temp -= 4;
          highscoreIndex+=1;
        }
        saveScore(score, choosenName);
        at = 3;
        currentLetter = 0;
      }
    }
    else{
      at = 0;
    }
  }
  if (BTN3held != ((getbtns()&0x4)>>2) && ((getbtns()&0x4))>>2){//Go down in the menu
    if (namePointer == 0){
      namePointer = 26;
    }
    else{
      namePointer-=1;
    }
  }
  if (BTN4held != ((getbtns()&0x8)>>3) && ((getbtns()&0x8))>>3){//Go down in the menu
    if (currentLetter > 0){
      currentLetter -= 1;
      namePointer = choosenName[currentLetter]-65;
    }
  }

  BTN1held = getbtns()&0x1;
  BTN2held = (getbtns()&0x2)>>1;
  BTN3held = (getbtns()&0x4)>>2;
  BTN4held = (getbtns()&0x8)>>3;
}

//Credits page
void creditsGenerateDisplay(){
  if (creditsIndex==0){
    displayString(0,0,"AUTHORS:",8);
    displayString(0,6,"---------------------",21);
    displayString(0,12,authors[0],21);
    displayString(0,18,authors[1],21);
  }
  else{
    displayString(0,0,"SPECIAL THANKS TO:   ",21);
    displayString(0,6,"---------------------",21);
    displayString(0,12,"THE KTH TEACHERS     ",21);
    displayString(0,18,"AND THE ASSISTANTS   ",21);
  }


  displayString(10,26,"B",1);
  displayString(40,26,"L",1);
  displayString(70,26,"-",1);
  displayString(100,26,"R",1);
}

void creditsIOEvent(){
  if (BTN1held != (getbtns()&0x1) && getbtns()&0x1){//Go up in the menu
    if (creditsIndex >= 1){//0 to 25 is letters, 26 is a space
      creditsIndex = 0;
    }
    else{
      creditsIndex+=1;
    }
  }

  if (BTN3held != ((getbtns()&0x4)>>2) && ((getbtns()&0x4))>>2){//Go down in the menu
    if (creditsIndex == 0){
      creditsIndex = 1;
    }
    else{
      creditsIndex-=1;
    }
  }
  if (BTN4held != ((getbtns()&0x8)>>3) && ((getbtns()&0x8))>>3){
    at = 0;
  }

  BTN1held = getbtns()&0x1;
  BTN3held = (getbtns()&0x4)>>2;
  BTN4held = (getbtns()&0x8)>>3;
}
