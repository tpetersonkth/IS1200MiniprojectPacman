#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include <Math.h>     /* Used for some math functions in this script*/
#include "mipslab.h"  /* Declatations for this project */



void ghostAI(sprite *ghost){
  int i;
  char spread = 0x0;
  //if(!spread){
	  ghost->targetX = pacman.x;
	  ghost->targetY = pacman.y;
  //}
  if (ghost->dead){
    ghost->counter += 1;
    //score = ghost->counter;
    if(ghost->counter >= 100){
      ghost->counter = 0;
      ghost->dead = 0;
      ghost->x = 62;
      ghost->y = 14;
      ghost->speedX = 0;
      ghost->speedY = 0;
    }
  }
  else{
    if(ghost->speedY == 0 && ghost->speedX == 0){
      ghost->direction = rand()%4;//((ghost->direction)+1)%4;//frameCounter%4;
      //score = frameCounter;
      updateSpeed(ghost);
    }
    else{
      if (rand()%1 == 0){
        int a[2];
        returnDirectionComplement(ghost->direction,&a);
        checkDirection(ghost,a[rand()%2]);
      }
	  /*
		int currentDirection = ghost->direction;
		int dist[4];
		int targetDirection = 4;
		int i = 0;
		while(i < 5){
			dist[i] = 200;
			i++;
		}
		checkDirection(ghost, 0);
		if (ghost->direction == 0 && ghost->direction != oppositeDirection(currentDirection)){
			dist[0] = calculateDistance(ghost->x -1, ghost->y, ghost->targetX, ghost->targetY);
		}
		checkDirection(ghost, 1);
		if (ghost->direction == 1 && ghost->direction != oppositeDirection(currentDirection)){
			dist[1] = calculateDistance(ghost->x, ghost->y -1, ghost->targetX, ghost->targetY);
		}
		checkDirection(ghost, 2);
		if (ghost->direction == 2 && ghost->direction != oppositeDirection(currentDirection)){
			dist[2] = calculateDistance(ghost->x +5, ghost->y, ghost->targetX, ghost->targetY);
		}
		checkDirection(ghost, 3);
		if (ghost->direction == 3 && ghost->direction != oppositeDirection(currentDirection)){
			dist[3] = calculateDistance(ghost->x, ghost->y +5, ghost->targetX, ghost->targetY);
		}

		if (dist[0] != 200){
			targetDirection = 0;
		}
		i = 1;
		while(i < 4){
			if (dist[i] != 200 && targetDirection != 4){
				if (dist[i] < dist[targetDirection]){
					targetDirection = i;
				}
		}
			i++;
		}
		if (dist[targetDirection] == 200){
			targetDirection = oppositeDirection(currentDirection);
		}
		ghost->direction = targetDirection;
		checkDirection(ghost, targetDirection);
		score = dist[targetDirection];*/
    }

  }
}

void updateSpeed(sprite *object){
  //score = object->direction;
  if (object->direction == 0){//0 = left, 1 = up, 2 = right, 3 = down
    object->speedX = -1;
    object->speedY = 0;
  }
  else if (object->direction == 1){
    object->speedX = 0;
    object->speedY = -1;
  }
  else if (object->direction == 2){
    object->speedX = 1;
    object->speedY = 0;
  }
  else if (object->direction == 3){
    object->speedX = 0;
    object->speedY = 1;
  }
}

int checkDirection(sprite *object, int dir){
	int collisionDetected = 0;
	int last_dir = object->direction;
	object->direction = dir;
	updateSpeed(object);
	object->x += object->speedX;
	object->y += object->speedY;
	int i = 0;
	while(i < wallAmount){
		if(spriteRectCollision(object,&walls[i])){
			collisionDetected = 1;
		}
		i++;
	}
	object->x -= object->speedX;
	object->y -= object->speedY;
	if (collisionDetected){
		object->direction = last_dir;
		updateSpeed(object);
	}
	return collisionDetected;
}

void returnDirectionComplement(int dir,int* array){
  if (dir==0||dir==2){
    array[0] = 1;
    array[1] = 3;
  }
  else{
    array[0] = 0;
    array[1] = 2;
  }
}

int calculateDistance(int x1, int y1, int x2, int y2){
  x1 = (x2-x1); //x1 becomes delta x
  y1 = (y2-y1);
  return sqroot((x1 * x1) + (y1 * y1));
}


int sqroot(int square)
{
    int root=square/3;
    int i;
    if (square <= 0) return 0;
    for (i=0; i<32; i++)
        root = (root + square / root) / 2;
    return root;
}

int oppositeDirection(int dir){
	int opp;
	if (dir == 0){
		opp = 2;
	}else if (dir == 1){
		opp = 3;
	}else if (dir == 2){
		opp = 0;
	}else if (dir == 3){
		opp = 1;
	}
	return opp;
}
