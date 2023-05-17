#include <GL/freeglut_std.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <iostream>
using namespace std;

#define WINDOW_W 1200
#define WINDOW_H 750

static GLfloat screenBottom = -30.5;
static const int FPS = 60;
static GLfloat stick1Position = 0.0;
static GLfloat stickThick = 1.0;
static GLfloat stickSize = 7;
static GLfloat moveXFactor = 0.3;
static GLfloat moveYFactor = 0.3;
static GLfloat rectX = -15;
static GLfloat rectY = -5;
static bool gameStarted = false;
static GLfloat newHeight;
static GLfloat newWidth;
const int brickAmount = 54;
static int life = 3;
static int score = 0;
static bool instr = false;
static int finish = 0;
static bool pressed = false;
static bool startScreen = true;
static bool instrStr = true;
static bool done = false;


struct _ball
{
	GLfloat radius = 1.0;
	GLfloat X = 0.0;
	GLfloat Y = -27;
	int directionX = 0;
	int directionY = 0;
}ball;

struct bricks {
	float x;
	float y;
	float width;
	float height;
	bool isBroken = false;
};
bricks bricksArray[brickAmount];

void createBricks() {
	float brickX = -49, brickY = 25;
	for (int i = 0; i < brickAmount; i++) {
		bricksArray[i].x = brickX;
		bricksArray[i].y = brickY;
		bricksArray[i].width = 10;
		bricksArray[i].height = 2.5;
		brickX += 11;
		if (brickX > 49) {
			brickX = -49;
			brickY -= 4;
		}
	}

	for (int i = 0; i < brickAmount; i++) {
		if (!bricksArray[i].isBroken) {
			glBegin(GL_QUADS);
			glColor3ub(0x87, 0xCE, 0xFA);
			glVertex2f(bricksArray[i].x, bricksArray[i].y);
			glVertex2f(bricksArray[i].x + bricksArray[i].width, bricksArray[i].y);
			glVertex2f(bricksArray[i].x + bricksArray[i].width, bricksArray[i].y + bricksArray[i].height);
			glVertex2f(bricksArray[i].x, bricksArray[i].y + bricksArray[i].height);
			glEnd();
		}
	}
}

bool CheckCollision(float ballX, float ballY, float ballRadius, float brickX, float brickY, float brickWidth, float brickHeight) {
	if (ballX + ballRadius >= brickX && ballX - ballRadius <= brickX + brickWidth && ballY + ballRadius >= brickY && ballY - ballRadius <= brickY + brickHeight) {
		ball.directionY = ball.directionY * (-1);
		finish += 1;
		score += 100;
		return true;
	}
	return false;
}

void reshape(GLint w, GLint h)
{
	newWidth = w;
	newHeight = h;
	glViewport(0, 0, w, h);
	GLfloat aspect = (GLfloat)w / (GLfloat)h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-50.0, 50.0, -50.0 / aspect, 50.0 / aspect, -1.0, 1.0);
}

void draw_circle(float x, float y, float radius) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, 0.0f);
	static const int circle_points = 150;
	static const float angle = 2.0f * 3.1416f / circle_points;

	glBegin(GL_POLYGON);
	double angle1 = 0.0;
	glVertex2d(radius * cos(0.0), radius * sin(0.0));
	int i;
	for (i = 0; i < circle_points; i++)
	{
		glVertex2d(radius * cos(angle1), radius * sin(angle1));
		angle1 += angle;
	}
	glEnd();

	glPopMatrix();
}

void start()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f); 
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(rectX + 5, -rectY + 5);
	char str1[] = "BRICK BREAKER GAME";
	for (int i = 0; i < strlen(str1); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str1[i]);
	}

	glRasterPos2f(-49, -30);
	char str2[] = "By: Yagmur Taze";
	for (int i = 0; i < strlen(str2); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str2[i]);
	}

	glRasterPos2f(30, -30);
	char str5[] = "Instructions (press ' i ')";
	for (int i = 0; i < strlen(str5); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str5[i]);
	}

	glRectf(rectX, 0, -rectX, rectY);

	glColor3f(0, 0, 0);
	glRasterPos2f(rectX + (-rectX/2) + 1, rectY-rectY/2.5);
	char str3[] = "Click to Start";
	for (int i = 0; i < strlen(str3); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str3[i]);
	}

	glColor3f(1, 1, 1);
	glRasterPos2f(rectX + 8, rectY - rectY - 10);
	char str4[] = "or press enter";
	for (int i = 0; i < strlen(str4); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str4[i]);
	}

	glutSwapBuffers();
}

