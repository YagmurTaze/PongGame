#include <iostream>
#include <windows.h>
#include <conio.h>
#include <unistd.h>
#include <stdio.h>

#define SIZE 15

int board[SIZE-5][SIZE];
const char *BBG = "Brick Breaker Game";
char *brick = "[___]";
char *wall1 = "| ";
char *wall2 = " |";
int gameStart = 0;
int instr = 0;
int didStart = 0;
int directionX = 0;
int directionY = 0;
int gameOver = 0;
int moveRacket = 0;
int score=0;
int life = 3;

int ballspeed=8;
int ballmovement=0;

int ballx = 40;
int bally = 23;

int racketx = 38;
int rackety = 24;

void gotoxy(unsigned int x, unsigned int y);
void draw_strxy(const char *str,unsigned int x, unsigned int y);
void draw_racket(void);
void draw_ball(void);
void clear_ball(void);
void clear_racket_right(void);
void clear_racket_left(void);
void clear_racket(void);
void checkCollision(void);
void checkCollision();
void move_ball(void);
void gameboard(void);
void game(void);
void checkDirection(void);
void update_instr(void);
void start_screen(void);
void end_screen(void);
void ball_control(void);
void instruction_screen(void);

using namespace std;

void gotoxy(unsigned int x, unsigned int y)
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD position = {x, y};
    SetConsoleCursorPosition(console, position);
}

void draw_strxy(const char *str,unsigned int x, unsigned int y)
{
	gotoxy(x,y);
	cout << str;
}

void draw_racket(void)
{
	const char *racket = "-----";
	if(!gameOver) draw_strxy(racket,racketx,rackety);
}

void draw_ball(void)
{
	const char *ball = "o";
	draw_strxy(ball,ballx,bally);
}

void clear_ball(void)
{
	const char *ball_delete = " ";
	draw_strxy(ball_delete,ballx,bally);
}

void clear_racket_right(void)
{
	const char *racket_delete = "  ";
	draw_strxy(racket_delete,racketx,rackety);
}

void clear_racket_left(void)
{
	const char *racket_delete = "  ";
	draw_strxy(racket_delete,racketx+4,rackety);
}

void clear_racket(void)
{
	const char *racket_delete = "     ";
	draw_strxy(racket_delete,racketx,rackety);
}

void update_score(void)
{
        const char *score_s = "Score:";
        draw_strxy(score_s,1,0);
        cout << score;
}

void update_life(void)
{
        const char *life_s = "Life:";
        draw_strxy(life_s,70,0);
        cout << life;
}

void update_instr(void)
{
  if(moveRacket == 1)
      {
        const char *start_s = "                    ";
        draw_strxy(start_s,32,15);
      }
}

void checkDirection()
{
    if (directionX ==  1) { ballx += 1;}
    if (directionY ==  1) { bally += 1;}
    if (directionX == -1) { ballx -= 1;}
    if (directionY == -1) { bally -= 1;}
}

void checkCollision()
{
  int search = (ballx-2) / 5;
  int deletebricky;
  int deletebrickx;
  if(bally < 12)
  {
    if(board[bally-2][search] == 0)
    {
      score += 100;
      update_score();
      board[bally-2][search] = 1;
      deletebricky=bally;
      deletebrickx=(search*5)+2;
      const char *brick_delete = "     ";
      draw_strxy(brick_delete,deletebrickx,deletebricky);
      directionY = -directionY;
    }
  }
}

void ball_control(void)
{
  if(ballx >= 77) { directionX = -directionX; ballx=77; }
  else if(ballx <= 1) { directionX = -directionX; ballx=1; }
  else if(bally >= 23)
  {
    if(ballx >= racketx && ballx < racketx + 5)
    {
      directionY = -directionY;
      draw_racket();
      bally=23;
    }
    else
    {
      if(life <= 1) {gameOver = 1;}
      life--;
      clear_ball();
      directionX = 0;
      directionY = 0;
      ballx = 40;
      bally = 23;
      clear_racket();
      racketx = 38;
      rackety = 24;
      draw_racket();
      update_life();
      const char *start_s = "press space to start";
      draw_strxy(start_s,32,15);
      moveRacket = 0;
      gameStart =0;
    }
  }
  else if(bally < 3) { directionY = -directionY; bally=3; }
}

void move_ball(void)
{
    if(ballmovement==0)
    {
     ball_control();
	 clear_ball();
	 checkDirection();
	 checkCollision();
	 draw_ball();
    }
}

void gameboard(void)
{
    int i, j, x, y;
    draw_strxy(BBG,34,0);
    update_score();
    update_life();
    update_instr();
    cout << "\n";
    cout << "\n";
    for(i = 0; i < SIZE - 5; i++)
    {
      cout << wall1;
      for(j = 0; j < SIZE; j++)
      {
        if(board[i][j] == 0)
        {
          cout << brick;
        }
      }
      cout << wall2;
      cout << "\n";
    }

    const char *start_s = "press space to start";
    draw_strxy(start_s,32,15);

    draw_racket();
    draw_ball();
}

