#include <GL/gl.h>
#include <GL/glut.h>

typedef float point3[3];

static GLfloat viewer[] = {0.0, 0.0, 10.0};
static GLfloat theta = 0.0;
static GLfloat phi = 0.0;
static GLfloat zoom = 1.0;
static GLfloat pix2angle;
static GLint status = 0;

static int x_pos_old = 0;
static int y_pos_old = 0;
static int z_pos_old = 0;
static int delta_x = 0;
static int delta_y = 0;
static int delta_z = 0;

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

    gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    Axes();

    if(status == 1) {
        theta += delta_x*pix2angle;
        phi += delta_y*pix2angle;
    } else if(status == 2) {
        zoom += delta_z*0.02;
    }

    glRotatef(theta, 0.0, 1.0, 0.0);
    glRotatef(phi, 1.0, 0.0, 0.0);
    glScalef(zoom, zoom, zoom);

    glColor3f(1.0f, 1.0f, 1.0f);

    glutWireTeapot(3.0);

    glFlush();

    glutSwapBuffers();
}

void MyInit() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
    pix2angle = 1080/(float)horizontal;
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

    MyInit();

    glEnable(GL_DEPTH_TEST);

    glutMainLoop();

    return 0;
}