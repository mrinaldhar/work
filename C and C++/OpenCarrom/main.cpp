#include <iostream>
#include <cmath>
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/OpenGL.h>
#else
#include <GL/glut.h>
#endif
#include <pthread.h>
#include <chrono>
#include <cstdio>
#include <thread>

using namespace std;

#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)
#define NUM_THREADS	11

pthread_t threads[NUM_THREADS];
// void update(int value);

void endTurn();

class scoreBoard {
public:
	volatile int board[2];
	int turn=0;
	int flag = 0;
	int shown = 0;
	int showwhat = 0;
	double lu, ld, ru, rd;
	void init() {
		turn = 0;
		board[0] = board[1] = 30;
	}
	void turnover() {
		turn = (turn + 1) % 2;
	}
	void pts(int val) {
		board[turn] += val;
	}
	void hide() {
		lu = ld = ru = rd = 0.0f;
		shown = 0;

	}
	void show() {
		shown = 1;
		lu = ru = rd = ld = 0.7f;

	}

};


class strikerAim {
public:
	double x, y, theta, length;
	void reset() {
		length = 0.5f;
		theta = DEG2RAD(90);

	}
	void cw(double valx, double valy) {
		theta -= DEG2RAD(2);
		x = valx + cos(theta) * length;
		y = valy + sin(theta) * length;
		cout << "Point is: " << y << " " << x << " " << cos(theta) << " " << length << endl;
	}
	void acw(double valx, double valy) {
		theta += DEG2RAD(2);
		x = valx + cos(theta) * length;
		y = valy + sin(theta) * length;
	}
	void update(double valx, double valy) {
		x = valx + cos(theta) * length;
		y = valy + sin(theta) * length;
	}
	void manual(double xman, double yman, double valx, double valy) {
		x = valx + xman;
		y = valy + yman;
	}

};
class CircularObj {
public:
	double x;
	double y;
	double rad;
	double rgb[3];
	double velx;
	double vely;
	double mass;
	void setpos(double xpos, double ypos) {
		x = xpos;
		y = ypos;
	}
	void setrad(double radius) {
		rad = radius;
	}
	void printpos() {
		cout << "Position is: (" << x << ", " << y << ")" << endl;
	}
	void disappear() {
		rad = 0.0;
	}
	void setcolor(double r, double g, double b) {
		velx = 0;
		vely = 0;
		rgb[0] = r;
		rgb[1] = g;
		rgb[2] = b;
	}

};

class Striker: public CircularObj{
public:
	double mass = 5.0;
};

class Coin: public CircularObj {
public:
	double mass = 5.0;
};

Coin coins[9];
Striker striker;
strikerAim striker_Aim;
scoreBoard scores;

int step=0;
double box_len = 1.80f;
double uk = 0.25;
int initial = 0;

void drawBall(float rad) {
    
   glBegin(GL_TRIANGLE_FAN);
   for(int i=0 ; i<360 ; i++) {
       glVertex2f(rad * cos(DEG2RAD(i)), rad * sin(DEG2RAD(i)));
   }
   glEnd();
}

void drawCircle(GLfloat x, GLfloat y, GLfloat r) {
    static const double inc = M_PI / 12;
    static const double max = 2 * M_PI;
    glBegin(GL_LINE_LOOP);
    for(double d = 0; d < max; d += inc) {
        glVertex2f(cos(d) * r + x, sin(d) * r + y);
    }
    glEnd();
}

