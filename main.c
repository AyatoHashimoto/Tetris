#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#define clearScreen() printf("\033[2J")
#define setPosition(x,y) printf("\033[%d;%dH",(y)+1,(x)*2+1)
#define setCharColor(n) printf("\033[3%dm",(n))
#define setBackColor(n) printf("\033[4%dm",(n))
#define setAttribute(n) printf("\033[%dm",(n))
#define cursolOn() printf("\033[?25h")
#define cursolOff() printf("\033[?25l")
#define HEIGHT 20
#define WIDTH 10

enum {
	BLACK,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	MAGENTA,
	CYAN,
	WHITE,
	DEFAULT = 9
};

enum {
	NORMAL,
	BLIGHT,
	DIM,
	UNDERBAR = 4,
	BLINK,
	REVERSE = 7,
	HIDE,
	STRIKE
};

typedef struct {
	char c;
	int charcolor;
	int backcolor;
	int attribute;
} Cell;

Cell block_type[7][4][4] = {
	'\0', RED, BLACK, NORMAL,
	'\0', RED, BLACK, NORMAL,
	' ', RED, BLACK, REVERSE,
	'\0', RED, BLACK, NORMAL,
	'\0', RED, BLACK, NORMAL,
	' ', RED, BLACK, REVERSE,
	' ', RED, BLACK, REVERSE,
	'\0', RED, BLACK, NORMAL,
	'\0', RED, BLACK, NORMAL,
	' ', RED, BLACK, REVERSE,
	'\0', RED, BLACK, NORMAL,
	'\0', RED, BLACK, NORMAL,
	'\0', RED, BLACK, NORMAL,
	'\0', RED, BLACK, NORMAL,
	'\0', RED, BLACK, NORMAL,
	'\0', RED, BLACK, NORMAL,

	'\0', BLUE, BLACK, NORMAL,
	' ', BLUE, BLACK, REVERSE,
	' ', BLUE, BLACK, REVERSE,
	'\0', BLUE, BLACK, NORMAL,
	'\0', BLUE, BLACK, NORMAL,
	' ', BLUE, BLACK, REVERSE,
	'\0', BLUE, BLACK, NORMAL,
	'\0', BLUE, BLACK, NORMAL,
	'\0', BLUE, BLACK, NORMAL,
	' ', BLUE, BLACK, REVERSE,
	'\0', BLUE, BLACK, NORMAL,
	'\0', BLUE, BLACK, NORMAL,
	'\0', BLUE, BLACK, NORMAL,
	'\0', BLUE, BLACK, NORMAL,
	'\0', BLUE, BLACK, NORMAL,
	'\0', BLUE, BLACK, NORMAL,
	
	'\0', GREEN, BLACK, NORMAL,
	'\0', GREEN, BLACK, NORMAL,
	'\0', GREEN, BLACK, NORMAL,
	'\0', GREEN, BLACK, NORMAL,
	'\0', GREEN, BLACK, NORMAL,
	' ', GREEN, BLACK, REVERSE,
	' ', GREEN, BLACK, REVERSE,
	'\0', GREEN, BLACK, NORMAL,
	' ', GREEN, BLACK, REVERSE,
	' ', GREEN, BLACK, REVERSE,
	'\0', GREEN, BLACK, NORMAL,
	'\0', GREEN, BLACK, NORMAL,
	'\0', GREEN, BLACK, NORMAL,
	'\0', GREEN, BLACK, NORMAL,
	'\0', GREEN, BLACK, NORMAL,
	'\0', GREEN, BLACK, NORMAL,

	'\0', YELLOW, BLACK, NORMAL,
	'\0', YELLOW, BLACK, NORMAL,
	'\0', YELLOW, BLACK, NORMAL,
	'\0', YELLOW, BLACK, NORMAL,
	'\0', YELLOW, BLACK, NORMAL,
	' ', YELLOW, BLACK, REVERSE,
	' ', YELLOW, BLACK, REVERSE,
	'\0', YELLOW, BLACK, NORMAL,
	'\0', YELLOW, BLACK, NORMAL,
	' ', YELLOW, BLACK, REVERSE,
	' ', YELLOW, BLACK, REVERSE,
	'\0', YELLOW, BLACK, NORMAL,
	'\0', YELLOW, BLACK, NORMAL,
	'\0', YELLOW, BLACK, NORMAL,
	'\0', YELLOW, BLACK, NORMAL,
	'\0', YELLOW, BLACK, NORMAL,

	'\0', MAGENTA, BLACK, NORMAL,
	' ', MAGENTA, BLACK, REVERSE,
	'\0', MAGENTA, BLACK, NORMAL,
	'\0', MAGENTA, BLACK, NORMAL,
	'\0', MAGENTA, BLACK, NORMAL,
	' ', MAGENTA, BLACK, REVERSE,
	'\0', MAGENTA, BLACK, NORMAL,
	'\0', MAGENTA, BLACK, NORMAL,
	'\0', MAGENTA, BLACK, NORMAL,
	' ', MAGENTA, BLACK, REVERSE,
	'\0', MAGENTA, BLACK, NORMAL,
	'\0', MAGENTA, BLACK, NORMAL,
	'\0', MAGENTA, BLACK, NORMAL,
	' ', MAGENTA, BLACK, REVERSE,
	'\0', MAGENTA, BLACK, NORMAL,
	'\0', MAGENTA, BLACK, NORMAL,

	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,
	' ', CYAN, BLACK, REVERSE,
	' ', CYAN, BLACK, REVERSE,
	' ', CYAN, BLACK, REVERSE,
	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,
	' ', CYAN, BLACK, REVERSE,
	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,

	'\0', WHITE, BLACK, NORMAL,
	' ', WHITE, BLACK, REVERSE,
	'\0', WHITE, BLACK, NORMAL,
	'\0', WHITE, BLACK, NORMAL,
	'\0', WHITE, BLACK, NORMAL,
	' ', WHITE, BLACK, REVERSE,
	'\0', WHITE, BLACK, NORMAL,
	'\0', WHITE, BLACK, NORMAL,
	'\0', WHITE, BLACK, NORMAL,
	' ', WHITE, BLACK, REVERSE,
	' ', WHITE, BLACK, REVERSE,
	'\0', WHITE, BLACK, NORMAL,
	'\0', WHITE, BLACK, NORMAL,
	'\0', WHITE, BLACK, NORMAL,
	'\0', WHITE, BLACK, NORMAL,
	'\0', WHITE, BLACK, NORMAL
};
extern int errno;
struct termios otty,ntty;