void start_screen(void)
{
  int i;
  while(!didStart)
  {
  	
  	  for(i = 0; i < 10;i++)
  	  {
    	cout << "\n";
  	  }
	  cout << "                                  BRICK BREAKER                                  ";
	  cout << "\n";
	  cout << "\n";
	  cout << "                               To start press ENTER                               ";
	  for(i = 0; i < 11;i++)
	  {
	    cout << "\n";
	  }
	  cout << " YAGMUR TAZE                                          Instructions(press 'i')     ";
	  char ch;
	  while(ch != 13 || ch != 105 || ch != 73)
	  {
	    ch = _getch();
	    if (ch == 13)//enter
	    {
	      didStart = 1;
	      break;
	    }
	    
	    if (ch == 105 || ch == 73)
	    {
	    	instr =1;
	    	instruction_screen();
	    	break;
		}
		
	  }
  }
 
}

void end_screen(void)
{
  int i;
  for(i = 0; i < 9;i++)
  {
    cout << "\n";
  }
  if(score==15000) cout << "                                      You WIN!!!\n";
  cout <<"                               Your score is  : ";
  cout << score;
  cout << "\n";
  cout << "\n";
  cout <<"                               To restart press ENTER                                ";
  for(i = 0; i < 10;i++)
  {
    cout << "\n";
  }
   cout << " YAGMUR TAZE                                                                     ";
   char ch;
   while(ch != 13)
  {
    ch = _getch();
    if (ch == 13)//enter
    {
      int gameStart = 0;
      directionX = 0;
      directionY = 0;
      gameOver = 0;
      moveRacket = 0;
      score=0;
      life = 3;
      ballspeed=5;
      ballmovement=0;
      ballx = 40;
      bally = 23;
      racketx = 38;
      rackety = 24;
      for(int i = 0; i < SIZE - 5; i++)
      {
        for(int j = 0; j < SIZE; j++)
        {
           board[i][j] = 0;
        }
      }
      system("CLS");
      gameboard();
      game();
      system("CLS");
      end_screen();
    }
  }
}

int keyboard(void)
{
   int keycode;
   keycode = 0;
   if (kbhit())
   {
    keycode = _getch();
    if (keycode == 224)
    {
        keycode = _getch();
    }
   }
   return keycode;
}

void game(void)
{
	gameboard();
    while(!gameOver)
    {
      int keycode = keyboard();

      if(keycode == 32 && gameStart==0)//space
      {
        gameStart = 1;
        directionX = 1;
        directionY = -1;
        moveRacket = 1;
        update_instr();
      }

      if(keycode == 75)//sol
      {
        if(racketx > 0 && didStart && moveRacket)
        {
          clear_racket_left();
          racketx = racketx - 1;
          draw_racket();
        }
      }

      if(keycode == 77)//sag
      {
        if(racketx < 75 && didStart && moveRacket)
        {
          clear_racket_right();
          racketx = racketx + 1;
          draw_racket();
        }
      }
      move_ball();

      ballmovement++;
      if(ballmovement>=ballspeed)
      {
      	ballmovement=0;
      }
      if(score==15000) break; //tum tuglalar kirildiysa
      usleep(1000);
    }
}

void instruction_screen(void) // bilgi ekrani
{
  system("CLS");
  int i;
  char ch;
  
  for(i = 0; i < 5;i++)
  {
    cout << "\n";
  }
  cout <<"                                  INSTRUCTIONS                                  ";
  cout << "\n";
  cout << "\n";
  cout <<"               The aim of the game is to break all the bricks by               ";
  cout << "\n";
  cout <<"               hitting the ball on the paddle.                                 ";
  cout << "\n";
  cout <<"               Move the paddle by pressing the left and right arrow keys.      ";
  cout << "\n";
  cout <<"               If you drop the ball 3 times, the game is over.                 ";
  cout << "\n";
  cout << "\n";
  cout <<"                                 HAVE FUN!!!                                   ";
  for(i = 0; i < 12;i++)
  {
    cout << "\n";
  }
  cout <<"                  Press (<--)Backspace to return to the menu                   ";
  cout << "\n";
  while(ch != 8)
	  {
	    ch = _getch();
	    if (ch == 8)//backspace
	    {
	      instr = 0;
	      break;
	    }
	  }
  while(instr);
  system("CLS");
  return;
}


int main()
{


    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    GetConsoleScreenBufferInfo(console, &bufferInfo);

    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;

    SetConsoleCursorInfo(console, &info);
    SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    SMALL_RECT windowSize = {0, 0, 80, 25};
    SetConsoleWindowInfo(console, TRUE, &windowSize);

    start_screen();
    system("CLS");
    game();
    system("CLS");
    end_screen();

    getchar();
    return 0;
}

