#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include <vector>

using namespace std;

#define N   50

typedef float point3[3];
typedef float vec[3];

static GLfloat viewer[] = {20.0, 0.0, 0.0};
static GLfloat upVector[] = {0.0, 1.0, 0.0};
static GLfloat lookAtVector[] = {0.0, 0.0, 0.0};
static GLfloat theta = 0.0;
static GLfloat phi = 0.0;
static GLfloat theta_o = 0.0;
static GLfloat phi_o = 0.0;
static GLfloat angles_L0[] = {0.0, 0.0};
static GLfloat angles_L1[] = {0.0, 0.0};
static GLfloat zoom = 1.0;
static GLfloat pix2angle;
static GLint status = 0;
static GLfloat R = 20.0;
static GLfloat R_light = 10.0;
static GLfloat upY = 1.0;
static GLfloat light0_position[] = {7.0, 7.0, 7.0, 1.0};
static GLfloat light1_position[] = {0.0, 10.0, 10.0, 1.0};
static GLfloat light0_colors[] = {1.0, 1.0, 1.0, 1.0}; 
static GLfloat light1_colors[] = {1.0, 0.0, 0.0, 1.0};
static bool light0_colors_dir[] = {0, 0, 1};
static bool light1_colors_dir[] = {1, 0, 0};
static bool mouse_moved = false;
static bool cullEnabled = true;
static int drawFlag = 0;

static int x_pos_old = 0;
static int y_pos_old = 0;
static int z_pos_old = 0;
static int delta_x = 0;
static int delta_y = 0;
static int delta_z = 0;

static int controlMode = 1;
static string texture = "tekstura.tga";

float x_s() {
    return (GLfloat)R*cos(theta*M_PI)*cos(phi*M_PI);
}

float y_s() {
    return (GLfloat)R*sin(phi*M_PI);
}

float z_s() {
    return (GLfloat)R*sin(theta*M_PI)*cos(phi*M_PI);
}

float xl_s(GLfloat *ang, GLfloat base) {
    return (GLfloat)(R_light + base)*cos(ang[0]*M_PI)*cos(ang[1]*M_PI) + base;
}

float yl_s(GLfloat *ang, GLfloat base) {
    return (GLfloat)(R_light + base)*sin(ang[1]*M_PI) + base;
}

float zl_s(GLfloat *ang, GLfloat base) {
    return (GLfloat)(R_light + base)*sin(ang[0]*M_PI)*cos(ang[1]*M_PI) + base;
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

/*************************************************************************************/
 // Funkcja wczytuje dane obrazu zapisanego w formacie TGA w pliku o nazwie
 // FileName, alokuje pamięć i zwraca wskaźnik (pBits) do bufora w którym
 // umieszczone są dane.
 // Ponadto udostępnia szerokość (ImWidth), wysokość (ImHeight) obrazu
 // tekstury oraz dane opisujące format obrazu według specyfikacji OpenGL
 // (ImComponents) i (ImFormat).
 // Jest to bardzo uproszczona wersja funkcji wczytującej dane z pliku TGA.
 // Działa tylko dla obrazów wykorzystujących 8, 24, or 32 bitowy kolor.
 // Nie obsługuje plików w formacie TGA kodowanych z kompresją RLE.
/*************************************************************************************/


GLbyte *LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat)
{

/*************************************************************************************/

// Struktura dla nagłówka pliku  TGA


    #pragma pack(1)           
    typedef struct                       
    {
        GLbyte    idlength;             
        GLbyte    colormaptype;          
        GLbyte    datatypecode;            
        unsigned short    colormapstart; 
        unsigned short    colormaplength;
        unsigned char     colormapdepth;  
        unsigned short    x_orgin;        
        unsigned short    y_orgin;        
        unsigned short    width;         
        unsigned short    height;        
        GLbyte    bitsperpixel;                  
        GLbyte    descriptor;            
    }TGAHEADER;
    #pragma pack(8)

    FILE *pFile;                   
    TGAHEADER tgaHeader;           
    unsigned long lImageSize;       
    short sDepth;                   
    GLbyte    *pbitsperpixel = NULL; 

          
/*************************************************************************************/ 

// Wartości domyślne zwracane w przypadku błędu

    *ImWidth = 0;               
    *ImHeight = 0;
    *ImFormat = GL_BGR_EXT;
    *ImComponents = GL_RGB8;
   
    pFile = fopen(FileName, "rb");
    if(pFile == NULL)
      return NULL;

/*************************************************************************************/
// Przeczytanie nagłówka pliku 


    fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);
                

/*************************************************************************************/

// Odczytanie szerokości, wysokości i głębi obrazu

    *ImWidth = tgaHeader.width;
    *ImHeight = tgaHeader.height;
    sDepth = tgaHeader.bitsperpixel / 8;


