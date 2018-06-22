#include "../include/ViewManager.h"

using namespace std;

ViewManager::ViewManager(int width, int height){
    hWidth=width/2;
    hHeight=height/2;
    //glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer(hWidth, hHeight/2);
    pos[0] = 0.0;
    pos[1] = 0.0;
    pos[2] = 70.0;
    
    at[0] = 0.0;
    at[1] = 0.0;
    at[2] = 0.0;
    
    up[0] = 0.0;
    up[1] = 1.0;
    up[2] = 0.0;
    
    pre_x=0.0;
    pre_y=0.0;
    anglex = 0.0;
    angley = 0.0;

    moveForward=false; moveBackward=false;
    moveLeft=false; moveRight=false;
    moveUp=false; moveDown=false;
}

void ViewManager::look(){
    // gluLookAt(0.0,-10.0,0.0,
	   //    0.0,0.0,0.0,
	   //    0.0,0.0,1.0
	   //   );
    gluLookAt(pos[0], pos[1], pos[2], at[0], at[1], at[2], up[0], up[1], up[2]);
    //glTranslatef(0.0, 0.0, pos[2]);
    glRotatef(anglex, 0.0, 1.0, 0.0);
    glRotatef(angley, 1.0, 0.0, 0.0);
}

void ViewManager::keyboard(unsigned char key, int x, int y){
	switch (key) {
        case 'w':
            moveForward = true;
            break;
        case 's':
            moveBackward = true;
            break;
	case 'a':
            moveLeft = true;
            break;
        case 'd':
            moveRight = true;
            break;
	case 'r':
            moveUp = true;
            break;
        case 'f':
            moveDown = true;
            break;
        default:
            break;
	}
}

void ViewManager::keyboardUp(unsigned char key, int x, int y){
	switch (key) {
        case 'w':
            moveForward = false;
            break;
        case 's':
            moveBackward = false;
            break;
        case 'a':
            moveLeft = false;
            break;
        case 'd':
            moveRight = false;
            break;
	case 'r':
            moveUp = false;
            break;
        case 'f':
            moveDown = false;
            break;
        default:
            break;
	}
}

void ViewManager::mouseMotion(int x, int y){
    int centerX = hWidth;
    int centerY = hHeight;
    glutWarpPointer(centerX, centerY);

    angley -= (y)*0.01;
    anglex += (x)*0.01;
    pre_x = x;
    pre_y = y;
}

void ViewManager::movement(){
    if(moveForward){
	pos[2]+=0.5;
	//at[2]+=0.5;
    }
    if(moveBackward){
	pos[2]-=0.5;
	//at[2]-=0.5;
    }
    if(moveLeft){
        anglex -= 0.5;
	//pos[0]-=0.5;
	//at[0]-=0.5;
    }
    if(moveRight){
        anglex += 0.5;
	//pos[0]+=0.5;
	//at[0]+=0.5;
    }
    if(moveUp){
        angley -= 0.5;
	//pos[1]-=0.5;
	//at[1]-=0.5;
    }
    if(moveDown){
        angley += 0.5;
	//pos[1]+=0.5;
	//at[1]+=0.5;
    }
}

ViewManager::~ViewManager(){
}