void end()
{
	done = true;

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1, 1, 1);
	glRasterPos2f(-10, 0);
	char str2[] = "GAME OVER";
	for (int i = 0; i < strlen(str2); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str2[i]);
	}
	glRasterPos2f(-10, -5);
	std::string scoreStr = "Your score : " + std::to_string(score); 
	for (int i = 0; i < scoreStr.length(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, scoreStr[i]);
	}

	glColor3f(1, 1, 1);
	glRasterPos2f(-49, -30);
	char str1[] = "To restrat press 'R'";
	for (int i = 0; i < strlen(str1); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str1[i]);
	}

	glutSwapBuffers();
}

void instructions()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1, 1, 1);
	glRasterPos2f(-10, 20);
	char str1[] = "INSTRUCTIONS";
	for (int i = 0; i < strlen(str1); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str1[i]);
	}

	glColor3f(0.3, 1, 1);
	glRasterPos2f(-20, 10);
	std::string str2 = "The aim of the game is to break all the bricks by";
	for (int i = 0; i < str2.length(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str2[i]);
	}

	glColor3f(1, 1, 0.3);
	glRasterPos2f(-20, 8);
	std::string str3 = "hitting the ball on the paddle.";
	for (int i = 0; i < str3.length(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str3[i]);
	}
	
	glColor3f(1, 0.3, 1);
	glRasterPos2f(-20, 6);
	std::string str4 = "Move the paddle by pressing the left and right arrow keys.";
	for (int i = 0; i < str4.length(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str4[i]);
	}

	glColor3f(1, 0.5, 0.3);
	glRasterPos2f(-20, 4);
	std::string str5 = "If you drop the ball 3 times, the game is over.";
	for (int i = 0; i < str5.length(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str5[i]);
	}

	glColor3f(0.3, 0.8, 0.2);
	glRasterPos2f(-8, 0);
	std::string str6 = "HAVE FUN!!";
	for (int i = 0; i < str6.length(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str6[i]);
	}

	glColor3f(1, 1, 1);
	glRasterPos2f(-49, -30);
	std::string str7 = "Press (<--)Backspace to return to the menu";
	for (int i = 0; i < str7.length(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str7[i]);
	}

	glutSwapBuffers();
}

void display()
{
	if (finish == brickAmount)
	{
		end();
	}
	else if (startScreen == true)
	{
		start();
	}
	else if (instr == true)
	{
		instructions();
	}
	else if (life < 1)
	{
		end();
	}
	else
	{
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glColor3f(1, 1, 1);
		glRasterPos2f(-49, 29);
		std::string scoreStr = "Score: " + std::to_string(score);
		for (int i = 0; i < scoreStr.length(); i++)
		{
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, scoreStr[i]);
		}

		glRasterPos2f(-8, 29);
		std::string titleStr = "BRICK BREAKER";
		for (int i = 0; i < titleStr.length(); i++)
		{
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, titleStr[i]);
		}

		glRasterPos2f(43, 29);
		std::string sayiStr = "Life: " + std::to_string(life);
		for (int i = 0; i < sayiStr.length(); i++)
		{
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, sayiStr[i]);
		}

		glRasterPos2f(38, 28);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, life);

		glColor3f(1, 1, 1);
		glRectf(stick1Position - stickSize, screenBottom, stick1Position + stickSize, screenBottom + stickThick);
		draw_circle(ball.X, ball.Y, ball.radius);
		createBricks();

		if (pressed == false)
		{
			glRasterPos2f(-10, -10);
			std::string iStr = "press --> or <-- to start";
			for (int i = 0; i < iStr.length(); i++)
			{
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, iStr[i]);
			}
		}

		glFlush();
		glutSwapBuffers();
	}
}

