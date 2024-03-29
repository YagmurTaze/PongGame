/*
* Copyright (C) 2014  Arjun Sreedharan
* License: GPL version 2 or higher http://www.gnu.org/licenses/gpl.html
*/
#include "keyboard_map.h"

/* there are 25 lines each of 80 columns; each element takes 2 bytes */
#define LINES 25
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define ENTER_KEY_CODE 0x1C
#define LEFT_KEY_CODE 0x4B 
#define RIGHT_KEY_CODE 0x4D 
#define SPACE_KEY_CODE 0x39 
#define SIZE 15

int board[SIZE-5][SIZE];
const char *BBG = "Brick Breaker Game";
char *brick = "|___|";
char *wall1 = "| ";
char *wall2 = " |";
int gameStart = 0;
int didStart = 0;
int instr = 0;
int directionX = 0;
int directionY = 0;
int gameOver = 0;
int moveRacket = 0;
int score=0;
int life = 3;

int ballx = 40;
int bally = 23;

int racketx = 38;
int rackety = 24;

extern unsigned char keyboard_map[128];
extern void keyboard_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt_ptr);

/* current cursor location */
unsigned int current_loc = 0;
/* video memory begins at address 0xb8000 */
char *vidptr = (char*)0xb8000;

void gotoxy(unsigned int x, unsigned int y);
void draw_strxy(const char *str,unsigned int x, unsigned int y);
void draw_racket(void);
void draw_ball(void);
void clear_ball(void);
void clear_racket_right(void);
void clear_racket_left(void);
void checkCollision(void);
void checkCollision();
void move_ball(void);
void print_integer(int n,int color);
void gameboard(void);
void game(void);
void checkDirection(void);
void update_instr(void);
void instruction_screen(void);
void start_screen(void);

struct IDT_entry {
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];

void idt_init(void)
{
	unsigned long keyboard_address;
	unsigned long idt_address;
	unsigned long idt_ptr[2];

	/* populate IDT entry of keyboard's interrupt */
	keyboard_address = (unsigned long)keyboard_handler;
	IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
	IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x21].zero = 0;
	IDT[0x21].type_attr = INTERRUPT_GATE;
	IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;

	/*     Ports
	*	 PIC1	PIC2
	*Command 0x20	0xA0
	*Data	 0x21	0xA1
	*/

	/* ICW1 - begin initialization */
	write_port(0x20 , 0x11);
	write_port(0xA0 , 0x11);

	/* ICW2 - remap offset address of IDT */
	/*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
	write_port(0x21 , 0x20);
	write_port(0xA1 , 0x28);

	/* ICW3 - setup cascading */
	write_port(0x21 , 0x00);
	write_port(0xA1 , 0x00);

	/* ICW4 - environment info */
	write_port(0x21 , 0x01);
	write_port(0xA1 , 0x01);
	/* Initialization finished */

	/* mask interrupts */
	write_port(0x21 , 0xff);
	write_port(0xA1 , 0xff);

	/* fill the IDT descriptor */
	idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	load_idt(idt_ptr);
}

void kb_init(void)
{
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	write_port(0x21 , 0xFD);
}

void kprint(const char *str,int color)
{
	unsigned int i = 0;
	while (str[i] != '\0') {
		vidptr[current_loc++] = str[i++];
		vidptr[current_loc++] = color;
	}
}

void kprint_newline(void)
{
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	current_loc = current_loc + (line_size - current_loc % (line_size));
}

void print_integer(int n, int color) // sayi print etme
{
    int i = 0;
    char buffer[20];  
    do {
        buffer[i++] = (n % 10) + '0';  
        n /= 10;
    } while (n != 0);
    while (i > 0) {
      vidptr[current_loc++] = buffer[--i];
      vidptr[current_loc++] = color;
    }
}


void clear_screen(void)
{
	unsigned int i = 0;
	while (i < SCREENSIZE) {
		vidptr[i++] = ' ';
		vidptr[i++] = 0x07;
	}
	current_loc = 0;
}

