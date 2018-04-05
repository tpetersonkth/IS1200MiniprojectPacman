//Edited by Thomas


uint8_t spi_send_recv(uint8_t data);

/* Declare functions from mipslabfunc.c */
char * itoaconv( int num );
void quicksleep(int cyc);

//Sprite
typedef struct sprite sprite;
struct sprite{
  int x;
  int lastX;
  int y;
  int lastY;
  int width;
  int height;
  int speedX;
  int speedY;
  int currentFrame;
  int frameAmount;//The amount of frames
  int frameNum; //The frame changing values. Either 1 or -1
  int direction;  //0 = left, 1 = up, 2 = right, 3 = down
  int dead; //1 = true and 0 = false
  int counter; //Used for the ghost's respawn time
  uint8_t targetX;
  uint8_t targetY;
  uint8_t *Images[4][3];
};

extern char displayBits[4][128];//[ROW][Column]
extern int frameCounter;
extern int totalFrameCounter;
extern int at;
extern int score;
extern int wallAmount;
extern int namePointer;

extern struct sprite walls[];
extern struct sprite pacman;

//Declaring Little Font as extern
extern const uint8_t const lFont[26][5];

//Numbers
extern const uint8_t const N0[3];
extern const uint8_t const N1[3];
extern const uint8_t const N2[3];
extern const uint8_t const N3[3];
extern const uint8_t const N4[3];
extern const uint8_t const N5[3];
extern const uint8_t const N6[3];
extern const uint8_t const N7[3];
extern const uint8_t const N8[3];
extern const uint8_t const N9[3];

extern const uint8_t *numbers[10];

//Minus sign
extern const uint8_t const MINUS[5];

//Dot
extern const uint8_t const DOT[5];

//Colon sign
extern const uint8_t const COLON[5];



//LittlePacman
extern const uint8_t const LPS[5];
extern const uint8_t const LPR0[5];
extern const uint8_t const LPR1[5];
extern const uint8_t const LPL0[5];
extern const uint8_t const LPL1[5];
extern const uint8_t const LPU0[5];
extern const uint8_t const LPU1[5];
extern const uint8_t const LPD0[5];
extern const uint8_t const LPD1[5];
extern const uint8_t const PacmanDead[12][5];

//Ghosts
extern const uint8_t const G0[5];
extern const uint8_t const G1[5];

//Wall image
extern const uint8_t const W[128];
extern const uint8_t const WR[5];
extern const uint8_t const WC[1];

//Points
extern const uint8_t const P[1];

//PowerUp
extern const uint8_t const PU[5];

int mainloop(void);

/* Written as part of i/o lab: getbtns, getsw */
int getbtns(void);
int getsw(void);
