#include <GL/glut.h>
#include <FreeImage.h>
#include "../include/ViewManager.h"
#include <string.h>

using namespace std;

int widthS=640, heightS=400;
int state=0;
bool grid=false, disp=true;
ViewManager camera;

int width = 0;
int height = 0;
int texture;

BYTE *pixels;
BYTE *disparity;
unsigned char* alpha;

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

int getIndicesCount( int width, int height ) {
    return (width*height) + (width-1)*(height-2);
}

float* getVertices( int width, int height ) {
    vertices = new float[ width * height * 3 ];
    int i = 0;
    int j = 0;
    for ( int row=0; row<height; row++ ) {
        for ( int col=0; col<width; col++ ) {
            vertices[i++] = (float) col/10;
            vertices[i++] = (float) row/10;
            vertices[i++] = (float)disparity[j++]/4.0;
        }
    }

    return vertices;
}

float* getColors( int width, int height) {
    colors = new float[ width * height * 4 ];
    int i = 0;
    BYTE* vector;
    if(state==0) vector=pixels;
    if(state==1) vector=disparity;

    int j=0;
    int k=0;
    for ( int row=0; row<height; row++ )
        for ( int col=0; col<width; col++ ) {
            colors[i] = (float)vector[k+2]/255.0;
            colors[i+1] = (float)vector[k+1]/255.0;
            colors[i+2] = (float)vector[k]/255.0;
            if (disparity[j]==0 || alpha[j]==1.0){
                colors[i+3] = 0.0;
            }
            else{
                colors[i+3] = 1.0;
            }
            j++;
            k+=3;
            i+=4;
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

int* setTransparencies(int width, int height ) {
    int i = width;
    int threshold = 3;
    for ( int row=1; row<height-1; row++ ) {
        for (int col=0; col<width; col++ ) {
            if(
                disparity[i]-disparity[i+1]      >threshold || 
                disparity[i]-disparity[i-1]      >threshold || 
                disparity[i]-disparity[i+width]  >threshold || 
                disparity[i]-disparity[i-width]  >threshold || 
                disparity[i]-disparity[i+width-1]>threshold || 
                disparity[i]-disparity[i-width+1]>threshold ||
                disparity[i]-disparity[i+width+1]>threshold || 
                disparity[i]-disparity[i-width-1]>threshold
            ){
                alpha[i]         = 1;
                alpha[i+1]       = 1;
                alpha[i-1]       = 1;
                alpha[i+width]   = 1;
                alpha[i-width]   = 1;
                alpha[i+width-1] = 1;
                alpha[i-width+1] = 1;
                alpha[i+width+1] = 1;
                alpha[i-width-1] = 1;
            }
            i++;
        }
    }
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	camera.look();

    glTranslated(-width/2/10,-height/2/10,0.0);

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, vertices );
    glColorPointer( 4, GL_FLOAT, 0, colors );
    glDrawElements( GL_TRIANGLE_STRIP, getIndicesCount(width,height), GL_UNSIGNED_INT, indices );
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    
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
        default:
            camera.keyboard(key,x,y);
            break;
	}
}

void keyboardUp(unsigned char key, int x, int y){
    camera.keyboardUp(key,x,y);
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
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);

	glutDisplayFunc(display);
    glEnable (GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    disparity=LoadImage((char*)"../cones/bw_disparity_L.pgm");
    pixels=LoadImage((char*)"../cones/conesL.ppm");
    alpha = (unsigned char*) malloc(width*height);

    getVertices(width,height);
    getIndices(width,height);
    memset(alpha, 0, width*height);
    setTransparencies(width,height);
    getColors(width, height);

    camera = ViewManager(widthS,heightS);
	glutMainLoop();

}
