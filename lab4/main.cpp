#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include <vector>

using namespace std;

#define N   100

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

GLfloat xuv(double u, double v) {
    return (-90.0*pow(u, 5) + 225.0*pow(u, 4) - 270.0*pow(u, 3) + 180.0*pow(u, 2) - 45.0*u)*cos(M_PI*v);
}

GLfloat yuv(double u, double v) {
    return 160.0*pow(u, 4) - 320.0*pow(u, 3) + 160.0*pow(u, 2);
}

GLfloat zuv(double u, double v) {
    return (-90.0*pow(u, 5) + 225.0*pow(u, 4) - 270.0*pow(u, 3) + 180.0*pow(u, 2) - 45.0*u)*sin(M_PI*v);
}

GLfloat xun(double u, double v) {
    return (-450.0*pow(u, 4) + 900.0*pow(u, 3) - 810.0*pow(u, 2) + 360.0*u - 45.0)*cos(M_PI*v);
}

GLfloat xvn(double u, double v) {
    return M_PI*(90.0*pow(u, 5) - 225.0*pow(u, 4) + 270.0*pow(u, 3) - 180.0*pow(u, 2) + 45.0*u)*sin(M_PI*v);
}

GLfloat yun(double u, double v) {
    return 640.0*pow(u, 3) - 960.0*pow(u, 2) + 320.0*u;
}

GLfloat yvn(double u, double v) { return 0.0f; }

GLfloat zun(double u, double v) {
    return (-450.0*pow(u, 4) + 900.0*pow(u, 3) - 810.0*pow(u, 2) + 360.0*u - 45.0)*sin(M_PI*v);
}

GLfloat zvn(double u, double v) {
    return -M_PI*(90.0*pow(u, 5) - 225.0*pow(u, 4) + 270.0*pow(u, 3) - 180.0*pow(u, 2) + 45.0*u)*cos(M_PI*v);
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

vector<vector<GLfloat>> createEgg() {
    vector<vector<GLfloat>> egg;
    for(int i = 0; i < N; i++) {
        float u = (float)((float)i/(N-1));
        for(int j = 0; j < N; j++) {
            float v = (float)((float)j/(N-1));
            vector<GLfloat> point = {xuv(u, v), yuv(u, v), zuv(u, v)};
            egg.push_back(point);
        }
    }

    return egg;
}

vector<vector<GLfloat>> createEggNormal() {
    vector<vector<GLfloat>> eggNormal;
    for(int i = 0; i < N; i++) {
        float u = (float)((float)i/(N-1));
        for(int j = 0; j < N; j++) {
            float v = (float)((float)j/(N-1));
            vector<vector<GLfloat>> point = {{xun(u, v), xvn(u, v)}, {yun(u, v), yvn(u, v)}, {zun(u, v), zvn(u, v)}};
            vector<GLfloat> normal = {
                                        point[1][0]*point[2][1] - point[2][0]*point[1][1],
                                        point[2][0]*point[0][1] - point[0][0]*point[2][1],
                                        point[0][0]*point[1][1] - point[1][0]*point[0][1]
                                    };
            double length = sqrt(pow(normal[0], 2) + pow(normal[1], 2) + pow(normal[2], 2));
            normal[0] = normal[0]/length;
            normal[1] = normal[1]/length;
            normal[2] = normal[2]/length;

            if(i >= N/2) {
                normal[0] *= -1.0;
                normal[1] *= -1.0;
                normal[2] *= -1.0;
            }
            eggNormal.push_back(normal);
        }
    }

    return eggNormal;
}

void drawEgg(vector<vector<GLfloat>> egg, vector<vector<GLfloat>> eggNormal) {
    glTranslatef(0.0, -5.0, 0.0);
    for(int i = 0; i < N*N - N - 1; i++) {
        glBegin(GL_TRIANGLE_STRIP);
        glNormal3f(eggNormal[i][0], eggNormal[i][1], eggNormal[i][2]);
        glVertex3f(egg[i][0], egg[i][1], egg[i][2]);

        glNormal3f(eggNormal[i+1][0], eggNormal[i+1][1], eggNormal[i+1][2]);
        glVertex3f(egg[i+1][0], egg[i+1][1], egg[i+1][2]);

        glNormal3f(eggNormal[i+N][0], eggNormal[i+N][1], eggNormal[i+N][2]);
        glVertex3f(egg[i+N][0], egg[i+N][1], egg[i+N][2]);

        glNormal3f(eggNormal[i+N+1][0], eggNormal[i+N+1][1], eggNormal[i+N+1][2]);
        glVertex3f(egg[i+N+1][0], egg[i+N+1][1], egg[i+N+1][2]);

        glNormal3f(eggNormal[i+2][0], eggNormal[i+2][1], eggNormal[i+2][2]);
        glVertex3f(egg[i+2][0], egg[i+2][1], egg[i+2][2]);

        glEnd();
    }
    
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

    //glutSolidTeapot(3.0);

    drawEgg(createEgg(), createEggNormal());

    glFlush();

    glutSwapBuffers();
}

void MyInit() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    /*************************************************************************************/

//  Definicja materiału z jakiego zrobiony jest czajnik
//  i definicja źródła światła

/*************************************************************************************/


/*************************************************************************************/ 
// Definicja materiału z jakiego zrobiony jest czajnik

    GLfloat mat_ambient[]  = {1.0, 1.0, 1.0, 1.0};       
    // współczynniki ka =[kar,kag,kab] dla światła otoczenia

    GLfloat mat_diffuse[]  = {1.0, 1.0, 1.0, 1.0};
    // współczynniki kd =[kdr,kdg,kdb] światła rozproszonego

    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
    // współczynniki ks =[ksr,ksg,ksb] dla światła odbitego               
   
    GLfloat mat_shininess  = {20.0};
    // współczynnik n opisujący połysk powierzchni

/*************************************************************************************/
// Definicja źródła światła

     GLfloat light_position[] = {0.0, 0.0, 10.0, 1.0};   
    // położenie źródła


    GLfloat light_ambient[] = {0.1, 0.1, 0.1, 1.0};
    // składowe intensywności świecenia źródła światła otoczenia
    // Ia = [Iar,Iag,Iab]

    GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};       
    // składowe intensywności świecenia źródła światła powodującego
    // odbicie dyfuzyjne Id = [Idr,Idg,Idb]

    GLfloat light_specular[]= {1.0, 1.0, 1.0, 1.0};
    // składowe intensywności świecenia źródła światła powodującego
    // odbicie kierunkowe Is = [Isr,Isg,Isb]

    GLfloat att_constant  = {1.0};
    // składowa stała ds dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

    GLfloat att_linear    = {0.05};
    // składowa liniowa dl dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

    GLfloat att_quadratic  = {0.001};
    // składowa kwadratowa dq dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

/*************************************************************************************/                                      
// Ustawienie parametrów materiału i źródła światła

/*************************************************************************************/
// Ustawienie patrametrów materiału


    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

/*************************************************************************************/
// Ustawienie parametrów źródła

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);


/*************************************************************************************/
// Ustawienie opcji systemu oświetlania sceny

    glShadeModel(GL_SMOOTH); // właczenie łagodnego cieniowania
    glEnable(GL_LIGHTING);   // właczenie systemu oświetlenia sceny
    glEnable(GL_LIGHT0);     // włączenie źródła o numerze 0
    glEnable(GL_DEPTH_TEST); // włączenie mechanizmu z-bufora

/*************************************************************************************/
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