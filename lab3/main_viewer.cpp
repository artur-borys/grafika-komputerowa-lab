#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <iostream>

typedef float point3[3];

static GLfloat viewer[] = {10.0, 0.0, 0.0};
static GLfloat upVector[] = {0.0, 1.0, 0.0};
static GLfloat lookAtVector[] = {0.0, 0.0, 0.0};
static GLfloat theta = 0.0;
static GLfloat phi = 0.0;
static GLfloat theta_o = 0.0;
static GLfloat phi_o = 0.0;
static GLfloat zoom = 1.0;
static GLfloat pix2angle;
static GLint status = 0;
static GLfloat R = 10.0;
static GLfloat upY = 1.0;

static int x_pos_old = 0;
static int y_pos_old = 0;
static int z_pos_old = 0;
static int delta_x = 0;
static int delta_y = 0;
static int delta_z = 0;

static int controlMode = 0;

float x_s() {
    return (GLfloat)R*cos(theta*M_PI)*cos(phi*M_PI);
}

float y_s() {
    return (GLfloat)R*sin(phi*M_PI);
}

float z_s() {
    return (GLfloat)R*sin(theta*M_PI)*cos(phi*M_PI);
}

void Mouse(int btn, int state, int x, int y) {
    if(btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        x_pos_old = x;
        y_pos_old = y;
        status = 1;
    } else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        z_pos_old = x;
        status = 2;
    } else {
        status = 0;
    }
}

void Motion(GLsizei x, GLsizei y) {
    delta_x = x - x_pos_old;
    delta_y = y - y_pos_old;
    delta_z = x - z_pos_old;
    x_pos_old = x;
    y_pos_old = y;
    z_pos_old = x;
    glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'r':
            controlMode = (controlMode == 0) ? 1 : 0;
            break;
        case 'm':
            controlMode = (controlMode == 2) ? 0 : 2;
            break;
    }
}

void Axes(void)
{

    point3  x_min = {-5.0, 0.0, 0.0};
    point3  x_max = { 5.0, 0.0, 0.0};
    // poczatek i koniec obrazu osi x

    point3  y_min = {0.0, -5.0, 0.0};
    point3  y_max = {0.0,  5.0, 0.0};
    // poczatek i koniec obrazu osi y

    point3  z_min = {0.0, 0.0, -5.0};
    point3  z_max = {0.0, 0.0,  5.0};
    //  poczatek i koniec obrazu osi y

    glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
    glBegin(GL_LINES); // rysowanie osi x

        glVertex3fv(x_min);
        glVertex3fv(x_max);

    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
    glBegin(GL_LINES);  // rysowanie osi y

        glVertex3fv(y_min);
        glVertex3fv(y_max);                           

    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
    glBegin(GL_LINES); // rysowanie osi z

        glVertex3fv(z_min);
        glVertex3fv(z_max);

    glEnd();

}

void RenderScene()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    gluLookAt(viewer[0] + lookAtVector[0], viewer[1] + lookAtVector[1], viewer[2] + lookAtVector[2], lookAtVector[0], lookAtVector[1], lookAtVector[2], 0.0, upY, 0.0);

    Axes();

    glColor3f(1.0f, 1.0f, 1.0f);

    if(controlMode == 1) {
        if(status == 1) {
            theta += delta_x*pix2angle/25;
            phi += delta_y*pix2angle/25;
        } else if(status == 2) {
            R -= delta_z*0.05;
            if(R < 5.0) {
                R = 5.0;
            } else if(R > 15.0) {
                R = 15.0;
            }
        }

        theta = fmod(theta, M_PI*2);
        phi = fmod(phi, M_PI*2);

        viewer[0] = x_s();
        viewer[1] = y_s();
        viewer[2] = z_s();

        GLfloat phiRad = fmod(phi*M_PI, M_PI*2);
        if(
            (phiRad > M_PI/2 && phiRad < M_PI*3/2) ||
            (phiRad > (-1)*M_PI*3/2 && phiRad < (-1)*M_PI/2)
        
        ) {
            upY = -1.0;
        } else {
            upY = 1.0;
        }
    } else if(controlMode == 2) {
        if(status == 1) {
            lookAtVector[0] += delta_x*pix2angle;
            lookAtVector[1] += delta_y*pix2angle;
        } else {
            lookAtVector[2] += delta_z*pix2angle;
        }
    } else {
        if(status == 1) {
            theta_o += delta_x*pix2angle;
            phi_o += delta_y*pix2angle;
        } else if(status == 2) {
            zoom += delta_z*0.02;
        }
        
    }

    glRotatef(theta_o, 0.0, 1.0, 0.0);
    glRotatef(phi_o, 1.0, 0.0, 0.0);
    glScalef(zoom, zoom, zoom);

    glutWireTeapot(3.0);

    glFlush();

    glutSwapBuffers();
}

void MyInit() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
    pix2angle = 360/(float)horizontal;
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluPerspective(70, 1.0, 1.0, 30.0);

    if(horizontal <= vertical) {
        glViewport(0, (vertical - horizontal)/2, horizontal, horizontal);
    } else {
        glViewport((horizontal-vertical)/2, 0, vertical, vertical);
    }

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
}

int main(int argc, char *argv[])
{
    glutInit( & argc, argv );
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1440, 900);
    glutCreateWindow("Rzutowanie perspektywiczne");
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutKeyboardFunc(Keyboard);

    MyInit();

    glEnable(GL_DEPTH_TEST);

    glutMainLoop();

    return 0;
}