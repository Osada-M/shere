// The solor system
// author : 3EP4 - 16 長田 将

#include <stdlib.h>
#include <math.h>
#include <GLUT/GLUT.h>
//#include <GL/glut.h>  // windows の場合はこっちを使う
#define PI 3.141592     // 円周率
#pragma GCC diagnostic ignored "-Wunused-variable"          // 未使用変数の警告を無視
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"  // 非推奨の関数の使用の警告を無視


int     width = 1000, height = 1000;
double  day = 0, hour_now = 0;
int     samplingTime = 50;
double  look_x = 0.0, look_y = 60.0, look_z = 120.0;            // 視点の位置
double  move_x = 0.0, move_y = 0.0, move_z = 0.0;               // クロックごとに視点移動する相対座標
double  move_x_add = 2.0, move_y_add = 2.0, move_z_add = 4.0;   // 視点移動する距離
double  move_x_sum = 0.0, move_y_sum = 0.0, move_z_sum = 0.0;   // 視点移動した距離の合計
int     fast_boo = 0;           // 再生速度のモード
int     scale_boo = 0;          // 表示倍率のモード
int     str_boo = 1;            // 天体名の表示・非表示のモード
const char *default_str = "";   // デフォルトの天体名
double rev_const = 15.0;        // 天文単位 ( 15.0 [1000万km] )
int wire_poly = 6;              // 惑星のワイヤーの角数
int wire_num = 6;               // 惑星の角あたりのワイヤーの本数
int i = 0, num = 0;             // for や while のための変数


double speed = 80.0;                            // 全体の移動倍率
const double speed_buf = 80.0;                  // 全体の移動倍率の初期値
const double revolution = 0.3;                  // 公転の半径倍率
double r_scale_sun = 0.000001*revolution;       // 太陽の半径の倍率
double r_scale = 0.000001*revolution;           // 惑星の半径の倍率
const double rev_y = revolution * 10.0;         // 公転軌道のy座標の倍率
double satellite_buf_radius = 1.0;               // 衛星の半径の倍率
double satellite_buf_kouten = 1.0;               // 衛星の公転半径の倍率
const double satellite_buf_radius_set = 3.0;     // 衛星の半径の拡大倍率
const double satellite_buf_kouten_set = 80.0;    // 衛星の公転半径の拡大倍率
const double planet_bigger_buf_set = 20.0;      // 拡大表示しても見えない小さなの天体への更なる拡大倍率
const double planet_bigger_buf_2_set = 2000.0;  // 更に拡大表示しても見えない小さなの天体への更なる拡大倍率
double planet_bigger_buf;                       // planet_bigger_buf_setを実際に計算に使う時に使用する変数
double hour = 24.0;                             // １時間の定義（ 1日/hourが１時間 ）
const double saturn_ring_max = 0.0008;          // 土星の環がある範囲の最大値
const double saturn_ring_min = 0.00004;         // 土星の環がある範囲の最小値
const double saturn_diff = 0.0008 - 0.00004;    // 土星の環がある範囲
const double uranus_ring_max = 0.00065;         // 天王星の環がある範囲の最大値
const double uranus_ring_min = 0.00025;         // 天王星の環がある範囲の最小値
const double uranus_diff = 0.00065 - 0.00025;   // 天王星の環がある範囲
double ring_buf = 1.0;                          // 環の表示倍率
const double ring_buf_set = 8.0;                // 環の拡大倍率


// 文字の表示
// DrawString(文字列、表示画面の横幅、表示画面の縦幅、文字列のスタート位置のx座標、y座標)
void DrawString(char *str, int w, int h, int x, int y)
{
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // ２次元描画
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // 文字列の描画
    glRasterPos2f(x, y);
    while(*str){
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *str++);
    }
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}