void keyboard_handler_main(void)
{
	unsigned char status;
	char keycode;

	/* write EOI */
	write_port(0x20, 0x20);

	status = read_port(KEYBOARD_STATUS_PORT);
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		keycode = read_port(KEYBOARD_DATA_PORT);
		if(keycode < 0)
			return;
          		
		if(keycode == SPACE_KEY_CODE && gameStart==0) {
		        
			gameStart = 1;
                        directionX = 1;
                        directionY = -1;
                        moveRacket = 1;
			update_instr();
			return;
		}

		if(keycode == ENTER_KEY_CODE) {
			didStart = 1;
			return;
		}
		
		if(keycode == LEFT_KEY_CODE) {
		        if(racketx > 0 && didStart && moveRacket)
		        {
		          clear_racket_left();
	                  racketx = racketx - 1;
	                  draw_racket();
		        }
			return;
		}
		
		if(keycode == RIGHT_KEY_CODE) {
		        if(racketx < 75 && didStart && moveRacket)
		        {
		          clear_racket_right();
	                  racketx = racketx + 1;
	                  draw_racket();
		        }
			return; 
		}
		
		if(keycode == 23) {
		        instr = 1;
			return; 
		}
		
		if(keycode == 14) {
		        instr = 0;
			return; 
		}

		vidptr[current_loc++] = keyboard_map[(unsigned char) keycode];
		vidptr[current_loc++] = 0x07;
	}
}
void sleep(int sec) // bekletme fonk.
{
	int i;
  for(i=0;i<sec;i++);

}

void gotoxy(unsigned int x, unsigned int y) // belirli koordinata gitmek icin hesaplama fonk.
{
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	current_loc = BYTES_FOR_EACH_ELEMENT * (x * COLUMNS_IN_LINE + y);
}

void draw_strxy(const char *str,unsigned int x, unsigned int y) // verilen koordinata yazi yazan fonk.
{
	gotoxy(y,x);
	kprint(str,0x0F);
}

void draw_racket(void) //raket cizimi
{
	const char *racket = "-----";
	if(!gameOver) draw_strxy(racket,racketx,rackety);
}

void draw_ball(void) //top cizimi
{
	const char *ball = "o";
	draw_strxy(ball,ballx,bally);
}

void clear_ball(void) // top hareket ettikten sonra eski konumunu siler
{
	const char *ball_delete = " ";
	draw_strxy(ball_delete,ballx,bally);
}

void clear_racket_right(void) // raket saga hareket ettikten sonra eski konumunu siler
{
	const char *racket_delete = "  ";
	draw_strxy(racket_delete,racketx,rackety);
}

void clear_racket_left(void) //raket sola hareket ettikten sonra eski konumunu siler
{
	const char *racket_delete = "  ";
	draw_strxy(racket_delete,racketx+4,rackety);
}

void clear_racket(void) //raketi tamamen siler
{
	const char *racket_delete = "     ";
	draw_strxy(racket_delete,racketx,rackety);
}

void update_score(void) // skor degisimlerini yazdirir
{
        const char *score_s = "Score:";
        draw_strxy(score_s,1,0); 
        print_integer(score, 0x02);
}

void update_life(void) // can degisimini yazdirir
{
        const char *life_s = "Life:";
        draw_strxy(life_s,70,0); 
        print_integer(life, 0x04);
}

void update_instr(void) // oyun baslangicinda verilen komutu yazdirir/siler
{
  if(moveRacket == 1)
      {
        const char *start_s = "                    ";
	draw_strxy(start_s,32,15);
      }
}

void gameboard(void) //oyun ekrani
{
    int i, j, x, y;
    draw_strxy(BBG,34,0);
    update_score();
    update_life();
    kprint_newline();
    kprint_newline();
    for(i = 0; i < SIZE - 5; i++) 
    {
      kprint(wall1,0x0F);
      for(j = 0; j < SIZE; j++) 
      {
        if(board[i][j] == 0)
        {
          kprint(brick,0x0B);
        }
      }
      kprint(wall2,0x0F);
      kprint_newline();
    } 
    draw_racket();
    draw_ball();
}