void initObjects() {
striker.setpos(0.0f, -0.56f);
    striker.setrad(0.065f);
    striker.setcolor(0.784f, 0.560f, 0.129f);
    striker.printpos();

    striker_Aim.reset();
   	striker_Aim.update(striker.x, striker.y);

   	scores.init();

	coins[0].setpos(0.0f, 0.13f);
    coins[0].setrad(0.05f);
    coins[0].setcolor(0.0f,0.0f,0.0f);

    coins[1].setpos(0.0f, -0.13f);
    coins[1].setrad(0.05f);
    coins[1].setcolor(0.0f,0.0f,0.0f);

    coins[2].setpos(0.13f, 0.0f);
    coins[2].setrad(0.05f);
    coins[2].setcolor(0.0f,0.0f,0.0f);

    coins[3].setpos(-0.13f, 0.0f);
    coins[3].setrad(0.05f);
    coins[3].setcolor(0.0f,0.0f,0.0f);

    coins[4].setpos(0.11f, 0.11f);
    coins[4].setrad(0.05f);
    coins[4].setcolor(0.79f,0.79f,0.79f);

    coins[5].setpos(0.11f, -0.11f);
    coins[5].setrad(0.05f);
    coins[5].setcolor(0.79f,0.79f,0.79f);

    coins[6].setpos(-0.11f, -0.11f);
    coins[6].setrad(0.05f);
    coins[6].setcolor(0.79f,0.79f,0.79f);

    coins[7].setpos(-0.11f, 0.11f);
    coins[7].setrad(0.05f);
    coins[7].setcolor(0.79f,0.79f,0.79f);

    coins[8].setpos(0.0f,0.0f);
    coins[8].setrad(0.05f);
    coins[8].setcolor(0.792f,0.109f,0.149f);
striker_Aim.reset();


}


