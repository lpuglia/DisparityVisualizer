#include <GL/glut.h>
#include <FreeImage.h>
#include <stdlib.h>
#include "../include/ViewManager.h"

using namespace std;

int widthS=800, heightS=600;
int state=0;
bool grid=false, disp=false;
ViewManager camera;

int width = 0;
int height = 0;
int texture;

BYTE *pixels;
BYTE *disparity;
BYTE *gaps;

float* vertices;
float* colors;
int* indices;

BYTE* LoadImage(char* filename) {
	BYTE* vector;
	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(filename, 0);
	FIBITMAP *dib = FreeImage_Load(fifmt, filename,0);
	if( dib != NULL ){
        vector = (BYTE*)FreeImage_GetBits(dib);
        width = FreeImage_GetWidth(dib);
        height = FreeImage_GetHeight(dib);
        //printf("%d %d\n", FreeImage_GetBPP(dib)/8, FreeImage_GetPitch(dib)/(FreeImage_GetBPP(dib)/8));
	}
	return vector;
}

int getVerticesCount( int width, int height ) {
    return width * height * 3;
}

int getIndicesCount( int width, int height ) {
    return (width*height) + (width-1)*(height-2);
}

float* getVertices( int width, int height ) {
    vertices = new float[ getVerticesCount( width, height ) ];
    int i = 0;

    for ( int row=0; row<height; row++ ) {
        for ( int col=0; col<width; col++ ) {
            vertices[i++] = (float) col/10;
            vertices[i++] = (float) row/10;
            vertices[i++] = 0.0f;
        }
    }

    return vertices;
}

float* getColors( int width, int height, int state ) {
    colors = new float[ getVerticesCount( width, height ) ];
    int i = 0;
    BYTE* vector;
    if(state==0) vector=pixels;
    if(state==1) vector=disparity;
    if(state==2) vector=gaps;

    if(state==1){
        int j=0;
        for ( int row=0; row<height; row++ )
            for ( int col=0; col<width; col++ ) {
                //gray scale
                colors[i] = (float)vector[j]/80.0;
                colors[i+1] = (float)vector[j]/80.0;
                colors[i+2] = (float)vector[j]/80.0;
                i+=3; j++;
            }
    }else
        for ( int row=0; row<height; row++ )
            for ( int col=0; col<width; col++ ) {
                colors[i] = (float)vector[i+2]/255.0;
                colors[i+1] = (float)vector[i+1]/255.0;
                colors[i+2] = (float)vector[i]/255.0;
                i+=3;
            }

    return colors;
}

int* getIndices( int width, int height ) {
    indices = new int[getIndicesCount(width, height)];
    int i = 0;

    for ( int row=0; row<height-1; row++ ) {
        if ( (row&1)==0 ) { // even rows
            for ( int col=0; col<width; col++ ) {
                indices[i++] = col + row * width;
                indices[i++] = col + (row+1) * width;
            }
        } else { // odd rows
            for ( int col=width-1; col>0; col-- ) {
                indices[i++] = col + (row+1) * width;
                indices[i++] = col - 1 + + row * width;
            }
        }
    }

    return indices;
}


void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	camera.look();

    glColor3f(1.0,1.0,1.0);
    glTranslated(-width/2/10,-height/2/10,0.0);

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, vertices );
    glColorPointer( 3, GL_FLOAT, 0, colors );
    glDrawElements( GL_TRIANGLE_STRIP, getIndicesCount(width,height), GL_UNSIGNED_INT, indices );
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );

    //vertices[((rand()%(width*height))*3)+2]=(float)((rand()%100)-50)/10;

	glutSwapBuffers();
}

void reshape(int w, int h){
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y){
    switch (key) {
        case 27:  /*  Escape key  */
            exit (0);
            break;
        case 'g':
            if(!grid){
                grid = true;
                glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            }else{
                grid = false;
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            }
            break;
        case 'h':
            state++;
            state%=3;
            getColors(width, height, state);
            break;
        case 'j':
            if(!disp){
                for(int i=0; i<width*height; i++){
                    vertices[i*3+2]=(float)disparity[i]/2;
                }
                disp=true;
            }else{
                for(int i=0; i<width*height; i++){
                    vertices[i*3+2]=0;
                }
                disp=false;
            }
        default:
            camera.keyboard(key,x,y);
            break;
	}
}

void keyboardUp(unsigned char key, int x, int y){
    camera.keyboardUp(key,x,y);
}

void mouseMotion(int x, int y){
    //camera.mouseMotion(x,y);
}

void entry(int status){
}

void idle(void){
    camera.movement();
    glutPostRedisplay();
}

int main(int argc, char **argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(widthS, heightS);
	glutCreateWindow("Disparity Map Visualizer");

 	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMotion);
    glutEntryFunc(entry);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);

	glutDisplayFunc(display);
	glEnable(GL_DEPTH_TEST);

    disparity=LoadImage("../cones/bw_disparity_L.pgm");
    pixels=LoadImage("../cones/conesL.ppm");
    gaps=LoadImage("../cones/result_finalL.ppm");

    getVertices(width,height);
    getIndices(width,height);
    getColors(width, height, state);

    camera = ViewManager(widthS,heightS);
    //glutSetCursor(GLUT_CURSOR_NONE);
	glutMainLoop();
}