/*************************************************************************************/
// Sprawdzenie, czy głębia spełnia założone warunki (8, 24, lub 32 bity)
   
    if(tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
        return NULL;

/*************************************************************************************/

// Obliczenie rozmiaru bufora w pamięci


    lImageSize = tgaHeader.width * tgaHeader.height * sDepth;


/*************************************************************************************/   

// Alokacja pamięci dla danych obrazu


     pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));
   
      if(pbitsperpixel == NULL)
        return NULL;

    if(fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
        {
        free(pbitsperpixel);
        return NULL;
        }
   

/*************************************************************************************/

// Ustawienie formatu OpenGL


    switch(sDepth)

        {

        case 3:    

            *ImFormat = GL_BGR_EXT;

            *ImComponents = GL_RGB8;

            break;

        case 4:

            *ImFormat = GL_BGRA_EXT;

            *ImComponents = GL_RGBA8;

            break;

        case 1:

            *ImFormat = GL_LUMINANCE;

            *ImComponents = GL_LUMINANCE8;

            break;

        };

     

    fclose(pFile);

       

    return pbitsperpixel;

}

/*************************************************************************************/ 

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
    mouse_moved = 1;
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
        case 't':
            texture = (texture.compare("tekstura.tga") == 0) ? "tekstura2.tga" : "tekstura.tga";
            glutSetWindowTitle("Teksturowanie");
            glutPostRedisplay();
            break;
        case 'c':
            if(cullEnabled) {
                glDisable(GL_CULL_FACE);
            } else {
                glEnable(GL_CULL_FACE);
            }
            cullEnabled = !cullEnabled;
            glutPostRedisplay();
            break;
        case 'k':
            drawFlag = (drawFlag + 1) % 3;
            glutPostRedisplay();
            break;
        case 'b':
            texture = "pwr_special.tga";
            glutSetWindowTitle("BIERNACENIE");
            glutPostRedisplay();
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

vector<vector<GLfloat>> createEggTexCoords() {
    vector<vector<GLfloat>> texCoords;
    for(int i = 0; i < N; i++) {
        float u = (float)((float)i/(N-1));
        for(int j = 0; j < N; j++) {
            float v = (float)((float)j/(N-1));
            vector<GLfloat> coord = {u, v};
            if(texCoords.size() > N*N / 2) {
                coord[0] = 1.0 - coord[0];
                coord[1] = 1.0 - coord[1];
            }
            texCoords.push_back(coord);
        }
    }

    return texCoords;
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
    vector<vector<GLfloat>> texCoords = createEggTexCoords();
    glTranslatef(0.0, -5.0, 0.0);
    for(int i = 0; i < N*N - N - 1; i++) {
        glBegin(GL_TRIANGLE_STRIP);
        glNormal3f(eggNormal[i][0], eggNormal[i][1], eggNormal[i][2]);
        glTexCoord2f(texCoords[i][0], texCoords[i][1]);
        glVertex3f(egg[i][0], egg[i][1], egg[i][2]);

        glNormal3f(eggNormal[i+1][0], eggNormal[i+1][1], eggNormal[i+1][2]);
        glTexCoord2f(texCoords[i+1][0], texCoords[i+1][1]);
        glVertex3f(egg[i+1][0], egg[i+1][1], egg[i+1][2]);

        glNormal3f(eggNormal[i+N][0], eggNormal[i+N][1], eggNormal[i+N][2]);
        glTexCoord2f(texCoords[i+N][0], texCoords[i+N][1]);
        glVertex3f(egg[i+N][0], egg[i+N][1], egg[i+N][2]);

        glNormal3f(eggNormal[i+N+1][0], eggNormal[i+N+1][1], eggNormal[i+N+1][2]);
        glTexCoord2f(texCoords[i+N+1][0], texCoords[i+N+1][1]);
        glVertex3f(egg[i+N+1][0], egg[i+N+1][1], egg[i+N+1][2]);

        glNormal3f(eggNormal[i+2][0], eggNormal[i+2][1], eggNormal[i+2][2]);
        glTexCoord2f(texCoords[i+2][0], texCoords[i+2][1]);
        glVertex3f(egg[i+2][0], egg[i+2][1], egg[i+2][2]);

        glEnd();
    }
    
}

void drawTriangle() {
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_TRIANGLES);
    glNormal3f(1.0, 0.0, 0.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(0.0, 5.0, 0.0);
    glNormal3f(1.0, 0.0, 0.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.0, 0.0, 5.0);
    glNormal3f(1.0, 0.0, 0.0);
    glTexCoord2f(0.5, 1.0);
    glVertex3f(0.0, 0.0, -5.0);
    glEnd();
}