void display(void) {
	glClearColor(0.737f, 0.529f, 0.364f, 1.0f);

	glClear (GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	glEnable( GL_LINE_SMOOTH );
glEnable( GL_POLYGON_SMOOTH );
glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_MODELVIEW);


	glColor3f(0.980f, 0.980f, 0.890f);
    glRectf(-0.90f,0.90f, 0.90f, -0.90f);	//Main rectangle board
	 glPushMatrix();


	 glPushMatrix();
	  glLineWidth(1.5f);

    glTranslatef(-0.83f, -0.83f, 0.0f);		//Four holes, at corners
 glColor3f(0.160f, 0.160f, 0.160f);
    drawBall(0.1f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.83f, 0.83f, 0.0f);
    drawBall(0.1f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.83f, 0.83f, 0.0f);
    drawBall(0.1f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.83f, -0.83f, 0.0f);
    drawBall(0.1f);
    glPopMatrix();		

    drawCircle(0.83, 0.83, 0.102f);
    drawCircle(0.83, -0.83, 0.102f);
    drawCircle(-0.83, 0.83, 0.102f);
    drawCircle(-0.83, -0.83, 0.102f);

    				//corner holes end



 glColor3f(0.556f, 0.070f, 0.039f);		//inside rectangle, just for styling
 glLineWidth(1.8f);
glBegin(GL_LINES);
    glVertex2f(0.58f,0.63f);
    glVertex2f(-0.58f,0.63f);
glEnd();

glBegin(GL_LINES);
    glVertex2f(-0.63f,0.58f);
    glVertex2f(-0.63f,-0.58f);
glEnd();
glBegin(GL_LINES);
    glVertex2f(-0.58f,-0.63f);
    glVertex2f(0.58f,-0.63f);
glEnd();
glBegin(GL_LINES);
    glVertex2f(0.63f,-0.58f);
    glVertex2f(0.63f,0.58f);
glEnd();
    drawCircle(0.0f,0.0f,0.22f);		//inside styling with circles
    drawCircle(-0.58f,-0.58f, 0.05f);
    drawCircle(0.58f,-0.58f,0.05f);
    drawCircle(0.58f,0.58f,0.05f);
    drawCircle(-0.58f,0.58f,0.05f);		//inside styling ends

 glColor3f(0.0f, 0.0f, 0.0f);		//Striker Aim

 glLineWidth(0.9f);

glBegin(GL_LINES);
    glVertex2f(striker.x, striker.y);
    glVertex2f(striker_Aim.x, striker_Aim.y);
glEnd();


int i;
for (i=0; i<9; i++) {
 	glPushMatrix();
    glTranslatef(coins[i].x, coins[i].y, 0.0f);
    glColor3f(coins[i].rgb[0],coins[i].rgb[1],coins[i].rgb[2]);
    drawBall(coins[i].rad);
    glPopMatrix();
 glColor3f(0.0f, 0.0f, 0.0f);		//Striker Aim
 glLineWidth(1.5f);
 if (coins[i].rgb[0] == 0.0f) {

    drawCircle(coins[i].x, coins[i].y, coins[i].rad+0.003f);
 glColor3f(1.0f, 1.0f, 1.0f);		//Striker Aim

 }
    drawCircle(coins[i].x, coins[i].y, 2*coins[i].rad/3);
    drawCircle(coins[i].x, coins[i].y, coins[i].rad);

}
 glLineWidth(1.5f);

	glPushMatrix();
    glTranslatef(striker.x, striker.y, 0.0f);
    glColor3f(striker.rgb[0],striker.rgb[1],striker.rgb[2]);
    drawBall(striker.rad);
    glPopMatrix();
 glColor3f(0.0f, 0.0f, 0.0f);		//Striker Aim

    drawCircle(striker.x, striker.y, 3*striker.rad/4);
    drawCircle(striker.x, striker.y, striker.rad);

    glPushMatrix();    
	string s;
	
    glTranslatef(-0.15f, 0.93f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);//needs to be called before RasterPos
    glRasterPos2i(0.0f , 0.0f );
    s = "Score : " + std::to_string(scores.board[scores.turn]);;
    void * font = GLUT_BITMAP_9_BY_15;

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();

    glPushMatrix(); 
	
    glTranslatef(0.53f, 0.93f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);//needs to be called before RasterPos
    glRasterPos2i(0.0f , 0.0f );
    s = "Player" + std::to_string(scores.turn+1);

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();
    // update(1);

    if (scores.shown == 1) {

	glColor3f(0.0549f, 0.749f, 0.9137f);
	glRectf(-scores.lu,scores.ru, scores.rd, -scores.ld);	//ScoreBoard
	glColor3f(0.0f, 0.0f, 0.0f);
	
	glPushAttrib(GL_ENABLE_BIT); 
	glLineStipple(5, 0xAAAA);  
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);
	glVertex2f(-scores.lu, scores.lu);
	glVertex2f(scores.ru, scores.ru);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(scores.lu, scores.lu);
	glVertex2f(scores.ru, -scores.ru);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(scores.lu, -scores.lu);
	glVertex2f(-scores.ru, -scores.ru);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(-scores.lu, -scores.lu);
	glVertex2f(-scores.ru, scores.ru);
	glEnd();

	glPopAttrib();
	 

	glPushMatrix(); 
	
    glTranslatef(-0.35f, 0.60f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "WELCOME TO OpenCARROM";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    s = "_____________________";
    glTranslatef(0.0f, -0.03f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
      for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();

    glPushMatrix(); 
	
    glTranslatef(-0.65f, 0.45f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "A Multi-threaded, Multi-player";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();

	glPushMatrix(); 
	
    glTranslatef(-0.20f, 0.40f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "openGL game by Mrinal Dhar";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    s = "_____________________";
    glTranslatef(-0.15f, -0.06f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
      for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();

    

    glPushMatrix(); 
	
    glTranslatef(-0.25f, 0.20f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = ":: Controls ::";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    s = "___________________________________";
    glTranslatef(-0.32f, -0.10f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
      for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    s = "___________________________________";
    glTranslatef(0.0f, -0.1f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
      for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();

    glPushMatrix(); 
	
    glTranslatef(-0.55f, 0.03f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "Key/Action";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();
    glPushMatrix(); 
	
    glTranslatef(0.15f, 0.03f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "What it does";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();

    glPushMatrix(); 
	
    glTranslatef(-0.60f, -0.1f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "<RETURN>";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();

    glPushMatrix(); 
	
    glTranslatef(-0.60f, -0.18f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "<ESC>";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();
    glPushMatrix(); 
	
    glTranslatef(-0.60f, -0.25f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "<SPACEBAR>";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();
    glPushMatrix(); 
	
    glTranslatef(-0.60f, -0.32f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "R";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();
    glPushMatrix(); 
	
    glTranslatef(-0.60f, -0.39f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "A or D";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();

    glPushMatrix(); 
	
    glTranslatef(-0.60f, -0.46f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "<ARROW UP/DOWN>";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();

    glPushMatrix(); 
	
    glTranslatef(-0.60f, -0.53f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "<ARROW LEFT/RIGHT>";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();

    glPushMatrix(); 
	
    glTranslatef(-0.60f, -0.60f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "<MOUSE>";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();



// ---

    glPushMatrix(); 
	
    glTranslatef(0.1f, -0.1f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "Show this screen";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();

    glPushMatrix(); 
	
    glTranslatef(0.1f, -0.18f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "Quit Game";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();
    glPushMatrix(); 
	
    glTranslatef(0.1f, -0.25f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "Hit Striker";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();
    glPushMatrix(); 
	
    glTranslatef(0.1f, -0.32f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "Reset Game";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();
    glPushMatrix(); 
	
    glTranslatef(0.1f, -0.39f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "Striker Aim";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();

    glPushMatrix(); 
	
    glTranslatef(0.1f, -0.46f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "Striker Power";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();

    glPushMatrix(); 
	
    glTranslatef(0.1f, -0.53f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "Adjust Striker";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();

    glPushMatrix(); 
	
    glTranslatef(0.1f, -0.60f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "Aim, Power & Hit";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();

    glPushMatrix(); 
	
    glTranslatef(-0.67f, -0.83f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);//needs to be called before glRasterPos strflag
    glRasterPos2i(0.0f , 0.0f );
    s = "Simplicity is the ultimate Sophistication";

    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();

}
	

    glutSwapBuffers();
	glFlush();

}

double ptDistance(CircularObj a, CircularObj b) {
	double dist;
	dist = (b.y - a.y)*(b.y - a.y) + (b.x - a.x)*(b.x - a.x);
	return sqrt(dist);
}

double rawDistance(double x1, double y1, double x2, double y2) {
	double dist;
	dist = (y1 - y2)*(y1 - y2) + (x1 - x2)*(x1 - x2);
	return sqrt(dist);
}

double ptSlope(double x1, double y1, double x2, double y2) {
	return (y1-y2)/(x1-x2);
}

double velSlope(CircularObj thisone) {
	return (thisone.vely)/(thisone.velx);

}

void *handleCollision(void * threadid) {

		// int * ip = (int *)threadid;

		// int i = *ip;
	while(1) {
	long i;
   i = (long)threadid;
		int j;
		float distance;
    float angle1;
    float angle2;
    float mag1;
    float mag2;
    float normal;
    float u1x, u2x, u1y, u2y;
    float v1x, v2x, v1y, v2y;  


		if(coins[i].rad + coins[i].x > box_len/2 || coins[i].x - coins[i].rad < -box_len/2)
        coins[i].velx *= -1;
    if(coins[i].rad + coins[i].y > box_len/2 || coins[i].y - coins[i].rad < -box_len/2)
        coins[i].vely *= -1;
 
      

        for(j = 0; j < 9; j++)
        {
            if(i != j)
            {
                distance = sqrt(pow(coins[i].x - coins[j].x, 2) + pow(coins[i].y - coins[j].y, 2));
                if(distance <= coins[i].rad + coins[j].rad)
                {
                float xdistance = coins[i].x - coins[j].x;
                float ydistance = coins[i].y - coins[j].y;
                float normalx = xdistance / distance;
                float normaly = ydistance / distance;
                float tangenty = normalx;
                float tangentx = -normaly;
                float ball1ScalarNormal = (normalx * (coins[i].velx)) + (normaly * (coins[i].vely));
                float ball2ScalarNormal = (normalx * (coins[j].velx)) + (normaly * (coins[j].vely));
                float ball1ScalarTangential = (tangentx * (coins[i].velx)) + (tangenty * (coins[i].vely));
                float ball2ScalarTangential = (tangentx * (coins[j].velx)) + (tangenty * (coins[j].vely));
                float ball1ScalarNormalAfter = (ball1ScalarNormal * (coins[i].mass - coins[j].mass) + 2 * coins[j].mass * ball2ScalarNormal) / (coins[i].mass + coins[j].mass);
                float ball2ScalarNormalAfter = (ball2ScalarNormal * (coins[j].mass - coins[i].mass) + 2 * coins[i].mass * ball1ScalarNormal) / (coins[i].mass + coins[j].mass);
                coins[i].velx = normalx*ball1ScalarNormalAfter + tangentx*ball1ScalarTangential;
                coins[i].vely = normaly*ball1ScalarNormalAfter + tangenty*ball1ScalarTangential;
                coins[j].velx = normalx*ball2ScalarNormalAfter + tangentx*ball2ScalarTangential;
                coins[j].vely = normaly*ball2ScalarNormalAfter + tangenty*ball2ScalarTangential;
 // cout << "\a";

                coins[i].velx *= 1.1;		//Dirty fix
                coins[i].vely *= 1.1;

                coins[j].velx *= 0.99;		//Dirty Fix
                coins[j].vely *= 0.99;

                }
            }
        }
        distance = sqrt(pow(striker.x - coins[i].x, 2) + pow(striker.y - coins[i].y, 2));
        if(distance <= striker.rad + coins[i].rad)
        {
           float xdistance = striker.x - coins[i].x;
           float ydistance = striker.y - coins[i].y;
           float normalx = xdistance / distance;
           float normaly = ydistance / distance;
           float tangenty = normalx;
           float tangentx = -normaly;
           float ball1ScalarNormal = (normalx * (striker.velx)) + (normaly * (striker.vely));
           float ball2ScalarNormal = (normalx * (coins[i].velx)) + (normaly * (coins[i].vely));
           float ball1ScalarTangential = (tangentx * (striker.velx)) + (tangenty * (striker.vely));
           float ball2ScalarTangential = (tangentx * (coins[i].velx)) + (tangenty * (coins[i].vely));
           float ball1ScalarNormalAfter = (ball1ScalarNormal * (striker.mass - coins[i].mass) + 2 * coins[i].mass * ball2ScalarNormal) / (coins[i].mass + striker.mass);
           float ball2ScalarNormalAfter = (ball2ScalarNormal * (coins[i].mass - striker.mass) + 2 * striker.mass * ball1ScalarNormal) / (coins[i].mass + striker.mass);
           striker.velx = normalx*ball1ScalarNormalAfter + tangentx*ball1ScalarTangential;
           striker.vely = normaly*ball1ScalarNormalAfter + tangenty*ball1ScalarTangential;
           coins[i].velx = normalx*ball2ScalarNormalAfter + tangentx*ball2ScalarTangential;
           coins[i].vely = normaly*ball2ScalarNormalAfter + tangenty*ball2ScalarTangential;

           coins[i].velx *= 1.1;	// Dirty Fix
           coins[i].vely *= 1.1;

           striker.velx *= 0.99;	//Dirty Fix
           striker.vely *= 0.99;
 // cout << "\a";
        }



        if (((coins[i].x > 0.73f && coins[i].y > 0.73f) || (coins[i].x < -0.73f && coins[i].y > 0.73f) || (coins[i].x < -0.73f && coins[i].y < -0.73f) || (coins[i].x > 0.73f && coins[i].y < -0.73f)) && coins[i].rad == 0.05f) {
        	coins[i].rad = 0.0f;
        	coins[i].x = -0.5f + (double)i/9;
        	coins[i].y = -0.95f;
      		std::this_thread::sleep_for(std::chrono::milliseconds(10));
      		coins[i].rad = 0.03f;
      		coins[i].velx = 0.0f;
      		coins[i].vely = 0.0f;
      		scores.pts(10);
      		if (i == 9) {
      			scores.pts(40);
      		}
      		scores.flag = 1;
        }

     coins[i].velx *= 0.95;
         coins[i].vely *= 0.95;
             coins[i].x += coins[i].velx;
      coins[i].y += coins[i].vely;


      std::this_thread::sleep_for(std::chrono::milliseconds(10));
 }
 // return handleCollision(threadid);
        pthread_exit(NULL);
}
void update(int value) {

    	int rc;
   int i;

 if (((striker.x > 0.73f && striker.y > 0.73f) || (striker.x < -0.73f && striker.y > 0.73f) || (striker.x < -0.73f && striker.y < -0.73f) || (striker.x > 0.73f && striker.y < -0.73f))) {
        scores.pts(-5);
        endTurn();
    }


    	 // Handle ball collisions with box
    if((striker.x + striker.rad > box_len/2 || striker.x - striker.rad < -box_len/2) && (striker.velx > 0.001 || striker.velx < -0.001))
        striker.velx *= -1;
    if((striker.y + striker.rad > box_len/2 || striker.y - striker.rad < -box_len/2) && (striker.vely > 0.001 || striker.vely < -0.001))
        striker.vely *= -1;

        if(((striker.velx < 0.001f) && (striker.velx > -0.001f)) && ((striker.vely < 0.001f) && (striker.vely > -0.001f)) && (striker.velx != striker.vely != 0))
    {
            endTurn();
    }

    if (striker.velx != striker.vely != 0) {
    	striker_Aim.length = 0.0f;
    	striker_Aim.update(striker.x, striker.y);
    }


    striker.velx *= 0.95;
    striker.vely *= 0.95;

    striker.x += striker.velx;
    striker.y += striker.vely;

    glutTimerFunc(10, update, 0);
}

void reset() {
	initObjects();
}

void endTurn() {

	glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
glPushMatrix(); // put current matrix on stack

glTranslatef(0.0f, 0.0f, 0.0f);
glRotatef(180, 0.0f, 0.0f, 1.0f); // rotate the robot on its y-axis
glPopMatrix();


	striker.velx = striker.vely = 0.0f;
    striker.y = -0.56f;
    striker.x = 0.0f;
   	striker_Aim.reset();
   	striker_Aim.update(striker.x, striker.y);
   	if (scores.flag == 0) {
   	scores.turnover();	
   	}
   	else {
   		scores.flag = 0;
   	}
}
void keyboard_1(unsigned char key, int x, int y) {

	if (key == 27) {
		exit(0);	
	}
	if (key == 114) {
		cout << "Game Reset!" << endl;
		reset();
	}
	if (key == 32) {
		striker.vely += abs(striker_Aim.length)/10 * sin(striker_Aim.theta);
        striker.velx += abs(striker_Aim.length)/10 * cos(striker_Aim.theta);
	}
	if (key == 97) {
		// cout << "Detected" << endl;
		striker_Aim.acw(striker.x, striker.y);
		// cout << striker_Aim.x << ", " << striker_Aim.y << endl;

	}
	if (key == 100) {
		striker_Aim.cw(striker.x, striker.y);
		// cout << striker_Aim.x << ", " << striker_Aim.y << endl;

	}
	if (key == 13) {
		if (scores.shown == 1) {
			scores.hide();
		}
		else {
			scores.show();
		}
	}

}


void keyboard_2(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT && striker.x > -0.58f)
        striker.x -= 0.01;

    if (key == GLUT_KEY_RIGHT && striker.x < 0.58f) 
        striker.x += 0.01;

	if (striker.vely==0) {
    if (key == GLUT_KEY_UP && striker_Aim.length < 1.0f)
        striker_Aim.length += 0.1f;
    if (key == GLUT_KEY_DOWN && striker_Aim.length > 0.1f)
        striker_Aim.length -= 0.1f; 
}

        striker_Aim.update(striker.x, striker.y); 

}

void handleStrikerMouseMove(int x, int y) {
    double oglx = 2*(double(x)/glutGet(GLUT_WINDOW_WIDTH)-0.5);
    striker.x = oglx;
}

void handleMouseclick(int button, int state, int x, int y) {
    if (state == GLUT_DOWN)
    {
    	if (button == GLUT_LEFT_BUTTON)
        {   
            striker.vely += abs(striker_Aim.length)/10 * sin(striker_Aim.theta);
        	striker.velx += abs(striker_Aim.length)/10 * cos(striker_Aim.theta);
        }   
        else if (button == GLUT_RIGHT_BUTTON)
        {    
        	// cout << "right"<<endl;
           glutMotionFunc(handleStrikerMouseMove);
        }
    	
    }
}

void handleMouseMove (int x, int y) {
    double oglx = 2*(double(x)/glutGet(GLUT_WINDOW_WIDTH)-0.5);
	
double ogly = 2*(1-double(y)/glutGet(GLUT_WINDOW_HEIGHT)-0.5);
double xdiff = oglx - striker.x;
double ydiff = ogly - striker.y;
double dist = rawDistance(oglx, ogly, striker.x, striker.y);
striker_Aim.length = dist/2;
if (striker_Aim.length > 1.0f) {
	striker_Aim.length = 1.0f;
}
striker_Aim.theta = tan(ogly/oglx);
// cout << "Theta: " << striker_Aim.theta << endl;
if (xdiff > 0) {
        striker_Aim.theta = atan(ptSlope(striker.x, striker.y, oglx, ogly));
      
    }
    else if (xdiff < 0) {
    	striker_Aim.theta = DEG2RAD(180) + atan(ptSlope(striker.x, striker.y, oglx, ogly));
        
    }
striker_Aim.update(striker.x, striker.y);
}


void reshape (int width, int height)
{ /* Called when the window is created, moved or resized */
	if ((GLsizei) width > glutGet(GLUT_SCREEN_HEIGHT)) {
		glutReshapeWindow(glutGet(GLUT_SCREEN_HEIGHT),glutGet(GLUT_SCREEN_HEIGHT));
	}
	else {
		glutReshapeWindow((GLsizei) width, (GLsizei) width);
	}
	glViewport (0, 0, (GLsizei) width, (GLsizei) height);
	glMatrixMode (GL_PROJECTION); /* Select the projection matrix */
	glLoadIdentity ();            /* Initialise it */
	glOrtho(-1.0,1.0, -1.0,1.0, -1.0,1.0); /* The unit cube */
	glMatrixMode (GL_MODELVIEW);  /* Select the modelview matrix */
}

void * reduceScore(void * i) {
    
    while (1) {
    	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    scores.board[scores.turn] -= 1;
    }
    pthread_exit(NULL);
}

void * showboard( void * i) {
	scores.show();
    	std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    reset();
    scores.hide();
    pthread_exit(NULL);
}
int main (int argc, char **argv) {

	// PlaySound("music.wav", NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);
	glutInit (&argc, argv);    /* Initialise OpenGL */
	glutInitWindowSize(550,550);
	glutInitWindowPosition(100,100);
	initObjects();

	glutCreateWindow ("OpenCarrom (C) by Mrinal Dhar. All rights reserved.");  /* Create the window */
	
	glutDisplayFunc (display); /* Register the "display" function */
	// showboard();
	 glutIdleFunc(display);

	 glutKeyboardFunc(keyboard_1);
	 glutSpecialFunc (keyboard_2);
	glutReshapeFunc (reshape);
	int i, rc;
	   for( i=0; i < 9; i++ ){
      cout << "main() : creating thread, " << i << endl;
      rc = pthread_create(&threads[i], NULL, 
                          handleCollision, (void *)i);
      if (rc){
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }
   rc = pthread_create(&threads[9], NULL, 
                          reduceScore, (void *)9);

rc = pthread_create(&threads[10], NULL, 
                          showboard, (void *)10);
    glutTimerFunc(10, update, 0);
    glutMouseFunc(handleMouseclick);
    glutPassiveMotionFunc(handleMouseMove);
	glutMainLoop ();

	return 0; 
}
