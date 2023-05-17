#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <unistd.h>
#include <iostream>

#define LEFT1           'A'
#define LEFT2           'a'
#define RIGHT1          'D'
#define RIGHT2          'd'
#define SPACE           32

#define SIZE 12

using namespace std;

int board[SIZE-5][SIZE];

int racketx = 355;
int rackety = 480;
int ballx = 400;
int bally = 468;
int ball_radius = 8;

int directionX = 0;
int directionY = 0;

int score = 0;
int life = 3;

int ballspeed=5;
int ballmovement=0;

int gameOver = 0;
int gameStart = 0;
int didStart = 0;
int moveRacket = 0;

struct bricks{
	int x;
	int y;
};
bricks bricksPosition[SIZE-5][SIZE];

void draw_racket(void) // raketi cizdir
{
	rectangle(racketx, rackety, racketx+90, rackety+10);
    setfillstyle(SOLID_FILL, LIGHTBLUE);
    floodfill(racketx+1, rackety+1, WHITE);
}

void clear_racket(void) // eski raketi sil
{
    rectangle(racketx, rackety, racketx+90, rackety+10);
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    floodfill(racketx, rackety, LIGHTGRAY);
}

void clear_ball(void) // eski topu sil
{
    circle(ballx, bally, ball_radius);
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    floodfill(ballx, bally, LIGHTGRAY);
}

void draw_ball(void) // topu ciz
{
	circle(ballx, bally, ball_radius);
    setfillstyle(SOLID_FILL, LIGHTBLUE);
    floodfill(ballx, bally, WHITE);
}

void checkDirection() // yone bakarak yeni pozisyonu ver
{
    if(ballmovement==0)
    {
        if (directionX ==  1) { ballx += 1;}
        if (directionY ==  1) { bally += 1;}
        if (directionX == -1) { ballx -= 1;}
        if (directionY == -1) { bally -= 1;}
    }
}


void gameboard() // oyun ekrani
{
    cleardevice();
    int x = 13;
    int y = 40;
    setbkcolor(LIGHTGRAY);
    outtextxy(320, 10, "Brick Breaker Game");
    outtextxy(10, 10, "Score : 0");
    outtextxy(720, 10, "Life : 3");
    for(int i = 0; i < SIZE - 5; i++)
    {
      for(int j = 0; j < SIZE; j++)
      {
        if(board[i][j] == 0) // tuglalari cizdir
        {
          bricksPosition[i][j].x = x;
          bricksPosition[i][j].y = y;
          rectangle(x, y, x+55, y+20);
          setfillstyle(SOLID_FILL, LIGHTBLUE);
          floodfill(x+1, y+1, WHITE);
        }
        x += 65;
      }
      y += 25;
      x = 13;
    }
    draw_racket();
    draw_ball();
}

void ball_control(void) //topun ekranin kenarlarina ve rakete carpimini kontrol et
{
  if(ballx >= 780) { directionX = -directionX; ballx=780; } // sag kenar
  else if(ballx <= 0) { directionX = -directionX; ballx=0; } // sol kenar
  else if(bally >= 470) // alt kenar
  {
    if(ballx >= racketx && ballx <= racketx + 90) // rakete carptiysa
    {
      directionY = -directionY;
      draw_racket();
      bally=470;
    }
    else
    {
      if(life <= 1) {gameOver = 1;}
      life--;
      char str1[10];
      sprintf(str1, "%d", life);
      outtextxy(755, 10, str1);
      clear_ball();
      directionX = 0;
      directionY = 0;
      ballx = 400;
      bally = 468;
      clear_racket();
      racketx = 355;
      rackety = 480;
      draw_racket();
      moveRacket = 0;
      gameStart = 0;
    }
  }
  else if(bally < 40) { directionY = -directionY; bally=40; } // ust kenar
}

void checkCollision()// tuglalara carpimi kontrol et
{
  for(int i = 0; i < SIZE - 5; i++)
    {
      for(int j = 0; j < SIZE; j++)
      {
        if(ballx + ball_radius >= bricksPosition[i][j].x && ballx - ball_radius <= bricksPosition[i][j].x + 55 && bally + ball_radius >= bricksPosition[i][j].y && bally - ball_radius <= bricksPosition[i][j].y + 20)
        {
            if(board[i][j] == 0) // carpmissa arkaplan rengine boya
            {
                int x = bricksPosition[i][j].x;
                int y = bricksPosition[i][j].y;
                rectangle(x, y, x+55, y+20);
                setfillstyle(SOLID_FILL, LIGHTGRAY);
                floodfill(x+1, y+1, LIGHTGRAY);
                score += 100;
                char str[10];
                sprintf(str, "%d", score);
                outtextxy(60, 10, str);
                board[i][j] = 1;
                directionY = -directionY;
            }
        }
      }
    }
}


void move_racket(int num) // raketi hareket ettir
{
    racketx += num;
    clear_racket();
    draw_racket();
}
void sleepmyself(void){
	for(int i=0;i<100*5;i++){

	}
}

void move_ball(void) //topun tum hareketleri
{
     ball_control();
     clear_ball();
     checkDirection();
     checkCollision();
     draw_ball();
     sleepmyself();
}

void gameControls() // klavye ayarlari
{
    int keyPressed;

    if (kbhit()) {
        keyPressed = getch();
        if (keyPressed == LEFT1 || keyPressed == LEFT2)
        {
            if(racketx > 0 && didStart && moveRacket)
            {
                move_racket(-7);
            }
        }
        else if (keyPressed == RIGHT1 || keyPressed == RIGHT2)
        {
            if(racketx < 708 && didStart && moveRacket)
            {
                move_racket(7);
            }
        }
        else if (keyPressed == SPACE)
        {
            if(moveRacket==0)
            {
              directionX = 1;
              directionY = -1;
            }
            moveRacket = 1;
            gameStart = 1;
        }

    }
}

void game() // ana oyun loopu
{
    while (!gameOver)
    {
       gameControls();
       move_ball();

       ballmovement++;
       if(ballmovement >= ballspeed)
       {
      	ballmovement=0;
       }
       if(score==8400) break;
       usleep(800);
    }
}

void start_screen(void) // baslangic ekrani
{
  int keyPressed;
  setbkcolor(LIGHTGRAY);
  setcolor(WHITE);
  settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 3);
  outtextxy(300, 200, "BRICK BREAKER GAME");
  outtextxy(300, 250, "To start press ENTER");
  settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 1);
  outtextxy(10, 460, "YAGMUR TAZE");
  outtextxy(630, 460, "Instructions(press ' i ' )");
   while (1)
   {
    if (kbhit() && getch() == 13)
    {
      didStart = 1;
      break;
    }
   }
   cleardevice();
}

void end_screen(void) // bitis ekrani
{
  cleardevice();
  if(score==8400) { outtextxy(300, 245, "YOU WIN!"); }
  outtextxy(300, 250, "Your score is: ");
  char str[10];
  sprintf(str, "%d", score);
  outtextxy(410, 250, str);
  outtextxy(300, 280, "To close press a key");
  outtextxy(10, 480, "YAGMUR TAZE");

}

int main()
{
    initwindow(800, 500, "Brick Breaker Game");
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    bar(0, 0, getmaxx(), getmaxy());

    start_screen();
    gameboard();
    game();
    end_screen();

    getch();
    closegraph();
    return 0;
}
