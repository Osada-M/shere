#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <time.h>


#define STEPCYCLE 400   /* 手足のひと振りに要する時のフレーム数　　 */
#define WALKCYCLE 4000  /* ステージ上を一周するのに要するフレーム数 */
#define M_PI 3.141592653589793  // 円周率


int width = 500, height = 250;
int samplingTime = 50;
char* ppm;
unsigned char texImage[256][256][3];
GLfloat color_buf[4] = {1.0, 1.0, 1.0, 1.0};
GLfloat color_buf_a[4] = {0.5, 0.5, 0.5, 1.0};  // 背景色、赤、緑、青、透明度
double p0[3], p1[3], p2[3], p3[3];
int is_rotate_z = 0;
int is_rotate_x = 0;
int is_rotate_c = 0;
int is_rotate_v = 0;
double angle_z = 0.0;
double angle_x = 0.0;
double angle_c = 0.0;
double angle_v = 0.0;


void readPPMImage(char* filename)
{
    FILE *fp;

    if ((fp = fopen(filename, "r")) == NULL){
        fprintf(stderr, "<!> Cannot open ppm file %s\n", filename);
        exit(1);
    }
    fread(texImage, 1, 256*256*3, fp);
    fclose(fp);
}

void setUpTexture(void)
{
    readPPMImage(ppm);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256,
                    0, GL_RGB, GL_UNSIGNED_BYTE, texImage);
}

/*
 * 直方体を描く
 */