// キーボードイベント
void myKeyboard(unsigned char key, int x, int y)
{
    // esc で終了
    if(key == 27){
        exit (0);
    }
    // 前進
    else if(key == 'w'){
        move_z = move_z_add;
        move_z_sum += move_z_add;
        move_y = move_y_add;
        move_y_sum += move_y_add;
    }
    // 後進
    else if(key == 's'){
        move_z = -move_z_add;
        move_z_sum -= move_z_add;
        move_y = -move_y_add;
        move_y_sum -= move_y_add;
    }
    // 左
    else if(key == 'a'){
        move_x = move_x_add;
        move_x_sum += move_x_add;
    }
    // 右
    else if(key == 'd'){
        move_x = -move_x_add;
        move_x_sum -= move_x_add;
    }
    // 視点のリセット
    else if(key == ' '){
        move_x = -move_x_sum;
        move_y = -move_y_sum;
        move_z = -move_z_sum;
        move_x_sum = move_y_sum = move_z_sum = 0.0;
    }
    // 再生速度の変更
    else if(key == 'z'){
        if(fast_boo == 1){
            fast_boo = 2;
            speed = 75.0*speed_buf;
        }else if(fast_boo == 2){
            fast_boo = 3;
            speed = speed_buf/40.0;
        }else if(fast_boo == 3){
            fast_boo = 0;
            speed = speed_buf;
        }else{
            fast_boo = 1;
            speed = 15.0*speed_buf;
        }
    }
    // 表示倍率の変更
    else if(key == 'x'){
        if(scale_boo == 1){
            scale_boo = 0;
            r_scale /= 50.0;
            r_scale_sun /= 5.0;
            wire_poly /= 3.0;
            wire_num /= 3.0;
            satellite_buf_radius = 1.0;
            satellite_buf_kouten = 1.0;
            ring_buf = 1.0;
        }else{
            scale_boo = 1;
            r_scale *= 50.0;
            r_scale_sun *= 5.0;
            wire_poly *= 3.0;
            wire_num *= 3.0;
            satellite_buf_radius = satellite_buf_radius_set;
            satellite_buf_kouten = satellite_buf_kouten_set;
            ring_buf = ring_buf_set;
        }
    }
    // 天体名の表示・非表示
    else if(key == 'c'){
        if(str_boo == 1){
            str_boo = 0;
        }else{
            str_boo = 1;
        }
    }
    glutPostRedisplay();
}


// 天体名の表示
// planet_name(天体半径、公転半径、公転周期、軌道軸角度、自転周期、自転軸角度、y座標の調整、天体名)
void planet_name(double radius, double kouten_r, double kouten_t, double kouten_tilt, double jiten_t, double jiten_tilt, double y_fix, const char *str, double str_x_fix, double str_y_fix)
{
    glColor3f(0.8, 0.8, 0.8);
    glTranslated(str_x_fix, str_y_fix, 0.0);
    glRotated((double)360*day/365/kouten_t, sin(kouten_tilt*PI/180), cos(kouten_tilt*PI/180), 0.0);
    glTranslated(kouten_r*revolution*rev_const, 0.0, 0.0);
    glRasterPos3f(0.0, 0.1+(0.1*y_fix), 0.0);
    const char* c = str;
    while(*c){
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c++);
    }
}


// 天体の定義
// planet(天体半径、公転半径、公転周期、軌道軸角度、自転周期、自転軸角度、y座標の調整、x座標の調整、赤、緑、青、天体名、天体を更なる拡大表示の対象にするかどうか)
void planet(double radius, double kouten_r, double kouten_t, double kouten_tilt, double jiten_t, double jiten_tilt, double y_fix, double x_fix, double r, double g, double b, const char *str, int small, double str_x_fix, double str_y_fix)
{
    if(small == 0){
        planet_bigger_buf = 1.0;
    }else if(small == 1){
        planet_bigger_buf = planet_bigger_buf_set;
    }else{
        planet_bigger_buf = planet_bigger_buf_2_set;
    }
    glPushMatrix();{
        glColor3f(r, g, b);
        glTranslated(x_fix, y_fix/rev_y, 0.0);
        glRotated((double)360*day/365/kouten_t, sin(kouten_tilt*PI/180), cos(kouten_tilt*PI/180), 0.0);
        glTranslated(kouten_r*revolution*rev_const, 0.0, 0.0);
        glRotated((double)360*jiten_t*hour_now/hour, sin(jiten_tilt*PI/180), cos(jiten_tilt*PI/180), 0.0);
        glutWireSphere(radius*r_scale*planet_bigger_buf, wire_poly, wire_num);
    }
    glPopMatrix();
    glPushMatrix();{
        if(str_boo == 1){
            planet_name(radius, kouten_r, kouten_t, kouten_tilt, jiten_t, jiten_tilt, y_fix, str, str_x_fix, str_y_fix);
        }else{
            planet_name(radius, kouten_r, kouten_t, kouten_tilt, jiten_t, jiten_tilt, y_fix, default_str, str_x_fix, str_y_fix);
        }
    }
    glPopMatrix();
}