void timer(int v)
{
	glutPostRedisplay();

	if (ball.X + ball.radius > 49.5 || ball.X + ball.radius < -47.5)
	{
		ball.directionX = ball.directionX * (-1);
	}

	if (ball.Y < screenBottom + stickThick + ball.radius)
	{
		if (((stick1Position - stickSize) < ball.X) && (ball.X < (stick1Position + stickSize)) && (ball.Y > screenBottom + stickThick))
		{
			ball.directionY = ball.directionY * (-1);
		}
		else if (ball.Y < screenBottom + stickThick)
		{
			ball.X = 0;
			ball.Y = -27;
			ball.directionX = 0;
			ball.directionY = 0;
			moveXFactor = 0.3;
			moveYFactor = 0.3;
			stick1Position = 0.0;
			pressed = false;
			life -= 1;
		
		}
	}

	if (ball.Y > 23 || ball.Y < -29.5)
	{
		ball.directionY = ball.directionY * -1;
	}

	for (int i = 0; i < brickAmount; i++) {
		if (bricksArray[i].isBroken == false)
		{
			if (CheckCollision(ball.X, ball.Y, ball.radius, bricksArray[i].x, bricksArray[i].y, bricksArray[i].width, bricksArray[i].height))
			{
				bricksArray[i].isBroken = true;
			}
		}
	}

	ball.X = ball.X + (moveXFactor * ball.directionX);
	ball.Y = ball.Y + (moveYFactor * ball.directionY);
	glutTimerFunc(1000 / FPS, timer, v);
}

void mouse(int button, int state, int x, int y)
{
	GLfloat x_ratio = newWidth / 800.0;
	GLfloat y_ratio = newHeight / 500.0;

	int start_button_left = (int)(280 * x_ratio);
	int start_button_top = (int)(250 * y_ratio);
	int start_button_right = (int)(520 * x_ratio);
	int start_button_bottom = (int)(290 * y_ratio);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (x > start_button_left && x < start_button_right && y > start_button_top && y < start_button_bottom)
		{
			startScreen = false;
		}
	}
}

void kbSpecial(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_RIGHT:
		if (pressed == false)
		{
			ball.directionX = 1;
			ball.directionY = 1;
			pressed = true;
		}
		if (stick1Position < 42)
		{
			stick1Position = stick1Position + 0.9;

		}
		break;
	case GLUT_KEY_LEFT:
		if (pressed == false)
		{
			ball.directionX = -1;
			ball.directionY = 1;
			pressed = true;
		}
		if (stick1Position > -42)
		{
			stick1Position = stick1Position - 0.9;
		}
		break;
	}
}

void restart()
{
	done = false;
	startScreen = true;
	ball.X = 0;
	ball.Y = -27;
	ball.directionX = 0;
	ball.directionY = 0;
	moveXFactor = 0.3;
	moveYFactor = 0.3;
	stick1Position = 0.0;
	pressed = false;
	life = 3;
	score = 0;

	for (int i = 0; i < brickAmount; i++)
	{
		bricksArray[i].isBroken = false;
	}
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 13: // enter
		if (!done)
		{
			startScreen = false;
			instr = false;
		}
		break;
	case 105: // i
		if (!done)
		{
			startScreen = false;
			instr = true;
		}
		break;

	case 73: // I
		if (!done)
		{
			startScreen = false;
			instr = true;
		}
		break;
	case 8: // backspace
		if (!done)
		{
			instr = false;
			startScreen = true;
		}
		break;
	case 114: // r
		restart();
		break;
	case 82: // R
		restart();
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(100, 90);
	glutInitWindowSize(WINDOW_W, WINDOW_H);
	glutCreateWindow("Brick Breaker");
	glutReshapeFunc(reshape); 
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutTimerFunc(100, timer, 0);
	glutSpecialFunc(kbSpecial);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
}