void checkDirection() // topun yonune gore yeni pozisyon belirleme
{	
  if (directionX ==  1) { ballx += 1;}
  if (directionY ==  1) { bally += 1;}
  if (directionX == -1) { ballx -= 1;}
  if (directionY == -1) { bally -= 1;}  
}

void move_ball(void) // topu kontrollerden gecirerek hareket ettirir
{
    clear_ball();
    checkDirection();
    checkCollision();
    draw_ball();
}

void checkCollision() // tuglalara carpimi kontrol eder
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

void ball_control() // topun duvarlara ve rakete carpimini kontrol eder
{
  move_ball();
  
  if(ballx >= 77) { directionX = -directionX; ballx=77; }
  else if(ballx <= 1) { directionX = -directionX; ballx=1; }
  else if(bally >= 23) 
  { 
    if(ballx >= racketx && ballx < racketx+5)
    {
      directionY = -directionY; 
      bally=23; 
    }
    else 
    { 
      if(life <= 1) {gameOver = 1;}
      gameStart = 0;
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
      moveRacket = 0;
    }
  }
  else if(bally < 3) { directionY = -directionY; bally=3; }
}

void end_screen(void) // bitis ekrani
{
  int i;
  for(i = 0; i < 10;i++)
  {
    kprint_newline();
  }
  kprint("                               Your score is  : ",0x0F);
  print_integer(score,0x02);
  kprint_newline();
  kprint_newline();
  kprint("                               To restart press any key                                ",0x0F);
  for(i = 0; i < 10;i++)
  {
    kprint_newline();
  }
  kprint(" YAGMUR TAZE                                                                    ",0x0B);
}

void start_screen(void) // baslangic ekrani
{
  int i;
  for(i = 0; i < 10;i++)
  {
    kprint_newline();
  }
  kprint("                                  BRICK BREAKER                                  ",0x0F);
  kprint_newline();
  kprint_newline();
  kprint("                               To start press ENTER                               ",0x0F);
  for(i = 0; i < 10;i++)
  {
    kprint_newline();
  }
  kprint(" YAGMUR TAZE                                           Instructions(press 'i')   ",0x0B);
  while(!didStart && !instr);
  if(instr == 1)
  {
    clear_screen();
    instruction_screen();
  }
  
  clear_screen();
}

void instruction_screen(void) // bilgi ekranis
{
  int i;
  for(i = 0; i < 5;i++)
  {
    kprint_newline();
  }
  kprint("                                  INTRUCTIONS                                  ",0x0F);
  kprint_newline();
  kprint_newline();
  kprint("               The aim of the game is to break all the bricks by               ",0x0A);
  kprint("               hitting the ball on the paddle.                                 ",0x0B);
  kprint("               Move the paddle by pressing the left and right arrow keys.      ",0x0C);
  kprint("               If you drop the ball 3 times, the game is over.                 ",0x0D);
  kprint_newline();
  kprint_newline();
  kprint("                                 HAVE FUN!!!                                   ",0x0F);
  for(i = 0; i < 12;i++)
  {
    kprint_newline();
  }
  kprint("                  Press (<--)Backspace to return to the menu                   ",0x0B);
  kprint_newline();
  while(instr);
  clear_screen();
  start_screen();
}


void game(void) // ana oyun dongusu
{
    gameboard();
    while(1)
    {
      if(moveRacket == 0)
      {
        const char *start_s = "press space to start";
	draw_strxy(start_s,32,15);
      }
      
      if(gameOver == 1)
      {
        break;
      }
      ball_control();
      sleep(13000000); // topun hizi
  }  
  clear_screen();
}

void kmain(void)
{
    idt_init();
    kb_init();  
    clear_screen();
    start_screen();
    game();
    end_screen();      
 }