static void myBox(double x, double y, double z)
{
  GLdouble hx = x * 0.5, hz = z * 0.5;

  GLdouble vertex[][3] = {
    { -hx,   -y, -hz },
    {  hx,   -y, -hz },
    {  hx,  0.0, -hz },
    { -hx,  0.0, -hz },
    { -hx,   -y,  hz },
    {  hx,   -y,  hz },
    {  hx,  0.0,  hz },
    { -hx,  0.0,  hz }
  };

  const static int face[][4] = {
    { 0, 1, 2, 3 },
    { 1, 5, 6, 2 },
    { 5, 4, 7, 6 },
    { 4, 0, 3, 7 },
    { 4, 5, 1, 0 },
    { 3, 2, 6, 7 }
  };

  const static GLdouble normal[][3] = {
    { 0.0, 0.0,-1.0 },
    { 1.0, 0.0, 0.0 },
    { 0.0, 0.0, 1.0 },
    {-1.0, 0.0, 0.0 },
    { 0.0,-1.0, 0.0 },
    { 0.0, 1.0, 0.0 }
  };

  int j;

    /* 材質を設定する */
    glMaterialfv(GL_FRONT, GL_DIFFUSE, color_buf);
    glEnable(GL_TEXTURE_2D);
    ppm = "img0.ppm";
    setUpTexture();

    glBegin(GL_QUADS);
    for (j = 0; j < 6; ++j) {
        glNormal3dv(normal[j]);
        glTexCoord2d(0.0, 0.0); glVertex3dv(vertex[face[j][3]]);
        glTexCoord2d(0.0, 1.0); glVertex3dv(vertex[face[j][2]]);
        glTexCoord2d(1.0, 1.0); glVertex3dv(vertex[face[j][1]]);
        glTexCoord2d(1.0, 0.0); glVertex3dv(vertex[face[j][0]]);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// 背景色の設定
static void myBox_a(double x, double y, double z)
{
  GLdouble hx = x * 0.5, hz = z * 0.5;

  GLdouble vertex[][3] = {
    { -hx,   -y, -hz },
    {  hx,   -y, -hz },
    {  hx,  0.0, -hz },
    { -hx,  0.0, -hz },
    { -hx,   -y,  hz },
    {  hx,   -y,  hz },
    {  hx,  0.0,  hz },
    { -hx,  0.0,  hz }
  };

  const static int face[][4] = {
    { 0, 1, 2, 3 },
    { 1, 5, 6, 2 },
    { 5, 4, 7, 6 },
    { 4, 0, 3, 7 },
    { 4, 5, 1, 0 },
    { 3, 2, 6, 7 }
  };

  const static GLdouble normal[][3] = {
    { 0.0, 0.0,-1.0 },
    { 1.0, 0.0, 0.0 },
    { 0.0, 0.0, 1.0 },
    {-1.0, 0.0, 0.0 },
    { 0.0,-1.0, 0.0 },
    { 0.0, 1.0, 0.0 }
  };

  int j;

    /* 材質を設定する */
    glMaterialfv(GL_FRONT, GL_DIFFUSE, color_buf_a);
    glBegin(GL_QUADS);
    for (j = 0; j < 6; ++j) {
        glNormal3dv(normal[j]);
        glVertex3dv(vertex[face[j][3]]);
        glVertex3dv(vertex[face[j][2]]);
        glVertex3dv(vertex[face[j][1]]);
        glVertex3dv(vertex[face[j][0]]);
    }
    glEnd();
}


/*
 * 腕／足
 */
static void armleg(double girth, double length, double r1, double r2)
{
    glRotated(r1, 1.0, 0.0, 0.0);
    myBox(girth, length, girth);
    glTranslated(0.0, -0.05 - length, 0.0);
    glRotated(r2, 1.0, 0.0, 0.0);
    myBox(girth, length, girth);
}

/*
 * 地面を描く
 */
static void myGround(double height)
{
    const static GLfloat ground[][4] = {
        { 0.6, 0.6, 0.6, 1.0 },
        { 0.3, 0.3, 0.3, 1.0 }
    };

    int i, j;

    glBegin(GL_QUADS);
    glNormal3d(0.0, 1.0, 0.0);
    for (j = -5; j <= 5; ++j) {
        for (i = -5; i < 5; ++i) {
            glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
            glVertex3d((GLdouble)i, height, (GLdouble)j);
            glVertex3d((GLdouble)i, height, (GLdouble)(j + 1));
            glVertex3d((GLdouble)(i + 1), height, (GLdouble)(j + 1));
            glVertex3d((GLdouble)(i + 1), height, (GLdouble)j);
        }
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

/*
 * 画面表示
 */
static void display(void)
{
    const static GLfloat lightpos[] = { 3.0, 4.0, 5.0, 1.0 }; /* 光源の位置 */
    static int frame = 0;                                     /* フレーム数 */

    /* STEPCYCLE に指定した枚数のフレームを描画する間に 0→1 に変化 */
    double t = (frame % STEPCYCLE) / (double)STEPCYCLE;

    /* WALKCYCLE に指定した枚数のフレームを描画する間に 0→1 に変化 */
    double s = (frame % WALKCYCLE) / (double)WALKCYCLE;

    /*
    * 以下の変数に値を設定する
    */

    double ll1 = 0.0; /* 箱男の左足の股関節の角度 */
    double ll2 = 0.0; /* 箱男の左足の膝関節の角度 */

    double rl1 = 0.0; /* 箱男の右足の股関節の角度 */
    double rl2 = 0.0; /* 箱男の右足の膝関節の角度 */

    double la1 = 0.0; /* 箱男の左腕の肩関節の角度 */
    double la2 = 0.0; /* 箱男の左腕の肘関節の角度 */

    double ra1 = 0.0; /* 箱男の右腕の肩関節の角度 */
    double ra2 = 0.0; /* 箱男の右腕の肘関節の角度 */

    double px = 0.0, pz = 0.0;      /* 箱男の位置 */
    double r = 0.0;                 /* 箱男の向き */
    double h = 0.0;                 /* 箱男の高さ */

    /* フレーム数（画面表示を行った回数）をカウントする */
    ++frame;

    /* 画面クリア */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* モデルビュー変換行列の初期化 */
    glLoadIdentity();

    /* 光源の位置を設定 */
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
    
    /* 視点の移動（物体の方を奥に移す）*/
    glTranslated(0.0, 0.0, -10.0);

    /* シーンの描画 */

    /* 地面 */
    myGround(-1.8);

    /* 箱男の位置と方向 */
    glTranslated(px, h, pz);
    glRotated(r, 0.0, 1.0, 0.0);

    glPushMatrix();{
        glTranslated(0.0, 5.0, 0.0);
        myBox_a(10.0, 10.0, -10.0);
    }
    glPopMatrix();
    
    /* 肩 */
    glPushMatrix();{
        glTranslated(-2.5, -0.8, 0.0);
        myBox(1.0, 1.0, 1.0);

        if(is_rotate_z == 1 && angle_z < 60.0){
            angle_z += 0.5;
        }
        else if(is_rotate_z == 3 && angle_z > 0.0){
            angle_z -= 0.5;
        }
        if(angle_z >= 60.0){
            is_rotate_z = 2;
        }
        else if(angle_z <= 0.0){
            is_rotate_z = 0;
        }

        if(is_rotate_x == 1 && angle_x < 60.0){
            angle_x += 0.5;
        }
        else if(is_rotate_x == 3 && angle_x > 0.0){
            angle_x -= 0.5;
        }
        if(angle_x >= 60.0){
            is_rotate_x = 2;
        }
        else if(angle_x <= 0.0){
            is_rotate_x = 0;
        }

        if(is_rotate_c == 1 && angle_c < 60.0){
            angle_c += 0.5;
        }
        else if(is_rotate_c == 3 && angle_c > 0.0){
            angle_c -= 0.5;
        }
        if(angle_c >= 60.0){
            is_rotate_c = 2;
        }
        else if(angle_c <= 0.0){
            is_rotate_c = 0;
        }

        if(is_rotate_v == 1 && angle_v < 60.0){
            angle_v += 0.5;
        }
        else if(is_rotate_v == 3 && angle_v > 0.0){
            angle_v -= 0.5;
        }
        if(angle_v >= 60.0){
            is_rotate_v = 2;
        }
        else if(angle_v <= 0.0){
            is_rotate_v = 0;
        }

        /* 上腕 */
        glTranslated(0.5, 0.0, 0.0);
        glRotated(angle_z, 0.0, 0.0, 1.0);
        glTranslated(0.75, 0.0, 0.0);
        myBox(1.5, 0.5, 0.5);

        /* 下腕 */ //armleg(0.2, 0.4, ll1, ll2);
        glPushMatrix();{
            glTranslated(0.75, 0.0, 0.0);
            glRotated(angle_x, 0.0, 0.0, 1.0);
            glTranslated(0.75, 0.0, 0.0);
            myBox(1.5, 0.5, 0.5);

            // 甲
            glPushMatrix();{
                glTranslated(0.75, 0.0, 0.0);
                glRotated(angle_c, 0.0, 0.0, 1.0);
                glTranslated(0.25, 0.0, 0.0);
                myBox(0.5, 0.5, 0.5);

                //　親指
                glPushMatrix();{
                    glTranslated(0.25, 0.0, 0.0);
                    glRotated(angle_v, 0.0, 0.0, 1.0);
                    glTranslated(0.0, 0.0, 0.5);
                    myBox(0.5, 0.2, 0.2);
                }
                glPopMatrix();

                //　人差し指
                glPushMatrix();{
                    glTranslated(0.25, 0.0, 0.0);
                    glRotated(angle_v, 0.0, 0.0, 1.0);
                    glTranslated(0.25, 0.0, 0.25);
                    myBox(0.5, 0.2, 0.2);
                }
                glPopMatrix();

                //　中指
                glPushMatrix();{
                    glTranslated(0.25, 0.0, 0.0);
                    glRotated(angle_v, 0.0, 0.0, 1.0);
                    glTranslated(0.25, 0.0, 0.0);
                    myBox(0.5, 0.2, 0.2);
                }
                glPopMatrix();

                //　薬指
                glPushMatrix();{
                    glTranslated(0.25, 0.0, 0.0);
                    glRotated(angle_v, 0.0, 0.0, 1.0);
                    glTranslated(0.25, 0.0, -0.25);
                    myBox(0.5, 0.2, 0.2);
                }
                glPopMatrix();

                //　小指
                glPushMatrix();{
                    glTranslated(0.25, 0.0, 0.0);
                    glRotated(angle_v, 0.0, 0.0, 1.0);
                    glTranslated(0.25, 0.0, -0.5);
                    myBox(0.5, 0.2, 0.2);
                }
                glPopMatrix();
            }
            glPopMatrix();
        }
        glPopMatrix();
    }
    glPopMatrix();

    glFlush();
}

static void resize(int w, int h)
{
  /* ウィンドウ全体をビューポートにする */
  glViewport(0, 0, w, h);

  /* 透視変換行列の指定 */
  glMatrixMode(GL_PROJECTION);

  /* 透視変換行列の初期化 */
  glLoadIdentity();
  gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);

  /* モデルビュー変換行列の指定 */
  glMatrixMode(GL_MODELVIEW);
}

static void keyboard(unsigned char key, int x, int y)
{
    /* ESC か q をタイプしたら終了 */
    if (key == '\033' || key == 'q') {
    exit(0);
    }
    else if(key == 'z'){
        is_rotate_z = (is_rotate_z + 1) % 4;
    }
    else if(key == 'x'){
        is_rotate_x = (is_rotate_x + 1) % 4;
    }
    else if(key == 'c'){
        is_rotate_c = (is_rotate_c + 1) % 4;
    }
    else if(key == 'v'){
        is_rotate_v = (is_rotate_v + 1) % 4;
    }
}

static void init(void)
{
  /* 初期設定 */
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glutInitWindowPosition(0, 0);
    glMatrixMode(GL_PROJECTION);
    glutInitWindowSize(height, width);
    gluPerspective(90.0, (double)width / (double)height, 0.1, 15.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void myTimer(int value)
{
    if (value==1)
    {
        glutTimerFunc(samplingTime,myTimer,1);
        glutPostRedisplay();
    }
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutTimerFunc(samplingTime, myTimer, 1);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow(argv[0]);
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboard);
    srand((unsigned int)time(NULL));
    init();
    glutMainLoop();
    return 0;
}