int wait_ms(int msec);
void init(void);
void reset(void);
int printCell(Cell a, int x, int y);
int clearCell(Cell a, int x, int y);
int checkRange(Cell a, int x, int y);
void copyBlock(Cell src[4][4], Cell dst[4][4]);
int printBlock(Cell Block[4][4], int x, int y);
int clearBlock(Cell Block[4][4], int x, int y);
int kbhit(void);
int getch(void);
int tinit(void);

int main(int argc, char *argv[]){
	init();
	Cell block[4][4];
	copyBlock(block_type[5],block);
	for(int y=0;y<HEIGHT;++y){
		printBlock(block,5,y);
		wait_ms(500);
		clearBlock(block,5,y);
	}
	reset();
}

int wait_ms(int msec){
	struct timespec r = {0, msec * 1000L * 1000L};
	return nanosleep(&r, NULL);
}

void init(void){	
	tinit();
	setBackColor(BLACK);
	setCharColor(WHITE);
	setAttribute(NORMAL);
	clearScreen();
	cursolOff();
}

void reset(void){
	setBackColor(DEFAULT);
	setCharColor(DEFAULT);
	setAttribute(NORMAL);
	clearScreen();
	cursolOn();
	setPosition(0,0);
	tcsetattr(1, TCSADRAIN, &otty);
	write(1, "\n", 1);
} 

int checkRange(Cell a, int x, int y){
	if(a.c == '\0' || x<0 || y<0 || x>=WIDTH || y>=HEIGHT)
		return -1;
	else
		return 0;
}

int printCell(Cell a, int x, int y){
	if(checkRange(a,x,y) == -1)
		return -1;
	setPosition(x,y);
	setAttribute(a.attribute);
	setBackColor(a.backcolor);
	setCharColor(a.charcolor);
	printf("%c%c",a.c, a.c); fflush(stdout);
	return 0;
}

int clearCell(Cell a, int x, int y){
	if(checkRange(a,x,y) == -1)
		return -1;
	setPosition(x,y);
	setAttribute(NORMAL);
	setBackColor(DEFAULT);
	setCharColor(DEFAULT);
	printf("  "); fflush(stdout);
	return 0;
}

void copyBlock(Cell src[4][4], Cell dst[4][4]){
	for(int i=0;i<4;++i)
		for(int j=0;j<4;++j)
			dst[i][j] = src[i][j];
}

int printBlock(Cell block[4][4], int x, int y){
	for(int i=0;i<4;++i)
		for(int j=0;j<4;++j)
			printCell(block[i][j], x + j, y + i);
	return 0;
}

int clearBlock(Cell block[4][4], int x, int y){
	for(int i=0;i<4;++i)
		for(int j=0;j<4;++j)
			clearCell(block[i][j], x + j, y + i);
	return 0;
}

int kbhit(void){
	int ret;
	fd_set rfd;
	struct timeval timeout = {0,0};
	FD_ZERO(&rfd);
	FD_SET(0, &rfd);
	ret = select(1, &rfd, NULL, NULL, &timeout);
	if(ret == 1)
		return 1;
	else
		return 0;
}

int getch(void){
	unsigned char c;
	int n;
	while ((n = read(0, &c, 1)) < 0 && errno == EINTR);
	if(n == 0)
		return -1;
	else
		return (int)c;
}

static void onsignal(int sig){
	signal(sig, SIG_IGN);
	switch(sig){
		case SIGINT:
		case SIGQUIT:
		case SIGTERM:
		case SIGHUP:
			exit(1);
			break;
	}
}

int tinit(void){
	if(tcgetattr(1, &otty) < 0)
		return -1;
	ntty = otty;
	ntty.c_iflag &= ~(INLCR|ICRNL|IXON|IXOFF|ISTRIP);
	ntty.c_oflag &= ~OPOST;
	ntty.c_lflag &= ~(ICANON|ECHO);
	ntty.c_cc[VMIN] = 1;
	ntty.c_cc[VTIME] = 0;
	tcsetattr(1, TCSADRAIN, &ntty);
	signal(SIGINT, onsignal);
	signal(SIGQUIT, onsignal);
	signal(SIGTERM, onsignal);
	signal(SIGHUP, onsignal);
	return 0;
}