// 軌道描画の定義
// circle(軌道半径、軌道軸角度、y座標の調整、x座標の調整)
void circle(double radius, double tilt, double y_fix, double x_fix, double x_pow, double y_pow){
    glColor3f(0.3, 0.3, 0.3);
    glTranslated(x_fix, y_fix/rev_y, 0.0);
    glRotated(-tilt, 0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    for(i=0; i<360; i++){
        glVertex3f(radius*(1-x_pow)*revolution*rev_const*sin(i*PI/180), 0.0, radius*(1-y_pow)*revolution*rev_const*cos(i*PI/180));
    }
    glEnd();
}


// 衛星の定義
// satellite(従属する天体の半径、従属する天体の公転半径、従属する天体の公転周期、従属する天体の軌道軸角度、従属する天体にあてたy座標の調整値、天体半径、公転半径、公転周期、軌道軸角度、自転周期、自転軸角度、y座標の調整、x座標の調整、赤、緑、青、描画軌道のy座標の調整、天体名、天体名のy座標の調整、リングの表示)
void satellite(double master_radius, double master_kouten_r, double master_kouten_t, double master_kouten_tilt, double master_y_fix, double radius, double kouten_r, double kouten_t, double kouten_tilt, double jiten_t, double jiten_tilt, double y_fix, double x_fix, double r, double g, double b, double line_y_fix, const char *str, double str_y_fix, int ring_boo, double str_x_fix_2, double str_y_fix_2)
{
    glRotated((double)360*day/365/master_kouten_t, master_kouten_r*sin(master_kouten_tilt*PI/180), master_kouten_r*cos(master_kouten_tilt*PI/180) , 0.0);
    glTranslated(master_kouten_r*revolution*rev_const, 0.0, 0.0);
    glPushMatrix();{planet(radius*satellite_buf_radius, kouten_r*satellite_buf_kouten, kouten_t, kouten_tilt, jiten_t, jiten_tilt, y_fix, x_fix, r, g, b, "", 0, 0.0, 0.0);}
    glPopMatrix();
    glPushMatrix();{
        if(str_boo == 1){
            planet_name(radius*satellite_buf_radius, kouten_r*satellite_buf_kouten, kouten_t, kouten_tilt, jiten_t, jiten_tilt, y_fix+str_y_fix, str, str_x_fix_2, str_y_fix_2);
        }else{
            planet_name(radius*satellite_buf_radius, kouten_r*satellite_buf_kouten, kouten_t, kouten_tilt, jiten_t, jiten_tilt, y_fix+str_y_fix, default_str, str_x_fix_2, str_y_fix_2);
        }
    }
    glPopMatrix();
//    衛星の軌道の描画
    if(ring_boo == 1){
        glTranslated(0.0, y_fix/rev_y, 0.0);
        glPushMatrix();{circle(kouten_r*satellite_buf_kouten, kouten_tilt, line_y_fix, x_fix, 0.0, 0.0);}
        glPopMatrix();

    }
}


// 土星の環の定義
// num : 1 ~ 100
void saturn_ring(float num){
    satellite(60268.0, 9.55491, 29.53216, 2.4886, 0.42559, 0.0, ring_buf*saturn_diff/100*num, 0.0, 25.33, 0.0, 0.0, -2.0, 0.0, 1.0, 1.0, 1.0, -3.0, "", 0.0, 1, 0.0, 0.0);
}


// 天王星の環の定義
// num : 1 ~ 100
void uranus_ring(float num){
    satellite(25559.0, 19.21845, 84.25301, 0.7733, 0.00197, 0.0, ring_buf*saturn_diff/100*num, 0.0, 97.86, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, -2.5, "", 0.0, 1, 0.0, 0.0);
}


// 楕円軌道への変換(円運動と反対方向に移動させて、楕円になるよう押しつぶしている。)
void ellipse(double radius, double kouten_t, double x_pow, double z_pow){
    glTranslated(-1*radius*revolution*rev_const*cos(2*PI*day/365/kouten_t)*x_pow, 0.0, radius*revolution*rev_const*sin(2*PI*day/365/kouten_t)*z_pow);
}


// ディスプレイ
void myDisplay(void)
{
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 上部に表示するコメント
    glColor3d(0.8, 0.8, 0.8);
    DrawString("The solor system,  ID : 3EP4-16,  Name : Osada Masashi", width, height, 30, 30);
    DrawString("[w] : flont", width, height, 30, 70);
    DrawString("[s] : back", width, height, 30, 100);
    DrawString("[a] : left", width, height, 30, 130);
    DrawString("[d] : right", width, height, 30, 160);
    DrawString("[space] : reset move", width, height, 30, 190);
    DrawString("[z] : run speed change", width, height, 250, 70);
    DrawString("[x] : planet size change", width, height, 250, 100);
    DrawString("[c] : planet name show / hide", width, height, 250, 130);

    // キーボードから得た移動量を反映
    glTranslated(move_x, move_y, move_z);
    move_x = move_y = move_z = 0.0;

    glPushMatrix();

    // 太陽
    glPushMatrix();{
        glColor3f(1.0, 0.5, 0.0);
        glRotated((double)360*27*day/365, 0.0, 1.0, 0.0);
        glutWireSphere(696000*r_scale_sun, 20, 20);
    }
    glPopMatrix();
    glPushMatrix();{
        if(str_boo == 1){
        glColor3f(0.8, 0.8, 0.8);
        glRasterPos3f(0.0, 0.3, 0.0);
        const char* c = "sun";
            while(*c){
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c++);
            }
        }else{
            const char* c = "";
                while(*c){
                    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c++);
                }
        }
    }
    glPopMatrix();

    // 水星
    glTranslated(-0.2, 0.0, 0.0);
    glPushMatrix();
    {
        glPushMatrix();{planet(2439.7, 0.3871, 0.2409, 7.01, 58.7, 0.01, -0.7, 0.0, 1.0, 1.0, 1.0, "mercury", 0, 0.0, 0.0);}
        glPopMatrix();
        glPushMatrix();{circle(0.3871, 7.01, 0.0, 0.0, 0.0, 0.0);}
        glPopMatrix();
    }
    glPopMatrix();

    // 金星
    glPushMatrix();{planet(6051.8, 0.7233, 0.6152, 3.39, 243, 177, -0.6, 0.0, 1.0, 1.0, 0.5, "venus", 0, 0.0, 0.0);}
    glPopMatrix();
    glPushMatrix();{circle(0.7233, 3.39, 0.0, 0.0, 0.0, 0.0);}
    glPopMatrix();

    // 地球
    glPushMatrix();{planet(6371.0, 1.0, 0.997, 0.0, 365, 23.4, 0.0, 0.0, 0.5, 1.0, 1.0, "earth", 0, 0.0, 0.0);}
    glPopMatrix();
    glPushMatrix();{circle(1.0, 0.0, 0.0, 0.0, 0.0, 0.0);}
    glPopMatrix();
    // 月(地)
    glPushMatrix();{satellite(6371.0, 1.0, 0.997, 0.0, 0.0, 1737.1, 0.0026, 0.07506, 5.15, 27.3, 6.69, -0.04, 0.0, 0.8, 0.8, 0.8, 0.0, "moon", -4.0, 1, 0.0, 0.0);}
    glPopMatrix();

    // 火星
    glTranslated(0.7, -0.5, 0.0);
    glPushMatrix();
    {
        glPushMatrix();{planet(3390.0, 1.5237, 1.8809, 1.85, 1.03, 25.2, -0.7, 0.0, 1.0, 0.5, 0.5, "mars", 0, 0.0, 0.0);}
        glPopMatrix();
        glPushMatrix();{circle(1.5237, 1.85, 0.0, 0.0, 0.0, 0.0);}
        glPopMatrix();
    }
    glPopMatrix();

    // セレス、またはケレス
    glPushMatrix();{planet(772, 2.7668, 4.602, 10.594, 0.3781, 4, -6.3, 0.0, 1.0, 1.0, 1.0, "ceres", 1, 0.0, 0.0);}
    glPopMatrix();
    glPushMatrix();{
        glTranslated(0.0, 0.0, 2.0);
        glRotated(-2.0, 1.0, 1.0, 0.0);
        circle(2.5, 10.594, 4.0, 0.0, 0.0, 0.0);
    }
    glPopMatrix();

    // 木星
    glPushMatrix();{planet(71492, 5.2026, 11.8622, 1.303, 0.414, 4, -1.6, 0.0, 1.0, 0.8, 0.5, "jupyter", 0, 0.0, 0.0);}
    glPopMatrix();
    glPushMatrix();{circle(5.2026, 1.303, 0.0, 0.0, 0.0, 0.0);}
    glPopMatrix();
    // イオ(木)
    glPushMatrix();{satellite(71492.0, 5.2026, 11.8622, 1.303, 0.414, 909.0, 0.0028, 0.0048, 0.036, 1.7692, 1.5424, -2.0, 0.0, 1.0, 1.0, 0.8, 0.0, "io", -20.0, 1, 0.0, 0.0);}
    glPopMatrix();
    // エウロパ(木)
    glPushMatrix();{satellite(71492.0, 5.2026, 11.8622, 1.303, 0.414, 1601.3695, 0.00447, 1.5512, 0.466, 3.551, 0.1, -2.0, 0.0, 1.0, 1.0, 0.8, 0.0, "europa", -35.0, 1, 0.0, 0.0);}
    glPopMatrix();
    // ガニメデ(木)
    glPushMatrix();{satellite(71492.0, 5.2026, 11.8622, 1.303, 0.414, 2631.2, 0.00714, 0.0196, 0.195, 7.155, 0.33, -2.0, 0.0, 0.8, 0.8, 0.8, 0.0, "ganymede", -50.0, 1, 0.0, 0.0);}
    glPopMatrix();
    // カリスト(木)
    glPushMatrix();{satellite(71492.0, 5.2026, 11.8622, 1.303, 0.414, 2410.3, 0.0126, 0.0457, 0.281, 16.689, 0.0, -2.0, 0.0, 0.7, 0.7, 0.7, 0.0, "callisto", -65.0, 1, 0.0, 0.0);}
    glPopMatrix();

    // 土星
    glTranslated(3.0, 0.0, 0.0);
    glPushMatrix();
    {
        glPushMatrix();{planet(60268.0, 9.55491, 29.53216, 2.4886, 0.42559, 25.33, -4.5, 0.0, 1.0, 0.8, 0.5, "saturn", 0, 0.0, 0.0);}
        glPopMatrix();
        glPushMatrix();{circle(9.55491, 2.4886, 1.0, 0.0, 0.0, 0.0);}
        glPopMatrix();
        // 土星の環
        glPushMatrix();{saturn_ring(1);}
        glPopMatrix();
        glPushMatrix();{saturn_ring(5);}
        glPopMatrix();
        glPushMatrix();{saturn_ring(25);}
        glPopMatrix();
        glPushMatrix();{saturn_ring(30);}
        glPopMatrix();
        glPushMatrix();{saturn_ring(45);}
        glPopMatrix();
        glPushMatrix();{saturn_ring(52);}
        glPopMatrix();
        glPushMatrix();{saturn_ring(56);}
        glPopMatrix();
        glPushMatrix();{saturn_ring(58);}
        glPopMatrix();
        glPushMatrix();{saturn_ring(60);}
        glPopMatrix();
        glPushMatrix();{saturn_ring(63);}
        glPopMatrix();
        glPushMatrix();{saturn_ring(68);}
        glPopMatrix();
        glPushMatrix();{saturn_ring(73);}
        glPopMatrix();
        glPushMatrix();{saturn_ring(78);}
        glPopMatrix();
        glPushMatrix();{saturn_ring(80);}
        glPopMatrix();
        glPushMatrix();{saturn_ring(95);}
        glPopMatrix();
        glPushMatrix();{saturn_ring(97);}
        glPopMatrix();
        glPushMatrix();{saturn_ring(100);}
        glPopMatrix();
        // タイタン(土)
        glPushMatrix();{satellite(60268.0, 9.55491, 29.53216, 2.4886, -4.5, 2574.73, 0.00815, 0.04369, 0.306, 15.94542, 0.0, -5.0, 0.0, 1.0, 0.7, 0.7, 0.0, "titan", -65.0, 1, 0.0, 0.0);}
        glPopMatrix();
    }
    glPopMatrix();

    //天王星
    glTranslated(8.0, -1.0, 5.0);
    glPushMatrix();
    {
        glPushMatrix();{planet(25559.0, 19.21845, 84.25301, 0.7733, 0.00197, 97.86, -2.5, 0.0, 0.8, 1.0, 1.0, "uranus", 0, 0.0, 0.0);}
        glPopMatrix();
        glPushMatrix();{circle(19.21845, 0.7733, 1.0, 0.0, 0.0, 0.0);}
        glPopMatrix();
        // 天王星の環
        glPushMatrix();{uranus_ring(1);}
        glPopMatrix();
        glPushMatrix();{uranus_ring(25);}
        glPopMatrix();
        glPushMatrix();{uranus_ring(50);}
        glPopMatrix();
        glPushMatrix();{uranus_ring(75);}
        glPopMatrix();
        glPushMatrix();{uranus_ring(92);}
        glPopMatrix();
        glPushMatrix();{uranus_ring(98);}
        glPopMatrix();
        glPushMatrix();{uranus_ring(100);}
        glPopMatrix();
    }
    glPopMatrix();

    //海王星
    glTranslated(-10.0, -3.0, -10.0);
    glPushMatrix();
    {
        glPushMatrix();{planet(24622.0, 30.047, 164.79, 1.76917, 0.00184, 28.32, -11.5, 0.0, 0.5, 0.5, 0.8, "neptune", 0, 0.0, 0.0);}
        glPopMatrix();
        glPushMatrix();{circle(30.047, 1.76917, 1.0, 0.0, 0.0, 0.0);}
        glPopMatrix();
        // トリトン(海)
        glPushMatrix();{satellite(24622.0, 30.047, 164.79, 1.76917, -11.5, 1353.4, 0.00237, 0.0161, 129.812, 5.87639, 0.0, -9.0, 0.0, 0.8, 0.8, 0.8, -2.5, "triton", -65.0, 1, 0.0, 0.0);}
        glPopMatrix();
    }
    glPopMatrix();

    //　冥王星
    glPushMatrix();{
        ellipse(39.44507, 247.74066, 0, 0.2);
        planet(1685.0, 39.44507, 247.74066, 17.089, -0.0175, 119.59, -200.0, 10.0, 0.8, 0.8, 0.8, "pluto", 1, 0.0, -50.0);
    }
    glPopMatrix();
    glPushMatrix();{circle(37.5, 17.089, -50.0, 25.5, 0.0, 0.2);}
    glPopMatrix();

    //　ハレー彗星
    glPushMatrix();{
        glTranslated(60.5, 50.0, 0.0);
        ellipse(17.83414, 75.3, 0, -0.5);
        planet(15.3, 17.83414, 75.3, 162.26269, 0.0, 0.0, 0.0, 0.0, 0.8, 0.8, 0.8, "halley", 2, 0.0, 0.0);
    }
    glPopMatrix();
    glPushMatrix();{
        glTranslated(0.0, 0.0, 7.0);
        glRotated(-3.0, 1.0, 1.0, 0.0);
        circle(16.1, 162.26269, 87.2, 64.3, 0.0, 0.5);
    }
    glPopMatrix();

    glPopMatrix();
	glutSwapBuffers();
    glFlush();
}


void myInit(char *progname)
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(progname);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, (double)width / (double)height, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(look_x, look_y, look_z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}


void myTimer(int value)
{
	if (value==1)
	{
		glutTimerFunc(samplingTime,myTimer,1);
        day += speed/hour;      // 日 (day)
        hour_now += speed;      // 時間 (hour)

		glutPostRedisplay();
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	myInit(argv[0]);
	glutTimerFunc(samplingTime,myTimer, 1);
	glutDisplayFunc(myDisplay);
    glutKeyboardFunc(myKeyboard);
	glutMainLoop();
	return 0;
}


#pragma GCC diagnostic warning "-Wunused-variable"
#pragma GCC diagnostic warning "-Wdeprecated-declarations"
