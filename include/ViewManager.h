#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H
#include <GL/glut.h>
#include <stdio.h>

class ViewManager
{
    GLfloat anglex,angley;
    GLint pre_x,pre_y;
    GLfloat hWidth, hHeight;
    GLfloat pos[3];
    GLfloat at[3];
    GLfloat up[3];
    
    GLboolean moveForward, moveBackward, moveLeft, moveRight, moveUp, moveDown;

    public:
        ViewManager(){};
        ViewManager(int width, int height);

        void look();
        void keyboard(unsigned char key, int x, int y);
        void keyboardUp(unsigned char key, int x, int y);
        void mouseMotion(int x, int y);
        void movement();

        virtual ~ViewManager();

};

#endif // VIEWMANAGER_H