void drawRect(vector<GLfloat> rotation) {
    glColor3f(1.0, 1.0, 1.0);

    glPushMatrix();

    glRotatef(rotation[0], 1.0, 0.0, 0.0);
    glRotatef(rotation[1], 0.0, 1.0, 0.0);
    glRotatef(rotation[2], 0.0, 0.0, 1.0);

    glBegin(GL_TRIANGLE_STRIP);

    glNormal3f(1.0, 0.0, 0.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(5.0, -5.0, -5.0);

    glNormal3f(1.0, 0.0, 0.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(5.0, 5.0, -5.0);

    glNormal3f(1.0, 0.0, 0.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(5.0, -5.0, 5.0);

    glNormal3f(1.0, 0.0, 0.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(5.0, 5.0, 5.0);

    glEnd();

    glPopMatrix();

}

void drawCube(){
    vector<GLfloat> rot1 = {0, 0, 0};
    vector<GLfloat> rot2 = {0, 0, 90.0};
    vector<GLfloat> rot3 = {180.0, 0, 180.0};
    vector<GLfloat> rot4 = {0, 0, -90.0};
    vector<GLfloat> rot5 = {0, 90.0, 0};
    vector<GLfloat> rot6 = {0, -90.0, 0};

    drawRect(rot1);
    drawRect(rot2);
    drawRect(rot3);
    drawRect(rot4);
    drawRect(rot5);
    drawRect(rot6);
}

void RenderScene()
{
    GLbyte *pBytes;
    GLint ImWidth, ImHeight, ImComponents;
    GLenum ImFormat;


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    gluLookAt(viewer[0] + lookAtVector[0], viewer[1] + lookAtVector[1], viewer[2] + lookAtVector[2], lookAtVector[0], lookAtVector[1], lookAtVector[2], 0.0, upY, 0.0);

    Axes();

    glColor3f(1.0f, 1.0f, 1.0f);

    pBytes = LoadTGAImage(texture.c_str(), &ImWidth, &ImHeight, &ImComponents, &ImFormat);

    /*************************************************************************************/

    // Zdefiniowanie tekstury 2-D

    glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);

    /*************************************************************************************/

    // Zwolnienie pamięci
        
    free(pBytes);

    if(mouse_moved) {
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

    }
    
    glPushMatrix();

    glRotatef(theta_o, 0.0, 1.0, 0.0);
    glRotatef(phi_o, 1.0, 0.0, 0.0);
    glScalef(zoom, zoom, zoom);

    switch(drawFlag) {
        case 0:
            drawTriangle();
            break;
        case 1:
            drawCube();
            break;
        case 2:
            drawEgg(createEgg(), createEggNormal());
            break;
    }
  
    glPopMatrix();
    glPushMatrix();
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glPopMatrix();
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_colors);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_colors);

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
   
    GLfloat mat_shininess  = {50.0};
    // współczynnik n opisujący połysk powierzchni

/*************************************************************************************/
// Definicja źródła światła


    GLfloat light_ambient[] = {0.1, 0.1, 0.1, 1.0};
    // składowe intensywności świecenia źródła światła otoczenia
    // Ia = [Iar,Iag,Iab]

    GLfloat light_diffuse[] = {};      
    // składowe intensywności świecenia źródła światła powodującego
    // odbicie dyfuzyjne Id = [Idr,Idg,Idb]

    GLfloat light_specular[]= {0.0, 0.0, 1.0, 1.0};
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


    GLfloat light_ambient2[] = {0.1, 0.1, 0.1, 1.0};
    // składowe intensywności świecenia źródła światła otoczenia
    // Ia = [Iar,Iag,Iab]

    GLfloat light_diffuse2[] = {1.0, 0.25, 0.0, 1.0};       
    // składowe intensywności świecenia źródła światła powodującego
    // odbicie dyfuzyjne Id = [Idr,Idg,Idb]

    GLfloat light_specular2[]= {1.0, 0.25, 0.0, 1.0};
    // składowe intensywności świecenia źródła światła powodującego
    // odbicie kierunkowe Is = [Isr,Isg,Isb]

    GLfloat att_constant2  = {1.0};
    // składowa stała ds dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

    GLfloat att_linear2    = {0.05};
    // składowa liniowa dl dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

    GLfloat att_quadratic2  = {0.001};
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
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_colors);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_colors);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);


/*************************************************************************************/
// Ustawienie opcji systemu oświetlania sceny

    glShadeModel(GL_SMOOTH); // właczenie łagodnego cieniowania
    //glEnable(GL_LIGHTING);   // właczenie systemu oświetlenia sceny
    glEnable(GL_LIGHT0);     // włączenie źródła o numerze 0
    glEnable(GL_DEPTH_TEST); // włączenie mechanizmu z-bufora

/*************************************************************************************/

/*************************************************************************************/

    // Teksturowanie będzie prowadzone tyko po jednej stronie ściany

    glEnable(GL_CULL_FACE);

    /*************************************************************************************/

    //  Przeczytanie obrazu tekstury z pliku o nazwie tekstura.tga

    //pBytes = LoadTGAImage("tekstura.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);

    /*************************************************************************************/

    // Zdefiniowanie tekstury 2-D

    //glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);

    /*************************************************************************************/

    // Zwolnienie pamięci
        
    //free(pBytes);

    /*************************************************************************************/

    // Włączenie mechanizmu teksturowania

    glEnable(GL_TEXTURE_2D);

    /*************************************************************************************/

    // Ustalenie trybu teksturowania

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    /*************************************************************************************/

    // Określenie sposobu nakładania tekstur

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
    pix2angle = 360/(float)horizontal;
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluPerspective(70, 1.0, 1.0, 100.0);

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
    glutCreateWindow("Tekstury");
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutKeyboardFunc(Keyboard);
    //glutIdleFunc(changeColors);

    MyInit();

    glEnable(GL_DEPTH_TEST);

    glutMainLoop();

    return 0;
}